#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos;

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diffStrength * material.diffuse) * light.diffuse;

    //specular
    float specularStrength = 0.5f;
    vec3 eyeTarget = normalize(viewPos - FragPos);
    vec3 temp = normalize(lightDir + eyeTarget);
    float spec = pow(max(dot(temp,norm),0.0),material.shininess);
    vec3 specular = (material.specular * spec) *  light.specular;

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
} 