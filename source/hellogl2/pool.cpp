#include "pool.h"


unsigned int Pool::loadMesh(std::string name, MeshRaw & meshraw) {
    if(strToId.find(name) != strToId.end())
        return strToId.at(name);

    strToId.insert({name, number});
    meshs.insert({number, meshraw});


    return number++;
}


unsigned int Pool::loadMesh(std::string name, std::vector<QVector3D> v, std::vector<QVector3D> n, std::vector< std::vector< typeMesh > > tri, std::vector<QVector2D> t) {
    if(strToId.find(name) != strToId.end())
        return strToId.at(name);

    MeshRaw newMesh;

    newMesh.vertex = v;
    newMesh.triangles = tri;
    newMesh.normals = n;
    newMesh.texCoord = t;


/*        QVector3D barycentre(0,0,0);
    for(QVector3D vi : newMesh.vertex) {
        barycentre += vi;
    }
    barycentre /= newMesh.vertex.size();
    for(QVector3D & vi : newMesh.vertex) {
        vi -= barycentre;
    }
*/
    std::vector<QVector3D> vertex2;
    std::vector<QVector3D> normals2;
    std::vector<QVector2D> texCoord2;



    // indices
    for(int i = 0; i < newMesh.triangles.size(); i++) {

        int ind1 = vertex2.size();
        int ind2 = vertex2.size();
        int ind3 = vertex2.size();

        // Et essayer utiliser flag de bit pour apprendre
        bool isAlreadyExist = false;
        bool isAlreadyExist1 = false;
        bool isAlreadyExist2 = false;
        for(int j = 0; j < vertex2.size(); j++) {
            if(newMesh.texCoord.size() > 0) {
                if(newMesh.vertex[newMesh.triangles[i][0].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][0].n] == normals2[j] && newMesh.texCoord[newMesh.triangles[i][0].t] == texCoord2[j]) {
                    isAlreadyExist = true;
                    ind1 = j;
                }
                if(newMesh.vertex[newMesh.triangles[i][1].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][1].n] == normals2[j] && newMesh.texCoord[newMesh.triangles[i][1].t] == texCoord2[j]) {
                    isAlreadyExist1 = true;
                    ind2 = j;
                }
                if(newMesh.vertex[newMesh.triangles[i][2].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][2].n] == normals2[j] && newMesh.texCoord[newMesh.triangles[i][2].t] == texCoord2[j]) {
                    isAlreadyExist2 = true;
                    ind3 = j;
                }
            } else {
                if(newMesh.vertex[newMesh.triangles[i][0].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][0].n] == normals2[j] ) {
                    isAlreadyExist = true;
                    ind1 = j;
                }
                if(newMesh.vertex[newMesh.triangles[i][1].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][1].n] == normals2[j] ) {
                    isAlreadyExist1 = true;
                    ind2 = j;
                }
                if(newMesh.vertex[newMesh.triangles[i][2].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][2].n] == normals2[j] ) {
                    isAlreadyExist2 = true;
                    ind3 = j;
                }
            }
            if(isAlreadyExist&&isAlreadyExist1&&isAlreadyExist2)
                break;
        }
        if(!isAlreadyExist) {
            vertex2.push_back(newMesh.vertex[newMesh.triangles[i][0].v]);
            normals2.push_back(newMesh.normals[newMesh.triangles[i][0].n]);
            if(newMesh.texCoord.size() > 0)
                texCoord2.push_back(newMesh.texCoord[newMesh.triangles[i][0].t]);
            ind1 = vertex2.size()-1;
        }
        if(!isAlreadyExist1) {
            vertex2.push_back(newMesh.vertex[newMesh.triangles[i][1].v]);
            normals2.push_back(newMesh.normals[newMesh.triangles[i][1].n]);
            if(newMesh.texCoord.size() > 0)
                texCoord2.push_back(newMesh.texCoord[newMesh.triangles[i][1].t]);
            ind2 = vertex2.size()-1;
        }
        if(!isAlreadyExist2) {
            vertex2.push_back(newMesh.vertex[newMesh.triangles[i][2].v]);
            normals2.push_back(newMesh.normals[newMesh.triangles[i][2].n]);
            if(newMesh.texCoord.size() > 0)
                texCoord2.push_back(newMesh.texCoord[newMesh.triangles[i][2].t]);
            ind3 = vertex2.size()-1;
        }
        newMesh.indices.push_back(ind1);
        newMesh.indices.push_back(ind2);
        newMesh.indices.push_back(ind3);
    }


    newMesh.normals = normals2;
    newMesh.vertex = vertex2;
    newMesh.texCoord = texCoord2;




    strToId.insert({name, number});
    meshs.insert({number, newMesh});


    return number++;
}

unsigned int Pool::loadMesh(std::string meshFile) {
    // Si il existe déjà on le recharge pas et on redonne le int
    if(strToId.find(meshFile) != strToId.end())
        return strToId.at(meshFile);

    MeshRaw newMesh;


    OBJIO::open<QVector3D, QVector2D, unsigned int, typeMesh>(meshFile, newMesh.vertex, newMesh.triangles, newMesh.normals, newMesh.texCoord);


    QVector3D barycentre(0,0,0);
    for(QVector3D vi : newMesh.vertex) {
        barycentre += vi;
    }
    barycentre /= newMesh.vertex.size();
    for(QVector3D & vi : newMesh.vertex) {
        vi -= barycentre;
    }

    std::vector<QVector3D> vertex2;
    std::vector<QVector3D> normals2;
    std::vector<QVector2D> texCoord2;



    // indices
    for(int i = 0; i < newMesh.triangles.size(); i++) {

        int ind1 = vertex2.size();
        int ind2 = vertex2.size();
        int ind3 = vertex2.size();

        // Et essayer utiliser flag de bit pour apprendre
        bool isAlreadyExist = false;
        bool isAlreadyExist1 = false;
        bool isAlreadyExist2 = false;
        for(int j = 0; j < vertex2.size(); j++) {
            if(newMesh.texCoord.size() > 0) {
                if(newMesh.vertex[newMesh.triangles[i][0].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][0].n] == normals2[j] && newMesh.texCoord[newMesh.triangles[i][0].t] == texCoord2[j]) {
                    isAlreadyExist = true;
                    ind1 = j;
                }
                if(newMesh.vertex[newMesh.triangles[i][1].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][1].n] == normals2[j] && newMesh.texCoord[newMesh.triangles[i][1].t] == texCoord2[j]) {
                    isAlreadyExist1 = true;
                    ind2 = j;
                }
                if(newMesh.vertex[newMesh.triangles[i][2].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][2].n] == normals2[j] && newMesh.texCoord[newMesh.triangles[i][2].t] == texCoord2[j]) {
                    isAlreadyExist2 = true;
                    ind3 = j;
                }
            } else {
                if(newMesh.vertex[newMesh.triangles[i][0].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][0].n] == normals2[j] ) {
                    isAlreadyExist = true;
                    ind1 = j;
                }
                if(newMesh.vertex[newMesh.triangles[i][1].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][1].n] == normals2[j] ) {
                    isAlreadyExist1 = true;
                    ind2 = j;
                }
                if(newMesh.vertex[newMesh.triangles[i][2].v] == vertex2[j] && newMesh.normals[newMesh.triangles[i][2].n] == normals2[j] ) {
                    isAlreadyExist2 = true;
                    ind3 = j;
                }
            }
            if(isAlreadyExist&&isAlreadyExist1&&isAlreadyExist2)
                break;
        }
        if(!isAlreadyExist) {
            vertex2.push_back(newMesh.vertex[newMesh.triangles[i][0].v]);
            normals2.push_back(newMesh.normals[newMesh.triangles[i][0].n]);
            if(newMesh.texCoord.size() > 0)
                texCoord2.push_back(newMesh.texCoord[newMesh.triangles[i][0].t]);
            ind1 = vertex2.size()-1;
        }
        if(!isAlreadyExist1) {
            vertex2.push_back(newMesh.vertex[newMesh.triangles[i][1].v]);
            normals2.push_back(newMesh.normals[newMesh.triangles[i][1].n]);
            if(newMesh.texCoord.size() > 0)
                texCoord2.push_back(newMesh.texCoord[newMesh.triangles[i][1].t]);
            ind2 = vertex2.size()-1;
        }
        if(!isAlreadyExist2) {
            vertex2.push_back(newMesh.vertex[newMesh.triangles[i][2].v]);
            normals2.push_back(newMesh.normals[newMesh.triangles[i][2].n]);
            if(newMesh.texCoord.size() > 0)
                texCoord2.push_back(newMesh.texCoord[newMesh.triangles[i][2].t]);
            ind3 = vertex2.size()-1;
        }
        newMesh.indices.push_back(ind1);
        newMesh.indices.push_back(ind2);
        newMesh.indices.push_back(ind3);
    }


    newMesh.normals = normals2;
    newMesh.vertex = vertex2;
    newMesh.texCoord = texCoord2;




    strToId.insert({meshFile, number});
    meshs.insert({number, newMesh});


    return number++;

}

MeshRaw & Pool::getMeshFromID(unsigned int id) {
    return meshs.at(id);
}

std::unordered_map<unsigned int, MeshRaw> & Pool::getAllMeshs(){
    return meshs;
}

unsigned int Pool::loadShader(QOpenGLShader::ShaderType type, std::string shaderName) {
    std::string total = shaderName;

    // Si il existe déjà on le recharge pas et on redonne le int
    if(strToId.find(total) != strToId.end())
        return strToId.at(total);

    // On crée un nouveau shader
    ShaderRaw newShader;

    newShader.shader = new QOpenGLShader(type);
    newShader.shader->compileSourceFile(shaderName.c_str());

    strToId.insert({total, number});
    shaders.insert({number, newShader});

    return number++;
}

ShaderRaw & Pool::getShaderFromID(unsigned int id) {
    return shaders.at(id);
}

unsigned int Pool::loadProgram(unsigned int Mesh_ID, unsigned int Texture_ID, unsigned int ShaderV_ID, unsigned int ShaderF_ID) {

    if(Mesh_ID == -1 || ShaderF_ID == -1 || ShaderV_ID == -1)
        return -1; // On devrait throw

    std::string total = std::to_string(Mesh_ID)+std::to_string(Texture_ID)+std::to_string(ShaderF_ID)+std::to_string(ShaderV_ID);

    if(strToId.find(total) != strToId.end())
        return strToId.at(total);
    

    MeshRaw & mesh = getMeshFromID(Mesh_ID);

    ProgramRaw* newProgram = new ProgramRaw;


    newProgram->m_program = new QOpenGLShaderProgram;
    newProgram->m_program->addShader(getShaderFromID(ShaderV_ID).shader);
    newProgram->m_program->addShader(getShaderFromID(ShaderF_ID).shader);
    newProgram->m_program->bindAttributeLocation("vertex", 0);
    newProgram->m_program->bindAttributeLocation("normal", 1);
    newProgram->m_program->bindAttributeLocation("texcoord", 2);

    newProgram->m_program->link();
    newProgram->m_program->bind();

    newProgram->m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&newProgram->m_vao);

    newProgram->m_vertexvbo.create();
    newProgram->m_vertexvbo.bind();
    newProgram->m_vertexvbo.allocate(&(mesh.vertex[0]), mesh.vertex.size() * 3 *sizeof(GL_FLOAT));
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), 0);
    newProgram->m_vertexvbo.release();


    newProgram->m_normalvbo.create();
    newProgram->m_normalvbo.bind();
    newProgram->m_normalvbo.allocate(&(mesh.normals[0]), mesh.normals.size() * 3 * sizeof(GL_FLOAT));
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
    newProgram->m_normalvbo.release();



    if(mesh.texCoord.size() > 0) {
        newProgram->m_texcoordvbo.create();
        newProgram->m_texcoordvbo.bind();
        newProgram->m_texcoordvbo.allocate(&(mesh.texCoord[0]), mesh.texCoord.size() * 2 * sizeof(GL_FLOAT));
        f->glEnableVertexAttribArray(2);
        f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), 0);
        newProgram->m_texcoordvbo.release();
    }


    f->glGenBuffers(1, &(newProgram->indBuff));
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newProgram->indBuff);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &(mesh.indices[0]), GL_STATIC_DRAW);



    newProgram->m_program->setUniformValue("cameraPos", QVector3D(0, 0, 0));
    newProgram->m_program->setUniformValue("material.color", QVector3D(0.5,0.5,0.5));
    newProgram->m_program->setUniformValue("material.shininess", 32.f);
    newProgram->m_program->setUniformValue("texture_diffuse", 0);
    newProgram->m_program->release();
    

    strToId.insert({total, number});
    programs.insert({number, newProgram});


    
    return number++;
}


ProgramRaw* Pool::getProgramFromID(unsigned int id) {
    return programs.at(id);
}

unsigned int Pool::loadTexture(std::string textureName) {

    // Si il existe déjà on le recharge pas et on redonne le int
    if(strToId.find(textureName) != strToId.end())
        return strToId.at(textureName);

    // On crée un nouveau shader
    TextureRaw newTexture;
    newTexture.texture = new QOpenGLTexture(QImage(textureName.c_str()).mirrored());


    strToId.insert({textureName, number});
    textures.insert({number, newTexture});

    return number++;
}


TextureRaw & Pool::getTextureFromID(unsigned int id) {
    return textures.at(id);
}
/* Pas de delete pour l'instant mais on devrait compter où est utilisé */
