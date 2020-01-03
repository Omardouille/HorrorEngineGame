    attribute vec4 vertex;
    attribute vec3 normal;
    attribute vec4 texcoord;
    varying vec3 vert;
    varying vec3 vertNormal;
    varying vec4 texc;
    uniform mat4 projMatrix;
    uniform mat4 mvMatrix;
    uniform mat3 normalMatrix;
    uniform mat4 mMatrix;
    
    void main() {
       texc = vertex;
       gl_Position = projMatrix * mvMatrix * vertex;
    }