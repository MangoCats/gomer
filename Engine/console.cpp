#include "console.h"

Console::Console(QObject *parent) : QObject(parent)
{ m_notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this); }

void Console::run()
{ // std::cout << "Gomer Engine" << std::endl;
  // std::cout << "> " << std::flush;
  connect(m_notifier, SIGNAL(activated(int)), this, SLOT(readCommand()));
}

void Console::readCommand()
{ std::string line;
  std::getline(std::cin, line);
  emit newline( QString::fromUtf8( line.c_str() ) );
  // std::cout << "> " << std::flush;
}

void Console::sendResponse( QString msg )
{ msg += "\n";
  std::cout << msg.toUtf8().data() << std::flush;
}
