#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include "data.hpp"

using namespace cv;

ColSensor::ColSensor() {
	upper_corner = Point(0, 0);
	lower_corner = Point(0, 0);
}

ColSensor::~ColSensor() {}

ColSensor::ColSensor(Point u_corner, Point l_corner, Mat *src){
	BORDER_THICKNESS = 2;
	color = Scalar(0,255,0);

	upper_corner=u_corner;
	lower_corner=l_corner;

	parent = src;
	//detect_area=src(Rect(u_corner.x, u_corner.y, l_corner.x-u_corner.x,l_corner.y-u_corner.y));
}

void ColSensor::drawRectangle(Mat src){
	rectangle(src,upper_corner,lower_corner,color,BORDER_THICKNESS);
}

Mat ColSensor::getDetectArea() const {
	return (*parent)(Rect(upper_corner.x, upper_corner.y, lower_corner.x-upper_corner.x,lower_corner.y-upper_corner.y));
}