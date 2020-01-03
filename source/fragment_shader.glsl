    varying highp vec3 vert;
    varying highp vec3 vertNormal;
    varying highp vec4 texc;
    uniform sampler2D texture_diffuse;

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
      vertNormal = normalize(vertNormal);
      highp vec3 col = vec3(0,0,0);

      highp vec3 c = material.color;

      highp float a = 1.0;

      if(c.x == -1) {
        c = texture(texture_diffuse, texc);
	highp float a = texture(texture_diffuse, texc).a;	
	}
      

      for(int i = 0; i < nb; i++) {
        Light light = lights[i];
        highp vec3 lightDir;
        highp float attenuation = 1.0;
        // 1 = point light, 0 = direcctionnal, 2 = pointlight
        if(light.type != 0) {
          lightDir = normalize(light.position - vert); // vecteur direction lumiere
          highp float distance = length(light.position - vert);
         // attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
        } else {
          lightDir = normalize(-light.direction); // vecteur direction lumiere  
        } 
        
        highp float theta = 0;
        if(light.type == 2) {
          theta = dot(lightDir, normalize(-light.direction));
        }

        if( (light.type == 2 && theta > light.cutoff) || light.type != 2) {
          highp vec3 viewDir = normalize(cameraPos - vert); // vecteur direction de la vue
          highp vec3 reflectDir = reflect(-lightDir, vertNormal); // vecteur direction de la reflexion
        //  highp float angle = dot(normalize(vertNormal), lightDir);


          highp float diff = max(dot(vertNormal, lightDir), 0.0);
          highp float spec = pow(max(clamp(dot(viewDir, reflectDir),0,1), 0.0), material.shininess);
          //spec = angle != 0.0 ? spec : 0.0;

          // highp vec3 col = clamp(color * 0.1 + color * 0.7 * diff + color * 0.3 * spec, 0.0, 1.0);
          highp vec3 ambient = c * light.ambient * attenuation;
          highp vec3 diffuse = c * diff * light.diffuse * attenuation;
          highp vec3 specular = c * spec * light.specular * attenuation;

          col += clamp(ambient+diffuse+specular, 0.0, 1.0);
      } else {
        highp vec3 ambient = c * light.ambient * attenuation;
        col += clamp(ambient, 0.0, 1.0);
      }
    }

    gl_FragColor = vec4(col , a);
}