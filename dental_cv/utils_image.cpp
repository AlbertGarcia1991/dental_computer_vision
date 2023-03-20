#include <opencv2/opencv.hpp>
#include "utils_image.h"
#include "win_utils.h"
#include "macros.h"
#include "globals.cpp"



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
        try
        {
            if (img.empty()) { throw 100; }
        }
        catch (...)
        {
            std::cout << "The read image is empty or not an image file" << std::endl;
            return img;
        }
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


// imgResize() overloading
cv::Mat imgResize(cv::Mat orgImg, float fx, float fy, int resizeFlag)
{
    cv::Size endSize = cv::Size((int) (orgImg.size().width * fx), (int) (orgImg.size().height * fy));
    if (resizeFlag == -1)
    {
        resizeFlag = _imgResizeFlagSelector(orgImg.size(), endSize);
    }
    return imgResize(orgImg, endSize, resizeFlag);
}


IMG_FORMAT _imgGetFormat(std::string format)
{
    // Ensure the input format string is in lower case
    for (int i = 0; i < format.length(); i++)
    {
        std::putchar(std::tolower(format[i]));
    }

    if (format == "png")
    {
        return PNG;
    }
    else if((format == "jpeg") || (format == "jpg"))
    {
        return JPEG;
    }
    else if (format == "tiff")
    {
        return TIFF;
    }
    else
    {
        return NON_VALID;
    }
}


void imgSave(cv::Mat img, std::string filename, std::vector<int> writeFlag)
{
    /*
    *   IMWRITE_JPEG_QUALITY        = 1,  //!< For JPEG, it can be a quality from 0 to 100 (the higher is the better). Default value is 95.
    *   IMWRITE_JPEG_PROGRESSIVE    = 2,  //!< Enable JPEG features, 0 or 1, default is False.
    *   IMWRITE_JPEG_OPTIMIZE       = 3,  //!< Enable JPEG features, 0 or 1, default is False.
    *   IMWRITE_JPEG_RST_INTERVAL   = 4,  //!< JPEG restart interval, 0 - 65535, default is 0 - no restart.
    *   IMWRITE_JPEG_LUMA_QUALITY   = 5,  //!< Separate luma quality level, 0 - 100, default is -1 - don't use.
    *   IMWRITE_JPEG_CHROMA_QUALITY = 6,  //!< Separate chroma quality level, 0 - 100, default is -1 - don't use.
    *   IMWRITE_JPEG_SAMPLING_FACTOR = 7, //!< For JPEG, set sampling factor. See cv::ImwriteJPEGSamplingFactorParams.
    *   IMWRITE_PNG_COMPRESSION     = 16, //!< For PNG, it can be the compression level from 0 to 9. A higher value means a smaller size and longer compression time. If specified, strategy is changed to IMWRITE_PNG_STRATEGY_DEFAULT (Z_DEFAULT_STRATEGY). Default value is 1 (best speed setting).
    *   IMWRITE_PNG_STRATEGY        = 17, //!< One of cv::ImwritePNGFlags, default is IMWRITE_PNG_STRATEGY_RLE.
    *   IMWRITE_PNG_BILEVEL         = 18, //!< Binary level PNG, 0 or 1, default is 0.
    *   IMWRITE_TIFF_RESUNIT = 256,//!< For TIFF, use to specify which DPI resolution unit to set; see libtiff documentation for valid values
    *   IMWRITE_TIFF_XDPI = 257,//!< For TIFF, use to specify the X direction DPI
    *   IMWRITE_TIFF_YDPI = 258,//!< For TIFF, use to specify the Y direction DPI
    *   IMWRITE_TIFF_COMPRESSION = 259,//!< For TIFF, use to specify the image compression scheme. See libtiff for integer constants corresponding to compression formats. Note, for images whose depth is CV_32F, only libtiff's SGILOG compression scheme is used. For other supported depths, the compression scheme can be specified by this flag; LZW compression is the default.
    */
    int formatDotIdx = filename.find_first_of('.');
    try
    {
        if (formatDotIdx == std::string::npos) { throw 110; }
    }
    catch (...)
    {
        std::cout << "The given output image filename does not containg format especifier (" << filename << ")" << std::endl;
        return;
    }
    std::string format = filename.substr(formatDotIdx + 1);
    IMG_FORMAT imgFormat = _imgGetFormat(format);
    try
    {
        if (imgFormat == NON_VALID) { throw 005; }
    }
    catch (...)
    {
        std::cout << "The given output image filename format is not valid (" << format << ")" << std::endl;
        return;
    }

    if (writeFlag == std::vector<int>({-1, -1}))
    {
        switch (imgFormat)
        {
        case PNG:
            writeFlag = {16, 1};
            break;
        case JPEG:
            writeFlag = {1, 90};
            break;
        case TIFF:
            writeFlag = {256, 300};
            break;
        default:
            writeFlag = std::vector<int>();
        }
    }

    bool ret = cv::imwrite(filename, img, writeFlag);
    try
    {
        if (!ret) { throw 101; }
    }
    catch (...)
    {
        std::cout << "Impossible to save the image" << std::endl;
    }
}


cv::Mat imgRotate(cv::Mat img, double angle)
{
    cv::Point2f centerCoord((img.cols - 1) / 2., (img.rows - 1) / 2.);
    cv::Mat rotationM = cv::getRotationMatrix2D(centerCoord, angle, 1.0);

    cv::Mat rotatedImg;
    cv::warpAffine(img, rotatedImg, rotationM, img.size());
    return rotatedImg;
}

cv::Mat imgFlip(cv::Mat img, bool horizontal)
{
    cv::Mat flippedImg;
    if (horizontal)
    {
        cv::flip(img, flippedImg, 1);
    }
    else
    {
        cv::flip(img, flippedImg, 0);
    }
    return flippedImg;
}

cv::Mat imgCrop(cv::Mat img, cv::Point2i topLeftCoord, cv::Point2i bottomRightCoord)
{
    try
    {
        if ((bottomRightCoord.x < topLeftCoord.x) || (bottomRightCoord.y < topLeftCoord.y)) { throw 120; }
        if ((topLeftCoord.x < 0) || (topLeftCoord.y < 0)) { throw 121; }
        if ((bottomRightCoord.x > img.cols) || (bottomRightCoord.y > img.rows)) { throw 122; }
    }
    catch (int error)
    {
        std::cout << "Impossible to crop the image within the given ranges (" << error << ")" << std::endl;
        return cv::Mat();
    }
    return img(cv::Range(topLeftCoord.x, topLeftCoord.y), cv::Range(bottomRightCoord.x, bottomRightCoord.y));
}

cv::Mat imgBinaryMask(cv::Mat img, cv::Mat mask)
{
    try
    {
        if ((img.rows != mask.rows) || (img.cols != mask.cols))
        {
            throw 130;
        }
        else
        {
            cv::Mat maskedImg;
            img.copyTo(maskedImg, mask);
            return maskedImg;
        }
    }
    catch (...)
    {
        std::cout << "Image and binary mask do not have the same dimensions" << std::endl;
    }
}


std::vector<float> unique(cv::Mat img, bool sort = false)
{
    if (img.channels() > 1 || img.type() != CV_32F)
    {
        std::cerr << "unique !!! Only works with CV_32F 1-channel Mat" << std::endl;
        return std::vector<float>();
    }

    std::vector<float> out;
    for (int y = 0; y < img.rows; ++y)
    {
        const float* row_ptr = img.ptr<float>(y);
        for (int x = 0; x < img.cols; ++x)
        {
            float value = row_ptr[x];

            if (std::find(out.begin(), out.end(), value) == out.end())
                out.push_back(value);
        }
    }

    if (sort)
        std::sort(out.begin(), out.end());

    return out;
}


bool isGrayscale(cv::Mat img)
{
    if (img.channels() == 1)
    {
        return true;
    }
    else
    {
        cv::Mat dst;
        cv::Mat bgr[3];
        cv::split(img, bgr);
        cv::absdiff(bgr[0], bgr[1], dst);

        if (cv::countNonZero(dst))
            return false;

        cv::absdiff(bgr[0], bgr[2], dst);
        return !cv::countNonZero(dst);
    }
}


bool isBlackAndWhite(cv::Mat img)
{
    if (isGrayscale(img))
    {
        if (img.channels() > 1)
        {
            cv::Mat bgr[3];
            cv::split(img, bgr);
            img = bgr[0];
        }
        for (int x = 0; x < img.cols; x++)
        {
            const float* row_ptr = img.ptr<float>(x);
            for (int y = 0; y < img.rows; y++)
            {
                if ((row_ptr[y] == 1.) || (row_ptr[y] == 0.))
                {
                    continue;
                }
                return false;
            }
        }
        return true;
    }
    return false;
}