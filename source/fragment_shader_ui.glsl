varying highp vec3 vert;
varying highp vec3 vertNormal;
varying highp vec4 texc;
uniform sampler2D texture_diffuse;


void main() {
   gl_FragColor = texture(texture_diffuse, texc);
}