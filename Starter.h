#ifndef GESTURESPICKER_RUNNER_H
#define GESTURESPICKER_RUNNER_H


#include "data.hpp"
#include "handGesture.hpp"
#include <vector>

enum State {PAUSE, WORKING};

class Starter {
public:
	Starter() : state(WORKING) { m.src = 0; col_detectors.clear();  };

    void readPalmColor();

    void recognize();

    void addPattern();

private:
    void waitForPalmCover(FramesHolder *m);

    void showWindows(FramesHolder m);

    void initTrackbars();

    VideoCapture camera;
    State state;

    FramesHolder m;
    HandGesture hg;
	std::vector<ColSensor> col_detectors;
};


#endif //GESTURESPICKER_RUNNER_H
