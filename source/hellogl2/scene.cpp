#pragma comment( lib, "OpenGL32.lib" )

#include "scene.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "basicio.h"
#include <QCoreApplication>
#include <QOpenGLTexture>
#include <unordered_set>



Object::Object() {
}

Container::Container() {
}


Container::~Container() {

    /*
    Essayer de créer / detruire pour voir si fuite ram
    */

   // On call le pool pour dire qu'on utilise plus un truc

   // On détruit les enfants aussi du coup?
   /*
    for(Object* o : children)
        delete o;
    */
}


void Container::draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) {
    for(Object* o : children)
        o->draw(m_proj, m_camera, posCamera, lights);
}

void Container::addChild(Object* o) {
    // Si déjà enfant de qqn il peut pas être un nouvel enfant => o nthrow
    children.push_back(o);
    o->addParent(this);
}

Mesh::Mesh() : Object() {
}


Mesh::Mesh(std::string meshFile, QVector3D color, Pool* poolFiles, std::string textureFile, std::string vertexShader, std::string fragmentShader) : Object() {

    Mesh_ID = poolFiles->loadMesh(meshFile);
    ShaderV_ID = poolFiles->loadShader(QOpenGLShader::Vertex, vertexShader);
    ShaderF_ID = poolFiles->loadShader(QOpenGLShader::Fragment, fragmentShader);
    Texture_ID = poolFiles->loadTexture(textureFile);
    Program_ID = poolFiles->loadProgram(Mesh_ID, -1, ShaderV_ID, ShaderF_ID);

    this->poolFiles = poolFiles;

    this->color = color;

}

Mesh::Mesh(std::string name, std::vector<QVector3D> v, std::vector<QVector3D> n, std::vector< std::vector< typeMesh > > tri, std::vector<QVector2D> t, QVector3D color, Pool* poolFiles, std::string textureFile, std::string vertexShader, std::string fragmentShader) : Object() {
    Mesh_ID = poolFiles->loadMesh(name, v, n, tri, t);
    ShaderV_ID = poolFiles->loadShader(QOpenGLShader::Vertex, vertexShader);
    ShaderF_ID = poolFiles->loadShader(QOpenGLShader::Fragment, fragmentShader);
    Texture_ID = poolFiles->loadTexture(textureFile);
    Program_ID = poolFiles->loadProgram(Mesh_ID, -1, ShaderV_ID, ShaderF_ID);

    this->poolFiles = poolFiles;

    this->color = color;

}

Mesh::Mesh(std::string name, MeshRaw meshraw,  Pool* poolFiles, std::string textureFile, std::string vertexShader , std::string fragmentShader){
    Mesh_ID = poolFiles->loadMesh(name, meshraw);
    ShaderV_ID = poolFiles->loadShader(QOpenGLShader::Vertex, vertexShader);
    ShaderF_ID = poolFiles->loadShader(QOpenGLShader::Fragment, fragmentShader);
    Texture_ID = poolFiles->loadTexture(textureFile);
    Program_ID = poolFiles->loadProgram(Mesh_ID, -1, ShaderV_ID, ShaderF_ID);

    this->poolFiles = poolFiles;

    this->color = QVector3D(-1,-1,-1);
}


Mesh::~Mesh() {
    // On appelle un destroy du pool, que si il est a 0, alors ça detruit
}

void Mesh::getAABB(QVector3D & bb, QVector3D & BB) {

    QVector3D tmpbb(9999,9999,9999);
    QVector3D tmpBB(-9999,-9999,-9999);

    QMatrix4x4 tot = getTotalMatrix();
    for(QVector3D v : poolFiles->getMeshFromID(Mesh_ID).vertex) {
        QVector3D tmpV = tot*v;

        if(tmpV[0] < tmpbb[0])
            tmpbb[0] = tmpV[0];
        if(tmpV[0] > tmpBB[0])
            tmpBB[0] = tmpV[0];
        if(tmpV[1] < tmpbb[1])
            tmpbb[1] = tmpV[1];
        if(tmpV[1] > tmpBB[1])
            tmpBB[1] = tmpV[1];
        if(tmpV[2] < tmpbb[2])
            tmpbb[2] = tmpV[2];
        if(tmpV[2] > tmpBB[2])
            tmpBB[2] = tmpV[2];
    }

    bb = tmpbb;
    BB = tmpBB;

}


void Mesh::draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) {
    

    // Optimiser le draw en gardant en compte le transform des parents dans les paramères du draw

    QMatrix4x4 m_world = this->getTotalMatrix();

    ProgramRaw* p = poolFiles->getProgramFromID(Program_ID);
    MeshRaw & m = poolFiles->getMeshFromID(Mesh_ID);


    

    // Pour draw
    QOpenGLVertexArrayObject::Binder vaoBinder(&(p->m_vao));

    p->m_program->bind();
    p->m_program->setUniformValue("material.color", color);
    p->m_program->setUniformValue("material.shininess", 32.f);
    p->m_program->setUniformValue("projMatrix", m_proj);
    p->m_program->setUniformValue("mvMatrix", m_camera * m_world);
    p->m_program->setUniformValue("mMatrix", m_world);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    p->m_program->setUniformValue("normalMatrix", normalMatrix);
    p->m_program->setUniformValue("cameraPos", posCamera);
    p->m_program->setUniformValue("nb", (unsigned short)lights.size());
    for(int i = 0; i < lights.size(); i++) {
        char buffer[64];

        Light* tmpLight = (Light *)lights[i];

        sprintf(buffer, "lights[%i].type", i);
        p->m_program->setUniformValue(buffer, tmpLight->t);
        sprintf(buffer, "lights[%i].direction", i);
        p->m_program->setUniformValue(buffer, tmpLight->getDirection());
        sprintf(buffer, "lights[%i].position", i);
        p->m_program->setUniformValue(buffer, tmpLight->getPosition());
        sprintf(buffer, "lights[%i].cutoff", i);
        p->m_program->setUniformValue(buffer, (GLfloat)cos(12.5f/3.14159526535f*180.f));
        sprintf(buffer, "lights[%i].ambient", i);
        p->m_program->setUniformValue(buffer, tmpLight->ambient);
        sprintf(buffer, "lights[%i].diffuse", i);
        p->m_program->setUniformValue(buffer, tmpLight->diffuse);
        sprintf(buffer, "lights[%i].specular", i);
        p->m_program->setUniformValue(buffer, tmpLight->specular);
        sprintf(buffer, "lights[%i].constant", i);
        p->m_program->setUniformValue(buffer, 1.0f);
        sprintf(buffer, "lights[%i].linear", i);
        p->m_program->setUniformValue(buffer, 0.090f);
        sprintf(buffer, "lights[%i].quadratic", i);
        p->m_program->setUniformValue(buffer, 0.032f);
    }

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    

    if(Texture_ID != -1) {
        TextureRaw & tex = poolFiles->getTextureFromID(Texture_ID);
        tex.texture->bind();
    }

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->indBuff);
    f->glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, nullptr);


    p->m_program->release();



   

   
}

Skybox::Skybox() : Mesh() {
}


Skybox::Skybox(Pool* poolFiles, std::string vertexShader, std::string fragmentShader) {
//https://learnopengl.com/Advanced-OpenGL/Cubemaps
//https://amin-ahmadi.com/2019/07/28/creating-a-skybox-using-c-qt-and-opengl/

    // On charge les textures

    const QImage posx = QImage("../nightsky1_left.png").convertToFormat(QImage::Format_RGBA8888); // r
    const QImage negx = QImage("../nightsky1_right.png").convertToFormat(QImage::Format_RGBA8888); // l

    const QImage posy = QImage("../nightsky1_up.png").convertToFormat(QImage::Format_RGBA8888); // t
    const QImage negy = QImage("../nightsky1_down.png").convertToFormat(QImage::Format_RGBA8888); // b

    const QImage posz = QImage("../nightsky1_front.png").convertToFormat(QImage::Format_RGBA8888); // front
    const QImage negz = QImage("../nightsky1_back.png").convertToFormat(QImage::Format_RGBA8888); // Bac


    // On crée une texture
    texture = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
    texture->create();
    // On fait la texture 3D
    texture->setSize(posx.width(), posx.height(), posx.depth());
    texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    texture->allocateStorage();

    texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveX,
                     QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     posx.constBits(), Q_NULLPTR);

    texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveY,
                     QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     posy.constBits(), Q_NULLPTR);

    texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ,
                     QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     posz.constBits(), Q_NULLPTR);

    texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeX,
                     QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     negx.constBits(), Q_NULLPTR);

    texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeY,
                     QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     negy.constBits(), Q_NULLPTR);

    texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ,
                     QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                     negz.constBits(), Q_NULLPTR);

    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);

    ShaderV_ID = poolFiles->loadShader(QOpenGLShader::Vertex, vertexShader);
    ShaderF_ID = poolFiles->loadShader(QOpenGLShader::Fragment, fragmentShader);

    QVector3D vertices[] =
    {
        {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {+1.0f, -1.0f, -1.0f},
        {+1.0f, -1.0f, -1.0f},
        {+1.0f, +1.0f, -1.0f},
        {-1.0f, +1.0f, -1.0f},

        {-1.0f, -1.0f, +1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, +1.0f, -1.0f},
        {-1.0f, +1.0f, -1.0f},
        {-1.0f, +1.0f, +1.0f},
        {-1.0f, -1.0f, +1.0f},

        {+1.0f, -1.0f, -1.0f},
        {+1.0f, -1.0f, +1.0f},
        {+1.0f, +1.0f, +1.0f},
        {+1.0f, +1.0f, +1.0f},
        {+1.0f, +1.0f, -1.0f},
        {+1.0f, -1.0f, -1.0f},

        {-1.0f, -1.0f, +1.0f},
        {-1.0f, +1.0f, +1.0f},
        {+1.0f, +1.0f, +1.0f},
        {+1.0f, +1.0f, +1.0f},
        {+1.0f, -1.0f, +1.0f},
        {-1.0f, -1.0f, +1.0f},

        {-1.0f, +1.0f, -1.0f},
        {+1.0f, +1.0f, -1.0f},
        {+1.0f, +1.0f, +1.0f},
        {+1.0f, +1.0f, +1.0f},
        {-1.0f, +1.0f, +1.0f},
        {-1.0f, +1.0f, -1.0f},

        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, +1.0f},
        {+1.0f, -1.0f, -1.0f},
        {+1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, +1.0f},
        {+1.0f, -1.0f, +1.0f}
    };

    m_program.addShader(poolFiles->getShaderFromID(ShaderV_ID).shader);
    m_program.addShader(poolFiles->getShaderFromID(ShaderF_ID).shader);
    m_program.bindAttributeLocation("vertex", 0);
    m_program.bindAttributeLocation("normal", 1);
    m_program.bindAttributeLocation("texcoord", 2);

    m_program.link();
    m_program.bind();

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_vertex.create();
    m_vertex.bind();
    m_vertex.allocate(vertices, 36 * 3 * sizeof(GL_FLOAT));
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), 0);
    m_vertex.release();
    
    m_program.setUniformValue("cameraPos", QVector3D(0, 0, 0));
    m_program.setUniformValue("texture_diffuse", 0);
    m_program.release();

    this->poolFiles = poolFiles;

    this->color = color;

}



Skybox::~Skybox() {
}


void Skybox::draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) {


    // Optimiser le draw en gardant en compte le transform des parents dans les paramères du draw

    QMatrix4x4 m_world = this->getTotalMatrix();

    QMatrix4x4 mvMatrix = QMatrix4x4(m_camera * m_world);
    mvMatrix(0,3) = 0;
    mvMatrix(1,3) = 0;
    mvMatrix(2,3) = 0;


    QMatrix4x4 mWorld = QMatrix4x4(m_world);
    mvMatrix(0,3) = 0;
    mvMatrix(1,3) = 0;
    mvMatrix(2,3) = 0;


    // Pour draw
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_program.bind();
    m_program.setUniformValue("projMatrix", m_proj);
    m_program.setUniformValue("mvMatrix", mvMatrix);
    m_program.setUniformValue("mMatrix", mWorld);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    m_program.setUniformValue("normalMatrix", normalMatrix);
    m_program.setUniformValue("cameraPos", posCamera);

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    texture->bind();
    /*
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->indBuff);
    f->glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, nullptr);
    */

    // La un draw array suffit pour le test après on refera mieux avec drawelements
    f->glDrawArrays(GL_TRIANGLES,0,36);
    m_program.release();






}


Plan::Plan(float s, float decoupe, QVector3D color) : Mesh() {

    /*
    std::vector<QVector3D> vertex;
    std::vector<QVector3D> normals;
    std::vector<std::vector<std::pair<unsigned int,unsigned int>>> triangles;
    const GLfloat siz = decoupe;
    GLfloat inter =  s/siz;

    QImage img = QImage("../heightmap->png");



     GLfloat x0 = 0.f;
     GLfloat y0 = 0.f;
     GLfloat x1 = inter;
     GLfloat y1 = 0.f;
     GLfloat x2 =  inter;
     GLfloat y2 =  inter;
     GLfloat x3 = 0.f;
     GLfloat y3 =  inter;

     for(int i = 0; i < siz; i++) {
        for(int j = 0; j < siz; j++) {
               // Point 1
                GLfloat x0n = x0+j*inter;
                GLfloat y0n = y0+i*inter;
                QRgb rgb0 = img.pixel((x0n/s)*(img.width()-1), (y0n/s)*(img.height()-1));
              //  GLfloat z0n = (1.0-qRed(rgb0)/255. )/2.*s;
                GLfloat z0n = 0;

                vertex.push_back(QVector3D(x0n, y0n, z0n));
                normals.push_back(QVector3D(0,0,-1));

                // Point 2
                GLfloat x1n = x1+j*inter;
                GLfloat y1n = y1+i*inter;
                QRgb rgb1 = img.pixel((x1n/s)*(img.width()-1), (y1n/s)*(img.height()-1));
             //   GLfloat z1n = (1.0-qRed(rgb1)/255. )/2.*s;
                               GLfloat z1n = 0;

                vertex.push_back(QVector3D(x1n, y1n, z1n));
                normals.push_back(QVector3D(0,0,-1));

                // Point 3
                GLfloat x2n = x2+j*inter;
                GLfloat y2n = y2+i*inter;
                QRgb rgb2 = img.pixel((x2n/s)*(img.width()-1), (y2n/s)*(img.height()-1));
              //  GLfloat z2n =(1.0- qRed(rgb2)/255. )/2.*s;
                              GLfloat z2n = 0;

                vertex.push_back(QVector3D(x2n, y2n, z2n));
                normals.push_back(QVector3D(0,0,-1));

                // Point 4
                GLfloat x3n = x3+j*inter;
                GLfloat y3n = y3+i*inter;
                QRgb rgb3 = img.pixel((x3n/s)*(img.width()-1), (y3n/s)*(img.height()-1));
              //  GLfloat z3n =(1.0- qRed(rgb3)/255. )/2.*s;
                GLfloat z3n = 0;
                vertex.push_back(QVector3D(x3n, y3n, z3n));
                normals.push_back(QVector3D(0,0,-1));

                std::vector<std::pair<unsigned int,unsigned int>> triangle1;
                std::vector<std::pair<unsigned int,unsigned int>> triangle2;

                triangle1.push_back(std::pair<unsigned int,unsigned int>(vertex.size()-4, normals.size()-4));
                triangle1.push_back(std::pair<unsigned int,unsigned int>(vertex.size()-1, normals.size()-1));
                triangle1.push_back(std::pair<unsigned int,unsigned int>(vertex.size()-3, normals.size()-3));

                triangle2.push_back(std::pair<unsigned int,unsigned int>(vertex.size()-2, normals.size()-2));
                triangle2.push_back(std::pair<unsigned int,unsigned int>(vertex.size()-3, normals.size()-3));
                triangle2.push_back(std::pair<unsigned int,unsigned int>(vertex.size()-1, normals.size()-1));

                triangles.push_back(triangle1);
                triangles.push_back(triangle2);


        }
     }

     this->vertex = vertex;
     this->normals = normals;
     this->triangles = triangles;
    */


}

Camera::Camera() : Object() {

}

Camera::~Camera() {

}

void Camera::draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) {
}




Light::Light(LType t, QVector3D a, QVector3D d, QVector3D s) : Object() {
    this->t = t;
    this->ambient = a;
    this->diffuse = d;
    this->specular = s;

}

Light::~Light() {

}

void Light::draw(QMatrix4x4 & m_proj, QMatrix4x4 & m_camera, QVector3D & posCamera, std::vector<Object*> & lights) {
}






