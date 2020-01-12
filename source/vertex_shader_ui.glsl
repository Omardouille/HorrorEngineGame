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
        vert = vertex;
        texc = vec4((vertex.x+1.0)/2, (vertex.y+1.0)/2, 0.0, 1.0);
        gl_Position = vec4(vertex.x, vertex.y, 0.0, 1.0); 
    }