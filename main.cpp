#include "Starter.h"

using namespace std;

int main(int argc, char *argv[]) {
    Starter starter = Starter();

	// Full mode
	if (argc == 1) {
		starter.readPalmColor();
		//starter.addPattern();
		starter.recognize();
	}

	// Add mode
	if (argc == 2 && strcmp(argv[1], "add") == 0) {
		starter.readPalmColor();
		starter.addPattern();
	}

	// Recognizing mode
	if (argc == 2 && strcmp(argv[1], "rec") == 0) {
		starter.readPalmColor();
		starter.recognize();
	}

	return 0;
}