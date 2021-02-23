#pragma once
#include<Windows.h>
#include<cstring>
#include <vector>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "GL/freeglut.h"

using namespace std;

class Grafica {
	bool init_succesful = false;

	GLFWwindow* window;
	GLuint fbo, render_buf[2];
	int width = 300;
	int height = 300;
	char* buffer=NULL;

	GLfloat light_diffuse[4] = { 1.0, 0.0, 0.0, 1.0 };  /* Red diffuse light. */
	GLfloat light_position[4] = { 1.0, 1.0, 1.0, 0.0 };  /* Infinite light location. */
	GLfloat n[6][3] = {  /* Normals for the 6 faces of a cube. */
	  {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
	  {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };
	GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
	  {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
	  {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
	GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertexes. */


	void InitScene();
	void drawBox();
public:
	Grafica();
	~Grafica();

	bool Init();
	void Destroy();

	void setSize(int, int);
	char* getBuffer() { return buffer; }

	void nextScence();
};