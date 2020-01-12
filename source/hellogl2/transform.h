#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QMatrix4x4>
#include <QCoreApplication>

class Transform {
private:
    QQuaternion r;
    QVector3D t;
    float s = 1; // Un Qvector pour le scale aussi?

    // Ca aurait été cool de faire le scale après la translation genre comme dans un Editeur 3D, on peut scale sans que ça bouge la pos


public:
    Transform();
    ~Transform();

    // Temporairement ici
    QVector3D pivot;


    QVector3D applyToPoint(QVector3D & p);
    // Pour l'instant j'ai pas besoin de apply to vector
   // QVector3D applyToVector(QVector3D v); // Translation = inutile 
    // Versor


    Transform combine_with(Transform & t);
    Transform inverse();


    // On additionne et multiplie pour ces fct là
    void translate(float x, float y, float z) {
        QVector3D v(x, y, z);
        v = r * v;
        t += v;

    }
    void rotate(float a, float x, float y, float z) {
        r *= QQuaternion::fromAxisAndAngle(x,y,z,a);

    }

    // On pourrait faire le scale sur x y z après
    void scale(float s) {
        this->s *= s;
    }

    // Je fais des setters et getters dans les cas où on veut directement modifier les valeurs
    // Après on aurait pu mettre en publique les composantes, mais bon comme ça on maitrise ce que fait le dev

    // Fonction Getter
    const QVector3D& getT() const {
        return t;
    }
    const QQuaternion& getR() const {
        return r;
    }
    const float& getS() const {
        return s;
    }
    // On retourne une matrice 4x4 issu d
    const QMatrix4x4 getMatrix() const {
        QMatrix4x4 matrixResult = QMatrix4x4(); 

        if(s != 1)
            matrixResult.scale(s);
        /*bool isPivot = !pivot.isNull();
        if(isPivot)
            matrixResult.translate(-pivot);
        */if(!r.isIdentity())
            matrixResult.rotate(r);
        /*if(isPivot)
            matrixResult.translate(pivot);
        */if(!t.isNull())
            matrixResult.setColumn(3,QVector4D(t,1));

        return matrixResult;
    }

    // Fonction setter (si on veut mettre des valeurs directement)
    void setT(QVector3D t) { // On fait copie et on stocke
        this->t = t;
    }
    void setR(QQuaternion r) { // On fait copie et on stocke
        this->r = r;
    }
    void setS(float s) { // On fait copie et on stocke
        this->s = s;
    }


    // En fait pour avoir le getPosition, on multiplie les matrices par le point du barycentre initial

    // La translation correspondant aux positions, on peut directement accéder aux positions
    // Là j'ai pas envie de le mettre en setter / getter
    float & x = t[0];
    float & y = t[1];
    float & z = t[2];




    // Pour l'instant j'utilise pas interpolate
    // Transform interpolate_with(Transform & t, float k);

};

#endif // TRANSFORM_H
