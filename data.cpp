#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include "data.hpp"

using namespace cv;
using namespace std;

ColSensor::ColSensor(){
	upper_corner=Point(0,0);
	lower_corner=Point(0,0);
}

ColSensor::ColSensor(Point u_corner, Point l_corner, Mat src){
	upper_corner=u_corner;
	lower_corner=l_corner;
	detect_area=src(Rect(u_corner.x, u_corner.y, l_corner.x-u_corner.x,l_corner.y-u_corner.y));
}

void ColSensor::drawRectangle(Mat src){
	rectangle(src,upper_corner,lower_corner,color,BORDER_THICKNESS);
}
