#include "widget.h"
#include "wall.h"
#include "player.h"
#include "ray.h"

#include <QGraphicsScene>
#include <QBrush>
#include <math.h>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#define PI 3.141592653589793

Widget::Widget(QWidget *parent) : QGraphicsView(parent)
{
    // Map scene
    this->setMinimumSize(650, 650);
    mapScene = new QGraphicsScene(this);
    setSceneRect(0, 0, 600, 600);
    setBackgroundBrush(QBrush(Qt::black));
    setScene(mapScene);

    // this->scale(0.1, 0.1);

    // Create moveTimer
    moveTimer = new QTimer();
    moveTimer->start(1000 / 60); // 140 FPS

    // Create elapsed timer
    elapsedTimer = new QElapsedTimer();
    elapsedTimer->start();
    connect(moveTimer, &QTimer::timeout, this, [=](){ elapsedTimer->restart(); });

    // Set walls
    Wall * wall = new Wall(nullptr);
    wall->setLine(200, 0, 200, 300);
    mapScene->addItem(wall);
    Wall * wall1 = new Wall(nullptr);
    wall1->setLine(200, 300, 300, 300);
    mapScene->addItem(wall1);
    Wall * wall2 = new Wall(nullptr);
    wall2->setLine(300, 0, 300, 300);
    mapScene->addItem(wall2);
    setBorderWalls();

    // Create player
    player = new Player(nullptr);
    // player->setRotation(180);
    connect(moveTimer, &QTimer::timeout, player, &Player::onMove);

    // Clear the map scene
    connect(moveTimer, &QTimer::timeout, this, &Widget::clearMapScene);
    connect(moveTimer, &QTimer::timeout, this, &Widget::clearGameScene);

    // Add player to the game scene
    mapScene->addItem(player);

    // Test ray
    for(double a = -30; a < 30; a += 0.2){
        Ray * ray = new Ray(a, player->pos(), mapScene); // First point
        ray->initialAngle = a;
        connect(moveTimer, &QTimer::timeout, ray, &Ray::cast);
        connect(ray, &Ray::sigRayCast, this, &Widget::onRayCast);
        player->rays.push_front(ray);
    }

    // Game scene
    gameScene = new QGraphicsScene(this);
    connect(moveTimer, &QTimer::timeout, this, &Widget::renderMap3D);

    foreach(QGraphicsItem *item, mapScene->items())
   {
       item->setFlag(QGraphicsItem::ItemIsMovable);
       item->setFlag(QGraphicsItem::ItemIsSelectable);
   }
}

Widget::~Widget()
{
}

void Widget::setBorderWalls()
{
    // Wall of scene
    Wall * topWall = new Wall(nullptr);
    topWall->setLine(0, 0, 600, 0);
    mapScene->addItem(topWall);
    Wall * bottomWall = new Wall(nullptr);
    bottomWall->setLine(0, 600, 600, 600);
    mapScene->addItem(bottomWall);
    Wall * leftWall = new Wall(nullptr);
    leftWall->setLine(0, 0, 0, 600);
    mapScene->addItem(leftWall);
    Wall * rightWall = new Wall(nullptr);
    rightWall->setLine(600, 0, 600, 600);
    mapScene->addItem(rightWall);
}

void Widget::clearGameScene(){
    gameScene->clear();
    // this->update();
}

double Widget::map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Widget::clearMapScene()
{
    foreach(QGraphicsItem * item, mapScene->items()){
        QGraphicsEllipseItem * e = dynamic_cast<QGraphicsEllipseItem*>(item);
        if(e != nullptr)
        {
            // Remove from the scene
            mapScene->removeItem(e);
            delete e;
        }
    }
    foreach(QGraphicsItem * item, mapScene->items()){
        QGraphicsLineItem * l = dynamic_cast<QGraphicsLineItem*>(item);

        if(l != nullptr && typeid(*l).name() != typeid(Wall).name())
        {
            // Remove from the scene
            mapScene->removeItem(l);
            delete l;
        }
    }
}

void Widget::keyPressEvent(QKeyEvent * ke)
{
    double offsetY = 5 * sin((player->rotation() + 180) * PI / 180.f);
    double offsetX = 5 * cos((player->rotation() + 180) * PI / 180.f);

    switch (ke->key()) {
        case Qt::Key_A:
            player->rotationInDegree = player->rotation() - 5;
            if(int(player->rotationInDegree) % 360 == 0) player->rotationInDegree = 0;
            foreach(Ray * ray, player->rays){
                ray->angle += 5;
            }
            break;
        case Qt::Key_D:
            player->rotationInDegree = player->rotation() + 5;
            if(int(player->rotationInDegree) % 360 == 0) player->rotationInDegree = 0;
            foreach(Ray * ray, player->rays){
                ray->angle -= 5;
            }
            break;
        case Qt::Key_W:
            // Calculate offsets
            player->dx = offsetX;
            player->dy = offsetY;
            break;
        case Qt::Key_S:
            // Calculate offsets
            player->dx = -offsetX;
            player->dy = -offsetY;
            break;
        case Qt::Key_M:
            setScene(mapScene);
            isGameSceneActive = false;
            break;
        case Qt::Key_Escape:
            setScene(gameScene);
            isGameSceneActive = true;
            break;
    }
}

void Widget::keyReleaseEvent(QKeyEvent *ke)
{
    player->dx = 0;
    player->dy = 0;
    player->rotationInDegree = player->rotation();
}

void Widget::onRayCast(QPointF * pt)
{
    if(isGameSceneActive) return;
    // Draw a point in the wall
    /*QGraphicsEllipseItem * ellipse = new QGraphicsEllipseItem(pt->x() - 50, pt->y() - 50, 100, 100);
    ellipse->setBrush(QBrush(Qt::blue));
    mapScene->addItem(ellipse);*/

    // Draw a line
    QGraphicsLineItem * line = mapScene->addLine(player->pos().x() + player->rect().width() / 2, player->pos().y() + player->rect().height() / 2, pt->x(), pt->y());
    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(1);
    line->setPen(pen);
    mapScene->addItem(line);

    delete pt;
    pt = nullptr;
}
int Widget::FixAng(int a) {
    if (a > 359) { a -= 360; }
    if (a < 0) { a += 360; }
    return a;
}
void Widget::renderMap3D()
{
    if(!isGameSceneActive) return;
    wallDrawCount = 0;
    double w = 600 / player->rays.length();
    double distProjPlane = - 300 / tan(60 / 2.f);

    int drawFloorCount = 0;
    // Draw floor
    /*for(int i = 300; i > 0; i-=2){
        int floorW = (600.0 / i * 5) * distProjPlane;
        qDebug() << floorW;
        for(int x1 = 300 - floorW, x2 = 300; x2 < 600; x1-=floorW, x2+=floorW){
            // Draw floor
            QGraphicsPixmapItem * floorRight = new QGraphicsPixmapItem(QPixmap(":/pics/wood.png").copy(0, drawFloorCount, 64, 1).scaled(floorW, 2));
            floorRight->setPos(x2, i);
            gameScene->addItem(floorRight);

            QGraphicsPixmapItem * floorLeft = new QGraphicsPixmapItem(QPixmap(":/pics/wood.png").copy(0, drawFloorCount, 64, 1).scaled(floorW, 2));
            floorLeft->setPos(x1, i);
            gameScene->addItem(floorLeft);

            drawFloorCount++;
            if(drawFloorCount == 64) drawFloorCount = 0;
        }
    }*/

    // Draw walls
    for(int i = 0; i < player->rays.length(); i++){
        Ray * ray = player->rays[i];
        // Fix eye fix
        double h = (600.0 / ray->distance) * distProjPlane;
        // Calculer la distance entre le premier point du mur et le point de contact du rayon
        double distWallPoint = QGraphicsLineItem(ray->wallHit->line().p1().x(), ray->wallHit->line().p1().y(), ray->hitPoint.x(), ray->hitPoint.y()).line().length();
        wallDrawCount = int(distWallPoint) % 64;
        if(wallDrawCount == 64) wallDrawCount = 0;
        QGraphicsPixmapItem * wall = new QGraphicsPixmapItem(QPixmap(":/pics/redbrick.png").copy(wallDrawCount, 0, 1, 64).scaled(2, h));
        wall->setPos(ray->initialAngle* -10 * w + 300, 300 - h / 2);
        gameScene->addItem(wall);
    }

    /*
    // Draw sprites
    for(int i = 0; i < player->rays.length(); i++){

    }*/
}
