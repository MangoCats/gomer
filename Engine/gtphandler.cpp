#include "gtphandler.h"
#include "console.h"
#include "config.h"

GtpHandler::GtpHandler(QCoreApplication *app, Shiai *p) : QObject(p), gp(p)
{ // qDebug( "GtpHandler constructor" );
  showBoardAfterPlay = false;
  debugWyrms  = false;
  debugRyoiki = false;
  debugJiyu   = false;
  debugDraco  = false;
  connect( this, SIGNAL(quit()), app, SLOT(quit()) );
  Console *cp = new Console(this);
  connect( cp,   SIGNAL(newline(QString)),  this, SLOT(receivedMessage(QString)) );
  connect( this, SIGNAL(response(QString)), cp,   SLOT(sendResponse(QString))    );
  connect( gp->mp, SIGNAL(echo(QString))  , cp,   SLOT(echo(QString)) );
  cp->run();
  handledCommands.append( "quit" );
#define      COMMAND_INDEX_QUIT                      0
  handledCommands.append( "protocol_version" );
#define      COMMAND_INDEX_PROTOCOL_VERSION          1
  handledCommands.append( "name" );
#define      COMMAND_INDEX_NAME                      2
  handledCommands.append( "version" );
#define      COMMAND_INDEX_VERSION                   3
  handledCommands.append( "known_command" );
#define      COMMAND_INDEX_KNOWN_COMMAND             4
  handledCommands.append( "list_commands" );
#define      COMMAND_INDEX_LIST_COMMANDS             5
  handledCommands.append( "help" );
#define      COMMAND_INDEX_HELP                      6
  handledCommands.append( "boardsize" );
#define      COMMAND_INDEX_BOARDSIZE                 7
  handledCommands.append( "query_boardsize" );
#define      COMMAND_INDEX_QUERY_BOARDSIZE           8
  handledCommands.append( "clear_board" );
#define      COMMAND_INDEX_CLEAR_BOARD               9
  handledCommands.append( "komi" );
#define      COMMAND_INDEX_KOMI                     10
  handledCommands.append( "get_komi" );
#define      COMMAND_INDEX_GET_KOMI                 11
  handledCommands.append( "color" );
#define      COMMAND_INDEX_COLOR                    12
  handledCommands.append( "list_stones" );
#define      COMMAND_INDEX_LIST_STONES              13
  handledCommands.append( "showboard" );
#define      COMMAND_INDEX_SHOWBOARD                14
  handledCommands.append( "black" );
#define      COMMAND_INDEX_BLACK                    15
  handledCommands.append( "playwhite" );
#define      COMMAND_INDEX_PLAYWHITE                16
  handledCommands.append( "play" );
#define      COMMAND_INDEX_PLAY                     17
  handledCommands.append( "p" );
#define      COMMAND_INDEX_P                        18
  handledCommands.append( "d" );
#define      COMMAND_INDEX_D                        19
  handledCommands.append( "e" );
#define      COMMAND_INDEX_E                        20
  handledCommands.append( "debug_draco" );
#define      COMMAND_INDEX_DEBUG_DRACO              21
  handledCommands.append( "debug_jiyu" );
#define      COMMAND_INDEX_DEBUG_JIYU               22
  handledCommands.append( "debug_ryoiki" );
#define      COMMAND_INDEX_DEBUG_RYOIKI             23
  handledCommands.append( "debug_wyrms" );
#define      COMMAND_INDEX_DEBUG_WYRMS              24
  handledCommands.append( "showboard_after_play" );
#define      COMMAND_INDEX_SHOWBOARD_AFTER_PLAY     25
  handledCommands.append( "sap" );
#define      COMMAND_INDEX_SAP                      26
  handledCommands.append( "noboard_after_play" );
#define      COMMAND_INDEX_NOBOARD_AFTER_PLAY       27
  handledCommands.append( "nap" );
#define      COMMAND_INDEX_NAP                      28
  handledCommands.append( "is_legal" );
#define      COMMAND_INDEX_IS_LEGAL                 29
  handledCommands.append( "captures" );
#define      COMMAND_INDEX_CAPTURES                 30
  handledCommands.append( "countlib" );
#define      COMMAND_INDEX_COUNTLIB                 31
  handledCommands.append( "findlib" );
#define      COMMAND_INDEX_FINDLIB                  32
  handledCommands.append( "genmove_black" );
#define      COMMAND_INDEX_GENMOVE_BLACK            33
  handledCommands.append( "genmove_white" );
#define      COMMAND_INDEX_GENMOVE_WHITE            34
  handledCommands.append( "genmove" );
#define      COMMAND_INDEX_GENMOVE                  35
  handledCommands.append( "g" );
#define      COMMAND_INDEX_G                        36
  handledCommands.append( "reg_genmove" );
#define      COMMAND_INDEX_REG_GENMOVE              37
  handledCommands.append( "gg_genmove" );
#define      COMMAND_INDEX_GG_GENMOVE               38
  handledCommands.append( "level" );
#define      COMMAND_INDEX_LEVEL                    39
}

/**
 * @brief GtpHandler::receivedMessage - a new message from the interface, handle it.
 * @param m - message received
 */
void  GtpHandler::receivedMessage( QString m )
{ qint32 id;
  QString command_name;
  QString arguments;
  m = trimComments( m ).trimmed().toLower();
  if ( m.size() <= 0 )
    return;  // 2.10 Empty lines and lines with only whitespace sent by the controller must be ignored by the engine.
  m.replace( QChar('\t'), QChar(' ') );
  parseReceivedMessage( m, &id, &command_name, &arguments );
  if ( command_name.size() <= 0 )
    { respond( false, id, "unknown command" );
      return;
    }
  QStringList args = arguments.split(QRegExp("[\r\n\t ]+"), QString::SkipEmptyParts);
  QString msg,cmd;
  qint32 sz,c,x,y,i;
  quint32 seed;
  Goishi *ip;
  bool success,ok;
  switch ( handledCommands.indexOf(command_name) )
    { case COMMAND_INDEX_QUIT:
        respond( true, id );
        if ( gp != nullptr )
          if ( gp->tp != nullptr )
            gp->tp->writeRuikei();
        emit quit();
        break;

      case COMMAND_INDEX_PROTOCOL_VERSION:
        respond( true, id, "2" );
        break;

      case COMMAND_INDEX_NAME:
        respond( true, id, "Gomer" );
        break;

      case COMMAND_INDEX_VERSION:
        msg = QString( "%1.%2.%3" )
              .arg( GomerEngine_VERSION_MAJOR )
              .arg( GomerEngine_VERSION_MINOR )
              .arg( GomerEngine_VERSION_PATCH );
        respond( true, id, msg );
        break;

      case COMMAND_INDEX_KNOWN_COMMAND:
        respond( true, id, handledCommands.contains( arguments ) ? "true" : "false" );
        break;

      case COMMAND_INDEX_HELP:
      case COMMAND_INDEX_LIST_COMMANDS:
        msg="";
        args = handledCommands;
        std::sort(args.begin(),args.end());
        foreach ( cmd, args )
          msg += cmd+"\n";
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

      case COMMAND_INDEX_QUERY_BOARDSIZE:
        if ( !checkGpNull( id ) ) break;
        if ( !checkBpNull( id ) ) break;
        if ( gp->bp->Xsize == gp->bp->Ysize )
          { respond( true, id, QString::number( gp->bp->Xsize ) ); break; }
        respond( true, id, QString::number( gp->bp->Xsize )+","+QString::number( gp->bp->Ysize ) );
        break;

      case COMMAND_INDEX_CLEAR_BOARD:
        if ( !checkGpNull( id ) ) break;
        gp->clearGoban();
        respond( true, id );
        break;

      case COMMAND_INDEX_KOMI:
        if ( !checkGpNull( id ) ) break;
        ok = true;
        gp->komi = arguments.toDouble( &ok );
        if ( ok )
          respond( true, id );
         else
          respond( false, id, "incorrect argument "+arguments );
        break;

      case COMMAND_INDEX_GET_KOMI:
        if ( !checkGpNull( id ) ) break;
        respond( true, id, QString::number( gp->komi ) );
        break;

      case COMMAND_INDEX_COLOR:
        if ( !checkBpNull( id ) ) break;
        ip = gp->bp->goishiAt( arguments );
        if ( ip == nullptr )
          { respond( true, id, "empty" ); break; }
        if ( ip->color == 0 )
          msg = "black";
         else if ( ip->color == 1 )
          msg = "white";
         else
          msg = QString::number( ip->color );
        respond( true, id, msg );
        break;

      case COMMAND_INDEX_LIST_STONES:
        if ( !checkBpNull( id ) ) break;
        c = interpretColor( arguments );
        if (( c < 0 ) || ( c >= gp->np ))
          { respond( false, id, "invalid color "+arguments ); break; }
        i = -1; msg = "";
        while ( ++i < gp->bp->nPoints() )
          { ip = gp->bp->grid.at(i);
            if ( ip != nullptr )
              if ( ip->color == c )
                msg.append( gp->bp->indexToVertex(i)+" " );
          }
        respond( true, id, msg );
        break;

      case COMMAND_INDEX_SHOWBOARD:
        if ( !checkGpNull( id ) ) break;
        respond( true, id, gp->showBoard() );
        break;

      case COMMAND_INDEX_BLACK:
        if ( !checkBpNull( id ) ) break;
        if ( arguments == "pass" )
          gp->pass();
         else
          { if ( !gp->bp->vertexToXY( arguments, &x, &y ) )
              { respond( false, id, "invalid vertex "+arguments ); break; }
            if ( !gp->playGoishi( x, y, 0 ) )
              { respond( false, id, "illegal move "+arguments ); break; }
          }
        respond( true, id );
        break;

      case COMMAND_INDEX_PLAYWHITE:
        if ( !checkBpNull( id ) ) break;
        if ( arguments == "pass" )
          gp->pass();
         else
          { if ( !gp->bp->vertexToXY( arguments, &x, &y ) )
              { respond( false, id, "invalid vertex "+arguments ); break; }
            if ( !gp->playGoishi( x, y, 1 ) )
              { respond( false, id, "illegal move "+arguments ); break; }
          }
        respond( true, id );
        break;

      case COMMAND_INDEX_PLAY:
      case COMMAND_INDEX_P:
        if ( !checkGpNull( id ) ) break;
        c = interpretColor( args.at(0) );
        if (( c < 0 ) || ( c >= gp->np ))
          { respond( false, id, "invalid color "+args.at(0) ); break; }
        if ( !checkBpNull( id ) ) break;
        if ( args.at(1) == "pass" )
          gp->pass();
         else
          { if ( !gp->bp->vertexToXY( args.at(1), &x, &y ) )
              { respond( false, id, "invalid vertex "+args.at(1) ); break; }
            if ( !checkTpNull( id ) ) break;
            if ( !gp->tp->legalMove( x, y, c ) )
              { respond( false, id, "illegal move "+args.at(0)+" "+args.at(1) ); break; }
            if ( !gp->playGoishi( x, y, c ) )
              { respond( false, id, "problem playing "+args.at(0)+" "+args.at(1) ); break; }
          }
        respond( true, id, postMoveMsg() );
        break;

      case COMMAND_INDEX_D:
        debugDraco  = true;
        debugJiyu   = true;
        debugRyoiki = true;
        debugWyrms  = true;
        showBoardAfterPlay = true;
        respond( true, id, "All debug ON" );
        break;

      case COMMAND_INDEX_E:
        debugDraco  = false;
        debugJiyu   = false;
        debugRyoiki = false;
        debugWyrms  = false;
        respond( true, id, "All debug OFF" );
        break;

      case COMMAND_INDEX_DEBUG_DRACO:
        debugDraco = !debugDraco;
        respond( true, id, debugDraco ? "Draco debug ON" : "Draco debug OFF" );
        break;

      case COMMAND_INDEX_DEBUG_JIYU:
        debugJiyu = !debugJiyu;
        respond( true, id, debugJiyu ? "Jiyu debug ON" : "Jiyu debug OFF" );
        break;

      case COMMAND_INDEX_DEBUG_RYOIKI:
        debugRyoiki = !debugRyoiki;
        respond( true, id, debugRyoiki ? "Ryoiki debug ON" : "Ryoiki debug OFF" );
        break;

      case COMMAND_INDEX_DEBUG_WYRMS:
        debugWyrms = !debugWyrms;
        respond( true, id, debugWyrms ? "Wyrm debug ON" : "Wyrm debug OFF" );
        break;

      case COMMAND_INDEX_SHOWBOARD_AFTER_PLAY:
      case COMMAND_INDEX_SAP:
        showBoardAfterPlay = true;
        respond( true, id );
        break;

      case COMMAND_INDEX_NOBOARD_AFTER_PLAY:
      case COMMAND_INDEX_NAP:
        showBoardAfterPlay = false;
        respond( true, id );
        break;

      case COMMAND_INDEX_LEVEL:
        if ( !checkMpNull( id ) ) break;
        ok = true;
        i = arguments.toInt( &ok );
        if (( !ok ) || ( i < 0 ))
          { respond( false, id, "incorrect argument "+arguments ); break; }
        gp->mp->level = i;
        respond( true, id );
        break;

      case COMMAND_INDEX_IS_LEGAL:
        if ( !checkGpNull( id ) ) break;
        c = interpretColor( args.at(0) );
        if (( c < 0 ) || ( c >= gp->np ))
          { respond( false, id, "invalid color "+args.at(0) ); break; }
        if ( !checkBpNull( id ) ) break;
        if ( !gp->bp->vertexToXY( args.at(1), &x, &y ) )
          { respond( false, id, "invalid vertex "+args.at(1) ); break; }
        if ( !checkTpNull( id ) ) break;
        respond( true, id, gp->tp->legalMove( x, y, c ) ? "1" : "0" );
        break;

      case COMMAND_INDEX_CAPTURES:
        if ( !checkGpNull( id ) ) break;
        c = interpretColor( arguments );
        if (( c < 0 ) || ( c >= gp->np ))
          { respond( false, id, "invalid color "+arguments ); break; }
        if ( gp->spl.size() <= c )
          { respond( false, id, "no gosu found for "+arguments ); break; }
        if ( gp->spl.at(c) == nullptr )
          { respond( false, id, arguments+" gosu is null" ); break; }
        respond( true, id, QString::number( gp->spl.at(c)->lid.size() ) );
        break;

      case COMMAND_INDEX_COUNTLIB:
        if ( !checkBpNull( id ) ) break;
        i = gp->bp->vertexToIndex( arguments );
        if ( i < 0 )
          { respond( false, id, "invalid vertex "+arguments ); break; }
        ip = gp->bp->goishi( i );
        if ( ip == nullptr )
          { respond( false, id, "empty vertex "+arguments ); break; }
        if ( ip->wp == nullptr )
          { respond( false, id, "null Wyrm @ "+arguments ); break; }
        respond( true, id, QString::number( ip->wp->libertyList.size() ) );
        break;

      case COMMAND_INDEX_FINDLIB:
        if ( !checkBpNull( id ) ) break;
        i = gp->bp->vertexToIndex( arguments );
        if ( i < 0 )
          { respond( false, id, "invalid vertex "+arguments ); break; }
        ip = gp->bp->goishi( i );
        if ( ip == nullptr )
          { respond( false, id, "empty vertex "+arguments ); break; }
        if ( ip->wp == nullptr )
          { respond( false, id, "null Wyrm @ "+arguments ); break; }
        foreach ( i, ip->wp->libertyList )
          msg.append( gp->bp->indexToVertex(i)+" " );
        respond( true, id, msg );
        break;

      case COMMAND_INDEX_GENMOVE_BLACK:
        if ( !checkMpNull( id ) ) break;
        cmd = gp->mp->genmove(0);
        if ( gp->bp->onBoard( cmd ) || ( cmd == "pass" ) )
          if ( !gp->playGoishi( cmd, 0 ) )
            { respond( false, id, "problem playing black "+cmd ); break; }
        respond( true, id, cmd );
        break;

      case COMMAND_INDEX_GENMOVE_WHITE:
        if ( !checkMpNull( id ) ) break;
        cmd = gp->mp->genmove(1);
        if ( gp->bp->onBoard( cmd ) || ( cmd == "pass" ) )
          if ( !gp->playGoishi( cmd, 1 ) )
            { respond( false, id, "problem playing white "+cmd ); break; }
        respond( true, id, cmd );
        break;

      case COMMAND_INDEX_G:
        if ( !checkMpNull( id ) ) break;
        c = gp->pt;
        cmd = gp->mp->genmove(c);
        if ( gp->bp->onBoard( cmd ) || ( cmd == "pass" ) )
          if ( !gp->playGoishi( cmd, c ) )
            { respond( false, id, "problem playing "+arguments+" "+cmd ); break; }
        msg = "\n"+postMoveMsg();
        respond( true, id, cmd+msg );
        break;

      case COMMAND_INDEX_GENMOVE:
        if ( !checkMpNull( id ) ) break;
        c = interpretColor( arguments );
        if (( c < 0 ) || ( c >= gp->np ))
          { respond( false, id, "invalid color "+arguments ); break; }
        cmd = gp->mp->genmove(c);
        if ( gp->bp->onBoard( cmd ) || ( cmd == "pass" ) )
          if ( !gp->playGoishi( cmd, c ) )
            { respond( false, id, "problem playing "+arguments+" "+cmd ); break; }
        msg = "\n"+postMoveMsg();
        respond( true, id, cmd+msg );
        break;

      case COMMAND_INDEX_REG_GENMOVE:
        if ( !checkMpNull( id ) ) break;
        c = interpretColor( arguments );
        if (( c < 0 ) || ( c >= gp->np ))
          { respond( false, id, "invalid color "+arguments ); break; }
        respond( true, id, gp->mp->genmove(c) );
        break;

      case COMMAND_INDEX_GG_GENMOVE:
        if ( !checkMpNull( id ) ) break;
        c = interpretColor( args.at(0) );
        if (( c < 0 ) || ( c >= gp->np ))
          { respond( false, id, "invalid color "+args.at(0) ); break; }
        if ( args.size() > 1 )
          { ok = true;
            seed = args.at(1).toUInt( &ok );
            if ( !ok )
              { respond( false, id, "invalid seed "+args.at(1) ); break; }
            gp->mp->rng.seed( seed );
          }
        respond( true, id, gp->mp->genmove(c) );
        break;

      default:
        respond( false, id, "unexpected_failure_to_find_command" );
    }

}

/**
 * @brief GtpHandler::postMoveMsg
 * @return non-standard GTP, debug info
 */
QString GtpHandler::postMoveMsg()
{ QString msg = showBoardAfterPlay ? gp->showBoard() : "";
  msg.append( debugJiyu   ? gp->tp->jp->show() : "" );
  msg.append( debugRyoiki ? gp->tp->cp->showRyoiki() : "" );
  msg.append( debugWyrms  ? gp->tp->showWyrms() : "" );
  msg.append( debugDraco  ? gp->tp->showDraco() : "" );
  return msg;
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

bool  GtpHandler::checkBpNull( qint32 id )
{ if ( gp == nullptr )
    { respond( false, id, "game pointer null" );
      return false;
    }
  if ( gp->bp == nullptr )
    { respond( false, id, "Goban pointer null" );
      return false;
    }
  return true;
}

bool  GtpHandler::checkTpNull( qint32 id )
{ if ( gp == nullptr )
    { respond( false, id, "game pointer null" );
      return false;
    }
  if ( gp->tp == nullptr )
    { respond( false, id, "Shiko pointer null" );
      return false;
    }
  return true;
}

bool  GtpHandler::checkMpNull( qint32 id )
{ if ( gp == nullptr )
    { respond( false, id, "game pointer null" );
      return false;
    }
  if ( gp->mp == nullptr )
    { respond( false, id, "Sakudo pointer null" );
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
  if ( !r.endsWith( "\n" ) )
    r.append("\n");
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
  if ( handledCommands.contains( words.at(1) ) ) // Command with id
    { *id = words.at(0).toInt();
      *command_name = words.at(1);
      *arguments = m.mid( m.indexOf( words.at(1) ) + words.at(1).size() ).trimmed();
      return;
    }
  return;
}

/**
 * @brief GtpHandler::interpretColor
 * @param cs - color string to interpret
 * @return color as an integer player number (0 black, 1 white) or -1 if invalid for this game
 */
qint32  GtpHandler::interpretColor( QString cs )
{ qint32 c = -1;
  if ( gp == nullptr )
    return c;
  if (( cs == "b" ) || ( cs == "black" ))
    c = 0;
  if (( cs == "w" ) || ( cs == "white" ))
    c = 1;
  bool ok = true;
  if ( c < 0 )
    c = cs.toInt(&ok);
  if (( c >= gp->np ) || ( c < 0 ) || !ok )
    c = -1;
  return c;
}
