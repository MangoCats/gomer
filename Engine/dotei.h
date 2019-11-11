#ifndef DOTEI_H
#define DOTEI_H

#include <QObject>

/**
 * @brief The Dotei 同定 class - identification labels for Goban/Ruikei row/column vertices
 */
class Dotei : public QObject
{
    Q_OBJECT
public:
    explicit Dotei(QObject *parent = nullptr);

    QString  goishiChar;
QStringList  Xlabels,Ylabels;
};

#endif // DOTEI_H
