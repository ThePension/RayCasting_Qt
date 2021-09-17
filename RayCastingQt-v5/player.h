#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsRectItem>

#include "ray.h"

class Player : public QObject, public QGraphicsRectItem
{
public:
    Player(QGraphicsItem * parent = 0);
    double dx = 0;
    double dy = 0;
    double rotationInDegree = 0;
    double angleRotationInDegree = 0;
    QList<Ray *> rays;

public slots:
    void onMove();

};

#endif // PLAYER_H
