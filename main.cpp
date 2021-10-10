#pragma once
#include <iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Classes/Shader.h"

//头文件多选一
//#include"Classes/Basic_Test.h"
#include"Classes/Lighting_Test.h"

#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/type_ptr.hpp"


int main()
{  
    //Basic_Test.h-----------------------------
    /*return DrawGraphicPrimirary();*/
    /*return DrawColorfulTriangle();*/
    /*return GlmTest();*/
    /*return BasicTransform();*/
    /*return TextureTest();*/
    /*return CameraTest();*/
    //Lighting_Test.h--------------------
    /*return LightTest();*/
    //return LightCastersTest();
    return ModelLoaded();
}


