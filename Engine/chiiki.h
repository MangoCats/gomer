#ifndef CHIIKI_H
#define CHIIKI_H

class Goban;
class Shiko;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "shiko.h"

/**
 * @brief The Chiiki 地域 class - calculating and estimating territory
 */
class Chiiki : public QObject
{
    Q_OBJECT
public:
    explicit  Chiiki(Shiko *p);
              Chiiki(Chiiki *pcp,Shiko *p);

signals:

public slots:

public:
    QPointer<Goban> bp;
    QPointer<Shiko> tp;
};

#endif // CHIIKI_H
