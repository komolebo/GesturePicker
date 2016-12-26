#ifndef GESTURESPICKER_FUNCTIONS_H
#define GESTURESPICKER_FUNCTIONS_H


#include "string"
#include "data.hpp"
#include "handGesture.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

/*
 * Processing functions
 */
float distanceP2P(Point a, Point b);

float getAngle(Point s, Point f, Point e);

void normalizeColors(int c_lower[N_COLOR_SENSORS][3],
                     int c_upper[N_COLOR_SENSORS][3],
                     int avg_colors[N_COLOR_SENSORS][3]);

int getMedian(vector<int> &val);

void getAverageColor(const ColSensor &sensor, int *avg);

void filterByPalmColor(FramesHolder *m, Sensors c_lower, Sensors c_upper, Sensors avg_colors);

int findBiggestContour(const vector<Contour> &contours);

void makeContours(FramesHolder *m, HandGesture* hg);

Figure convertPattern2Figure(const Contour &pattern);

float compareFigures(const Figure &fig1, const Figure &fig2);

Figure *normalize(const Figure &fig);

void placePicture2Origin(Contour &picture);

void orderContourFromOrigin(Contour &);

/*
 * IO functions
 */
void appendFile(const Contour &pattern);

void readFile(vector<Contour> &patterns);

void dumpMessage(Mat src, string text);

#endif
