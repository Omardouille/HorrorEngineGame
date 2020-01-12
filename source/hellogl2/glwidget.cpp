/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <iostream>
#include <scene.h>
#include "OBJ_Loader.h"
#include "collision.h"

template<class T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi )
{
    assert( !(hi < lo) );
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

bool GLWidget::m_transparent = false;


GLWidget::GLWidget(QWidget *parent, int nFPS)
    : QOpenGLWidget(parent),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      m_program(0),
      FPS(nFPS)
{
    setFocusPolicy(Qt::StrongFocus);
    cursor = QCursor(Qt::BlankCursor);
    this->setCursor(cursor);
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}




void GLWidget::cleanup()
{
    // ON appelle les cleanups des autres

    /*
    if (m_program == nullptr)
        return;
    makeCurrent();
    m_vertexvbo.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();*/
}

// On charge un obj qui en contient plusieurs
Container* loadMultiplesMesh(std::string filename, Pool* poolFiles) {

    Container* bigMeshs = new Container();
    objl::Loader Loader;
    qDebug("On Load File\n");
    bool loadout = Loader.LoadFile(filename);
    qDebug("Finish\n");


    if (loadout)
    {
      //  std::cout << "nombre de mesh : " << Loader.LoadedMeshes.size() << std::endl;
        for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
        {
            //Debug() << i << " - " << Loader.LoadedMeshes.size() << std::endl;
            objl::Mesh curMesh = Loader.LoadedMeshes[i];
            
            /*
            std::unordered_map<std::string, unsigned int> & strToId = poolFiles->strToId;
            if(strToId.find(name) != strToId.end())
                return strToId.at(name);
            */

            std::vector<QVector3D> vertex;
            std::vector<QVector3D> normals;
            std::vector<QVector2D> texCoord;


            for(int j=0;j<curMesh.Vertices.size();j++){
                QVector3D v =  QVector3D(curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z);
                vertex.push_back(v);
                QVector3D n = QVector3D(curMesh.Vertices[j].Normal.X,curMesh.Vertices[j].Normal.Y,curMesh.Vertices[j].Normal.Z);
                normals.push_back(n);
                QVector2D t = QVector2D(curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y);
                texCoord.push_back(t);
            }

            /*
            QVector3D barycentre(0,0,0);
            for(QVector3D vi : vertex) {
                barycentre += vi;
            }
            barycentre /= vertex.size();
            for(QVector3D & vi : vertex) {
                vi -= barycentre;
            }*/
            

            MeshRaw newMesh;
            newMesh.normals = normals;
            newMesh.vertex = vertex;
            newMesh.texCoord = texCoord;
            newMesh.indices = curMesh.Indices;
            Mesh* mesh = new Mesh(filename+curMesh.MeshName,newMesh, poolFiles, "../forest/" + curMesh.MeshMaterial.map_Kd);
            //mesh->transform.translate(barycentre[0], barycentre[1], barycentre[2]);
           // std::cout << "nom texture : " << curMesh.MeshMaterial.map_Kd << std::endl;
            bigMeshs->addChild(mesh);
        }
    }

    return bigMeshs;
}



float min(std::vector<float> v){
    float min = 9999;
    for(float f : v){
        if(f<min)
            min = f;
    }
    return min;
}

float max(std::vector<float> v){
    float max = -9999;
    for(float f : v){
        if(f>max)
            max = f;
    }
    return max;
}



void GLWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);


    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    poolFiles = new Pool();
    root = new Container();

    skybox = new Skybox(poolFiles, "../vertex_shader_skybox.glsl", "../fragment_shader_skybox.glsl");


    parentCamera = new Container();
    parentCamera->transform.rotate(45, 1,0,0);
    parentCamera->transform.y = 0.51;
    root->addChild(parentCamera);


    light1 = new Light(POINT_LIGHT, QVector3D(0.1,0.1,0.1), QVector3D(0.6,0.6,0.6), QVector3D(0.1,0.1,0.1));
    light1->transform.translate(0, 2, 0);
    light1->transform.rotate(135,1,0,0);
    root->addChild(light1);

/*
    light2 = new Light(DIRECTIONAL_LIGHT, QVector3D(0.1,0.1,0.1), QVector3D(0.7,0.7,0.7), QVector3D(0.3,0.3,0.3));
    light2->transform.translate(10, 0, 0);
    light2->transform.rotate(135,1,0,0);
    root->addChild(light2);
*/
    Container* voila = loadMultiplesMesh("../forest/forest.obj",poolFiles);


    root->addChild(voila);



    flashlight = new Mesh("../flashlight.obj", QVector3D(-1,-1,-1), poolFiles, "../flashlight.png");

    systemeflash = new Container();
    systemeflash->addChild(flashlight);

    //systemeflash->addChild(light2);

    systemeflash->transform.scale(0.005);
    systemeflash->transform.rotate(180, 0,0,1);
    systemeflash->transform.rotate(-100, 1,0,0);
    systemeflash->transform.y = -0.08;
    systemeflash->transform.z = -0.22;
    systemeflash->transform.x = 0.1;
    parentCamera->addChild(systemeflash);


    light2 = new Light(SPOT_LIGHT, QVector3D(0.1,0.1,0.1), QVector3D(0.7,0.7,0.7), QVector3D(0.3,0.3,0.3));
        light2->transform.rotate(180, 1,0,0);
    //light2->transform.x = 1;
   // light2->transform.y = 1;
    parentCamera->addChild(light2);

  //  root->addChild(light2);
   
   /* lune = new Mesh("../cube.obj", QVector3D(0.5,0.5,0.5), poolFiles, "../flashlight.png");
    lune->transform.translate(1, 1, 0);
    root->addChild(lune);
    */



    QBasicTimer* timer = new QBasicTimer();
    timer->start( (1./(double)60)*1000., this);
}


// Move camera, on bouge la camera en fonction des touches (on translate la camera)
void GLWidget::moveCamera(int pos) {

   if(pos == 0)
        isUp = true;
    else if(pos == 1)
        isLeft = true;
    else if(pos == 3)
        isRight = true;
    else if(pos == 2) {
        isDown = true;
    } else {
        isUp = false;
        isLeft = false;
        isRight = false;
        isDown = false;

    }

   update();

}


bool checkCollision(Container* parent, QVector3D & pbb, QVector3D & pBB) {
    for(Object* o : parent->children) {
        if(instanceof<Mesh>(o)) {
            QVector3D bb; QVector3D BB;
            dynamic_cast<Mesh*>(o)->getAABB(bb, BB);
            if(dynamic_cast<Mesh*>(o)->isCollider == false)
                continue;

            if( ( ( (bb[0] <= pbb[0]) && (bb[1] <= pbb[1]) && (bb[2] <= pbb[2]) ) && ( (BB[0] >= pBB[0]) && (BB[1] >= pBB[1]) && (BB[2] >= pBB[2]) )  ) 
                || ( ( (bb[0] <= pbb[0]) && (bb[1] <= pbb[1]) && (bb[2] <= pbb[2]) ) && ( (BB[0] >= pBB[0]) && (BB[1] >= pBB[1]) && (BB[2] >= pBB[2]) )  ) ) {
                    return true;
            }

        } else if(instanceof<Container>(o)) {
            if(checkCollision(dynamic_cast<Container*>(o), pbb, pBB))
                return true;
        }
    }

    return false;
}


// Update General
void GLWidget::timerEvent(QTimerEvent *)
{

    // Systeme souris
    float dx = cursor.pos().x() - m_lastPos.x();
    float dy = cursor.pos().y() - m_lastPos.y();

    dx *= 2560/W;
    dy *= 1440/H;
    double sensi = 0.05;


    QQuaternion t = parentCamera->transform.getR();
    QVector3D euler = t.toEulerAngles();

    double pitch = clamp( (euler[0]-sensi*dy), -89., 89.);
    double yaw = euler[1]-sensi*dx;


    QQuaternion e = QQuaternion::fromEulerAngles(pitch, yaw, euler[2]);


    e.normalize();
    parentCamera->transform.setR(e);
    m_lastPos = cursor.pos();

    if(cursor.pos().x() > W-100 || cursor.pos().x() < W+100 || cursor.pos().y() > H-100 || cursor.pos().y() < H+100) {
        cursor.setPos(this->geometry().center());
        m_lastPos = this->geometry().center();
    }



    // Ordre de notre update 
    // On recupère les inputs 
    // On avance

    QVector3D toutdroit(0,0,0.02);
    QVector3D agauche(0.02,0,0);
   // QVector3D toutdroit = camera->transform.getR().rotatedVector(QVector3D(0,0,0.02));
   // QVector3D agauche = camera->transform.getR().rotatedVector(QVector3D(0.02,0,0));

    float y = parentCamera->transform.y;
    float x = parentCamera->transform.x;
    float z = parentCamera->transform.z;
    if(isUp){
        parentCamera->transform.translate(-toutdroit[0], -toutdroit[1], -toutdroit[2]);
    }
    else if(isLeft){
        parentCamera->transform.translate(-agauche[0], -agauche[1], -agauche[2]);
    }
    else if(isRight){
        parentCamera->transform.translate(agauche[0], agauche[1], agauche[2]);
    }
    else if(isDown) {
        parentCamera->transform.translate(toutdroit[0], toutdroit[1], toutdroit[2]);
    }
    parentCamera->transform.y = y; // Le y change pas de base

    // On va check les collisions
    // Et on va reculer dans la direction que l'on est venu sinon
    // On recupère tous les meshs via le root
    std::vector<Mesh*> allMeshs;
    // On recupère le bb du joueur
    QVector3D pbb; QVector3D pBB;
    pbb = parentCamera->getPosition() - QVector3D(0.02f, 0.5f, 0.0f);
    pBB = parentCamera->getPosition() + QVector3D(0.02f, 0.5f, 0.0f);

    if(checkCollision(root, pbb, pBB)) {
        parentCamera->transform.y = y;
        parentCamera->transform.x = x;
        parentCamera->transform.z = z;
    }
    
        

    update();
    
}


// Paint = Rendu, ça se fait a la fin
// Mais si on faisait bien on devrait afficher le rendu d'avant les calculs
void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


    // On pourrait stocker les adresses des lumieres pour les recup direct
    // C'est temporaire
    // On pourrait calculer après directement les positions une fois avec les val nécessaires
    std::vector<Object*> lights;
   // lights.push_back(light1);
    lights.push_back(light2);


    QMatrix4x4 im = parentCamera->getTotalInvertedMatrix();
    QVector3D p = parentCamera->getPosition();

    glDepthMask(GL_FALSE);
    skybox->draw(m_proj, im, p, lights);
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_BLEND);
    root->draw(m_proj, im, p, lights);
    glDisable(GL_BLEND);

}


void GLWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Z)
        moveCamera(0);
    else if(e->key() == Qt::Key_Q)
        moveCamera(1);
    else if(e->key() == Qt::Key_S)
        moveCamera(2);
    else if(e->key() == Qt::Key_D)
        moveCamera(3);
    else if(e->key() == Qt::Key_A)
        moveCamera(4);
    else if(e->key() == Qt::Key_E)
        moveCamera(5);

}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    moveCamera(5);
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100000.0f);
    W = w;
    H = h;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
  //  m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    /*
            cursor.setPos(0,0);

    float dx = event->pos().x() - m_lastPos.x();
    float dy = event->pos().y() - m_lastPos.y();

    dx *= 2560/W;
    dy *= 1440/H;
    double sensi = 0.05;


    QQuaternion t = camera->transform.getR();
    QVector3D euler = t.toEulerAngles();

    double pitch = clamp( (euler[0]-sensi*dy), -89., 89.);
    double yaw = euler[1]-sensi*dx;


    QQuaternion e = QQuaternion::fromEulerAngles(pitch, yaw, euler[2]);


    e.normalize();
    camera->transform.setR(e);


    m_lastPos = event->pos();
    */
}
