#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec4 lightColor;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
}; 
uniform Material material;

void main()
{
    vec4 ambient = material.ambient * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffStrength = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diffStrength * material.diffuse * lightColor;

    //specular
    vec3 eyeTarget = normalize(viewPos - FragPos);
    vec3 temp = normalize(lightDir + eyeTarget);
    float spec = pow(max(dot(temp,norm),0.0),material.shininess);
    vec4 specular = material.specular * spec * lightColor;

    vec4 result = (ambient + diffuse + specular);
    FragColor = result;
} 