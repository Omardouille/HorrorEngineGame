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

        /*
        QMatrix4x4 m_world;
        Object* nav = this;
        while(nav != nullptr) {
            m_world = m_world*nav->transform.inverse().getMatrix();
            nav = nav->parent;
        }
        */


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


protected:
    unsigned int Mesh_ID;
    unsigned int Texture_ID;
    unsigned int ShaderF_ID;
    unsigned int ShaderV_ID;
    unsigned int Program_ID;
    QVector3D color;
    Pool* poolFiles;


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
        return this->transform.getR().rotatedVector(QVector3D(0,0,1));
    }

    LType t;
    QVector3D ambient;
    QVector3D diffuse;
    QVector3D specular;

};


#endif // SCENE_H
