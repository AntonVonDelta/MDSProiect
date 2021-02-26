#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include "Grafica.h"

using namespace std;

Grafica render;

int main() {
	if (render.Init()){
		cout<<"OpenGL initialized succesfully!"<<endl;
	} else {
		cout<<"OpenGL error!"<<endl;
	}

	if (render.IsInitialized()) {
		while (true) {
			int choice;
			cout << "0 - Move back  | 1 - Move front" << endl;
			cout << "2 - Move left | 3 - Move right" << endl;
			cout << " 6 - Nothin'" << endl;

			cin >> choice;
			render.moveScene(choice);

			// Draw on internal buffers the next scene
			render.nextScence();

			ofstream fo("img.bin", ofstream::binary | ofstream::trunc);
			fo.write(render.getBuffer(), render.getBufferSize());
			fo.flush();
		}

	}

	render.Destroy();
	return 0;
}