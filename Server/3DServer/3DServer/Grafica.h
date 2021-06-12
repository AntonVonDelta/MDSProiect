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
	GLfloat x, y, z;
};
struct Normal {
	GLfloat x, y, z;
};
struct Triangle {
	bool hasNormal;
	Normal normals[3];
	Vertex v[3];
};

class Grafica {
protected:
	bool init_succesful = false;

	Normal move_vector;
	Normal rotate_vector;

	GLFWwindow* window;
	GLuint fbo, render_buf[2];
	int width = 300;
	int height = 300;
	char* buffer=NULL;

	GLfloat light_diffuse[4] = { 1.0, 0.0, 0.0, 1.0 };  /* Red diffuse light. */
	GLfloat light_position[4] = { 1.0, 1.0, 1.0, 0.0 };  /* Infinite light location. */
	vector<Triangle> object_definition;

	void initScene();
	void drawScene();
public:
	Grafica();
	~Grafica();
	Grafica(const Grafica& other);

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
	int getWidth();
	int getHeight();

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
	void nextScene();

	/// <summary>
	/// Translates/moves the scene
	/// </summary>
	void moveScene(int direction, float amount);

	/// <summary>
	/// Rotates the scene
	/// </summary>
	/// <param name="direction"></param>
	void rotateScene(int direction,float amount);

	// Loads the object given
	// The format is plain ascii string following the ascii standard
	// This function throws exception if the format is malformed. The what() function provides user-readeable code
	void loadObject(string input);
};