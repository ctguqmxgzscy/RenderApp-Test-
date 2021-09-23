#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffStrength * lightColor;

    //specular
    float specularStrength = 0.5f;
    vec3 eyeTarget = normalize(viewPos - FragPos);
    vec3 temp = normalize(lightDir + eyeTarget);
    float spec = pow(max(dot(temp,norm),0.0),128);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 