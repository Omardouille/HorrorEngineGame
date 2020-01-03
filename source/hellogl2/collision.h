#ifndef COLLISION_H
#define COLLISION_H

#include <QVector3D>
#include <unordered_map>
class Collision
{
public:
    bool detectCollision(QVector3D playerPos, std::unordered_map<unsigned int, QVector3D> world, std::unordered_map<unsigned int, QVector3D> size);
    bool AABBtoAABB(QVector3D tBox1, QVector3D tBox2, float widthX, float height, float widthZ);
};

#endif // COLLISION_H
