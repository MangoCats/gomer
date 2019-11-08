#ifndef KOGAI_H
#define KOGAI_H

class Ruikei;
class Soshi;
#include <QDataStream>
#include <QObject>
#include <QPointer>
#include "ruikei.h"
#include "soshi.h"

/**
 * @brief The Kogai 梗概 class - what does the Ruikei know about
 *   the future potential outcomes of its position
 */
class Kogai : public QObject
{
    Q_OBJECT
public:
    explicit Kogai(Ruikei *p = nullptr);
             Kogai(QDataStream &ds, Ruikei *p = nullptr);
  QByteArray toByteArray();
        bool fromByteArray( QByteArray );
        bool isValid();

    QList<QPointer<Soshi> > ftl;  /// Friendly Territory List
    QList<QPointer<Soshi> > otl;  /// Opponent Territory List

};

#endif // KOGAI_H
