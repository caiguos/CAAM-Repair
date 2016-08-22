
#include "ImageObject.h"
#include <coreplugin\icore.h>
#include <coreplugin\progressmanager\progressmanager.h>

using namespace Blade;
using namespace cv;

ImageObject* Blade::theImageObj = NULL;

ImageObject::ImageObject(void)
	: m_bDetection(false)
{
	theImageObj = this;

	readSettings();
}

ImageObject::~ImageObject()
{
	theImageObj = NULL;
}

void ImageObject::readSettings()
{
	QSettings *settings = Core::ICore::settings();
	settings->beginGroup("BladeImageProcess");

	m_dBoundThres = settings->value("BoundaryThreshold", 100).toDouble();

	settings->endGroup();
}

void ImageObject::writeSettings()
{
	QSettings *settings = Core::ICore::settings();
	settings->beginGroup("BladeImageProcess");

	settings->setValue("BoundaryThreshold", m_dBoundThres);

	settings->endGroup();
}

/**
* Code for thinning a binary image using Zhang-Suen algorithm.
*/
void thinningIteration(IplImage* img, bool odd)
{
	IplImage* marker = cvCloneImage(img); 
	cvZero(marker);

	//  p9	p2	p3
	//	p8	p1	p4
	//	p7	p6	p5
	for (int i = 1; i < img->height-1; i++)
	{
		for (int j = 1; j < img->width-1; j++)
		{
			uchar p2 = CV_IMAGE_ELEM(img, uchar, i-1, j);
			uchar p3 = CV_IMAGE_ELEM(img, uchar, i-1, j+1);
			uchar p4 = CV_IMAGE_ELEM(img, uchar, i, j+1);
			uchar p5 = CV_IMAGE_ELEM(img, uchar, i+1, j+1);
			uchar p6 = CV_IMAGE_ELEM(img, uchar, i+1, j);
			uchar p7 = CV_IMAGE_ELEM(img, uchar, i+1, j-1);
			uchar p8 = CV_IMAGE_ELEM(img, uchar, i, j-1);
			uchar p9 = CV_IMAGE_ELEM(img, uchar, i-1, j-1);

			int a = 0;
			if (p2 == 0 && p3 != 0) a++;
			if (p3 == 0 && p4 != 0) a++; 
			if (p4 == 0 && p5 != 0) a++; 
			if (p5 == 0 && p6 != 0) a++; 
			if (p6 == 0 && p7 != 0) a++; 
			if (p7 == 0 && p8 != 0) a++; 
			if (p8 == 0 && p9 != 0) a++; 
			if (p9 == 0 && p2 != 0) a++;
			if(a==1)
			{
				int b = 0;
				if (p2!=0) b++;
				if (p3!=0) b++;
				if (p4!=0) b++;
				if (p5!=0) b++;
				if (p6!=0) b++;
				if (p7!=0) b++;
				if (p8!=0) b++;
				if (p9!=0) b++;
				if(b>1 && b<7)
				{
					int m1 = odd ? (p2 * p4 * p6) : (p2 * p4 * p8);
					int m2 = odd ? (p4 * p6 * p8) : (p2 * p6 * p8);
					if (m1 == 0 && m2 == 0)
						CV_IMAGE_ELEM(marker, uchar, i, j) = 255;
				}
			}
		}
	}

	cvNot(marker, marker);
	cvAnd(img, marker, img);
	cvReleaseImage(&marker);
}

// Code for thinning a binary image using Zhang-Suen algorithm.
IplImage* ThinningZhangSuen(IplImage* img)
{
	IplImage* skel = cvCloneImage(img); 
	IplImage* prev = cvCloneImage(img); 
	IplImage* diff = cvCloneImage(img); 
	cvZero(prev);

	while (cvCountNonZero(diff) > 0)
	{
		thinningIteration(skel, true);
		thinningIteration(skel, false);
		cvAbsDiff(skel, prev, diff);
		cvCopyImage(skel, prev);
	} 

	cvReleaseImage(&prev);
	cvReleaseImage(&diff);
	return skel;
}

void ImageObject::DetectBoundary(const IplImage* grayImage, IplImage* displayImage)
{
	if(grayImage==NULL || displayImage==NULL)
		return;

	Core::ProgressManager* progress = Core::ICore::instance()->progressManager();
	emit progress->signalAddTask(tr("Create Toolpath"), "Blade.CreateToolpath", 0, 4);

	IplImage *src_temp = cvCreateImage(cvGetSize(grayImage), grayImage->depth, 1);
	
	//if(grayImage->nChannels==3)
	//	cvCopyImage(grayImage, displayImage);
	//else
	//	cvCvtColor(grayImage, displayImage, CV_GRAY2RGB);
	//cvThreshold(grayImage, src_temp, m_dBoundThres, 255, CV_THRESH_BINARY);

	// Daheng camera outputs Bayer_RG pixel format
	cvCvtColor(grayImage, displayImage, CV_BayerRG2RGB);
	cvCvtColor(grayImage, src_temp, CV_BayerRG2GRAY);
	cvThreshold(src_temp, src_temp, m_dBoundThres, 255, CV_THRESH_BINARY);

	cvMorphologyEx(src_temp,src_temp,NULL,NULL,CV_MOP_CLOSE,1);

	CvMemStorage* storage1=cvCreateMemStorage(0);
	CvSeq *contour=NULL;
	cvFindContours(src_temp,storage1,&contour,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

	if(contour!=NULL)
	{
		cvApproxPoly(contour, sizeof(CvContour), storage1, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.1,0);

		cvDrawContours(displayImage,
			contour, cvScalar(0,0,200),
			cvScalar(50), 1, 2, CV_AA);
	}

	emit progress->signalSetProgressValue(1);
	src_temp = ThinningZhangSuen(src_temp);
	cvCvtColor(src_temp, displayImage, CV_GRAY2RGB);
	emit progress->signalSetProgressValue(2);

	cvReleaseImage(&src_temp);
	cvReleaseMemStorage(&storage1);

	emit progress->signalSetProgressValue(4);
}

void ImageObject::slotCapture()
{
	emit signalDataUpdated();
}

#include "GeneratedFiles/moc_ImageObject.cpp"
