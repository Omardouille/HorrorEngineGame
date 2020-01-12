#include "collision.h"


bool Collision::AABBtoAABB(QVector3D tBox1,QVector3D tBox2, float widthX, float height, float widthZ)
{
    float sizeP = 0.02f;//largeur
    float sizeH = 0.5f;//hauteur
    widthX /=2;
    height /=2;
    widthZ /=2;
    return(tBox1.x() + sizeP > tBox2.x() - widthX &&
        tBox1.x() - sizeP < tBox2.x() + widthX &&
        tBox1.y() + sizeH > tBox2.y() - height &&
        tBox1.y() - sizeH < tBox2.y() + height &&
        tBox1.z() + sizeP > tBox2.z() - widthZ &&
        tBox1.z() - sizeP < tBox2.z() + widthZ);
}

bool Collision::detectCollision(QVector3D playerPos,std::unordered_map<unsigned int, QVector3D> world, std::unordered_map<unsigned int, QVector3D> size)
{
    for( const auto& pos : world ) {
        QVector3D s = size.at(pos.first);
        if (AABBtoAABB(playerPos, pos.second,s.x(),s.y(),s.z()))
            return true;
    }
    return false;//no collision with world
}
