#include "utils_image.h"
#include <opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <iostream>

using std::string;


int main() {
	string imgSrc = "datasets/TeethSegmentation/1.png";
	cv::Mat img = imgLoad(imgSrc);
	img = imgResize(img, cv::Size(400, 400));
	img = imgRotate(img, 45);
	
	cv::imshow("Image", img);
	cv::waitKey(0);

	return 0;
}