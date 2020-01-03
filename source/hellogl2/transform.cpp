#include "transform.h"

Transform::Transform() {
   
}


Transform::~Transform() {

}

// Il doit plus être vraiment valide?
QVector3D Transform::applyToPoint(QVector3D & p) {
    QVector3D newPoint(p); // On fait la copie ici, mais sinon on pourrait supprimer et supprimer le & je pense

   // Pour l'instant on fait en mode osef
    // Echelle Rotation puis translation

    if(s != 1)
        newPoint = newPoint * s;
    if(!r.isIdentity())
        newPoint = r * newPoint;
    if(!t.isNull())
        newPoint = newPoint + t;
       
    return newPoint;
}
Transform Transform::combine_with(Transform & t) {
    Transform newTransform;

/*
    // est ce que les conditions ça optimise vraiment ? par rapport au cout de la multiplication?
    if(!t.m.isIdentity())
        newTransform.m = QMatrix4x4(t.getMatrix()); // Clone = plus rapide que multiplication?

    if(s != 1)
        newTransform.m.scale(s);
    if(!r.isIdentity())
        newTransform.m.rotate(r);
    if(!t.isNull())
        newTransform.m.translate(t);
    
*/
    return newTransform;

}

Transform Transform::inverse() {
    Transform newTransform;

    newTransform.t = QVector3D(0,0,0)-t;
    newTransform.r = r.inverted();
    newTransform.s = 1/s;

    return newTransform;
}

