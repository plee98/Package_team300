#include "imageprocessing.h"

void ImageProcessor::Binarialize(const Mat& hsvImg, char flag, Mat& binImg)
{
	Mat tmpBin(binImg.size(), CV_8UC1);

	if (flag & EColor::BLACK)
    {
        inRange(hsvImg, MIN_HSV_BLACK, MAX_HSV_BLACK, tmpBin);
        bitwise_or(tmpBin, binImg, binImg);
    }   
    if (flag & EColor::BLUE)
	{
        inRange(hsvImg, MIN_HSV_BLUE, MAX_HSV_BLUE, tmpBin);
        bitwise_or(tmpBin, binImg, binImg);
    }	
}

Mat ImageProcessor::PreProcess(const Mat &src)
{
    Mat imgThresholded, imgHSV, dst;

    cvtColor(src, imgHSV, COLOR_BGR2HSV);

    inRange(imgHSV, MIN_HSV_BLACK, MAX_HSV_BLACK, imgThresholded);

    dst = birdViewTranform(imgThresholded);

    imshow("Bird View", dst);

    fillLane(dst);

    //imshow("Binary", imgThresholded);

    return dst;

    //Mat binImg, hsvImg, birdViewImg;

    // cvtColor(colorImg, hsvImg, COLOR_BGR2HSV);

    // inRange(hsvImg, MIN_HSV_BLACK, MAX_HSV_BLACK, binImg);

    // birdViewImg = birdViewTranform(binImg);

    // imshow("Bird View", birdViewImg);

    // fillLane(birdViewImg);

    // imshow("Binary", binImg);

    //return birdViewImg;
}

void ImageProcessor::fillLane(Mat &src)
{
    vector<Vec4i> lines;
    HoughLinesP(src, lines, 1, CV_PI/180, 1);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        line(src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255), 3, CV_AA);
    }
}

Mat ImageProcessor::morphological(const Mat &img)
{
    Mat dst;

    // erode(img, dst, getStructuringElement(MORPH_ELLIPSE, Size(1, 1)) );
    // dilate( dst, dst, getStructuringElement(MORPH_ELLIPSE, Size(1, 1)) );

    dilate(img, dst, getStructuringElement(MORPH_ELLIPSE, Size(10, 20)) );
    erode(dst, dst, getStructuringElement(MORPH_ELLIPSE, Size(10, 20)) );

    // blur(dst, dst, Size(3, 3));

    return dst;
}

void ImageProcessor::transform(Point2f* src_vertices, Point2f* dst_vertices, Mat& src, Mat &dst){
    Mat M = getPerspectiveTransform(src_vertices, dst_vertices);
    warpPerspective(src, dst, M, dst.size(), INTER_LINEAR, BORDER_CONSTANT);
}

Mat ImageProcessor::birdViewTranform(const Mat &src)
{
    Point2f src_vertices[4];

    int width = src.size().width;
    int height = src.size().height;

    src_vertices[0] = Point(0, SKYLINE);
    src_vertices[1] = Point(width, SKYLINE);
    src_vertices[2] = Point(width, height);
    src_vertices[3] = Point(0, height);

    Point2f dst_vertices[4];
    dst_vertices[0] = Point(0, 0);
    dst_vertices[1] = Point(BIRDVIEW_WIDTH, 0);
    dst_vertices[2] = Point(BIRDVIEW_WIDTH - 105, BIRDVIEW_HEIGHT);
    dst_vertices[3] = Point(105, BIRDVIEW_HEIGHT);

    Mat M = getPerspectiveTransform(src_vertices, dst_vertices);

    Mat dst(BIRDVIEW_HEIGHT, BIRDVIEW_WIDTH, CV_8UC3);
    warpPerspective(src, dst, M, dst.size(), INTER_LINEAR, BORDER_CONSTANT);

    return dst;
}