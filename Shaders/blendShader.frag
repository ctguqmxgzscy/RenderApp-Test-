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
    vec3 norm = normalize(Normal);
    if(norm.x==-1.0f){
        norm.x=0;
        norm.y=1.0f;
        norm.z=1.0f;
    }
    else if(norm.y==-1.0f){
        norm.x=1.0f;
        norm.y=0;
        norm.z=1.0f;
    }
      else if(norm.z==-1.0f){
        norm.x=1.0f;
        norm.y=1.0f;
        norm.z=0;
    }
    
    FragColor = vec4(norm,.5f);
} 