#pragma once
#include <iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Classes/Shader.h"
//#include"Classes/Basic_Test.h"
#include"Classes/Lighting_Test.h"
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/type_ptr.hpp"


int main()
{  
    /*return DrawGraphicPrimirary();*/
    /*return DrawColorfulTriangle();*/
    /*return GlmTest();*/
    /*return BasicTransform();*/
    /*return TextureTest();*/
    /*return CameraTest();*/
    return LightTest();
}


