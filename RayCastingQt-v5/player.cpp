#include "player.h"
#include <QBrush>

Player::Player(QGraphicsItem * parent) : QGraphicsRectItem(parent)
{
    this->setRect(0, 0, 20, 20);
    this->setBrush(QBrush(Qt::green, Qt::SolidPattern));
    this->setPos(100, 100);
    this->setTransformOriginPoint(10, 10);
}

void Player::onMove()
{
    // this->setPos(this->pos().x() + dx, this->pos().y() + dy);

    this->moveBy(dx, dy);

    foreach(Ray * ray, rays){
        ray->setX(this->pos().x());
        ray->setY(this->pos().y());
    }
    this->setRotation(rotationInDegree);
    // this->ray->angle = -rotationInDegree;
}
