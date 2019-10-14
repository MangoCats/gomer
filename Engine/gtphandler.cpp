#include "gtphandler.h"

GtpHandler::GtpHandler(QCoreApplication *app, Game *parent) : QObject(parent), gp(parent)
{ qDebug( "GtpHandler constructor" );
  connect( this, SIGNAL(exit(int)), app, SLOT(exit(int)) );
  handledCommands.append( "quit" );
  handledCommands.append( "clear_board" );
}

/**
 * @brief GtpHandler::receivedMessage - a new message from the interface, handle it.
 * @param m - message received
 */
void  GtpHandler::receivedMessage( QString m )
{ qint32 id;
  QString command_name;
  QString arguments;
  m = trimComments( m );
  parseReceivedMessage( m, &id, &command_name, &arguments );
  if ( command_name.size() <= 0 )
    { respond( false, id, "no_supported_command_found" );
      return;
    }
  switch ( handledCommands.indexOf(command_name) )
    { case 0: // quit
        respond( true, id );
        emit exit( 0 );
        break;

      case 1: // clear_board
        if ( gp != nullptr )
          { gp->clearBoard();
            respond( true, id );
          }
         else
          respond( false, id, "game_object_null" );
        break;
    }

}

/**
 * @brief GtpHandler::respond - send a formatted response
 * @param pf - pass or fail
 * @param id - id number ( < 0 is no id )
 * @param msg - message, if any
 */
void GtpHandler::respond( bool pf, qint32 id, QString msg )
{ QString r = pf ? "=" : "?";
  if ( id >= 0 )
    r.append( QString("%1").arg(id) );
  if ( msg.size() > 0 )
    r.append( " "+msg );
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


