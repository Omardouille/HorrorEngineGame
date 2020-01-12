#ifndef POOL_H
#define POOL_H


#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <QVector3D>
#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include "basicio.h"



struct typeMesh {
    unsigned int v;
    unsigned int n;
    unsigned int t;
};

struct MeshRaw {
    std::vector<QVector3D> vertex;
    std::vector<QVector3D> normals;
    std::vector<QVector2D> texCoord;
    std::vector<std::vector<typeMesh>> triangles; // Je pourrais retirer triangles après
    std::vector<unsigned int> indices;
};


struct TextureRaw {
    QOpenGLTexture* texture;
};

struct ShaderRaw {
    QOpenGLShader* shader;
};

// VAO d'un maillage
// On devrait faire program directement
struct ProgramRaw {
    QOpenGLShaderProgram *m_program;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vertexvbo;
    QOpenGLBuffer m_normalvbo;
    QOpenGLBuffer m_texcoordvbo;
    QOpenGLBuffer m_indicevbo;
    GLuint indBuff; 

};



// Classe pool pour eviter de charger plusieurs fois
class Pool
{
public:
    Pool() {}
    //Container* loadMultiplesMesh(std::string filename);

    unsigned int loadMesh(std::string name, MeshRaw meshraw);
    unsigned int loadMesh(std::string name, std::vector<QVector3D> v, std::vector<QVector3D> n, std::vector< std::vector< typeMesh > > tri, std::vector<QVector2D> t);
    unsigned int loadMesh(std::string meshFile);
    MeshRaw & getMeshFromID(unsigned int id);
    std::unordered_map<unsigned int, MeshRaw> & getAllMeshs();
    unsigned int loadShader(QOpenGLShader::ShaderType type, std::string shaderName);
    ShaderRaw & getShaderFromID(unsigned int id);
    unsigned int loadProgram(unsigned int Mesh_ID, unsigned int Texture_ID, unsigned int ShaderV_ID, unsigned int ShaderF_ID);
    ProgramRaw* getProgramFromID(unsigned int id);
    unsigned int loadTexture(std::string textureName);
    TextureRaw & getTextureFromID(unsigned int id);
    /* Pas de delete pour l'instant mais on devrait compter où est utilisé */



    //Container* loadMultObj(std::string filename, std::string vertexShader, std::string fragmentShader);

private:
    unsigned int number = 0;
    std::unordered_map<std::string, unsigned int> strToId;
    std::unordered_map<unsigned int, MeshRaw> meshs;
    std::unordered_map<unsigned int, TextureRaw> textures;
    std::unordered_map<unsigned int, ShaderRaw> shaders;
    std::unordered_map<unsigned int, ProgramRaw*> programs;
 
};







#endif // POOL_H
