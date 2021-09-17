#include "ray.h"
#include "wall.h"

#include <QGraphicsItem>
#define PI 3.141592653589793

Ray::Ray(double angle, QPointF point, QGraphicsScene * gameScene)
{
    this->setX(point.x());
    this->setY(point.y());
    this->angle = angle;
    this->gameScene = gameScene;
}

Ray::Ray(double angle, int x, int y)
{
    this->setX(x);
    this->setY(y);
    this->angle = angle;
}

void Ray::cast()
{
    QPointF * closestPoint = nullptr;
    double closestPointDistance = 1000000000;

    foreach(QGraphicsItem * item, gameScene->items()){
        Wall * wall = dynamic_cast<Wall*>(item);
        if(wall != nullptr)
        {
            // Check rays
            double x1 = wall->line().x1();
            double y1 = wall->line().y1();
            double x2 = wall->line().x2();
            double y2 = wall->line().y2();

            double y3 = this->pos().y();
            double x3 = this->pos().x();
            double y4 = this->pos().y() + cos((this->angle - 90) * PI / 180);
            double x4 = this->pos().x() + sin((this->angle - 90) * PI / 180);

            double den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
            if (den != 0) {
                double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
                double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
                if (t > 0 && t < 1 && u > 0) {
                    QPointF pt;
                    pt.setX(x1 + t * (x2 - x1));
                    pt.setY(y1 + t * (y2 - y1));

                    // Calculate distance beetween player and that point
                    QPointF vector;
                    vector.setX(this->x() - pt.x());
                    vector.setY(this->y() - pt.y());

                    double dist = sqrt(pow(vector.x(), 2) + pow(vector.y(), 2));
                    if(dist < closestPointDistance){
                        closestPoint = new QPointF(pt.x(), pt.y());
                        closestPointDistance = dist;
                        this->distance = dist;
                        wallHit = wall;
                        hitPoint = QPointF(*closestPoint);
                    }
                }
            }
        }
    }
    if(closestPoint != nullptr){
        emit sigRayCast(closestPoint);
    }
}
