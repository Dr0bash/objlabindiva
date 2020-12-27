#include <glew.h>
#include <freeglut.h>
#include "glm/glm.hpp"
#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLShader.cpp"
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <SOIL/SOIL.h>
#include "Affind3D.h"

GLuint Program;
GLint Unif_color;

GLShader shaderVBO;
GLShader shader;
GLuint* VBO_vertex;
GLuint VBO_color;
GLuint VBO_Attrib_vertex;
GLuint VBO_Attrib_color;
GLuint VBO_Unif_matrix;
GLuint* VBO_normal;
GLuint* VBO_texture;
GLuint Attrib_vertex;
GLuint Attrib_color;
GLuint Unif_matrix;
GLuint* textures;
GLShader toon_shader;
GLShader default_shader;
GLShader gachi_muchi_shader;
GLShader plane_shader;
GLShader cube_shader;
GLShader knot_shader;
GLShader sphere_shader;
string* filename;
int object_count = 5;

GLfloat** ambient_mat_arr;
GLfloat** diffuse_mat_arr;
GLfloat** specular_mat_arr;
GLfloat** emission_mat_arr;
GLfloat* shininess_arr;
string* texture_path;
glm::vec3* positions;
glm::vec3* rotations;
glm::vec3* scales;

glm::vec3 proj_light_pos;
glm::vec3 proj_light_rot;

int light0on = 1;
int light1on = 1;
int light2on = 1;
bool knot = false;
float textcoef = 0.5;

enum ShType
{
	def = 0,
	toon,
	gachi
};
ShType cur_type;
int* indicies_count;

double rotate_x = 0;
double rotate_y = 0;
double rotate_y_light = 0;
double rotate_z = 0;
void checkOpenGLerror()
{
	const GLubyte* kek;
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
	{
		kek = gluErrorString(errCode);
		std::cout << kek << std::endl;
	}

}

Polyhedron ParceFromFile(string filename)
{
	Polyhedron polyhedron;

	Point point(1, 2, 3);
	char buff[1000]; // буфер промежуточного хранения считываемого из файла текста
	ifstream fin(filename); // открыли файл для чтения

	regex myregex;
	int mode = 0;

	while (!fin.eof())
	{
		fin.getline(buff, 1000); // считали строку из файла
		string s = buff;
		if (s[0] == 'v')
		{
			if (s[1] == ' ')
			{
				myregex = regex("v (\-?\\d+,\\d+) (\-?\\d+,\\d+) (\-?\\d+,\\d+)");
				mode = 0;
			}
			else if (s[1] == 'n')
			{
				myregex = regex("vn (\-?\\d+,\\d+) (\-?\\d+,\\d+) (\-?\\d+,\\d+)");
				mode = 1;
			}
			else if (s[1] == 't')
			{
				myregex = regex("vt (\-?\\d+,\\d+) (\-?\\d+,\\d+)");
				mode = 2;
			}
		}
		else if (s[0] == 'f')
		{
			myregex = regex("f (\\d+/\\d+/\\d+) (\\d+/\\d+/\\d+) (\\d+/\\d+/\\d+)");
			mode = 3;
		}
		else
			continue;
		auto words_begin = sregex_iterator(s.begin(), s.end(), myregex);
		auto words_end = sregex_iterator();
		for (sregex_iterator i = words_begin; i != words_end; i++)
		{
			smatch match = *i;
			if (mode == 0)
			{
				Point p(stod(match[1]), stod(match[2]), stod(match[3]));
				polyhedron.pointList.push_back(p);
			}
			else if (mode == 1)
			{
				Point p(stod(match[1]), stod(match[2]), stod(match[3]));
				polyhedron.normalList.push_back(p);
			}
			else if (mode == 2)
			{
				Point2D p(stod(match[1]), stod(match[2]));
				polyhedron.texturePoint.push_back(p);
			}
			else if (mode == 3)
			{
				vector<tuple<int, int, int>> polygon = vector<tuple<int, int, int>>();
				for (int j = 1; j < match.size(); j++)
				{
					regex point = regex("(\\d+)/(\\d+)/(\\d+)");
					string s0 = match[j];
					auto matchpoint = sregex_iterator(s0.begin(), s0.end(), point);
					polygon.push_back(make_tuple(stoi((*matchpoint)[1]) - 1, stoi((*matchpoint)[2]) - 1, stoi((*matchpoint)[3]) - 1));
				}
				polyhedron.polygons.push_back(polygon);
			}
		}
	}

	fin.close(); // закрываем файл

	return polyhedron;
}

void initArrays()
{
	textures = new GLuint[6];
	ambient_mat_arr = new GLfloat* [5];
	diffuse_mat_arr = new GLfloat* [5];
	specular_mat_arr = new GLfloat* [5];
	emission_mat_arr = new GLfloat* [5];
	shininess_arr = new GLfloat[5];
	texture_path = new string[6];
	positions = new glm::vec3[5];
	rotations = new glm::vec3[5];
	scales = new glm::vec3[5];
	proj_light_pos = glm::vec3(-5,-0.25,0);
	proj_light_rot = glm::vec3(90, 0, 0);

	float general_dif = 20;
	//monkey

	ambient_mat_arr[0] = new GLfloat[4]{ 1.5, 1.5, 1.5, 1 };
	diffuse_mat_arr[0] = new GLfloat[4]{ general_dif, general_dif, general_dif, 1 };
	specular_mat_arr[0] = new GLfloat[4]{ 0.1f, 0.1f, 0.1f, 1 };
	emission_mat_arr[0] = new GLfloat[4]{ 0, 0, 0, 1 };
	shininess_arr[0] = 1;

	positions[0] = glm::vec3(-5, 0, 0);
	rotations[0] = glm::vec3(0,90,0);
	scales[0] = glm::vec3(1.5,1.5,1.5);

	texture_path[0] = "water.jpg";

	filename[0] = "monkey_smol.obj";

	//cube

	ambient_mat_arr[1] = new GLfloat[4]{ 1.5, 1.5, 1.5, 1 };
	diffuse_mat_arr[1] = new GLfloat[4]{ 40, 40, 40, 1 };
	specular_mat_arr[1] = new GLfloat[4]{ 0.1f, 0.1f, 0.1f, 1 };
	emission_mat_arr[1] = new GLfloat[4]{ 0, 0, 0, 1 };
	shininess_arr[1] = 1;

	positions[1] = glm::vec3(5,0.001,0);
	rotations[1] = glm::vec3(0,0,0);
	scales[1] = glm::vec3(1.5,1.5,1.5);

	texture_path[1] = "stone.jpg";
	filename[1] = "cube.obj";
	//plain

	ambient_mat_arr[2] = new GLfloat[4]{ 1.5, 1.5, 1.5, 1 };
	diffuse_mat_arr[2] = new GLfloat[4]{ 70, 70, 70, 1 };
	specular_mat_arr[2] = new GLfloat[4]{ 0.1f, 0.1f, 0.1f, 1 };
	emission_mat_arr[2] = new GLfloat[4]{ 0, 0, 0, 1 };
	shininess_arr[2] = 1;

	positions[2] = glm::vec3(0,-1.5,0);
	rotations[2] = glm::vec3(0,0,0);
	scales[2] = glm::vec3(10,1,10);

	texture_path[2] = "123.jpg";
	filename[2] = "plane.obj";
	//sphere
	ambient_mat_arr[3] = new GLfloat[4]{ 1.5, 1.5, 1.5, 1 };
	diffuse_mat_arr[3] = new GLfloat[4]{ general_dif, general_dif, general_dif, 1 };
	specular_mat_arr[3] = new GLfloat[4]{ 0.1f, 0.1f, 0.1f, 1 };
	emission_mat_arr[3] = new GLfloat[4]{ 0, 0, 0, 1 };
	shininess_arr[3] = 1;

	positions[3] = glm::vec3(0,0,5);
	rotations[3] = glm::vec3(0,0,0);
	scales[3] = glm::vec3(1.5,1.5,1.5);

	texture_path[3] = "tree.jpg";
	filename[3] = "shar.obj";
	//knot

	ambient_mat_arr[4] = new GLfloat[4]{ 1.5, 1.5, 1.5, 1 };
	diffuse_mat_arr[4] = new GLfloat[4]{ general_dif, general_dif, general_dif, 1 };
	specular_mat_arr[4] = new GLfloat[4]{ 0.1f, 0.1f, 0.1f, 1 };
	emission_mat_arr[4] = new GLfloat[4]{ 0, 0, 0, 1 };
	shininess_arr[4] = 1;

	positions[4] = glm::vec3(0,0,-5);
	rotations[4] = glm::vec3(0,0,0);
	scales[4] = glm::vec3(2,2,2);

	texture_path[4] = "sand.jpg";
	filename[4] = "cone.obj";

	texture_path[5] = "dirt.jpg";
	
}

void initVBO()
{
	//file

	setlocale(LC_ALL, "rus"); // корректное отображение Кириллицы

	for (int a = 0; a < object_count; a++)
	{
		Polyhedron polyhedron = ParceFromFile(filename[a]);
		
		GLint* indices = new int[polyhedron.polygons.size() * 3];
		GLint* indicesnorm = new int[polyhedron.polygons.size() * 3];
		GLint* indicestext = new int[polyhedron.polygons.size() * 3];
		int pointer = 0;
		for (int i = 0; i < polyhedron.polygons.size(); i++)
		{
			for (int j = 0; j < polyhedron.polygons[i].size(); j++)
			{
				indices[pointer] = get<0>(polyhedron.polygons[i][j]);
				indicestext[pointer] = get<1>(polyhedron.polygons[i][j]);
				indicesnorm[pointer] = get<2>(polyhedron.polygons[i][j]);
				++pointer;
			}
		}

		vector<Point> okpoints = vector<Point>();
		vector<Point2D> oktextures = vector<Point2D>();
		vector<Point> oknormals = vector<Point>();

		for (int i = 0; i < polyhedron.polygons.size() * 3; i++)
		{
			okpoints.push_back(polyhedron.pointList[indices[i]]);
			oktextures.push_back(polyhedron.texturePoint[indicestext[i]]);
			oknormals.push_back(polyhedron.normalList[indicesnorm[i]]);
		}


		glGenBuffers(1, &VBO_vertex[a]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex[a]);
		glBufferData(GL_ARRAY_BUFFER, okpoints.size() * sizeof(float) * 3, &okpoints[0],
			GL_STATIC_DRAW);

		glGenBuffers(1, &VBO_normal[a]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_normal[a]);
		glBufferData(GL_ARRAY_BUFFER, oknormals.size() * sizeof(float) * 3, &oknormals[0], //todo
			GL_STATIC_DRAW);

		glGenBuffers(1, &VBO_texture[a]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_texture[a]);

		glBufferData(GL_ARRAY_BUFFER, oktextures.size() * sizeof(float) * 2, &oktextures[0],
			GL_STATIC_DRAW);

		indicies_count[a] = pointer;
	}
	
	checkOpenGLerror();
}

void freeVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	for (int a = 0; a < 1; a++)
	{
		glDeleteBuffers(1, &VBO_vertex[a]);
		glDeleteBuffers(1, &VBO_normal[a]);
		glDeleteBuffers(1, &VBO_texture[a]);
	}
}

void initShader()
{
	//const char* vertex_Source =
	//	"#version 330 core\n"
	//	"#define VERT_POSITION 0\n"
	//	"#define VERT_TEXCOORD 1\n"
	//	"#define VERT_NORMAL 2\n"
	//	"layout(location=VERT_POSITION) in vec3 position;\n"
	//	"layout(location=VERT_TEXCOORD) in vec2 texcoord;\n"
	//	"layout(location=VERT_NORMAL) in vec3 normal;\n"
	//	"uniform struct Transform{\n"
	//	"	mat4 model;\n"
	//	"	mat4 viewProjection;\n"
	//	"	mat4 normal;\n"
	//	"	vec3 viewPosition;\n"
	//	"} transform;\n"
	//	"uniform struct PointLight{\n"
	//	"	vec4 position;\n"
	//	"	vec4 ambient;\n"
	//	"	vec4 diffuse;\n"
	//	"	vec4 specular;\n"
	//	"	vec3 attenuation;\n"
	//	"} light;\n"
	//	"out struct Vertex{\n"
	//	"	vec2 texcoord;\n"
	//	"	vec3 normal;\n"
	//	"	vec3 lightDir;\n"
	//	"	vec3 viewDir;\n"
	//	"	float distance;\n"
	//	"} Vert;\n"
	//	"void main(void){\n"
	//	"	vec4 vertex = transform.model*vec4(position,1.0);\n"
	//	"	vec4 lightDir = light.position - vertex;\n"
	//	"	gl_Position = transform.viewProjection*vertex;\n"
	//	"	Vert.texcoord=texcoord;\n"///////////////////////////////
	//	"	vec4 kek = transform.normal * vec4(normal, 1);\n"
	//	"	Vert.normal= vec3(kek.x, kek.y, kek.z);\n"
	//	"	Vert.lightDir=vec3(lightDir);\n"
	//	"	Vert.viewDir=transform.viewPosition - vec3(vertex);\n"
	//	"	Vert.distance=length(lightDir);\n"
	//	"}\n";
	//const char* fragment_Source =
	//	"#version 330 core\n"
	//	"#define FRAG_OUTPUT0 0\n"
	//	"layout(location=FRAG_OUTPUT0) out vec4 color;\n"
	//	"uniform struct PointLight{\n"
	//	"	vec4 position;\n"
	//	"	vec4 ambient;\n"
	//	"	vec4 diffuse;\n"
	//	"	vec4 specular;\n"
	//	"	vec3 attenuation;\n"
	//	"} light;\n"
	//	"uniform struct Material{\n"
	//	"	sampler2D texture;\n"
	//	"	vec4 ambient;\n"
	//	"	vec4 diffuse;\n"
	//	"	vec4 specular;\n"
	//	"	vec4 emission;\n"
	//	"	float shininess;\n"
	//	"} material;\n"
	//	"in struct Vertex {\n"
	//	"	vec2 texcoord;\n"
	//	"	vec3 normal;\n"
	//	"	vec3 lightDir;\n"
	//	"	vec3 viewDir;\n"
	//	"	float distance;\n"
	//	"} Vert;\n"
	//	"void main(void){\n"
	//	"	vec3 normal = normalize(Vert.normal);\n"
	//	"	vec3 lightDir = normalize(Vert.lightDir);\n"
	//	"	vec3 viewDir = normalize(Vert.viewDir);\n"
	//	"	float attenuation=1.0/(light.attenuation[0]+\n"
	//	"	light.attenuation[1]*Vert.distance+\n"
	//	"	light.attenuation[2]*Vert.distance*Vert.distance);\n"
	//	"	color=material.emission;\n"
	//	"	color +=material.ambient*light.ambient*attenuation;\n"
	//	"	float Ndot=max(dot(normal,lightDir),0.0);\n"
	//	"	color += material.diffuse*light.diffuse*Ndot*attenuation; \n"
	//	"	vec3 h = normalize(lightDir + viewDir);\n"	
	//	"	float RdotVpow = max(pow(dot(normal, h), material.shininess), 0.0);\n"
	//	"	color+=material.specular*light.specular*RdotVpow*attenuation;\n"
	//	//"	color*=vec4(1, 0, 0, 1);\n"
	//	//"color*= vec4(Vert.texcoord, 0,1);\n"
	//	"  color*=texture(material.texture,Vert.texcoord);\n"///////////////////////////////
	//	"}\n";

	const char* vertex_Source =
		"#version 330 core\n"
		"#define VERT_POSITION 0\n"
		"#define VERT_TEXCOORD 1\n"
		"#define VERT_NORMAL 2\n"
		"layout(location=VERT_POSITION) in vec3 position;\n"
		"layout(location=VERT_TEXCOORD) in vec2 texcoord;\n"
		"layout(location=VERT_NORMAL) in vec3 normal;\n"
		"uniform struct Transform{\n"
		"	mat4 model;\n"
		"	mat4 viewProjection;\n"
		"	mat4 normal;\n"
		"	vec3 viewPosition;\n"
		"} transform;\n"
		"uniform struct PointLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"} light, light1;\n"
		"uniform struct ProjectionLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"	vec3 spot_direction;\n"
		"	float spot_cutoff;\n"
		"	float spot_exponent;\n"
		"} light2;\n"
		"out struct Vertex{\n"
		"	vec2 texcoord;\n"
		"	vec3 normal;\n"
		"	vec3 lightDir;\n"
		"	vec3 lightDir1;\n"
		"	vec3 lightDir2;\n"
		"	vec3 viewDir;\n"
		"	float distance;\n"
		"	float distance1;\n"
		"	float distance2;\n"
		"} Vert;\n"
		"void main(void){\n"
		"	vec4 vertex = transform.model*vec4(position,1.0);\n"
		"	vec4 lightDir = light.position - vertex;\n"
		"	vec4 lightDir1 = light1.position - vertex;\n"
		"	vec4 lightDir2 = light2.position - vertex;\n"
		"	gl_Position = transform.viewProjection*vertex;\n"
		"	Vert.texcoord=texcoord;\n"
		"	vec4 kek = transform.normal * vec4(normal, 1);\n"
		"	Vert.normal= vec3(kek.x, kek.y, kek.z);\n"
		"	Vert.lightDir=vec3(lightDir);\n"
		"	Vert.lightDir1=vec3(lightDir1);\n"
		"	Vert.lightDir2=vec3(lightDir2);\n"
		"	Vert.viewDir=transform.viewPosition - vec3(vertex);\n"
		"	Vert.distance=length(lightDir);\n"
		"	Vert.distance1=length(lightDir1);\n"
		"	Vert.distance2=length(lightDir2);\n"
		"}\n";

	const char* fragment_Source =
		"#version 330 core\n"
		"#define FRAG_OUTPUT0 0\n"
		"layout(location=FRAG_OUTPUT0) out vec4 color;\n"
		"uniform struct PointLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"} light, light1;\n"
		"uniform struct ProjectionLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"	vec3 spot_direction;\n"
		"	float spot_cutoff;\n"
		"	float spot_exponent;\n"
		"} light2;\n"
		"uniform struct Material{\n"
		"	sampler2D texture;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec4 emission;\n"
		"	float shininess;\n"
		"} material;\n"
		"in struct Vertex {\n"
		"	vec2 texcoord;\n"
		"	vec3 normal;\n"
		"	vec3 lightDir;\n"
		"	vec3 lightDir1;\n"
		"	vec3 lightDir2;\n"
		"	vec3 viewDir;\n"
		"	float distance;\n"
		"	float distance1;\n"
		"	float distance2;\n"
		"} Vert;\n"
		"uniform int light0on;\n"
		"uniform int light1on;\n"
		"uniform int light2on;\n"
		"void main(void){\n"
		"	vec3 normal = normalize(Vert.normal);\n"
		"	vec3 lightDir = normalize(Vert.lightDir);\n"
		"	vec3 lightDir1 = normalize(Vert.lightDir1);\n"
		"	vec3 lightDir2 = normalize(Vert.lightDir2);\n"
		"	vec3 viewDir = normalize(Vert.viewDir);\n"
		"	float attenuation=1.0/(light.attenuation[0]+\n"
		"	light.attenuation[1]*Vert.distance+\n"
		"	light.attenuation[2]*Vert.distance*Vert.distance);\n"
		"	float attenuation1=1.0/(light1.attenuation[0]+\n"
		"	light1.attenuation[1]*Vert.distance1+\n"
		"	light1.attenuation[2]*Vert.distance1*Vert.distance1);\n"
		//расчет прожекторного источника
		"	float spotEffect = dot(normalize(light2.spot_direction), -lightDir2);\n"
		"	float spot = float(spotEffect>light2.spot_cutoff);\n"
		"	spotEffect =max(pow(spotEffect, light2.spot_exponent), 0.0);\n"
		"	float attenuation2=spot*spotEffect/(light2.attenuation[0]+\n"
		"	light2.attenuation[1]*Vert.distance2+\n"
		"	light2.attenuation[2]*Vert.distance2*Vert.distance2);\n"
		//
		"	color=material.emission;\n"
		"	if (light0on == 1)\n"
			"	color +=material.ambient*light.ambient*attenuation;\n"
		"	if (light1on == 1)\n"
		"		color +=material.ambient*light1.ambient*attenuation1;\n"
		"	if (light2on == 1)\n"
		"		color +=material.ambient*light2.ambient*attenuation2;\n"
		"	float Ndot=max(dot(normal,lightDir),0.0);\n"
		"	float Ndot1=max(dot(normal,lightDir1),0.0);\n"
		"	float Ndot2=max(dot(-normal,lightDir2),0.0);\n"
		"	if (light0on == 1)\n"
		"		color += material.diffuse*light.diffuse*Ndot*attenuation; \n"
		"	if (light1on == 1)\n"
		"		color += material.diffuse*light1.diffuse*Ndot1*attenuation1; \n"
		"	if (light2on == 1)\n"
		"		color += material.diffuse*light2.diffuse*Ndot2*attenuation2; \n"
		"	vec3 h = normalize(lightDir + viewDir);\n"
		"	vec3 h1 = normalize(lightDir1 + viewDir);\n"
		"	vec3 h2 = normalize(lightDir2 + viewDir);\n"
		"	float RdotVpow = max(pow(dot(normal, h), material.shininess), 0.0);\n"
		"	float RdotVpow1 = max(pow(dot(normal, h1), material.shininess), 0.0);\n"
		"	float RdotVpow2 = max(pow(dot(-normal, h2), material.shininess), 0.0);\n"
		"	if (light0on == 1)\n"
		"		color+=material.specular*light.specular*RdotVpow*attenuation;\n"
		"	if (light1on == 1)\n"
		"		color+=material.specular*light1.specular*RdotVpow1*attenuation1;\n"
		"	if (light2on == 1)\n"
		"		color+=material.specular*light2.specular*RdotVpow2*attenuation2;\n"
		//"	color*=vec4(1, 0, 0, 1);\n"
		"  color*=texture(material.texture,Vert.texcoord);\n"///////////////////////////////
		"}\n";

	const char* vertex_Source_knot =
		"#version 330 core\n"
		"#define VERT_POSITION 0\n"
		"#define VERT_TEXCOORD 1\n"
		"#define VERT_NORMAL 2\n"
		"layout(location=VERT_POSITION) in vec3 position;\n"
		"layout(location=VERT_TEXCOORD) in vec2 texcoord;\n"
		"layout(location=VERT_NORMAL) in vec3 normal;\n"
		"uniform struct Transform{\n"
		"	mat4 model;\n"
		"	mat4 viewProjection;\n"
		"	mat4 normal;\n"
		"	vec3 viewPosition;\n"
		"} transform;\n"
		"uniform struct PointLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"} light, light1;\n"
		"uniform struct ProjectionLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"	vec3 spot_direction;\n"
		"	float spot_cutoff;\n"
		"	float spot_exponent;\n"
		"} light2;\n"
		"out struct Vertex{\n"
		"	vec2 texcoord;\n"
		"	vec3 normal;\n"
		"	vec3 lightDir;\n"
		"	vec3 lightDir1;\n"
		"	vec3 lightDir2;\n"
		"	vec3 viewDir;\n"
		"	float distance;\n"
		"	float distance1;\n"
		"	float distance2;\n"
		"} Vert;\n"
		"out float attenuation;\n"
		"out float attenuation1;\n"
		"out float attenuation2;\n"
		"out float Ndot;\n"
		"out float Ndot1;\n"
		"out float Ndot2;\n"
		"out float Rdot;\n"
		"out float Rdot1;\n"
		"out float Rdot2;\n"
		"void main(void){\n"
		"	vec4 vertex = transform.model*vec4(position,1.0);\n"
		"	vec4 lightDir = light.position - vertex;\n"
		"	vec4 lightDir1 = light1.position - vertex;\n"
		"	vec4 lightDir2 = light2.position - vertex;\n"
		"	gl_Position = transform.viewProjection*vertex;\n"
		"	Vert.texcoord=texcoord;\n"
		"	vec4 kek = transform.normal * vec4(normal, 1);\n"
		"	Vert.normal= vec3(kek.x, kek.y, kek.z);\n"
		"	Vert.lightDir=vec3(lightDir);\n"
		"	Vert.lightDir1=vec3(lightDir1);\n"
		"	Vert.lightDir2=vec3(lightDir2);\n"
		"	Vert.viewDir=transform.viewPosition - vec3(vertex);\n"
		"	vec3 viewDir = normalize(Vert.viewDir);\n"
		"	Vert.distance=length(Vert.lightDir);\n"
		"	Vert.distance1=length(Vert.lightDir1);\n"
		"	Vert.distance2=length(Vert.lightDir2);\n"
		"	vec3 normal = normalize(Vert.normal);\n"
		"	vec3 lightDirn = normalize(Vert.lightDir);\n"
		"	vec3 lightDirn1 = normalize(Vert.lightDir1);\n"
		"	vec3 lightDirn2 = normalize(Vert.lightDir2);\n"
		"	attenuation=1.0/(light.attenuation[0]+\n"
		"	light.attenuation[1]*Vert.distance+\n"
		"	light.attenuation[2]*Vert.distance*Vert.distance);\n"
		"	attenuation1=1.0/(light1.attenuation[0]+\n"
		"	light1.attenuation[1]*Vert.distance1+\n"
		"	light1.attenuation[2]*Vert.distance1*Vert.distance1);\n"
		//расчет прожекторного источника
		"	float spotEffect = dot(normalize(light2.spot_direction), -lightDirn2);\n"
		"	float spot = float(spotEffect>light2.spot_cutoff);\n"
		"	spotEffect =max(pow(spotEffect, light2.spot_exponent), 0.0);\n"
		"	attenuation2 = spot*spotEffect/(light2.attenuation[0]+\n"
		"	light2.attenuation[1]*Vert.distance2+\n"
		"	light2.attenuation[2]*Vert.distance2*Vert.distance2);\n"
		"	Ndot=max(dot(normal, lightDirn),0.0);\n"
		"	Ndot1=max(dot(normal,lightDirn1),0.0);\n"
		"	Ndot2=max(dot(-normal,lightDirn2),0.0);\n"
		"	vec3 h = normalize(lightDirn + viewDir);\n"
		"	vec3 h1 = normalize(lightDirn1 + viewDir);\n"
		"	vec3 h2 = normalize(lightDirn2 + viewDir);\n"
		"	Rdot = max(pow(dot(normal, h), 1), 0.0);\n"
		"	Rdot1 = max(pow(dot(normal, h1), 1), 0.0);\n"
		"	Rdot2 = max(pow(dot(-normal, h2), 1), 0.0);\n"
		"}\n";

	const char* fragment_Source_knot =
		"#version 330 core\n"
		"#define FRAG_OUTPUT0 0\n"
		"layout(location=FRAG_OUTPUT0) out vec4 color;\n"
		"uniform struct PointLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"} light, light1;\n"
		"uniform struct ProjectionLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"	vec3 spot_direction;\n"
		"	float spot_cutoff;\n"
		"	float spot_exponent;\n"
		"} light2;\n"
		"uniform struct Material{\n"
		"	sampler2D texture;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec4 emission;\n"
		"	float shininess;\n"
		"} material;\n"
		"in struct Vertex {\n"
		"	vec2 texcoord;\n"
		"	vec3 normal;\n"
		"	vec3 lightDir;\n"
		"	vec3 lightDir1;\n"
		"	vec3 lightDir2;\n"
		"	vec3 viewDir;\n"
		"	float distance;\n"
		"	float distance1;\n"
		"	float distance2;\n"
		"} Vert;\n"
		"uniform int light0on;\n"
		"uniform int light1on;\n"
		"uniform int light2on;\n"
		"in float attenuation;\n"
		"in float attenuation1;\n"
		"in float attenuation2;\n"
		"in float Ndot;\n"
		"in float Ndot1;\n"
		"in float Ndot2;\n"
		"in float Rdot;\n"
		"in float Rdot1;\n"
		"in float Rdot2;\n"
		"void main(void){\n"
		"	color = material.emission;\n"
		"	if (light0on == 1)\n"
		"		color +=material.ambient*light.ambient*attenuation;\n"
		"	if (light1on == 1)\n"
		"		color +=material.ambient*light1.ambient*attenuation1;\n"
		"	if (light2on == 1)\n"
		"		color +=material.ambient*light2.ambient*attenuation2;\n"
		"	if (light0on == 1)\n"
		"		color += material.diffuse*light.diffuse*Ndot*attenuation; \n"
		"	if (light1on == 1)\n"
		"		color += material.diffuse*light1.diffuse*Ndot1*attenuation1; \n"
		"	if (light2on == 1)\n"
		"		color += material.diffuse*light2.diffuse*Ndot2*attenuation2; \n"
		"	if (light0on == 1)\n"
		"		color+=material.specular*light.specular*Rdot*attenuation;\n"
		"	if (light1on == 1)\n"
		"		color+=material.specular*light1.specular*Rdot1*attenuation1;\n"
		"	if (light2on == 1)\n"
		"		color+=material.specular*light2.specular*Rdot2*attenuation2;\n"
		//"	color=vec4(1, 0, 0, 1);\n"
		"  color*=texture(material.texture,Vert.texcoord);\n"///////////////////////////////

		"}\n";

	const char* fragment_Source_plane =
		"#version 330 core\n"
		"#define FRAG_OUTPUT0 0\n"
		"layout(location=FRAG_OUTPUT0) out vec4 color;\n"
		"uniform struct PointLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"} light, light1;\n"
		"uniform struct ProjectionLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"	vec3 spot_direction;\n"
		"	float spot_cutoff;\n"
		"	float spot_exponent;\n"
		"} light2;\n"
		"uniform struct Material{\n"
		"	sampler2D texture;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec4 emission;\n"
		"	float shininess;\n"
		"} material;\n"
		"in struct Vertex {\n"
		"	vec2 texcoord;\n"
		"	vec3 normal;\n"
		"	vec3 lightDir;\n"
		"	vec3 lightDir1;\n"
		"	vec3 lightDir2;\n"
		"	vec3 viewDir;\n"
		"	float distance;\n"
		"	float distance1;\n"
		"	float distance2;\n"
		"} Vert;\n"
		"uniform int light0on;\n"
		"uniform int light1on;\n"
		"uniform int light2on;\n"
		"void main(void){\n"
		"	vec3 normal = normalize(Vert.normal);\n"
		"	vec3 lightDir = normalize(Vert.lightDir);\n"
		"	vec3 lightDir1 = normalize(Vert.lightDir1);\n"
		"	vec3 lightDir2 = normalize(Vert.lightDir2);\n"
		"	vec3 viewDir = normalize(Vert.viewDir);\n"
		"	float attenuation=1.0/(light.attenuation[0]+\n"
		"	light.attenuation[1]*Vert.distance+\n"
		"	light.attenuation[2]*Vert.distance*Vert.distance);\n"
		"	float attenuation1=1.0/(light1.attenuation[0]+\n"
		"	light1.attenuation[1]*Vert.distance1+\n"
		"	light1.attenuation[2]*Vert.distance1*Vert.distance1);\n"
		//расчет прожекторного источника
		"	float spotEffect = dot(normalize(light2.spot_direction), -lightDir2);\n"
		"	float spot = float(spotEffect>light2.spot_cutoff);\n"
		"	spotEffect =max(pow(spotEffect, light2.spot_exponent), 0.0);\n"
		"	float attenuation2=spot*spotEffect/(light2.attenuation[0]+\n"
		"	light2.attenuation[1]*Vert.distance2+\n"
		"	light2.attenuation[2]*Vert.distance2*Vert.distance2);\n"
		//
		"	color=material.emission;\n"
		"	if (light0on == 1)\n"
		"	color +=material.ambient*light.ambient*attenuation;\n"
		"	if (light1on == 1)\n"
		"		color +=material.ambient*light1.ambient*attenuation1;\n"
		"	if (light2on == 1)\n"
		"		color +=material.ambient*light2.ambient*attenuation2;\n"
		"	float Ndot=max(dot(normal,lightDir),0.0);\n"
		"	float Ndot1=max(dot(normal,lightDir1),0.0);\n"
		"	float Ndot2=max(dot(normal,lightDir2),0.0);\n"
		"	if (light0on == 1)\n"
		"		color += material.diffuse*light.diffuse*Ndot*attenuation; \n"
		"	if (light1on == 1)\n"
		"		color += material.diffuse*light1.diffuse*Ndot1*attenuation1; \n"
		"	if (light2on == 1)\n"
		"		color += material.diffuse*light2.diffuse*Ndot2*attenuation2; \n"
		"	vec3 h = normalize(lightDir + viewDir);\n"
		"	vec3 h1 = normalize(lightDir1 + viewDir);\n"
		"	vec3 h2 = normalize(lightDir2 + viewDir);\n"
		"	float RdotVpow = max(pow(dot(normal, h), material.shininess), 0.0);\n"
		"	float RdotVpow1 = max(pow(dot(normal, h1), material.shininess), 0.0);\n"
		"	float RdotVpow2 = max(pow(dot(normal, h2), material.shininess), 0.0);\n"
		"	if (light0on == 1)\n"
		"	color+=material.specular*light.specular*RdotVpow*attenuation;\n"
		"	if (light1on == 1)\n"
		"		color+=material.specular*light1.specular*RdotVpow1*attenuation1;\n"
		"	if (light2on == 1)\n"
		"		color+=material.specular*light2.specular*RdotVpow2*attenuation2;\n"
		//"	color*=vec4(1, 0, 0, 1);\n"
		"  color*=texture(material.texture,Vert.texcoord);\n"///////////////////////////////
		"}\n";

	const char* fragment_Source_cube =
		"#version 330 core\n"
		"#define FRAG_OUTPUT0 0\n"
		"layout(location=FRAG_OUTPUT0) out vec4 color;\n"
		"uniform struct PointLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"} light, light1;\n"
		"uniform struct ProjectionLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"	vec3 spot_direction;\n"
		"	float spot_cutoff;\n"
		"	float spot_exponent;\n"
		"} light2;\n"
		"uniform struct Material{\n"
		"	sampler2D texture;\n"
		"	sampler2D texture1;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec4 emission;\n"
		"	float shininess;\n"
		"} material;\n"
		"in struct Vertex {\n"
		"	vec2 texcoord;\n"
		"	vec3 normal;\n"
		"	vec3 lightDir;\n"
		"	vec3 lightDir1;\n"
		"	vec3 lightDir2;\n"
		"	vec3 viewDir;\n"
		"	float distance;\n"
		"	float distance1;\n"
		"	float distance2;\n"
		"} Vert;\n"
		"uniform int light0on;\n"
		"uniform int light1on;\n"
		"uniform int light2on;\n"
		"uniform float textcoef;\n"
		"void main(void){\n"
		"	vec3 normal = normalize(Vert.normal);\n"
		"	vec3 lightDir = normalize(Vert.lightDir);\n"
		"	vec3 lightDir1 = normalize(Vert.lightDir1);\n"
		"	vec3 lightDir2 = normalize(Vert.lightDir2);\n"
		"	vec3 viewDir = normalize(Vert.viewDir);\n"
		"	float attenuation=1.0/(light.attenuation[0]+\n"
		"	light.attenuation[1]*Vert.distance+\n"
		"	light.attenuation[2]*Vert.distance*Vert.distance);\n"
		"	float attenuation1=1.0/(light1.attenuation[0]+\n"
		"	light1.attenuation[1]*Vert.distance1+\n"
		"	light1.attenuation[2]*Vert.distance1*Vert.distance1);\n"
		//расчет прожекторного источника
		"	float spotEffect = dot(normalize(light2.spot_direction), -lightDir2);\n"
		"	float spot = float(spotEffect>light2.spot_cutoff);\n"
		"	spotEffect =max(pow(spotEffect, light2.spot_exponent), 0.0);\n"
		"	float attenuation2=spot*spotEffect/(light2.attenuation[0]+\n"
		"	light2.attenuation[1]*Vert.distance2+\n"
		"	light2.attenuation[2]*Vert.distance2*Vert.distance2);\n"
		//
		"	color=material.emission;\n"
		"	if (light0on == 1)\n"
		"	color +=material.ambient*light.ambient*attenuation;\n"
		"	if (light1on == 1)\n"
		"		color +=material.ambient*light1.ambient*attenuation1;\n"
		"	if (light2on == 1)\n"
		"		color +=material.ambient*light2.ambient*attenuation2;\n"
		"	float Ndot=max(dot(normal,lightDir),0.0);\n"
		"	float Ndot1=max(dot(normal,lightDir1),0.0);\n"
		"	float Ndot2=max(dot(-normal,lightDir2),0.0);\n"
		"	if (light0on == 1)\n"
		"		color += material.diffuse*light.diffuse*Ndot*attenuation; \n"
		"	if (light1on == 1)\n"
		"		color += material.diffuse*light1.diffuse*Ndot1*attenuation1; \n"
		"	if (light2on == 1)\n"
		"		color += material.diffuse*light2.diffuse*Ndot2*attenuation2; \n"
		"	vec3 h = normalize(lightDir + viewDir);\n"
		"	vec3 h1 = normalize(lightDir1 + viewDir);\n"
		"	vec3 h2 = normalize(lightDir2 + viewDir);\n"
		"	float RdotVpow = max(pow(dot(normal, h), material.shininess), 0.0);\n"
		"	float RdotVpow1 = max(pow(dot(normal, h1), material.shininess), 0.0);\n"
		"	float RdotVpow2 = max(pow(dot(-normal, h2), material.shininess), 0.0);\n"
		"	if (light0on == 1)\n"
		"	color+=material.specular*light.specular*RdotVpow*attenuation;\n"
		"	if (light1on == 1)\n"
		"		color+=material.specular*light1.specular*RdotVpow1*attenuation1;\n"
		"	if (light2on == 1)\n"
		"		color+=material.specular*light2.specular*RdotVpow2*attenuation2;\n"
		//"	color*=vec4(1, 0, 0, 1);\n"
		"  color*=mix(texture(material.texture,Vert.texcoord),texture(material.texture1,Vert.texcoord), textcoef);\n"
		//"  color*=texture(material.texture,Vert.texcoord);\n"///////////////////////////////
		"}\n";

	const char* fragment_Source_sphere =
		"#version 330 core\n"
		"#define FRAG_OUTPUT0 0\n"
		"layout(location=FRAG_OUTPUT0) out vec4 color;\n"
		"uniform struct PointLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"} light, light1;\n"
		"uniform struct ProjectionLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"	vec3 spot_direction;\n"
		"	float spot_cutoff;\n"
		"	float spot_exponent;\n"
		"} light2;\n"
		"uniform struct Material{\n"
		"	sampler2D texture;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec4 emission;\n"
		"	float shininess;\n"
		"} material;\n"
		"in struct Vertex {\n"
		"	vec2 texcoord;\n"
		"	vec3 normal;\n"
		"	vec3 lightDir;\n"
		"	vec3 lightDir1;\n"
		"	vec3 lightDir2;\n"
		"	vec3 viewDir;\n"
		"	float distance;\n"
		"	float distance1;\n"
		"	float distance2;\n"
		"} Vert;\n"
		"uniform int light0on;\n"
		"uniform int light1on;\n"
		"uniform int light2on;\n"
		"uniform float textcoef;\n"
		"void main(void){\n"
		"	vec3 normal = normalize(Vert.normal);\n"
		"	vec3 lightDir = normalize(Vert.lightDir);\n"
		"	vec3 lightDir1 = normalize(Vert.lightDir1);\n"
		"	vec3 lightDir2 = normalize(Vert.lightDir2);\n"
		"	vec3 viewDir = normalize(Vert.viewDir);\n"
		"	float attenuation=1.0/(light.attenuation[0]+\n"
		"	light.attenuation[1]*Vert.distance+\n"
		"	light.attenuation[2]*Vert.distance*Vert.distance);\n"
		"	float attenuation1=1.0/(light1.attenuation[0]+\n"
		"	light1.attenuation[1]*Vert.distance1+\n"
		"	light1.attenuation[2]*Vert.distance1*Vert.distance1);\n"
		//расчет прожекторного источника
		"	float spotEffect = dot(normalize(light2.spot_direction), -lightDir2);\n"
		"	float spot = float(spotEffect>light2.spot_cutoff);\n"
		"	spotEffect =max(pow(spotEffect, light2.spot_exponent), 0.0);\n"
		"	float attenuation2=spot*spotEffect/(light2.attenuation[0]+\n"
		"	light2.attenuation[1]*Vert.distance2+\n"
		"	light2.attenuation[2]*Vert.distance2*Vert.distance2);\n"
		//
		"	color=material.emission;\n"
		"	if (light0on == 1)\n"
		"	color +=material.ambient*light.ambient*attenuation;\n"
		"	if (light1on == 1)\n"
		"		color +=material.ambient*light1.ambient*attenuation1;\n"
		"	if (light2on == 1)\n"
		"		color +=material.ambient*light2.ambient*attenuation2;\n"
		"	float Ndot=max(dot(normal,lightDir),0.0);\n"
		"	float Ndot1=max(dot(normal,lightDir1),0.0);\n"
		"	float Ndot2=max(dot(-normal,lightDir2),0.0);\n"
		"	if (light0on == 1)\n"
		"		color += material.diffuse*light.diffuse*Ndot*attenuation; \n"
		"	if (light1on == 1)\n"
		"		color += material.diffuse*light1.diffuse*Ndot1*attenuation1; \n"
		"	if (light2on == 1)\n"
		"		color += material.diffuse*light2.diffuse*Ndot2*attenuation2; \n"
		"	vec3 h = normalize(lightDir + viewDir);\n"
		"	vec3 h1 = normalize(lightDir1 + viewDir);\n"
		"	vec3 h2 = normalize(lightDir2 + viewDir);\n"
		"	float RdotVpow = max(pow(dot(normal, h), material.shininess), 0.0);\n"
		"	float RdotVpow1 = max(pow(dot(normal, h1), material.shininess), 0.0);\n"
		"	float RdotVpow2 = max(pow(dot(-normal, h2), material.shininess), 0.0);\n"
		"	if (light0on == 1)\n"
		"	color+=material.specular*light.specular*RdotVpow*attenuation;\n"
		"	if (light1on == 1)\n"
		"		color+=material.specular*light1.specular*RdotVpow1*attenuation1;\n"
		"	if (light2on == 1)\n"
		"		color+=material.specular*light2.specular*RdotVpow2*attenuation2;\n"
		//"	color*=vec4(1, 0, 0, 1);\n"
		"  color*=mix(texture(material.texture,Vert.texcoord), vec4(0.1,1,0.1,1), textcoef);\n"
		//"  color*=texture(material.texture,Vert.texcoord);\n"///////////////////////////////
		"}\n";

	

	const char* toon_fragment_Source = 
		"#version 330 core\n"
		"#define FRAG_OUTPUT0 0\n"
		"layout(location=FRAG_OUTPUT0) out vec4 color;\n"
		"uniform struct PointLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"} light, light1;\n"
		"uniform struct Material{\n"
		"	sampler2D texture;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec4 emission;\n"
		"	float shininess;\n"
		"} material;\n"
		"in struct Vertex {\n"
		"	vec2 texcoord;\n"
		"	vec3 normal;\n"
		"	vec3 lightDir;\n"
		"	vec3 lightDir1;\n"
		"	vec3 viewDir;\n"
		"	float distance;\n"
		"	float distance1;\n"
		"} Vert;\n"
		"void main(void){\n"
		"	vec3 normal = normalize(Vert.normal);\n"
		"	vec3 lightDir = normalize(Vert.lightDir);\n"
		"	vec3 viewDir = normalize(Vert.viewDir);\n"
		"	float diff = 0.2 + max(dot(normal, lightDir), 0.0);\n"
		"	if (diff < 0.4)\n"
		"		color = material.diffuse * 0.3f;\n"
		"	else if (diff < 0.7)\n"
		"		color = material.diffuse;\n"
		"	else color = material.diffuse * 1.3f;\n"
		"	color*=vec4(1, 0, 0, 1);\n"
		//"color*=texture(material.texture,Vert.texcoord);\n"
		"}\n";

	const char* gachi_fragment_Source =
		"#version 330 core\n"
		"#define FRAG_OUTPUT0 0\n"
		"layout(location=FRAG_OUTPUT0) out vec4 color;\n"
		"in vec4 coolColor;\n"
		"in vec4 warmColor;\n"
		"uniform struct PointLight{\n"
		"	vec4 position;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	vec4 specular;\n"
		"	vec3 attenuation;\n"
		"} light, light1;\n"
		"uniform struct Material{\n"
		"	sampler2D texture;\n"
		"	vec4 ambient;\n"
		"	vec4 diffuse;\n"
		"	float diffuseCool;\n"
		"	float diffuseWarm;\n"
		"	vec4 specular;\n"
		"	vec4 emission;\n"
		"	float shininess;\n"
		"} material;\n"
		"in struct Vertex {\n"
		"	vec2 texcoord;\n"
		"	vec3 normal;\n"
		"	vec3 lightDir;\n"
		"	vec3 lightDir1;\n"
		"	vec3 viewDir;\n"
		"	float distance;\n"
		"	float distance1;\n"
		"} Vert;\n"
		"void main(void){\n"
		"	vec3 normal = normalize(Vert.normal);\n"
		"	vec3 lightDir = normalize(Vert.lightDir);\n"
		"	vec3 viewDir = normalize(Vert.viewDir);\n"
		"	vec3 reflectDir = normalize(reflect(-lightDir, normal));\n" // check it again
		"	vec4 coolColor = vec4(0, 0, 0.7f, 1);\n"
		"	vec4 warmColor = vec4(0.7f, 0, 0, 1);\n"
		"	color = vec4(0, 0.7f, 0, 1);\n"
		"	float Ndot = max(dot(normal,lightDir),0.0);\n"
		"	vec4 kCoolColor = coolColor + material.diffuseCool * material.diffuse;\n"
		"	vec3 kCool = vec3(min(kCoolColor.x, 1), min(kCoolColor.y, 1), min(kCoolColor.z, 1));\n"
		"	vec4 kWarmColor = warmColor + material.diffuseWarm * material.diffuse;\n"
		"	vec3 kWarm = vec3(min(kWarmColor.x, 1), min(kWarmColor.y, 1), min(kWarmColor.z, 1));\n"
		"	vec3 kFinal = mix(kCool, kWarm, Ndot);\n"
		//"	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 1000.0);\n"
		"	vec3 finalColor = kFinal;\n"
		"	color = vec4(min(finalColor.x, 1.0), min(finalColor.y, 1.0), min(finalColor.z, 1.0), 1.0);\n"
		//"color*=texture(material.texture,Vert.texcoord);\n"
		"}\n";
	
	default_shader = GLShader();
	default_shader.load(vertex_Source, fragment_Source);

	plane_shader = GLShader();
	plane_shader.load(vertex_Source, fragment_Source_plane);

	cube_shader = GLShader();
	cube_shader.load(vertex_Source, fragment_Source_cube);

	knot_shader = GLShader();
	knot_shader.load(vertex_Source_knot, fragment_Source_knot);

	sphere_shader = GLShader();
	sphere_shader.load(vertex_Source, fragment_Source_sphere);

	toon_shader = GLShader();
	toon_shader.load(vertex_Source, toon_fragment_Source);
	auto kek = glm::vec3(0, 0, 0) + 3.0f;
	gachi_muchi_shader = GLShader();
	gachi_muchi_shader.load(vertex_Source, gachi_fragment_Source);
	
	initVBO();
}
void resizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);
}

void render2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto roty = glm::radians(rotate_y_light);
	//y
	glm::mat4 rotate_mat = glm::mat4(cos(roty), 0, sin(roty), 0,
		0, 1, 0, 0,
		-sin(roty), 0, cos(roty), 0,
		0, 0, 0, 1);

	auto roty_2 = glm::radians(proj_light_rot[1]);
	glm::mat4 rotate_mat_2 = glm::mat4(cos(roty_2), 0, sin(roty_2), 0,
		0, 1, 0, 0,
		-sin(roty_2), 0, cos(roty_2), 0,
		0, 0, 0, 1);
	glm::mat4 translate_mat = glm::mat4(1, 0, 0, positions[0][0],
		0, 1, 0, positions[0][1],
		0, 0, 1, positions[0][2],
		0, 0, 0, 1);

	glm::vec4 light_base_pos = glm::vec4(proj_light_pos[0], proj_light_pos[1], proj_light_pos[2], 1);

	
	//light_base_pos = rotate_mat_2 * light_base_pos;

	//light_base_pos = translate_mat * light_base_pos;
	light_base_pos = rotate_mat * light_base_pos;

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0, 9, -15), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 startModel = glm::mat4(1.0f);
	startModel = glm::rotate(startModel, glm::radians((float)rotate_x), glm::vec3(1, 0, 0));
	startModel = glm::rotate(startModel, glm::radians((float)rotate_y), glm::vec3(0, 1, 0));
	startModel = glm::rotate(startModel, glm::radians((float)rotate_z), glm::vec3(0, 0, 1));
	for (int a = 0; a < object_count; a++)
	{
		glm::mat4 Model = startModel;
		Model = glm::translate(Model, positions[a]);
		Model = glm::scale(Model, scales[a]);
		Model = glm::rotate(Model, glm::radians((float)rotations[a][0]), glm::vec3(1, 0, 0));
		Model = glm::rotate(Model, glm::radians((float)rotations[a][1]), glm::vec3(0, 1, 0));
		Model = glm::rotate(Model, glm::radians((float)rotations[a][2]), glm::vec3(0, 0, 1));
		glm::mat4 MVP = Projection * View * Model;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(View * Model));
		glm::mat4 viewProjection = Projection * View;
		GLfloat viewPos[] = { 11, 9, 9 };

		GLfloat lightPos[] = { 0, 4, 0, 1 };
		GLfloat ambient[] = { 1, 1, 1 , 1 };
		GLfloat diffuse[] = { 1, 1, 1, 1 };
		GLfloat specular[] = { 1, 1, 1, 1 };
		GLfloat attenuation[] = { 0.5f, 0.5f, 0.5f };
		
		GLfloat lightPos1[] = { 0, 4, 7, 1 };
		GLfloat ambient1[] = { 0.5, 0.5, 0.5 , 1 };
		GLfloat diffuse1[] = { 0.5, 0.5, 0.5, 1 };
		GLfloat specular1[] = { 1, 1, 1, 1 };
		GLfloat attenuation1[] = { 0.5f, 0.5f, 0.5f };

		GLfloat ambient2[] = { 0.8, 0, 0 , 1 };
		GLfloat diffuse2[] = { 1.5, 0, 0, 1 };
		GLfloat specular2[] = { 1, 1, 1, 1 };
		GLfloat attenuation2[] = { 0.5f, 0.5f, 0.5f };
		//GLfloat spot_direction2[] = { glm::radians((float)proj_light_rot[0]), glm::radians((float)proj_light_rot[1]), glm::radians((float)proj_light_rot[2]) };
		
		glm::vec4 light_base = glm::vec4(1, 0, 0, 1);
		
		auto x = rotate_mat*light_base;
		x = rotate_mat_2 * x;
		//auto y = translate_mat * light_base_pos;
		GLfloat spot_direction2[] = { x[0], x[1], x[2] };
		GLfloat lightPos2[] = { light_base_pos[0], light_base_pos[1], light_base_pos[2], 1 };
		
		GLfloat spot_cutoff2 = 0.95;
		GLfloat spot_exponent2 = 10;
		

		GLfloat* ambient_mat = ambient_mat_arr[a];
		GLfloat* diffuse_mat = diffuse_mat_arr[a];
		GLfloat* specular_mat = specular_mat_arr[a];
		GLfloat* emission_mat = emission_mat_arr[a];
		GLfloat shininess = 1;

		GLfloat toon_diffuse_mat[] = { 0.5f, 0.8, 0.8, 1 };

		GLfloat diffuseCool = 0.8f;
		GLfloat diffuseWarm = 0.4f;
		GLfloat gachi_diffuse[] = { 0.2f, 0.6f, 0.2f, 1 };
		GLfloat coolColor[] = { 0, 0 , 0.2f, 1 };
		GLfloat warmColor[] = { 0.2f, 0 , 0, 1 };
		if (a == 1)
			shader = cube_shader;
		else if (a == 2)
			shader = plane_shader;
		else if (a == 3)
			shader = sphere_shader;
		else if (a == 4 && knot)
			shader = knot_shader;
		else
			shader = default_shader;

		shader.use();

		glUniformMatrix4fv(shader.getUniformLocation("transform.model"), 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(shader.getUniformLocation("transform.viewProjection"), 1, GL_FALSE, &viewProjection[0][0]);
		glUniformMatrix4fv(shader.getUniformLocation("transform.normal"), 1, GL_FALSE, &normalMatrix[0][0]);
		glUniform3fv(shader.getUniformLocation("transform.viewPosition"), 1, viewPos);

		glUniform4fv(shader.getUniformLocation("light.position"), 1, lightPos);
		glUniform4fv(shader.getUniformLocation("light.ambient"), 1, ambient);
		glUniform4fv(shader.getUniformLocation("light.diffuse"), 1, diffuse);
		glUniform4fv(shader.getUniformLocation("light.specular"), 1, specular);
		glUniform3fv(shader.getUniformLocation("light.attenuation"), 1, attenuation);
		glUniform1f(shader.getUniformLocation("light.shininess"), shininess);
		glUniform1i(shader.getUniformLocation("light0on"), light0on);

		glUniform4fv(shader.getUniformLocation("light1.position"), 1, lightPos1);
		glUniform4fv(shader.getUniformLocation("light1.ambient"), 1, ambient1);
		glUniform4fv(shader.getUniformLocation("light1.diffuse"), 1, diffuse1);
		glUniform4fv(shader.getUniformLocation("light1.specular"), 1, specular1);
		glUniform3fv(shader.getUniformLocation("light1.attenuation"), 1, attenuation1);
		glUniform1f(shader.getUniformLocation("light1.shininess"), shininess);
		glUniform1i(shader.getUniformLocation("light1on"), light1on);

		glUniform4fv(shader.getUniformLocation("light2.position"), 1, lightPos2);
		glUniform4fv(shader.getUniformLocation("light2.ambient"), 1, ambient2);
		glUniform4fv(shader.getUniformLocation("light2.diffuse"), 1, diffuse2);
		glUniform4fv(shader.getUniformLocation("light2.specular"), 1, specular2);
		glUniform3fv(shader.getUniformLocation("light2.spot_direction"), 1, spot_direction2);
		glUniform3fv(shader.getUniformLocation("light2.attenuation"), 1, attenuation2);
		glUniform1f(shader.getUniformLocation("light2.shininess"), shininess);
		glUniform1f(shader.getUniformLocation("light2.spot_cutoff"), spot_cutoff2);
		glUniform1f(shader.getUniformLocation("light2.spot_exponent"), spot_exponent2);
		glUniform1i(shader.getUniformLocation("light2on"), light2on);

		glUniform4fv(shader.getUniformLocation("material.ambient"), 1, ambient_mat);
		switch (cur_type)
		{
		case def:
			glUniform4fv(shader.getUniformLocation("material.diffuse"), 1, diffuse_mat);
			break;
		case toon:
			glUniform4fv(shader.getUniformLocation("material.diffuse"), 1, toon_diffuse_mat);
			break;
		case gachi:
			glUniform4fv(shader.getUniformLocation("material.diffuse"), 1, gachi_diffuse);
			glUniform1f(shader.getUniformLocation("material.diffuseCool"), diffuseCool);
			glUniform1f(shader.getUniformLocation("material.diffuseWarm"), diffuseWarm);
			glUniform4fv(shader.getUniformLocation("coolColor"), 1, coolColor);
			glUniform4fv(shader.getUniformLocation("warmColor"), 1, warmColor);
			break;
		default:
			break;
		}



		glUniform4fv(shader.getUniformLocation("material.specular"), 1, specular_mat);
		glUniform4fv(shader.getUniformLocation("material.emission"), 1, emission_mat);
		glUniform1f(shader.getUniformLocation("material.shininess"), shininess);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[a]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glUniform1i(shader.getUniformLocation("material.texture"), 0);

		if (a == 1)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textures[5]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glUniform1i(shader.getUniformLocation("material.texture1"), 1);

			glUniform1f(shader.getUniformLocation("textcoef"), textcoef);
		}
		else if (a == 3)
			glUniform1f(shader.getUniformLocation("textcoef"), textcoef);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex[a]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_texture[a]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_normal[a]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, indicies_count[a]);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		shader.off();
	}
	
	checkOpenGLerror();
	glFlush();
	glutSwapBuffers();
}
void specialKeys(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT: positions[0][0] += 0.5; proj_light_pos[0] += 0.5; break;
	case GLUT_KEY_RIGHT: positions[0][0] -= 0.5; proj_light_pos[0] -= 0.5; break;
	case GLUT_KEY_UP: positions[0][2] += 0.5; proj_light_pos[2] += 0.5; break;
	case GLUT_KEY_DOWN: positions[0][2] -= 0.5; proj_light_pos[2] -= 0.5; break;
	case GLUT_KEY_PAGE_UP: proj_light_rot[1] += 5; rotations[0][1] -= 5;  break;
	case GLUT_KEY_PAGE_DOWN: proj_light_rot[1] -= 5; rotations[0][1] += 5; break;
	case GLUT_KEY_F1: light0on = (light0on + 1) % 2; break; //свет точечный первый
	case GLUT_KEY_F2: light1on = (light1on + 1) % 2; break; //свет точечный второй
	case GLUT_KEY_F3: light2on = (light2on + 1) % 2; break; //свет прожекторный
	case GLUT_KEY_F4: knot = !knot; break;
	case GLUT_KEY_F5: rotate_y += 5; rotate_y_light -= 5; break;
	case GLUT_KEY_F6: rotate_y -= 5; rotate_y_light += 5; break;
	case GLUT_KEY_F7: textcoef >=0.1 ? textcoef-=0.1 : textcoef = 0; break;
	case GLUT_KEY_F8: textcoef <= 0.9 ? textcoef += 0.1 : textcoef = 1; break;
	}
	glutPostRedisplay();

}

void loadTextures()
{
	textures[0] = SOIL_load_OGL_texture(texture_path[0].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[1] = SOIL_load_OGL_texture(texture_path[1].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[2] = SOIL_load_OGL_texture(texture_path[2].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[3] = SOIL_load_OGL_texture(texture_path[3].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[4] = SOIL_load_OGL_texture(texture_path[4].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	textures[5] = SOIL_load_OGL_texture(texture_path[5].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
}
int main(int argc, char **argv)
{
	filename = new string[5];
	indicies_count = new int[5];

	initArrays();
	VBO_vertex = new GLuint[5];
	VBO_normal = new GLuint[5];
	VBO_texture = new GLuint[5];
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Simple shaders");
	glClearColor(0, 0, 0, 0);
	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status)
	{
		std::cout << "Error: " << glewGetErrorString(glew_status) << "\n";
		return 1;
	}
	if (!GLEW_VERSION_2_0)
	{
		std::cout << "No support for OpenGL 2.0 found\n";
		return 1;
	}
	cur_type = def;
	glEnable(GL_DEPTH_TEST);
	loadTextures();
	initShader();
	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(render2);
	glutSpecialFunc(specialKeys);
	glutMainLoop();
	freeVBO();
}