#pragma once

#include <cv.h>
#include <highgui.h>
#include <opencv2\opencv.hpp>

extern float pi;
extern bool isclicked;
extern CvSeq* mouSeq;   //最终提取出的点存放在这个序列里
extern CvPoint *mouPoint;
extern IplImage *src3;
//extern CvSeq* mouSeq_Offset;
extern CvSeq* mouSeq_Adjust;
//extern IplImage *src;
//extern cv::Mat result;
extern IplImage *src_Adjust;
extern CvSeq* mouSeq_Remove;
extern IplImage *src_Remove;

extern CvSeq* mouSeq_Sequence;
extern IplImage *src_Sequence;

extern CvSeq* mouSeq_Result;
extern IplImage *src_Result;