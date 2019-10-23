#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QSocketNotifier>
#include <iostream>

/**
 * @brief The Console class - connects messages to/from stdout/stdin
 */
class Console : public QObject
{
    Q_OBJECT
public:
    explicit  Console(QObject *parent = nullptr);
        void  run();

signals:
        void  newline( QString );

public slots:
        void  readCommand();
        void  sendResponse( QString );

private:
    QSocketNotifier *m_notifier;


};

#endif // CONSOLE_H
