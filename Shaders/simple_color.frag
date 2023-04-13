#version 330                                                                        
out vec4 FragColor;       

in vec3 FragPos;
in vec3 Normal;
in vec2 Tex;
                                                                              
void main()                                                                         
{                                                                                   
    FragColor = vec4(vec3(Normal), 1.0);                                        
}