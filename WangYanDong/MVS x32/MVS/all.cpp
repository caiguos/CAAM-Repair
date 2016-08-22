#pragma once

#include "all.h"

    float pi=3.14159;
	bool isclicked;//=false;
	CvSeq* mouSeq;//=NULL;
	CvPoint *mouPoint;//=NULL;
	IplImage *src3;//=NULL;
	//CvSeq* mouSeq_Offset;
	CvSeq* mouSeq_Adjust;
	//cv::Mat result;
	IplImage *src_Adjust;
	CvSeq* mouSeq_Remove;
    IplImage *src_Remove;
	CvSeq* mouSeq_Sequence;
    IplImage *src_Sequence;
	
	CvSeq* mouSeq_Result;
    IplImage *src_Result;