#pragma once
#include<Windows.h>
#include<cstring>
#include <vector>
#include<string>
#include <sstream>
#include <algorithm>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "GL/freeglut.h"

using namespace std;

struct Vertex {
	float x, y, z;
};

struct Triangle {
	Vertex v[3];
};

class Grafica {
	bool init_succesful = false;

	float move_amount = 1;
	float rotate_amount = 10;

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

	vector<Triangle> object_definition;

	void initScene();
	void drawBox();
public:
	Grafica();
	~Grafica();

	/// <summary>
	/// Create context for openGL. This creates a hidden window; no drawing is performed on it
	/// </summary>
	/// <returns>True if the initialization succeds</returns>
	bool init();

	void destroy();
	/// <summary>
	/// </summary>
	/// <returns>True if the class was succesfully initialized</returns>
	bool isInitialized() { return init_succesful; }

	void setSize(int, int);
	/// <summary>
	/// Get pointer to internal memory buffer. Do not delete this memory!
	/// </summary>
	/// <returns>The buffer address</returns>
	char* getBuffer() { return buffer; }

	/// <summary>
	/// Get the memory buffer size according to the set render size
	/// </summary>
	/// <returns>The size of the buffer in bytes</returns>
	int getBufferSize() {
		return width * height * 4;
	}

	/// <summary>
	/// Advances the scene. Also copies the video memory to the output buffer
	/// </summary>
	void nextScence();

	/// <summary>
	/// Translates/moves the scene
	/// </summary>
	void moveScene(int direction);

	/// <summary>
	/// Rotates the scene
	/// </summary>
	/// <param name="direction"></param>
	void rotateScene(int direction);

	void loadObject(string input);
};