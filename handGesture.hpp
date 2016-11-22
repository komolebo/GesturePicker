#ifndef GESTURESPICKER_HAND_GESTURE_H
#define GESTURESPICKER_HAND_GESTURE_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "data.hpp"


using namespace cv;
using namespace std;


class HandGesture {
public:
    HandGesture(){};

    Contour         contour;
    vector<int>     hullI;
    Contour         hullP;
    vector<Vec4i>   defects;
    Contour         fingerTips;

    Contour palmContour;
    Figure  palmFigure;


    void initVectors();

    void analyzeContour(Contour &);

    void analyzeBRect();

    void formPalmContour();

    void draw(FramesHolder *m);

    bool isHand() const { return mayBeHand; }

private:
    void removeRedundantEndPoints(vector<Vec4i> newDefects);

    void eliminateDefects();

    bool mayBeHand;
    Rect bRect;
};


#endif
