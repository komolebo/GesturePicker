#ifndef GESTURESPICKER_RUNNER_H
#define GESTURESPICKER_RUNNER_H


#include "data.hpp"
#include "handGesture.hpp"

enum State {PAUSE, WORKING};

class Starter {
public:
    Starter() : state(WORKING) {m.src = 0; };

    void readPalmColor();

    void recognize();

    void addPattern();

private:
    void waitForPalmCover(FramesHolder *m, vector<ColSensor> &col_detectors);

    void showWindows(FramesHolder m);

    void initTrackbars();

    VideoCapture camera;
    State state;

    FramesHolder m;
    HandGesture hg;
};


#endif //GESTURESPICKER_RUNNER_H
