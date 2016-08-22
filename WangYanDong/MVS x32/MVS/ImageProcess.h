#pragma once

#include <QtWidgets/QMainWindow>
#include<string>
//#include<HVDAILT.H>
//#include<HVDef.h>
//#include<Raw2Rgb.h>
//#include<opencv2/core/core.hpp>
//#include<opencv2/imgproc/imgproc.hpp>
//#include<opencv2/highgui/highgui.hpp>
#include<Windows.h>
#include <QObject>
#include <cv.h>
#include <highgui.h>
#include <opencv2\opencv.hpp>
#include <iostream>
//using namespace std;

namespace mvs
{
	class ImageProcess : public QObject
{
	Q_OBJECT

public:
	ImageProcess();
	~ImageProcess(void);

private:

   int offset;
    //float pi;
   CvPoint *replacePoint;
    //bool isclicked;
   // CvSeq* mouSeq;   //最终提取出的点存放在这个序列里
    //CvPoint *mouPoint;
    //IplImage *src3;
   
   IplImage *src;
   IplImage *src_temp;
   double Threshold;

   cv::Mat result;
   IplImage *ipl_result;
   /* IplImage *src_ThresholdtoProcess;
	IplImage *src_Process;
	IplImage *src_temp_ThresholdtoProcess;
	IplImage *src_temp_Process;*/
   //CvSeq* mouSeq01;
   IplImage *src_Offset;
   CvSeq* mouSeq_Offset;
   CvMemStorage* storage3;
   CvMemStorage* storage_Adjust;
   CvMemStorage* storage_Remove;
   CvMemStorage* storage_Offset;
    CvMemStorage* storage_Sequence;
	CvMemStorage* storage_Result;
	BOOL flag_Offset;
	BOOL flag_Adjust;
	BOOL flag_Remove;
	BOOL flag_Process;
	BOOL flag_Sequence;
	IplImage *src_Sequence;
public:

	//计算点上的法向量
  static CvPoint2D32f ** methdvector(const CvPoint* a,CvPoint* vector1,CvPoint* vector2);

   //计算出依顺序排列的三个点形成的两条直线角度
   float angle(float x,float y,float a ,float b);

  //除去轮廓上过于尖锐的点
   CvSeq* deleteangle(CvSeq* dst1,float ang);

  //手动改变图形
   static CvSeq* replaceContourPoint(CvSeq* src,CvPoint* rePoint);

   CvSeq* contourOffset(CvSeq* src,int offsetval);

   static void drawVector(CvSeq* src,int offsetval,void *para);
  
    static void mousehandler(int event,int x,int y,int flags,void* param);
	
	static void mousehandler_removepoints(int event,int x,int y,int flags,void* param);
	static CvSeq* deleteContourPoint(CvSeq* src,CvPoint* rePoint);

	static void mousehandler_setsequence(int event,int x,int y,int flags,void* param);
	static CvSeq* sequenceContourPoint(CvSeq* src,CvPoint* rePoint,CvSeq* src_Seq);

//   friend void passmousehandler(void* data);  //声明一个友元函数
//   void fun1()
//   {
//	   cvSetMouseCallback("Manual Adjustment",passmousehandler,this);
//   }
//
//   private:
//	   void passmousehandler(void* data)
//{
//     ((ImageProcess*)data->mousehandler);
//}
   public Q_SLOTS:

	   void OpenImage();
	   void SetThreshold();
	   void imageProcess();
	   //void SetThresholdandProcess();
	   void ManualAdjustment();
	   void GenerateVal3File();
	   void SetOffset();
	   void RemovePoints();
	   void SetPointSequence();
	};
}