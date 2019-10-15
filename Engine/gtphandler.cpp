#include "gtphandler.h"

GtpHandler::GtpHandler(QCoreApplication *app, Game *parent) : QObject(parent), gp(parent)
{ qDebug( "GtpHandler constructor" );
  connect( this, SIGNAL(exit(int)), app, SLOT(exit(int)) );
  handledCommands.append( "quit" );
#define      COMMAND_INDEX_QUIT                 0
  handledCommands.append( "protocol_version" );
#define      COMMAND_INDEX_PROTOCOL_VERSION     1
  handledCommands.append( "name" );
#define      COMMAND_INDEX_NAME                 2
  handledCommands.append( "version" );
#define      COMMAND_INDEX_VERSION              3
  handledCommands.append( "known_command" );
#define      COMMAND_INDEX_KNOWN_COMMAND        4
  handledCommands.append( "list_commands" );
#define      COMMAND_INDEX_LIST_COMMANDS        5
  handledCommands.append( "boardsize" );
#define      COMMAND_INDEX_BOARDSIZE            6
  handledCommands.append( "clear_board" );
#define      COMMAND_INDEX_CLEAR_BOARD          7
  handledCommands.append( "komi" );
#define      COMMAND_INDEX_KOMI                 8
  handledCommands.append( "showboard" );
#define      COMMAND_INDEX_SHOWBOARD            9
}

/**
 * @brief GtpHandler::receivedMessage - a new message from the interface, handle it.
 * @param m - message received
 */
void  GtpHandler::receivedMessage( QString m )
{ qint32 id;
  QString command_name;
  QString arguments;
  m = trimComments( m ).trimmed();
  if ( m.size() <= 0 )
    return;  // 2.10 Empty lines and lines with only whitespace sent by the controller must be ignored by the engine.
  m.replace( QChar('\t'), QChar(' ') );
  parseReceivedMessage( m, &id, &command_name, &arguments );
  if ( command_name.size() <= 0 )
    { respond( false, id, "no_supported_command_found" );
      return;
    }
  QString msg,cmd;
  qint32 sz;
  bool success;
  switch ( handledCommands.indexOf(command_name) )
    { case COMMAND_INDEX_QUIT:
        respond( true, id );
        emit exit( 0 );
        break;

      case COMMAND_INDEX_PROTOCOL_VERSION:
        respond( true, id, "2" );
        break;

      case COMMAND_INDEX_NAME:
        respond( true, id, "Gomer" );
        break;

      case COMMAND_INDEX_VERSION:
        respond( true, id, "0.0.1" );
        break;

      case COMMAND_INDEX_KNOWN_COMMAND:
        respond( true, id, handledCommands.contains( arguments ) ? "true" : "false" );
        break;

      case COMMAND_INDEX_LIST_COMMANDS:
        msg="";
        foreach ( cmd, handledCommands )
          msg += cmd +"\n";
        respond( true, id, msg );
        break;

      case COMMAND_INDEX_BOARDSIZE:
        if ( !checkGpNull( id ) ) break;
        sz = arguments.toInt();
        if ( sz < 5 )
          { respond( false, id, "unacceptable size" );
            break;
          }
        success = gp->resizeGoban( sz, sz );
        if ( !success )
          { respond( false, id, "unacceptable size" );
            break;
          }
        respond( true, id );
        break;

      case COMMAND_INDEX_CLEAR_BOARD:
        if ( !checkGpNull( id ) ) break;
        gp->clearGoban();
        respond( true, id );
        break;

      case COMMAND_INDEX_KOMI:
        if ( !checkGpNull( id ) ) break;
        gp->komi = arguments.toDouble();
        respond( true, id );
        break;

      case COMMAND_INDEX_SHOWBOARD:
        if ( !checkGpNull( id ) ) break;
        respond( true, id, gp->showBoard() );
        break;

      default:
        respond( false, id, "unexpected_failure_to_find_command" );
    }

}

/**
 * @brief GtpHandler::checkGpNull - frequently used safety/error check
 * @param id - for use in the failure message
 * @return false if gp is nullptr
 */
bool  GtpHandler::checkGpNull( qint32 id )
{ if ( gp == nullptr )
    { respond( false, id, "game_object_null" );
      return false;
    }
  return true;
}

/**
 * @brief GtpHandler::respond - send a formatted response
 * @param pf - pass or fail
 * @param id - id number ( < 0 is no id )
 * @param responseMsg - response message, if any
 */
void GtpHandler::respond( bool pf, qint32 id, QString responseMsg )
{ QString r = pf ? "=" : "?";
  if ( id >= 0 )
    r.append( QString("%1").arg(id) );
  if ( responseMsg.size() > 0 )
    r.append( " "+responseMsg );
  r.append( "\n\n" );
  emit response(r);
}

/**
 * @brief GtpHandler::trimComments
 * @param m - received message
 * @return m with any comments removed
 */
QString GtpHandler::trimComments( QString m )
{ if ( !m.contains("#") )
    return m;
  return m.left( m.indexOf("#") ).trimmed();
}

/**
 * @brief GtpHandler::parseReceivedMessage - split a received message line into component parts
 * @param m - full message line
 * @param id - optional id number, returns as -1 if no id was found
 * @param command_name - returns as empty string, if no handled command was found
 * @param arguments - words which follow the command_name, if any
 */
void  GtpHandler::parseReceivedMessage( QString m, qint32 *id, QString *command_name, QString *arguments )
{  // defaults, if no id is included in the message
  *id = -1;
  *command_name = "";
  *arguments = "";
  QStringList words = m.split(QRegExp("[\r\n\t ]+"), QString::SkipEmptyParts);
  if ( words.size() < 1 )
    return;
  if ( handledCommands.contains( words.at(0) ) ) // Command without id
    { *command_name = words.at(0);
      *arguments = m.mid( m.indexOf( words.at(0) ) + words.at(0).size() ).trimmed();
      return;
    }
  if ( words.size() < 2 )
    return;
  if ( handledCommands.contains( words.at(1) ) ) // Command without id
    { *id = words.at(0).toInt();
      *command_name = words.at(1);
      *arguments = m.mid( m.indexOf( words.at(1) ) + words.at(0).size() ).trimmed();
      return;
    }
  return;
}


