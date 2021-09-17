#include "wall.h"

#include <QPen>

Wall::Wall(QGraphicsItem * parent) : QGraphicsLineItem(parent)
{
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(3);
    setPen(pen);
}
