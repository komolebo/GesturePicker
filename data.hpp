#ifndef GESTURESPICKER_DATA_H
#define GESTURESPICKER_DATA_H


#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

#define PI 3.14159
#define ORIGCOL2COL CV_BGR2HLS
#define COL2ORIGCOL CV_HLS2BGR

#define N_COLOR_SENSORS         7
#define SENSOR_RECT_LEN         20
#define FRAMES_WAIT_FOR_PALM    80
#define FRAMES_READ_AVER_COL    30
#define SUCCESS_FRAMES_PER_SEC  5
#define SUCCESS_LIKENESS        0.1

using namespace cv;
using namespace std;


typedef int Sensors[N_COLOR_SENSORS][3];
typedef vector<Point>        Contour;
typedef vector<CvPoint2D32f> Figure;

static const char *const MAIN_WINDOW_NAME     = "Main";
static const char *const TRACKBAR_WINDOW_NAME = "Control";
static const char *const TEST_WINDOW          = "Test_window";
static vector<Contour> patterns;
static vector<Figure > figures;

static Sensors avg_colors;
static Sensors c_lower, c_upper;

static vector<string> IpcMessages;


struct FramesHolder{
    Mat srcBlur, src, bw;
    vector<Mat> bwList;
};


struct ColSensor{
    Point upper_corner, lower_corner;
    Mat detect_area;
    const Scalar color = Scalar(0,255,0);
    const int BORDER_THICKNESS = 2;


    ColSensor();
    ColSensor(Point upper_corner, Point lower_corner, Mat src);

    void drawRectangle(Mat src);
};

#endif
