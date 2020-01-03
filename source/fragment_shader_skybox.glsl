    varying highp vec3 vert;
    varying highp vec3 vertNormal;
    varying highp vec4 texc;
    uniform samplerCube texture_diffuse;

    struct Material {
      uniform highp vec3 color; // qui deviendra la texture

      uniform highp float shininess;
    };

    struct Light {
      uniform highp int type;

      uniform highp vec3 position;
      uniform highp vec3 direction; // Peut être vide si pas utilisé
      uniform highp float cutoff; // angle


      uniform highp vec3 ambient;
      uniform highp vec3 diffuse;
      uniform highp vec3 specular;

      //https://learnopengl.com/Lighting/Light-casters
      uniform highp float constant;
      uniform highp float linear;
      uniform highp float quadratic;


    };

    //uniform highp Light light;
    #define MAX_LIGHT 100
    uniform int nb;
    uniform Light lights[MAX_LIGHT];

    uniform highp Material material;
    uniform highp vec3 cameraPos;

    void main() {
      highp vec3 c = texture(texture_diffuse, texc);
      gl_FragColor = vec4(c , 1.0);
  }