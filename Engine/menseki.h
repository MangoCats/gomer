#ifndef MENSEKI_H
#define MENSEKI_H

#include <QObject>


/**
 * @brief The Menseki 面積 class - base class
 *   for play areas like Goban and Ruikei
 */
class Menseki : public QObject
{
    Q_OBJECT
public:
     explicit  Menseki(QObject *parent = nullptr);
        QChar  colorToChar( qint32 c );

signals:

public slots:

public:
      QString  goishiChar;
  QStringList  Xlabels,Ylabels;
};

#endif // MENSEKI_H
