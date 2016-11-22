#include "opencv2/imgproc/imgproc.hpp"
#include "Starter.h"

using namespace cv;
using namespace std;


int main() {
    Starter starter = Starter();

    starter.readPalmColor();

    starter.addPattern();

    starter.recognize();
}
