#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include "Grafica.h"

using namespace std;

Grafica render;

int test1(int a, int b) { return a + b; }

int main() {
	if (render.Init()){
		cout<<"OpenGL initialized succesfully!"<<endl;
	} else {
		cout<<"OpenGL error!"<<endl;
	}

	if (render.IsInitialized()) {
		// Draw on internal buffers the next scene
		render.nextScence();

		ofstream fo("img.bin", ofstream::binary | ofstream::trunc);
		fo.write(render.getBuffer(),render.getBufferSize());
		fo.flush();
	}

	render.Destroy();
	return 0;
}