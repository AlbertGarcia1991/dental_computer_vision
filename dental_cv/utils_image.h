#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>


cv::Mat imgLoad(std::string src, int read_flag = cv::IMREAD_COLOR);
cv::Mat imgResize(cv::Mat orgImg, cv::Size newSize, int resizeFlag = -1);
cv::Mat imgResize(cv::Mat orgImg, float fx, float fy, int resizeFlag = -1);