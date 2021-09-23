#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 Tex;
  
uniform vec3 lightPos; 
uniform vec3 viewPos;

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emmission;
    float shininess;
};

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
uniform Material material;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,Tex));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * vec3(texture(material.diffuse,Tex)) * diff;

    //specular
    vec3 eyeTarget = normalize(viewPos - FragPos);
    vec3 temp = normalize(lightDir + eyeTarget);
    float spec = pow(max(dot(temp,norm),0.0),material.shininess);
    vec3 specular = light.specular *  vec3(texture(material.specular,Tex)) * spec;

    vec3 emmiColor=vec3(texture(material.emmission,Tex));
    if(emmiColor.x>0.4f||emmiColor.y>0.4f||emmiColor.z>0.4f){
        FragColor = vec4(vec3(texture(material.emmission,Tex)), 1.0);
    }
    else{
        vec3 result = (ambient + diffuse + specular);
        FragColor = vec4(result, 1.0);
    }
} 