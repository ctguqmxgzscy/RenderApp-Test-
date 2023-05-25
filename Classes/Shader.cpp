
#include"Shader.h"

Shader::~Shader()
{
	glDeleteShader(ID);
	vertexCode.clear();
	fragmentCode.clear();
	vertexName.clear();
	fragmentName.clear();
}

void Shader::Init()
{
	int success;
	char infoLog[512];
	GLuint vertexID, fragmentID;
	// Compile Vertex Shader
	vertexID = glCreateShader(GL_VERTEX_SHADER);
	const char* vertex_shader_src_cstr = vertexCode.c_str();
	glShaderSource(vertexID, 1, &vertex_shader_src_cstr, NULL);
	glCompileShader(vertexID);
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
		std::cout << "ERROR Compile Vertex Shader: ID [" << vertexID << "]   error: " << infoLog << std::endl;
		exit(1);
	}
	// Compile Fragment Shader
	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragment_shader_src_cstr = fragmentCode.c_str();
	glShaderSource(fragmentID, 1, &fragment_shader_src_cstr, NULL);
	glCompileShader(fragmentID);
	glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	ID = glCreateProgram();
	glAttachShader(ID, vertexID);
	glAttachShader(ID, fragmentID);
	glLinkProgram(ID);
	// check for linking errors
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

}

void Shader::ReCompile()
{
	glDeleteShader(ID);

	int success;
	char infoLog[512];
	GLuint vertexID, fragmentID;

	vertexID = glCreateShader(GL_VERTEX_SHADER);
	const char* vertex_shader_src_cstr = vertexCode.c_str();
	glShaderSource(vertexID, 1, &vertex_shader_src_cstr, NULL);
	glCompileShader(vertexID);

	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
		std::cout << "ERROR Compile Vertex Shader: ID [" << vertexID << "]   error: " << infoLog << std::endl;
	}

	// Compile Fragment Shader
	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragment_shader_src_cstr = fragmentCode.c_str();
	glShaderSource(fragmentID, 1, &fragment_shader_src_cstr, NULL);
	glCompileShader(fragmentID);

	glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Link shaders
	ID = glCreateProgram();
	glAttachShader(ID, vertexID);
	glAttachShader(ID, fragmentID);
	glLinkProgram(ID);
	// check for linking errors
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDetachShader(ID, GL_VERTEX_SHADER);
	glDeleteShader(vertexID);
	glDetachShader(ID, GL_FRAGMENT_SHADER);
	glDeleteShader(fragmentID);
}

Shader Shader::operator=(const Shader& rhs) noexcept
{
	ID = rhs.ID;
	fragmentCode= std::string(rhs.fragmentCode.c_str());
	fragmentName = std::string(rhs.fragmentName.c_str());
	vertexCode = std::string(rhs.vertexCode.c_str());
	vertexName = std::string(rhs.vertexName.c_str());
	ReCompile();
	return *this;
}

Shader Shader::operator=(Shader&& rhs) noexcept
{
	ID = rhs.ID;
	fragmentCode = rhs.fragmentCode;
	fragmentName = rhs.fragmentName;
	vertexCode = rhs.vertexCode;
	vertexName = rhs.vertexName;
	ReCompile();
	return *this;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {

	vertexName = std::string(vertexPath);
	fragmentName = std::string(fragmentPath);

	vertexName = vertexName.substr(8, vertexName.size() - 8);
	fragmentName = fragmentName.substr(8, fragmentName.size() - 8);

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	//保证ifstream对象可以抛出异常
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		//将文件流转换为字符串流
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//转换完后关闭文件流
		vShaderFile.close();
		fShaderFile.close();
		//将字符串流转换为字符串
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "FAILED::SHADER::FILE_NOT_SUCCESSFULLY_READ!" << std::endl;
	}
	Init();
}

Shader::Shader(const Shader& rhs)
{
	glDeleteShader(this->ID);
	this->ID = rhs.ID;
	this->fragmentCode = std::string(rhs.fragmentCode.c_str());
	this->fragmentName = std::string(rhs.fragmentName.c_str());
	this->vertexCode = std::string(rhs.vertexCode.c_str());
	this->vertexName = std::string(rhs.vertexName.c_str());
	this->ReCompile();
	std::cout << "Shader Copy Constructor" << std::endl;
}

Shader::Shader(Shader&& rhs) noexcept
{
	glDeleteShader(this->ID);
	this->ID = rhs.ID;
	this->fragmentCode = rhs.fragmentCode;
	this->fragmentName = rhs.fragmentName;
	this->vertexCode = rhs.vertexCode;
	this->vertexName = rhs.vertexName;
	this->ReCompile();
	std::cout << "Shader Move Constructor" << std::endl;
}



void Shader::use() {
	glUseProgram(this->ID);
}

void Shader::setBool(const std::string& name, bool value)const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setUint(const std::string& name, unsigned int value) const
{
	glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setDirLight(const DirectionalLight& light) const
{
	this->setVec3("dirLight.direction", light.direction);
	this->setVec3("dirLight.ambient", light.ambient);
	this->setVec3("dirLight.diffuse", light.diffuse);
	this->setVec3("dirLight.specular", light.specular);
}

void Shader::setPointLight(const PointLight& light) const
{
	this->setVec3("pointLight.position", light.position);
	this->setVec3("pointLight.ambient", light.ambient);
	this->setVec3("pointLight.diffuse", light.diffuse);
	this->setVec3("pointLight.specular", light.specular);

}

void Shader::setSpotLight(const SpotLight& light) const
{
	this->setVec3("spotLight.position", light.position);
	this->setVec3("spotLight.direction", light.direction);

	this->setVec3("spotLight.ambient", light.ambient);
	this->setVec3("spotLight.diffuse", light.diffuse);
	this->setVec3("spotLight.specular", light.specular);

	this->setFloat("spotLight.innerCutOut", light.innerCutOut);
	this->setFloat("spotLight.outterCutOut", light.outterCutOut);
}

void Shader::setPBRPointLight(const PBRPointLight& light) const
{
	this->setVec3("pointLight.position", light.position);
	this->setVec3("pointLight.lightColor", light.lightColor);

}

void Shader::setPBRDirLight(const PBRDirctionalLight& light) const
{
	this->setVec3("dirLight.direction", light.direction);
	this->setVec3("dirLight.lightColor", light.lightColor);

}

void Shader::setMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const
{
	this->setMat4("model", model);
	this->setMat4("view", view);
	this->setMat4("projection", projection);
}
