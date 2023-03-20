#include <opencv2/opencv.hpp>
#include "utils_image.h"
#include "win_utils.h"
#include "macros.h"



cv::Mat imgLoad(std::string src, int readFlag)
{
    /*
    * Reading flags:
    *    IMREAD_UNCHANGED = -1, //!< If set, return the loaded image as is (with alpha channel, otherwise it gets cropped). Ignore EXIF orientation.
    *    IMREAD_GRAYSCALE = 0,  //!< If set, always convert image to the single channel grayscale image (codec internal conversion).
    *    IMREAD_COLOR = 1,  //!< If set, always convert image to the 3 channel BGR color image.
    *    IMREAD_ANYDEPTH = 2,  //!< If set, return 16-bit/32-bit image when the input has the corresponding depth, otherwise convert it to 8-bit.
    *    IMREAD_ANYCOLOR = 4,  //!< If set, the image is read in any possible color format.
    *    IMREAD_LOAD_GDAL = 8,  //!< If set, use the gdal driver for loading the image.
    *    IMREAD_REDUCED_GRAYSCALE_2 = 16, //!< If set, always convert image to the single channel grayscale image and the image size reduced 1/2.
    *    IMREAD_REDUCED_COLOR_2 = 17, //!< If set, always convert image to the 3 channel BGR color image and the image size reduced 1/2.
    *    IMREAD_REDUCED_GRAYSCALE_4 = 32, //!< If set, always convert image to the single channel grayscale image and the image size reduced 1/4.
    *    IMREAD_REDUCED_COLOR_4 = 33, //!< If set, always convert image to the 3 channel BGR color image and the image size reduced 1/4.
    *    IMREAD_REDUCED_GRAYSCALE_8 = 64, //!< If set, always convert image to the single channel grayscale image and the image size reduced 1/8.
    *    IMREAD_REDUCED_COLOR_8 = 65, //!< If set, always convert image to the 3 channel BGR color image and the image size reduced 1/8.
    *    IMREAD_IGNORE_ORIENTATION = 128 //!< If set, do not rotate the image according to EXIF's orientation flag.
    */
    cv::Mat img;
	if (isFile(src))
	{
        img = cv::imread(src, readFlag);
        if (img.empty())
        {
            throw std::runtime_error("The read image is empty or not an image file");
        }
        return img;
	}
    return img;
}


int _imgResizeFlagSelector(cv::Size orgSize, cv::Size endSize)
{
    if (ABS_MAX(endSize.width / orgSize.width, endSize.height / orgSize.height) > 0)
    {
        return cv::INTER_LINEAR;
    }
    else
    {
        if (ABS_MAX(endSize.width / orgSize.width, endSize.height / orgSize.height) > -10)
        {
            return cv::INTER_AREA;
        }
        else
        {
            return cv::INTER_LANCZOS4;
        }
    }
}

cv::Mat imgResize(cv::Mat orgImg, cv::Size newSize, int resizeFlag)
{
    /*
    * Resizing flags:
    *    INTER_NEAREST = 0,  // nearest neighbor interpolation
    *    INTER_LINEAR = 1,  // bilinear interpolation
    *    INTER_CUBIC = 2,  // bicubic interpolation
    *    INTER_AREA = 3,  // resampling using pixel area relation
    *    INTER_LANCZOS4 = 4,  // Lanczos interpolation over 8x8 neighborhood 
    *    INTER_LINEAR_EXACT = 5,  // Bit exact bilinear interpolation
    *    INTER_NEAREST_EXACT = 6,  // Bit exact nearest neighbor interpolation
    *    INTER_MAX = 7,  // mask for interpolation codes
    *    WARP_FILL_OUTLIERS = 8,  // fills all of the destination image pixels
    *    WARP_INVERSE_MAP = 16  // inverse transformation
    * 
    * If increasing image size, use LINEAR or CUBIC (more computationally expensive). Is downsizing image use AREA or LANCZOS4 if x10.
    */
    if (resizeFlag == -1)
    {
        resizeFlag = _imgResizeFlagSelector(orgImg.size(), newSize);
    }
    cv::Mat newImg;
    cv::resize(orgImg, newImg, newSize, resizeFlag);
    return newImg;
}

// imgResize overloading
cv::Mat imgResize(cv::Mat orgImg, float fx, float fy, int resizeFlag)
{
    cv::Size endSize = cv::Size((int) (orgImg.size().width * fx), (int) (orgImg.size().height * fy));
    if (resizeFlag == -1)
    {
        resizeFlag = _imgResizeFlagSelector(orgImg.size(), endSize);
    }
    return imgResize(orgImg, endSize, resizeFlag);
}