#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>


cv::Mat imgLoad(std::string src, int read_flag = cv::IMREAD_COLOR);
cv::Mat imgResize(cv::Mat orgImg, cv::Size newSize, int resizeFlag = -1);
cv::Mat imgResize(cv::Mat orgImg, float fx, float fy, int resizeFlag = -1);
void imgSave(cv::Mat img, std::string filename, std::vector<int> writeFlag = {-1, -1});
cv::Mat imgRotate(cv::Mat img, double angle);
cv::Mat imgFlip(cv::Mat img, bool horizontal);
cv::Mat imgCrop(cv::Mat img, cv::Point2i topLeftCoord, cv::Point2i bottomRightCoord);
cv::Mat imgBinaryMask(cv::Mat img, cv::Mat mask);
bool isGrayscale(cv::Mat img);
bool isBlackAndWhite(cv::Mat img);