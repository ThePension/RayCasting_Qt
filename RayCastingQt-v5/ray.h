#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "wall.h"

class Ray : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Ray(double angle, QPointF point, QGraphicsScene * gameScene);
    Ray(double angle, int x, int y);
    double angle = 0;
    double initialAngle = 0;
    double distance;
    QGraphicsScene * gameScene = nullptr;
    Wall * wallHit;
    QPointF hitPoint;
public slots:
    void cast();
signals:
    void sigRayCast(QPointF *);
};

#endif // RAY_H
