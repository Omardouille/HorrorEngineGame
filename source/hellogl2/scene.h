#ifndef SCENE_H
#define SCENE_H


#include "pool.h"
#include <vector>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QBasicTimer>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include "transform.h"



class Object {
public :
    Object();

    virtual void draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) = 0;
   /* virtual void update() = 0;
    virtual void start() = 0;
*/

    QMatrix4x4 getTotalMatrix() {
        QMatrix4x4 m_world;
        Object* nav = this;
        while(nav != nullptr) {
            m_world = nav->transform.getMatrix()*m_world;
            nav = nav->parent;
        }

        return m_world;
    }
    QMatrix4x4 getTotalInvertedMatrix() {
        return getTotalMatrix().inverted();
    }
    QVector3D getPosition() {
        // Pour l'instant on suppose qu'on a pas choisi de position centrale
        QVector3D p(0,0,0);
        return this->getTotalMatrix()*p;
    }

    void addParent(Object* o) {
        parent = o;
    }


//protected :
    Object* parent = nullptr;
    Transform transform;


};

class Container : public Object {
public :
    Container();
    ~Container();

    void draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) override;


    void addChild(Object* o);

    std::vector<Object*> children;


};



class Mesh : public Object {
public:
    Mesh();
    Mesh(std::string name, std::vector<QVector3D> v, std::vector<QVector3D> n, std::vector< std::vector< typeMesh > > tri, std::vector<QVector2D> t, QVector3D color, Pool* poolFiles, std::string textureFile, std::string vertexShader, std::string fragmentShader);
    Mesh(std::string meshFile, QVector3D color, Pool* poolFiles, std::string textureFile = "../skybox.png", std::string vertexShader = "../vertex_shader.glsl", std::string fragmentShader = "../fragment_shader.glsl");
    Mesh(std::string name, MeshRaw meshraw,  Pool* poolFiles, std::string textureFile, std::string vertexShader = "../vertex_shader.glsl", std::string fragmentShader = "../fragment_shader.glsl");
    ~Mesh();


    void draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) override;

    unsigned int getMeshID(){
        return Mesh_ID;
    }

    void getAABB(QVector3D & bb, QVector3D & BB);

    bool isCollider = true;
    bool isDraw = true;


protected:
    unsigned int Mesh_ID;
    unsigned int Texture_ID;
    unsigned int ShaderF_ID;
    unsigned int ShaderV_ID;
    unsigned int Program_ID;
    QVector3D color;
    Pool* poolFiles;


};

class Pile : public Mesh {
public:
    Pile() {}
    Pile(int type, Pool* poolFiles) : Mesh("../pile.obj", QVector3D(-1,-1,-1), poolFiles, "../textureduracell.png") {
        this->type = type;
    }
    ~Pile() {}

    int getType() {
        return type;
    }
    int type = 0;    

};

class Ennemi : public Mesh {
public:
    Ennemi() {}
    Ennemi(std::vector<QVector3D> ps, std::vector<QQuaternion> rs, int type, Pool* poolFiles) : Mesh("../forest/eye.obj", QVector3D(-1,-1,-1), poolFiles, "../forest/eye-texture.jpg") {
        this->ps = ps;
        this->rs = rs;        
        this->notInit = true;
        this->type = type;

    }
    ~Ennemi() {}

    QVector3D getDirection() {
        QVector3D m_world = QVector3D(0,0,1);
        Object* nav = this;
        while(nav != nullptr) {
            m_world = nav->transform.getR().rotatedVector(m_world);
            nav = nav->parent;
        }
        return m_world;
    }

    void death() {
        transform.y += 1000;
        dead = true;
    }

    void nextStep(QVector3D pos) {
        if(dead)
            return;

        if(notInit) {
            notInit = false;
            transform.scale(0.5);
            transform.x = ps[posnumber][0];
            transform.y = ps[posnumber][1];
            transform.z = ps[posnumber][2];
            posnumber = 1;
            if(type == 0) {
                Texture_ID = poolFiles->loadTexture("../forest/eye1.png");
            } else if(type == 1) {
                Texture_ID = poolFiles->loadTexture("../forest/eye2.png");
            } else if(type == 2) {
                Texture_ID = poolFiles->loadTexture("../forest/eye3.png");
            }
           // isCollider = false;
            
            vDir = (getPosition()-ps[posnumber]);
        }

        double lengthWithPlayer = (getPosition()-pos).length();

        if(lengthWithPlayer <= distanceBeforeAttackingPlayer) {
            QVector3D direct = (getPosition()-pos).normalized();
            direct *= speed;
            transform.x -= direct[0];
            transform.y -= direct[1];
            transform.z -= direct[2];

            float angle = atan2(direct[2], direct[0]);
            QQuaternion q = QQuaternion::fromAxisAndAngle(0, 1, 0, 180-angle/3.1415926535*180);
            transform.setR(q);

        } else {
            double length = (getPosition()-ps[posnumber]).length();
            if(length <= 0.09) { // marge erreur
                transform.x = ps[posnumber][0];
                transform.y = ps[posnumber][1];
                transform.z = ps[posnumber][2];
                if(!retour)
                    posnumber++;
                else
                    posnumber--;
                if(posnumber < 0) {
                    posnumber = 1;
                    retour = false;
                }
                if(posnumber >= ps.size()) {
                    posnumber = ps.size()-2;
                    retour = true;
                }
                vDir = (getPosition()-ps[posnumber]);
                
            }

            
            vDir.normalize();
            vDir *= speed;
            float angle = atan2(vDir[2], vDir[0]);
            QQuaternion q = QQuaternion::fromAxisAndAngle(0, 1, 0, 180-angle/3.1415926535*180);
            transform.setR(q);
            transform.x -= vDir[0];
            transform.y -= vDir[1];
            transform.z -= vDir[2];
        }


    }

    QVector3D vDir;
    int type = 0;
    bool dead = false;
    bool notInit = false;
    bool retour = false;
    float distanceBeforeAttackingPlayer = 8;
    float speed = 0.05;
    int posnumber = 0;
    std::vector<QVector3D> ps;
    std::vector<QQuaternion> rs;

};

class Skybox : public Mesh {
public:
    Skybox();
    Skybox(Pool* poolFiles, std::string vertexShader = "../vertex_shader.glsl", std::string fragmentShader = "../fragment_shader.glsl");
    ~Skybox();

    void draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) override;

protected:
    QOpenGLTexture* texture; // Pas en pointer pour test
    QOpenGLShaderProgram m_program;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vertex;

};

class UI : public Object {
public:
    UI();
    UI(Pool* poolFiles, std::string vertexShader = "../vertex_shader.glsl", std::string fragmentShader = "../fragment_shader.glsl");
    ~UI();

    void draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) override;

protected:
    unsigned int Texture_ID;
    unsigned int ShaderF_ID;
    unsigned int ShaderV_ID;
    unsigned int Program_ID;
        QVector3D color;
    Pool* poolFiles;
    QOpenGLTexture* texture; // Pas en pointer pour test
    QOpenGLShaderProgram m_program;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vertex;

};

class Plan : public Mesh {
public:
    Plan(float s, float decoupe, QVector3D color);
};


class Camera : public Object {
public:
    Camera();
    ~Camera() ;

    void draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) override;


};

enum LType { DIRECTIONAL_LIGHT, POINT_LIGHT, SPOT_LIGHT };

class Light : public Object {
public:
    Light(LType t, QVector3D a, QVector3D d, QVector3D s);
    ~Light();

    void draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) override;

    // On pourrait le mettre au truc davant
    QVector3D getDirection() {
        QVector3D m_world = QVector3D(0,0,1);
        Object* nav = this;
        while(nav != nullptr) {
            m_world = nav->transform.getR().rotatedVector(m_world);
            nav = nav->parent;
        }
        return m_world;
        //return this->transform.getR().rotatedVector(QVector3D(0,0,1));
    }

    LType t;
    QVector3D ambient;
    QVector3D diffuse;
    QVector3D specular;

};

class FlashLight : public Light {
public:
    FlashLight() : Light(SPOT_LIGHT, QVector3D(0.1,0.1,0.1), QVector3D(0.6,0.6,0.6), QVector3D(0.1,0.1,0.1)) {
        ambient = ambients[currentLight];
        diffuse = diffuses[currentLight];
        specular = speculars[currentLight];

    }
    ~FlashLight() {}

    void changeLight(int i) {
        currentLight = i;
        ambient = ambients[currentLight];
        diffuse = diffuses[currentLight];
        specular = speculars[currentLight];
    }

    int currentLight = 0;
    int nbLights = 4;
    QVector3D ambients[4] = { QVector3D(0.1, 0.1, 0.1),  QVector3D(0.1, 0.1, 0.1), QVector3D(0.1, 0.1, 0.1), QVector3D(0.1, 0.1, 0.1)};
    QVector3D diffuses[4] = { QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 1.0),  QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0)};
    QVector3D speculars[4] = { QVector3D(0.1, 0.1, 0.1),  QVector3D(0.1, 0.1, 0.1), QVector3D(0.1, 0.1, 0.1), QVector3D(0.1, 0.1, 0.1)};

};


#endif // SCENE_H
