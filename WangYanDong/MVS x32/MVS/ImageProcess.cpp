#include "ImageProcess.h"
#include <QFileDialog>
#include <QtGui>
#include<Windows.h>
#include <QtWidgets>
//#include <cv.h>
//#include <highgui.h>
//#include <opencv2\opencv.hpp>
#include <iostream>
//using namespace std;
#include<string>
#include "manager.h"
//#include "mvs.h"
#include "all.h"
#include "robotlib.h"
#include<vector>


//using namespace std;
using Robot::GetRobot;

using namespace mvs;

ImageProcess::ImageProcess()
{
   //using namespace std; //offset=30;
	//pi=3.14159;
	replacePoint=&cvPoint(0,0);
	/*isclicked=false;
	mouSeq=NULL;
	mouPoint=NULL;
	src3=NULL;*/
    src=NULL;
	src_temp=NULL;
	//cvNamedWindow( "Manual Adjustment", CV_WINDOW_AUTOSIZE );
	/*src_ThresholdtoProcess=NULL;
	src_Process=NULL;
	src_temp_ThresholdtoProcess=NULL;
	src_temp_Process=NULL;*/
	 //cv::Mat result;
	flag_Process=FALSE;
	flag_Offset=FALSE;
	flag_Adjust=FALSE;
	flag_Remove=FALSE;
	flag_Sequence=FALSE;
}


ImageProcess::~ImageProcess(void)
{
  
}

void ImageProcess::OpenImage()
{
	QString fileName;
	fileName=QFileDialog::getOpenFileName(NULL,QObject::tr("Open Image"),".",QObject::tr("BMP Files ( *.bmp );;JPEG Files (*.jpg);;PNG Files(*.png)"));
	//C:/Users/Administrator/Desktop
	if (fileName.isNull())  
       return ;
	else
	{
		 //const char* OpenPathName = fileName.toStdString().data();
		 string OpenPathName;
		 OpenPathName = fileName.toStdString();
	     //src=cvLoadImage(OpenPathName,0); 
		 cv::Mat img_temp;
	     img_temp=cv::imread(OpenPathName);//路径中不能有中文，QString转string会乱码
		 cv::Mat img;
		 cvtColor(img_temp,img,CV_RGB2GRAY);//转为灰度图
		 
		 //cv::Mat result;
		 img.copyTo(result);
		
		 //src=(&(IplImage)img);  //Mat convert to IplImage,src赋值
		 
		 cv::Mat img01;
		 cv::Size dsize=cv::Size(img.cols*0.5,img.rows*0.5);
		 resize(img,img01,dsize);

		 //cv::cvtColor(img01,img01,CV_BGR2RGB);//opencv convert to qt
         QImage image=QImage((const unsigned char*)(img01.data),img01.cols,img01.rows,img01.step,QImage::Format_Indexed8);
		 
		 manager* pmanager=manager::instance();
    
         pmanager->GetMVS()->ui.label_ProcessResult->setPixmap(QPixmap::fromImage(image));
		  
		// ipl_result=&IplImage(result);
		 /*cvNamedWindow("result0",1);
	     cvShowImage("result0",ipl_result);*/
	  
	  }
}

void ImageProcess::SetThreshold()
{
	/*cvNamedWindow("result",1);
	cv::imshow("result",result);*/
	//cvCopyImage(ipl_result,src);
	//cvReleaseImage(&src);
	//src=&IplImage(result);  
	/*IplImage **/ipl_result=&IplImage(result);
	/*cvNamedWindow("ipl_result",1);
	cvShowImage("ipl_result",ipl_result);*/
	 src=cvCloneImage(ipl_result);
	//cvReleaseImage(&ipl_result);
	/*cvNamedWindow("src",1);
	cvShowImage("src",src);*/

    src_temp=cvCreateImage(cvGetSize(src),src->depth,1); 
	//double Threshold;
	
	manager* pmanager=manager::instance();
    Threshold=pmanager->GetMVS()->ui.lineEdit_Threshold->text().toDouble(); //设置阈值
    
	cvThreshold(src,src,Threshold,255,CV_THRESH_BINARY);  //阈值分割，应交互式设置值
	
	cv::Mat img;
	img=cv::Mat(src);

	//改变图像尺寸为原来1/4
	cv::Mat img01;
	cv::Size dsize=cv::Size(img.cols*0.5,img.rows*0.5);
	resize(img,img01,dsize); 
	
	QImage image=QImage((unsigned char*)(img01.data),img01.cols,img01.rows,img01.step,QImage::Format_Indexed8);
	
	//manager* pmanager=manager::instance();
    
    pmanager->GetMVS()->ui.label_ProcessResult->setPixmap(QPixmap::fromImage(image));

	// cvReleaseImage(&ipl_result);
	
	//// 检查无问题
	/*cvNamedWindow("src",1);
	cvShowImage("src",src);*/
	
	//函数间变量传递
	/*src_ThresholdtoProcess=cvCloneImage(src);
	src_temp_ThresholdtoProcess=cvCloneImage(src_temp);*/
	
	////  检查无问题
	/*cvNamedWindow("src_ThresholdtoProcess",1);
	cvShowImage("src_ThresholdtoProcess",src_ThresholdtoProcess);*/
}

void ImageProcess::imageProcess()
{
//	cvNamedWindow("src_ThresholdtoProcess",1);
//	cvShowImage("src_ThresholdtoProcess",src_ThresholdtoProcess);
//
//	//函数间变量传递
//	src_Process=cvCloneImage(src_ThresholdtoProcess);
//	src_temp_Process=cvCloneImage(src_temp_ThresholdtoProcess);
//	
//	
//////
//	cvNamedWindow("src_Process",1);
//	cvShowImage("src_Process",src_Process);
//
//	cvNamedWindow("src_temp_Process0",1);
//	cvShowImage("src_temp_Process0",src_temp_Process);

	CvMemStorage* storagetemp=cvCreateMemStorage(0);
	////
    src3=cvCloneImage(src);   //复制src到src3，src改变，src3也会随之改变，此时src3为阈值分割后的二值化图像
	
	//CvMemStorage* storage1=cvCreateMemStorage(0);
	CvMemStorage* storage1=cvCreateMemStorage(0);
	//CvMemStorage* storage2=cvCreateMemStorage(0);//参数block_size对应内存器中每个内存块的大小，为0时内存块默认大小为64k
	CvMemStorage* storage2=cvCreateMemStorage(0);//block_size存储块的大小以字节表示，如果是0 byte，当前内存块默认大小为64k
	CvSeq *contour=NULL,*mcont=NULL;
	
	IplImage *temp=cvCreateImage(cvGetSize(src),src->depth,src->nChannels);//temp为阈值分割后的src，不会随其改变
	
	//cvCopyImage(src_Process,src_temp_Process);  //把前者复给后者
	src_temp=cvCloneImage(src);
////
	/*cvNamedWindow("src_temp0",1);
	cvShowImage("src_temp0",src_temp);*/

	IplImage *src1=cvCloneImage(src);
	for(int i=1;i<=10;i++)
	{
	cvMorphologyEx(src_temp,src_temp,temp,NULL,CV_MOP_OPEN,4);
    cvMorphologyEx(src_temp,src_temp,temp,NULL,CV_MOP_CLOSE,4);

     //smooth the image
    cvSmooth(src_temp,src_temp,CV_BLUR,3,3);
    cvSmooth(src_temp,src_temp,CV_GAUSSIAN,3,3);
	cvSmooth(src_temp,src_temp,CV_MEDIAN,3,3);
	}
	////
	/*cvNamedWindow("src_temp",1);
	cvShowImage("src_temp",src_temp);*/

	cvFindContours(src_temp,storage1,&contour,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);    //CV_CHAIN_APPROX_SIMPLE压缩水平方向，垂直方向，对角线方向的元素，只保留该方向的终点坐标，例如一个矩形轮廓只需4个点来保存轮廓信息

	CvSeq* contourtemp=cvCloneSeq(contour,storagetemp);

	CvSeq* contour1=deleteangle(contour,-0.93);
	
	cvZero(src_temp);
	mcont=cvApproxPoly(contour1,sizeof(CvContour),storage2,CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.001,0);  //cvContourPerimeter求轮廓周长
     //画出矢量图

	//图像偏置

//	CvSeq*mcont1=contourOffset(mcont,offset);
	
	//mcont=contourOffset(mcont,offset);   //图像偏置


	CvSeq* mcont2=deleteangle(mcont,-0.2);
	//mcont2=contourOffset(mcont2,-offset);
	/*CvMemStorage**/ storage3=cvCreateMemStorage(0);
	mouSeq=cvCloneSeq(mcont2,storage3);
	
///////////////////////////////////
    drawVector(mcont2,-20,src);
//////////////////////////////////
	/*cvDrawContours(
		src_temp,
		contour,
		cvScalar(100),
		cvScalar(50),
		1
	
		);
	cvDrawContours(
		src_temp,
		mcont2,
		cvScalar(100),
		cvScalar(50),
		1
		);*/
	 
	
	cvDrawContours(
		src,
		mcont2,
		cvScalar(100),
		cvScalar(50),
		1
		);
	

	//IplImage* smallimg=cvCreateImage(cvSize(src_temp->width/2,src_temp->height/2),src_temp->depth,src->nChannels);    //cvCreateImage创建首地址并分配存储空间
	//cvResize(src_temp,smallimg,CV_INTER_LINEAR);    //匹配src_temp到smallimg的大小等参数
	//cvShowImage("src",src);

	//改变图像尺寸为原来1/4
	/*IplImage* src01;
	IplImage* src02=NULL;
	src01=cvCreateImage(cvSize(src->width/2,src->height/2),src->depth,src->nChannels);
	cvCopyImage(src,src02);
	cvResize(src02,src01,CV_INTER_LINEAR);*/ 
	
	//cvShowImage("Manual Adjustment",src);
	///////////
	//src3=cvCloneImage(src);//把src给src3
	
   /*cvNamedWindow( "src", CV_WINDOW_AUTOSIZE );
	cvShowImage("src",src);*/
	//cvSetMouseCallback("Manual Adjustment",mousehandler,(void*)src3);*/
	
	cv::Mat img;
	img=cv::Mat(src);

	//改变图像尺寸为原来1/4
	cv::Mat img01;
	cv::Size dsize=cv::Size(img.cols*0.5,img.rows*0.5);
	resize(img,img01,dsize); 
	//img01=cvCreateImage(cvSize(img->width/2,img->height/2),img->depth,img->nChannels);
	//cvResize(img,img01,CV_INTER_LINEAR); 

	//cv::cvtColor(img01,img01,CV_BGR2RGB);
	QImage image=QImage((unsigned char*)(img01.data),img01.cols,img01.rows,img01.step,QImage::Format_Indexed8);
	
	manager* pmanager=manager::instance();
    
    pmanager->GetMVS()->ui.label_ProcessResult->setPixmap(QPixmap::fromImage(image));
		
	/*cvNamedWindow("src_temp",1);
    cvShowImage("src_temp",smallimg);*/
	//cvWaitKey(0);
	
	/////*****
	cvReleaseMemStorage(&storage1);
	cvReleaseMemStorage(&storage2);
	cvReleaseMemStorage(&storagetemp);
	
	//cvReleaseImage(&src);
	cvReleaseImage(&src_temp);
	//cvReleaseImage(&src02);
	//cvDestroyWindow("src");
	flag_Process=TRUE;
	
	storage_Result=cvCreateMemStorage(0);
	mouSeq_Result=cvCloneSeq(mouSeq,storage_Result);
	cvClearSeq(mouSeq_Result);
	/*for(int i=0;i<mouSeq->total;i++)
	{
     cvSeqPop(mouSeq_Result,0);
	}*/
}

void ImageProcess::SetOffset()
{
	storage_Offset=cvCreateMemStorage(0);

	if(flag_Process){mouSeq_Offset=cvCloneSeq(mouSeq,storage_Offset);}
	else{
	if(flag_Remove){mouSeq_Offset=cvCloneSeq(mouSeq_Remove,storage_Offset);cvReleaseMemStorage(&storage_Remove);}
	else{
		if(flag_Adjust){mouSeq_Offset=cvCloneSeq(mouSeq_Adjust,storage_Offset);cvReleaseMemStorage(&storage_Adjust);}
		else{
			if(flag_Offset){mouSeq_Offset=cvCloneSeq(mouSeq_Offset,storage_Offset);}//cvReleaseMemStorage(&storage_Offset);}
			else{mouSeq_Offset=cvCloneSeq(mouSeq,storage_Offset);}
		}
	}
	}
	//cvReleaseMemStorage(&storage_Offset);
	 
	/*CvSeq**/ 
	//cvReleaseMemStorage(&storage3);
	
   manager* pmanager=manager::instance();
   offset=pmanager->GetMVS()->ui.lineEdit_Offset->text().toInt(); //设置
   mouSeq_Offset=contourOffset(mouSeq_Offset,offset);
   mouSeq_Offset=deleteangle(mouSeq_Offset,-0.2);
  // cvReleaseImage(&src_Offset);
   /*IplImage **/src_Offset=cvCloneImage(src3);
   cvDrawContours(
		src_Offset,
		mouSeq_Offset,
		cvScalar(100),
		cvScalar(50),
		1
		);
   drawVector(mouSeq_Offset,-20,src_Offset);

    cv::Mat img;
	img=cv::Mat(src_Offset);

	//改变图像尺寸大小为1/4
	cv::Mat img01;
	cv::Size dsize=cv::Size(img.cols*0.5,img.rows*0.5);
	resize(img,img01,dsize); 

	//cv::cvtColor(img,img,CV_BGR2RGB);
	QImage image=QImage((unsigned char*)(img01.data),img01.cols,img01.rows,img01.step,QImage::Format_Indexed8);
    pmanager->GetMVS()->ui.label_ProcessResult->setPixmap(QPixmap::fromImage(image));
	flag_Offset=TRUE;
	flag_Remove=FALSE;
	flag_Adjust=FALSE;
	
}

void ImageProcess::ManualAdjustment()
{
	 storage_Adjust=cvCreateMemStorage(0);
	if(flag_Offset){mouSeq_Adjust=cvCloneSeq(mouSeq_Offset,storage_Adjust);cvReleaseMemStorage(&storage_Offset);}
	else{
		if(flag_Process){mouSeq_Adjust=cvCloneSeq(mouSeq,storage_Adjust);}
		else{
			if(flag_Adjust){mouSeq_Adjust=cvCloneSeq(mouSeq_Adjust,storage_Adjust);}
			else{
				if(flag_Remove){mouSeq_Adjust=cvCloneSeq(mouSeq_Remove,storage_Adjust);cvReleaseMemStorage(&storage_Remove);}
				else{mouSeq_Adjust=cvCloneSeq(mouSeq,storage_Adjust);}
				}
			}
		}
	
	 
	/*if(mouSeq_Offset==NULL){mouSeq_Adjust=cvCloneSeq(mouSeq,storage_Adjust);
	                     cvReleaseMemStorage(&storage3);}
	else{
		mouSeq_Adjust=cvCloneSeq(mouSeq_Offset,storage_Adjust);
		cvReleaseMemStorage(&storage_Offset);
	}*/
	//cvReleaseMemStorage(&storage2);
	cvNamedWindow( "Manual Adjustment", CV_WINDOW_AUTOSIZE );
	if(flag_Offset)
	{
     cvShowImage("Manual Adjustment",src_Offset);
	 cvSetMouseCallback("Manual Adjustment",mousehandler,NULL);//(void*)src
	 cvReleaseImage(&src_Offset);
	}
	else
	{
		if(flag_Process){
	cvShowImage("Manual Adjustment",src);
	cvSetMouseCallback("Manual Adjustment",mousehandler,NULL);//(void*)src_Offset
	//cvReleaseImage(&src_Offset);
	    }
		else{
			if(flag_Adjust){
    cvShowImage("Manual Adjustment",src_Adjust);
	cvSetMouseCallback("Manual Adjustment",mousehandler,NULL);
			cvReleaseImage(&src_Adjust);}
			else{
				if(flag_Remove){
    cvShowImage("Manual Adjustment",src_Remove);
	cvSetMouseCallback("Manual Adjustment",mousehandler,NULL);//(void*)src_Offset
	cvReleaseImage(&src_Remove);}
				else{
				 cvShowImage("Manual Adjustment",src);
	            cvSetMouseCallback("Manual Adjustment",mousehandler,NULL);}
			}
		}
	}
	flag_Process=FALSE;
	flag_Offset=FALSE;
	flag_Adjust=TRUE;
	flag_Remove=FALSE;

	cvWaitKey(0);
	cvDestroyWindow("Manual Adjustment");
	
}

void ImageProcess::RemovePoints()
{
	//cvDestroyWindow("Manual Adjustment");
	 storage_Remove=cvCreateMemStorage(0);
	 if(flag_Offset){mouSeq_Remove=cvCloneSeq(mouSeq_Offset,storage_Remove); cvReleaseMemStorage(&storage_Offset);}
	 else{
		 if(flag_Process){mouSeq_Remove=cvCloneSeq(mouSeq,storage_Remove);}
		 else{
			 if(flag_Adjust){mouSeq_Remove=cvCloneSeq(mouSeq_Adjust,storage_Remove); cvReleaseMemStorage(&storage_Adjust);}
			 else{
				 if(flag_Remove){mouSeq_Remove=cvCloneSeq(mouSeq_Remove,storage_Remove);}
				 else{mouSeq_Remove=cvCloneSeq(mouSeq,storage_Remove);}
			 }
		 }
	 }
	//cvReleaseMemStorage(&storage_Adjust);
	cvNamedWindow( "Remove Points", CV_WINDOW_AUTOSIZE );

	if(flag_Offset)
	{
		cvShowImage("Remove Points",src_Offset);
	    cvSetMouseCallback("Remove Points",mousehandler_removepoints,(void*)src_Offset);
		cvReleaseImage(&src_Offset);
	}
	else
      {
		if(flag_Process)
		{
        cvShowImage("Remove Points",src);
	    cvSetMouseCallback("Remove Points",mousehandler_removepoints,(void*)src);
		//cvReleaseImage(&src);
		}
		else
		{
			if(flag_Adjust){
    cvShowImage("Remove Points",src_Adjust);
	cvSetMouseCallback("Remove Points",mousehandler_removepoints,(void*)src_Adjust);
	cvReleaseImage(&src_Adjust);}
			else{
				if(flag_Remove){cvShowImage("Remove Points",src_Remove);
	cvSetMouseCallback("Remove Points",mousehandler_removepoints,(void*)src_Remove);
	       cvReleaseImage(&src_Remove);}
				else{cvShowImage("Remove Points",src);
	                cvSetMouseCallback("Remove Points",mousehandler_removepoints,(void*)src);}
		}
	}
	}
	flag_Process=FALSE;
	flag_Offset=FALSE;
	flag_Adjust=FALSE;
	flag_Remove=TRUE;
     cvWaitKey(0);
	 cvDestroyWindow("Remove Points");
	 
}

void ImageProcess::SetPointSequence()
{
	if(flag_Sequence){cvClearSeq(mouSeq_Result);}
	//cvDestroyWindow("Remove Points");
	/*CvMemStorage**/ 
	/*storage_Result=cvCreateMemStorage(0);
	mouSeq_Result=cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),storage_Result);*///CV_32FC2  CvPoint2D32f
	
	storage_Sequence=cvCreateMemStorage(0);
	 if(flag_Offset){mouSeq_Sequence=cvCloneSeq(mouSeq_Offset,storage_Sequence);cvReleaseMemStorage(&storage_Offset);}
	 else{
		 if(flag_Process){mouSeq_Sequence=cvCloneSeq(mouSeq,storage_Sequence);}
		 else{
			 if(flag_Adjust){mouSeq_Sequence=cvCloneSeq(mouSeq_Adjust,storage_Sequence);cvReleaseMemStorage(&storage_Adjust);}
			 else{
				 if(flag_Remove){mouSeq_Sequence=cvCloneSeq(mouSeq_Remove,storage_Sequence);cvReleaseMemStorage(&storage_Remove);}
				 else{
					 if(flag_Sequence){mouSeq_Sequence=cvCloneSeq(mouSeq_Sequence,storage_Sequence);}
				 }
			 }
		 }
	 }
	
	/*if(mouSeq_Remove!=NULL){mouSeq_Sequence=cvCloneSeq(mouSeq_Remove,storage_Sequence);
	                cvReleaseMemStorage(&storage_Remove);
	}
	else
	{
		if(mouSeq_Adjust!=NULL){mouSeq_Sequence=cvCloneSeq(mouSeq_Adjust,storage_Sequence);
		         cvReleaseMemStorage(&storage_Adjust);}
		else{
			if(mouSeq_Offset!=NULL){mouSeq_Sequence=cvCloneSeq(mouSeq_Offset,storage_Sequence);
			                   cvReleaseMemStorage(&storage_Offset);}
			else{mouSeq_Sequence=cvCloneSeq(mouSeq,storage_Sequence);
			    cvReleaseMemStorage(&storage3);}
		}
    }*/
	
	cvNamedWindow( "Set Point Sequence", CV_WINDOW_AUTOSIZE );
	
	if(flag_Offset)
	{
	    cvShowImage("Set Point Sequence",src_Offset);
	    cvSetMouseCallback("Set Point Sequence",mousehandler_setsequence,(void*)src_Offset);
		src_Sequence=cvCloneImage(src_Offset);
		cvReleaseImage(&src_Offset);
	}
	else{
		if(flag_Process)
		{
        cvShowImage("Set Point Sequence",src);
	    cvSetMouseCallback("Set Point Sequence",mousehandler_setsequence,(void*)src);
		src_Sequence=cvCloneImage(src);
		//cvReleaseImage(&src);
		}
		else{
			if(flag_Adjust)
			{
			cvShowImage("Set Point Sequence",src_Adjust);
	        cvSetMouseCallback("Set Point Sequence",mousehandler_setsequence,(void*)src_Adjust);
			src_Sequence=cvCloneImage(src_Adjust);
			cvReleaseImage(&src_Adjust);
			}
			else{
				if(flag_Remove){
            cvShowImage("Set Point Sequence",src_Remove);
	        cvSetMouseCallback("Set Point Sequence",mousehandler_setsequence,(void*)src_Remove);
			src_Sequence=cvCloneImage(src_Remove);
			cvReleaseImage(&src_Remove);}
				else{
					if(flag_Sequence){cvShowImage("Set Point Sequence",src_Sequence);
					cvSetMouseCallback("Set Point Sequence",mousehandler_setsequence,(void*)src_Sequence);}
				}
			}
		}
	}
	//cvReleaseMemStorage(&storage_Sequence);//*******************************************
	flag_Process=FALSE;
	flag_Offset=FALSE;
	flag_Adjust=FALSE;
	flag_Remove=FALSE;
	flag_Sequence=TRUE;

	cvWaitKey(0);
	if(mouSeq_Result->total==38){
		cvDestroyWindow("Set Point Sequence");}
}

void ImageProcess::GenerateVal3File()  //三维转换
{
	//释放上一步内存
	cvReleaseMemStorage(&storage_Sequence);
	cvReleaseImage(&src_Sequence);          //释放上一步内存

	//保存提取出来的像素坐标点(u,v)
	double pixel[2][36];    //保存提取出来的像素坐标点(u,v)
	for(int i=0;i<mouSeq_Result->total;i++)
		{
      CvPoint* p = CV_GET_SEQ_ELEM(CvPoint,mouSeq_Result,i);
	  pixel[0][i]=p->x;
	  pixel[1][i]=p->y;
	  //std::cout<<p->x<<","<<p->y<<endl;
        }
	
	//转化为相机坐标系中的点(Xc,Yc)
	double fx=7973.83453,fy=7988.59815,u0=825.77215,v0=413,Zc=456.5;//相机内参数
	double Pc[2][36];  //转化为相机坐标系中的点(Xc,Yc)
	for(int i=0;i<mouSeq_Result->total;i++)
		{
			Pc[0][i]=(pixel[0][i]-u0)*Zc/fx;
			Pc[1][i]=(pixel[1][i]-v0)*Zc/fy;
	}

	//转化为法兰盘坐标系中的点
	double R_camera2flange[3][3];
	double t_camera2flange[3][1];
	R_camera2flange[0][0]=-0.6731;R_camera2flange[0][1]=-0.7399;R_camera2flange[0][2]=0.0183;
	R_camera2flange[1][0]=0.7401;R_camera2flange[1][1]=-0.6733;R_camera2flange[1][2]=-0.0013;
	R_camera2flange[2][0]=0.0384;R_camera2flange[2][1]=0.0011;R_camera2flange[2][2]=0.9998;
	//t_camera2flange[0][0]= -64.6009;t_camera2flange[1][0]= -94.1738;t_camera2flange[2][0]=105.6201;
	//t_camera2flange[0][0]= -64.6009;t_camera2flange[1][0]= -90.4407;t_camera2flange[2][0]=112.5460;
	//t_camera2flange[0][0]= -64.6009;t_camera2flange[1][0]= -94.1738;t_camera2flange[2][0]=112.5460;
	//t_camera2flange[0][0]= -63.5053;t_camera2flange[1][0]= -94.1738;t_camera2flange[2][0]=112.5460;
	//t_camera2flange[0][0]= -62.5053;t_camera2flange[1][0]= -94.1738;t_camera2flange[2][0]=112.5460;
	//t_camera2flange[0][0]= -60.1041;t_camera2flange[1][0]= -90.5097;t_camera2flange[2][0]=112.5460;//第二次估算得到的数据
	//t_camera2flange[0][0]= -64.2809;t_camera2flange[1][0]= -87.5926;t_camera2flange[2][0]=112.5460;//估算得到的数据
	//t_camera2flange[0][0]= -60.1041;t_camera2flange[1][0]= -94.1738;t_camera2flange[2][0]=112.5460;
	//t_camera2flange[0][0]= -59.4837;t_camera2flange[1][0]= -94.1738;t_camera2flange[2][0]=112.5460;
	//t_camera2flange[0][0]= -59.4837;t_camera2flange[1][0]= -95.1738;t_camera2flange[2][0]=112.5460;
	t_camera2flange[0][0]= -59.0041;t_camera2flange[1][0]= -94.1738;t_camera2flange[2][0]=112.5460;
	/*R_camera2flange[0][0]=-0.6790;R_camera2flange[0][1]=-0.7394;R_camera2flange[0][2]=0.0276;
	R_camera2flange[1][0]=0.7388;R_camera2flange[1][1]=-0.6796;R_camera2flange[1][2]=-0.0299;
	R_camera2flange[2][0]=0.0415;R_camera2flange[2][1]=-0.0009;R_camera2flange[2][2]=0.9991;
	t_camera2flange[0][0]= -66.0279;t_camera2flange[1][0]= -85.2460;t_camera2flange[2][0]=112.5460;*/

	double Pf[3][36];  //转化为法兰盘坐标系中的点
	
	for(int i=0;i<mouSeq_Result->total;i++)
		{
			Pf[0][i]=R_camera2flange[0][0]*Pc[0][i]+R_camera2flange[0][1]*Pc[1][i]+R_camera2flange[0][2]*Zc+t_camera2flange[0][0];
			Pf[1][i]=R_camera2flange[1][0]*Pc[0][i]+R_camera2flange[1][1]*Pc[1][i]+R_camera2flange[1][2]*Zc+t_camera2flange[1][0];
			Pf[2][i]=R_camera2flange[2][0]*Pc[0][i]+R_camera2flange[2][1]*Pc[1][i]+R_camera2flange[2][2]*Zc+t_camera2flange[2][0];
      }

	//转化为机器人基坐标系中的点
	const double Pi=3.141592653;
    const double DEG2RAD=Pi/180;
    const double RAD2DEG=180/Pi;

	double Rx,Ry,Rz;
	vector<double> Pos(6,0);
	GetRobot()->GetRobotJntCartPos(Pos,vector<double>(6,0));
	double t_flange2base[3][1];
	t_flange2base[0][0]=Pos[0]*1000;
	t_flange2base[1][0]=Pos[1]*1000;
	t_flange2base[2][0]=Pos[2]*1000;
	double R_flange2base[3][3];
	Rx=Pos[3];Ry=Pos[4];Rz=Pos[5];
	R_flange2base[0][0]=cos(Rz)*cos(Ry);R_flange2base[0][1]=-sin(Rz)*cos(Ry);R_flange2base[0][2]=sin(Ry);
	R_flange2base[1][0]=sin(Rz)*cos(Rx)+cos(Rz)*sin(Ry)*sin(Rx);R_flange2base[1][1]=cos(Rz)*cos(Rx)-sin(Rz)*sin(Ry)*sin(Rx);R_flange2base[1][2]=-cos(Ry)*sin(Rx);
	R_flange2base[2][0]=sin(Rz)*sin(Rx)-cos(Rz)*sin(Ry)*cos(Rx);R_flange2base[2][1]=cos(Rz)*sin(Rx)+sin(Rz)*sin(Ry)*cos(Rx);R_flange2base[2][2]=cos(Ry)*cos(Rx);
	
	double Pr[3][36];   //转化为机器人基坐标系中的点
	for(int i=0;i<mouSeq_Result->total;i++)
		{
            Pr[0][i]=R_flange2base[0][0]*Pf[0][i]+R_flange2base[0][1]*Pf[1][i]+R_flange2base[0][2]*Pf[2][i]+t_flange2base[0][0];
			Pr[1][i]=R_flange2base[1][0]*Pf[0][i]+R_flange2base[1][1]*Pf[1][i]+R_flange2base[1][2]*Pf[2][i]+t_flange2base[1][0];
			Pr[2][i]=R_flange2base[2][0]*Pf[0][i]+R_flange2base[2][1]*Pf[1][i]+R_flange2base[2][2]*Pf[2][i]+t_flange2base[2][0];
     }
	double Zr;
	Zr=-51.12;
	 QFile f("d:\\test.txt");
	if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		cout << "Open failed." << endl;
		//return -1;
	}
    QTextStream txtOutput(&f);
	
	txtOutput <<"Position of flange in robot base coordinates"<< endl;
	txtOutput <<"X="<<t_flange2base[0][0]<< ","<<"y="<<t_flange2base[1][0]<<","<<"Z="<<t_flange2base[2][0]<<endl;
	txtOutput <<"Rx="<<Rx*RAD2DEG<<","<<"Ry="<<Ry*RAD2DEG<<","<<"Rz="<<Rz*RAD2DEG<<endl;

	txtOutput <<"Points in pixel coordinates"<< endl;
	txtOutput <<"p0 "<<"x="<<pixel[0][0]<< ","<<"y="<<pixel[1][0]<<endl;
	txtOutput <<"p1 "<<"x="<<pixel[0][1]<< ","<<"y="<<pixel[1][1]<<endl;
	txtOutput <<"p2 "<<"x="<<pixel[0][2]<< ","<<"y="<<pixel[1][2]<<endl;
	txtOutput <<"p3 "<<"x="<<pixel[0][3]<< ","<<"y="<<pixel[1][3]<<endl;
	txtOutput <<"p4 "<<"x="<<pixel[0][4]<< ","<<"y="<<pixel[1][4]<<endl;
	txtOutput <<"p5 "<<"x="<<pixel[0][5]<< ","<<"y="<<pixel[1][5]<<endl;
	txtOutput <<"p6 "<<"x="<<pixel[0][6]<< ","<<"y="<<pixel[1][6]<<endl;
	txtOutput <<"p7 "<<"x="<<pixel[0][7]<< ","<<"y="<<pixel[1][7]<<endl;
	txtOutput <<"p8 "<<"x="<<pixel[0][8]<< ","<<"y="<<pixel[1][8]<<endl;
	txtOutput <<"p9 "<<"x="<<pixel[0][9]<< ","<<"y="<<pixel[1][9]<<endl;
	txtOutput <<"p10 "<<"x="<<pixel[0][10]<< ","<<"y="<<pixel[1][10]<<endl;
	txtOutput <<"p11 "<<"x="<<pixel[0][11]<< ","<<"y="<<pixel[1][11]<<endl;
	txtOutput <<"p12 "<<"x="<<pixel[0][12]<< ","<<"y="<<pixel[1][12]<<endl;

	txtOutput <<"Points in camera coordinates"<< endl;
	txtOutput <<"p0 "<<"x="<<Pc[0][0]<< ","<<"y="<<Pc[1][0]<<endl;
	txtOutput <<"p1 "<<"x="<<Pc[0][1]<< ","<<"y="<<Pc[1][1]<<endl;
	txtOutput <<"p2 "<<"x="<<Pc[0][2]<< ","<<"y="<<Pc[1][2]<<endl;
	txtOutput <<"p3 "<<"x="<<Pc[0][3]<< ","<<"y="<<Pc[1][3]<<endl;
	txtOutput <<"p4 "<<"x="<<Pc[0][4]<< ","<<"y="<<Pc[1][4]<<endl;
	txtOutput <<"p5 "<<"x="<<Pc[0][5]<< ","<<"y="<<Pc[1][5]<<endl;
	txtOutput <<"p6 "<<"x="<<Pc[0][6]<< ","<<"y="<<Pc[1][6]<<endl;
	txtOutput <<"p7 "<<"x="<<Pc[0][7]<< ","<<"y="<<Pc[1][7]<<endl;
	txtOutput <<"p8 "<<"x="<<Pc[0][8]<< ","<<"y="<<Pc[1][8]<<endl;
	txtOutput <<"p9 "<<"x="<<Pc[0][9]<< ","<<"y="<<Pc[1][9]<<endl;
	txtOutput <<"p10 "<<"x="<<Pc[0][10]<< ","<<"y="<<Pc[1][10]<<endl;
	txtOutput <<"p11 "<<"x="<<Pc[0][11]<< ","<<"y="<<Pc[1][11]<<endl;
	txtOutput <<"p12 "<<"x="<<Pc[0][12]<< ","<<"y="<<Pc[1][12]<<endl;

	txtOutput <<"Points in flange coordinates"<< endl;
	txtOutput <<"p0 "<<"x="<<Pf[0][0]<< ","<<"y="<<Pf[1][0]<<endl;
	txtOutput <<"p1 "<<"x="<<Pf[0][1]<< ","<<"y="<<Pf[1][1]<<endl;
	txtOutput <<"p2 "<<"x="<<Pf[0][2]<< ","<<"y="<<Pf[1][2]<<endl;
	txtOutput <<"p3 "<<"x="<<Pf[0][3]<< ","<<"y="<<Pf[1][3]<<endl;
	txtOutput <<"p4 "<<"x="<<Pf[0][4]<< ","<<"y="<<Pf[1][4]<<endl;
	txtOutput <<"p5 "<<"x="<<Pf[0][5]<< ","<<"y="<<Pf[1][5]<<endl;
	txtOutput <<"p6 "<<"x="<<Pf[0][6]<< ","<<"y="<<Pf[1][6]<<endl;
	txtOutput <<"p7 "<<"x="<<Pf[0][7]<< ","<<"y="<<Pf[1][7]<<endl;
	txtOutput <<"p8 "<<"x="<<Pf[0][8]<< ","<<"y="<<Pf[1][8]<<endl;
	txtOutput <<"p9 "<<"x="<<Pf[0][9]<< ","<<"y="<<Pf[1][9]<<endl;
	txtOutput <<"p10 "<<"x="<<Pf[0][10]<< ","<<"y="<<Pf[1][10]<<endl;
	txtOutput <<"p11 "<<"x="<<Pf[0][11]<< ","<<"y="<<Pf[1][11]<<endl;
	txtOutput <<"p12 "<<"x="<<Pf[0][12]<< ","<<"y="<<Pf[1][12]<<endl;

	txtOutput <<"Points in robot base coordinates"<< endl;
	txtOutput <<"p0 "<<"x="<<Pr[0][0]<< ","<<"y="<<Pr[1][0]<<endl;
	txtOutput <<"p1 "<<"x="<<Pr[0][1]<< ","<<"y="<<Pr[1][1]<<endl;
	txtOutput <<"p2 "<<"x="<<Pr[0][2]<< ","<<"y="<<Pr[1][2]<<endl;
	txtOutput <<"p3 "<<"x="<<Pr[0][3]<< ","<<"y="<<Pr[1][3]<<endl;
	txtOutput <<"p4 "<<"x="<<Pr[0][4]<< ","<<"y="<<Pr[1][4]<<endl;
	txtOutput <<"p5 "<<"x="<<Pr[0][5]<< ","<<"y="<<Pr[1][5]<<endl;
	txtOutput <<"p6 "<<"x="<<Pr[0][6]<< ","<<"y="<<Pr[1][6]<<endl;
	txtOutput <<"p7 "<<"x="<<Pr[0][7]<< ","<<"y="<<Pr[1][7]<<endl;
	txtOutput <<"p8 "<<"x="<<Pr[0][8]<< ","<<"y="<<Pr[1][8]<<endl;
	txtOutput <<"p9 "<<"x="<<Pr[0][9]<< ","<<"y="<<Pr[1][9]<<endl;
	txtOutput <<"p10 "<<"x="<<Pr[0][10]<< ","<<"y="<<Pr[1][10]<<endl;
	txtOutput <<"p11 "<<"x="<<Pr[0][11]<< ","<<"y="<<Pr[1][11]<<endl;
	txtOutput <<"p12 "<<"x="<<Pr[0][12]<< ","<<"y="<<Pr[1][12]<<endl;

	txtOutput <<"Output File"<< endl;
    txtOutput <<"p0=compose(pOrigin,world,{"<<Pr[0][0]<<","<<Pr[1][0]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p1=compose(pOrigin,world,{"<<Pr[0][1]<<","<<Pr[1][1]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p2=compose(pOrigin,world,{"<<Pr[0][2]<<","<<Pr[1][2]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p3=compose(pOrigin,world,{"<<Pr[0][3]<<","<<Pr[1][3]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p4=compose(pOrigin,world,{"<<Pr[0][4]<<","<<Pr[1][4]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p5=compose(pOrigin,world,{"<<Pr[0][5]<<","<<Pr[1][5]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p6=compose(pOrigin,world,{"<<Pr[0][6]<<","<<Pr[1][6]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p7=compose(pOrigin,world,{"<<Pr[0][7]<<","<<Pr[1][7]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p8=compose(pOrigin,world,{"<<Pr[0][8]<<","<<Pr[1][8]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p9=compose(pOrigin,world,{"<<Pr[0][9]<<","<<Pr[1][9]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p10=compose(pOrigin,world,{"<<Pr[0][10]<<","<<Pr[1][10]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p11=compose(pOrigin,world,{"<<Pr[0][11]<<","<<Pr[1][11]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
    txtOutput <<"p12=compose(pOrigin,world,{"<<Pr[0][12]<<","<<Pr[1][12]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p13=compose(pOrigin,world,{"<<Pr[0][13]<<","<<Pr[1][13]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p14=compose(pOrigin,world,{"<<Pr[0][14]<<","<<Pr[1][14]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p15=compose(pOrigin,world,{"<<Pr[0][15]<<","<<Pr[1][15]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p16=compose(pOrigin,world,{"<<Pr[0][16]<<","<<Pr[1][16]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p17=compose(pOrigin,world,{"<<Pr[0][17]<<","<<Pr[1][17]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p18=compose(pOrigin,world,{"<<Pr[0][18]<<","<<Pr[1][18]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p19=compose(pOrigin,world,{"<<Pr[0][19]<<","<<Pr[1][19]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p20=compose(pOrigin,world,{"<<Pr[0][20]<<","<<Pr[1][20]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p21=compose(pOrigin,world,{"<<Pr[0][21]<<","<<Pr[1][21]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p22=compose(pOrigin,world,{"<<Pr[0][22]<<","<<Pr[1][22]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p23=compose(pOrigin,world,{"<<Pr[0][23]<<","<<Pr[1][23]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p24=compose(pOrigin,world,{"<<Pr[0][24]<<","<<Pr[1][24]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p25=compose(pOrigin,world,{"<<Pr[0][25]<<","<<Pr[1][25]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p26=compose(pOrigin,world,{"<<Pr[0][26]<<","<<Pr[1][26]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p27=compose(pOrigin,world,{"<<Pr[0][27]<<","<<Pr[1][27]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p28=compose(pOrigin,world,{"<<Pr[0][28]<<","<<Pr[1][28]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p29=compose(pOrigin,world,{"<<Pr[0][29]<<","<<Pr[1][29]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p30=compose(pOrigin,world,{"<<Pr[0][30]<<","<<Pr[1][30]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p31=compose(pOrigin,world,{"<<Pr[0][31]<<","<<Pr[1][31]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p32=compose(pOrigin,world,{"<<Pr[0][32]<<","<<Pr[1][32]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p33=compose(pOrigin,world,{"<<Pr[0][33]<<","<<Pr[1][33]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p34=compose(pOrigin,world,{"<<Pr[0][34]<<","<<Pr[1][34]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p35=compose(pOrigin,world,{"<<Pr[0][35]<<","<<Pr[1][35]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;
	txtOutput <<"p36=compose(pOrigin,world,{"<<Pr[0][36]<<","<<Pr[1][36]<<","<<Zr<<",0.01,-179.99,44.99})"<<endl;

	txtOutput <<"movel(p0,flange,mFast)"<<endl;
    txtOutput <<"movel(p0,flange,mFast)"<<endl;
    txtOutput <<"movel(p1,flange,mSlow)"<<endl;
    txtOutput <<"movec(p2,p3,flange,mSlow)"<<endl;
    txtOutput <<"movec(p4,p5,flange,mSlow)"<<endl;
    txtOutput <<"movec(p6,p7,flange,mSlow)"<<endl;
    txtOutput <<"movec(p8,p9,flange,mSlow)"<<endl;
    txtOutput <<"movec(p10,p11,flange,mSlow)"<<endl;
	//txtOutput <<"movel(p12,flange,mSlow)"<<endl;
	txtOutput <<"movec(p12,p13,flange,mSlow)"<<endl;
	txtOutput <<"movec(p14,p15,flange,mSlow)"<<endl;
	txtOutput <<"movec(p16,p17,flange,mSlow)"<<endl;
	txtOutput <<"movec(p18,p19,flange,mSlow)"<<endl;
	txtOutput <<"movec(p20,p21,flange,mSlow)"<<endl;
	txtOutput <<"movec(p22,p23,flange,mSlow)"<<endl;
	txtOutput <<"movec(p24,p25,flange,mSlow)"<<endl;
	txtOutput <<"movec(p26,p27,flange,mSlow)"<<endl;
	txtOutput <<"movec(p28,p29,flange,mSlow)"<<endl;
	txtOutput <<"movec(p30,p31,flange,mSlow)"<<endl;
	txtOutput <<"movec(p32,p33,flange,mSlow)"<<endl;
	txtOutput <<"movec(p34,p35,flange,mSlow)"<<endl;
	txtOutput <<"movel(p36,flange,mSlow)"<<endl;
    
   // txtOutput <<"movel(p0,flange,mSlow)"<<endl;
    txtOutput <<"waitEndMove()"<<endl;
	
	f.close();
	cvReleaseMemStorage(&storage_Result);
}

//void ImageProcess::GenerateVal3File()   //二维转换
//{
//	//释放上一步内存
//	cvReleaseMemStorage(&storage_Sequence);
//	cvReleaseImage(&src_Sequence);          //释放上一步内存
//	
//	//保存提取出来的像素坐标点(u,v)
//	double pixel[2][13];    //保存提取出来的像素坐标点(u,v)
//	for(int i=0;i<mouSeq_Result->total-1;i++)
//		{
//      CvPoint* p = CV_GET_SEQ_ELEM(CvPoint,mouSeq_Result,i);
//	  pixel[0][i]=p->x;
//	  pixel[1][i]=p->y;
//	  //std::cout<<p->x<<","<<p->y<<endl;
//        }
//	
//	//转化为相机坐标系中的点(Xc,Yc)
//	double fx=7973.83453,fy=7988.59815,u0=825.77215,v0=413,Zc=456.5;//相机内参数
//	double Pc[2][13];  //转化为相机坐标系中的点(Xc,Yc)
//	for(int i=0;i<mouSeq_Result->total-1;i++)
//		{
//			Pc[0][i]=(pixel[0][i]-u0)*Zc/fx;
//			Pc[1][i]=(pixel[1][i]-v0)*Zc/fy;
//	}
//
//	//转化为法兰盘坐标系中的点
//	double R_camera2flange[2][2];
//	double t_camera2flange[3][1];
//	double Rz_camera2flange=2.3093;  //132.3131度
//	R_camera2flange[0][0]=cos(Rz_camera2flange);R_camera2flange[0][1]=-sin(Rz_camera2flange);
//	R_camera2flange[1][0]=sin(Rz_camera2flange);R_camera2flange[1][1]=cos(Rz_camera2flange);
//	t_camera2flange[0][0]= -64.6009;t_camera2flange[1][0]= -94.1738;t_camera2flange[2][0]=105.6201;
//	
//	double Pf[2][13];  //转化为法兰盘坐标系中的点
//	
//	for(int i=0;i<mouSeq_Result->total-1;i++)
//		{
//			Pf[0][i]=R_camera2flange[0][0]*Pc[0][i]+R_camera2flange[0][1]*Pc[1][i]+t_camera2flange[0][0];
//			Pf[1][i]=R_camera2flange[1][0]*Pc[0][i]+R_camera2flange[1][1]*Pc[1][i]+t_camera2flange[1][0];
//			 }
//	
//	//转化为机器人基坐标系中的点
//	const double Pi=3.141592653;
//    const double DEG2RAD=Pi/180;
//    const double RAD2DEG=180/Pi;
//	
//	double Rx,Ry,Rz;
//	vector<double> Pos(6,0);
//	GetRobot()->GetRobotJntCartPos(Pos,vector<double>(6,0));
//	double t_flange2base[3][1];
//	t_flange2base[0][0]=Pos[0]*1000;
//	t_flange2base[1][0]=Pos[1]*1000;
//	t_flange2base[2][0]=Pos[2]*1000;
//	double R_flange2base[2][2];
//	Rx=Pos[3];Ry=Pos[4];Rz=Pos[5];
//	R_flange2base[0][0]=cos(Rz);R_flange2base[0][1]=-sin(Rz);
//	R_flange2base[1][0]=sin(Rz);R_flange2base[1][1]=cos(Rz);
//	double temp_x,temp_y;
//	double P_temp[2][13];
//	double Pr[2][13];   //转化为机器人基坐标系中的点
//	
//	for(int i=0;i<mouSeq_Result->total-1;i++)
//		{ 
//		    P_temp[0][i]=R_flange2base[0][0]*Pf[0][i]+R_flange2base[0][1]*Pf[1][i];//+t_flange2base[0][0];
//			P_temp[1][i]=R_flange2base[1][0]*Pf[0][i]+R_flange2base[1][1]*Pf[1][i];//+t_flange2base[1][0];
//			}
//	if(fabs(fabs(Rx*RAD2DEG)-180)<0.2)
//			{
//          for(int i=0;i<mouSeq_Result->total-1;i++)
//		{ 
//            temp_y =-P_temp[1][i];
//		    Pr[1][i]=temp_y+t_flange2base[1][0];
//			Pr[0][i]=P_temp[0][i]+t_flange2base[0][0];
//			}
//	    }
//
//	if (fabs(fabs(Ry*RAD2DEG)-180)<0.2)
//			{
//       for(int i=0;i<mouSeq_Result->total-1;i++)
//		{ 
//            temp_x=-P_temp[0][i];
//			Pr[0][i]=temp_x+t_flange2base[0][0];
//			Pr[1][i]=P_temp[1][i]+t_flange2base[1][0];
//			}
//	     }
//
//	//for(int i=0;i<mouSeq_Result->total-1;i++)
//		//{
//      
//	  QFile f("d:\\test.txt");
//	if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
//	{
//		cout << "Open failed." << endl;
//		//return -1;
//	}
//    QTextStream txtOutput(&f);
//	//CvPoint* p = CV_GET_SEQ_ELEM(CvPoint,mouSeq_Result,i);
//	txtOutput <<fabs(fabs(Rx*RAD2DEG)-180)<< endl;
//	txtOutput <<fabs(fabs(Ry*RAD2DEG)-180)<< endl;
//	txtOutput <<"Position of flange in robot base coordinates"<< endl;
//	txtOutput <<"X="<<t_flange2base[0][0]<< ","<<"y="<<t_flange2base[1][0]<<","<<"Z="<<t_flange2base[2][0]<<endl;
//	txtOutput <<"Rx="<<Rx*RAD2DEG<<","<<"Ry="<<Ry*RAD2DEG<<","<<"Rz="<<Rz*RAD2DEG<<endl;
//
//	txtOutput <<"Points in pixel coordinates"<< endl;
//	txtOutput <<"p0 "<<"x="<<pixel[0][0]<< ","<<"y="<<pixel[1][0]<<endl;
//	txtOutput <<"p1 "<<"x="<<pixel[0][1]<< ","<<"y="<<pixel[1][1]<<endl;
//	txtOutput <<"p2 "<<"x="<<pixel[0][2]<< ","<<"y="<<pixel[1][2]<<endl;
//	txtOutput <<"p3 "<<"x="<<pixel[0][3]<< ","<<"y="<<pixel[1][3]<<endl;
//	txtOutput <<"p4 "<<"x="<<pixel[0][4]<< ","<<"y="<<pixel[1][4]<<endl;
//	txtOutput <<"p5 "<<"x="<<pixel[0][5]<< ","<<"y="<<pixel[1][5]<<endl;
//	txtOutput <<"p6 "<<"x="<<pixel[0][6]<< ","<<"y="<<pixel[1][6]<<endl;
//	txtOutput <<"p7 "<<"x="<<pixel[0][7]<< ","<<"y="<<pixel[1][7]<<endl;
//	txtOutput <<"p8 "<<"x="<<pixel[0][8]<< ","<<"y="<<pixel[1][8]<<endl;
//	txtOutput <<"p9 "<<"x="<<pixel[0][9]<< ","<<"y="<<pixel[1][9]<<endl;
//	txtOutput <<"p10 "<<"x="<<pixel[0][10]<< ","<<"y="<<pixel[1][10]<<endl;
//	txtOutput <<"p11 "<<"x="<<pixel[0][11]<< ","<<"y="<<pixel[1][11]<<endl;
//	txtOutput <<"p12 "<<"x="<<pixel[0][12]<< ","<<"y="<<pixel[1][12]<<endl;
//
//	txtOutput <<"Points in camera coordinates"<< endl;
//	txtOutput <<"p0 "<<"x="<<Pc[0][0]<< ","<<"y="<<Pc[1][0]<<endl;
//	txtOutput <<"p1 "<<"x="<<Pc[0][1]<< ","<<"y="<<Pc[1][1]<<endl;
//	txtOutput <<"p2 "<<"x="<<Pc[0][2]<< ","<<"y="<<Pc[1][2]<<endl;
//	txtOutput <<"p3 "<<"x="<<Pc[0][3]<< ","<<"y="<<Pc[1][3]<<endl;
//	txtOutput <<"p4 "<<"x="<<Pc[0][4]<< ","<<"y="<<Pc[1][4]<<endl;
//	txtOutput <<"p5 "<<"x="<<Pc[0][5]<< ","<<"y="<<Pc[1][5]<<endl;
//	txtOutput <<"p6 "<<"x="<<Pc[0][6]<< ","<<"y="<<Pc[1][6]<<endl;
//	txtOutput <<"p7 "<<"x="<<Pc[0][7]<< ","<<"y="<<Pc[1][7]<<endl;
//	txtOutput <<"p8 "<<"x="<<Pc[0][8]<< ","<<"y="<<Pc[1][8]<<endl;
//	txtOutput <<"p9 "<<"x="<<Pc[0][9]<< ","<<"y="<<Pc[1][9]<<endl;
//	txtOutput <<"p10 "<<"x="<<Pc[0][10]<< ","<<"y="<<Pc[1][10]<<endl;
//	txtOutput <<"p11 "<<"x="<<Pc[0][11]<< ","<<"y="<<Pc[1][11]<<endl;
//	txtOutput <<"p12 "<<"x="<<Pc[0][12]<< ","<<"y="<<Pc[1][12]<<endl;
//
//	txtOutput <<"Points in flange coordinates"<< endl;
//	txtOutput <<"p0 "<<"x="<<Pf[0][0]<< ","<<"y="<<Pf[1][0]<<endl;
//	txtOutput <<"p1 "<<"x="<<Pf[0][1]<< ","<<"y="<<Pf[1][1]<<endl;
//	txtOutput <<"p2 "<<"x="<<Pf[0][2]<< ","<<"y="<<Pf[1][2]<<endl;
//	txtOutput <<"p3 "<<"x="<<Pf[0][3]<< ","<<"y="<<Pf[1][3]<<endl;
//	txtOutput <<"p4 "<<"x="<<Pf[0][4]<< ","<<"y="<<Pf[1][4]<<endl;
//	txtOutput <<"p5 "<<"x="<<Pf[0][5]<< ","<<"y="<<Pf[1][5]<<endl;
//	txtOutput <<"p6 "<<"x="<<Pf[0][6]<< ","<<"y="<<Pf[1][6]<<endl;
//	txtOutput <<"p7 "<<"x="<<Pf[0][7]<< ","<<"y="<<Pf[1][7]<<endl;
//	txtOutput <<"p8 "<<"x="<<Pf[0][8]<< ","<<"y="<<Pf[1][8]<<endl;
//	txtOutput <<"p9 "<<"x="<<Pf[0][9]<< ","<<"y="<<Pf[1][9]<<endl;
//	txtOutput <<"p10 "<<"x="<<Pf[0][10]<< ","<<"y="<<Pf[1][10]<<endl;
//	txtOutput <<"p11 "<<"x="<<Pf[0][11]<< ","<<"y="<<Pf[1][11]<<endl;
//	txtOutput <<"p12 "<<"x="<<Pf[0][12]<< ","<<"y="<<Pf[1][12]<<endl;
//
//	txtOutput <<"Points in robot base coordinates"<< endl;
//	txtOutput <<"p0 "<<"x="<<Pr[0][0]<< ","<<"y="<<Pr[1][0]<<endl;
//	txtOutput <<"p1 "<<"x="<<Pr[0][1]<< ","<<"y="<<Pr[1][1]<<endl;
//	txtOutput <<"p2 "<<"x="<<Pr[0][2]<< ","<<"y="<<Pr[1][2]<<endl;
//	txtOutput <<"p3 "<<"x="<<Pr[0][3]<< ","<<"y="<<Pr[1][3]<<endl;
//	txtOutput <<"p4 "<<"x="<<Pr[0][4]<< ","<<"y="<<Pr[1][4]<<endl;
//	txtOutput <<"p5 "<<"x="<<Pr[0][5]<< ","<<"y="<<Pr[1][5]<<endl;
//	txtOutput <<"p6 "<<"x="<<Pr[0][6]<< ","<<"y="<<Pr[1][6]<<endl;
//	txtOutput <<"p7 "<<"x="<<Pr[0][7]<< ","<<"y="<<Pr[1][7]<<endl;
//	txtOutput <<"p8 "<<"x="<<Pr[0][8]<< ","<<"y="<<Pr[1][8]<<endl;
//	txtOutput <<"p9 "<<"x="<<Pr[0][9]<< ","<<"y="<<Pr[1][9]<<endl;
//	txtOutput <<"p10 "<<"x="<<Pr[0][10]<< ","<<"y="<<Pr[1][10]<<endl;
//	txtOutput <<"p11 "<<"x="<<Pr[0][11]<< ","<<"y="<<Pr[1][11]<<endl;
//	txtOutput <<"p12 "<<"x="<<Pr[0][12]<< ","<<"y="<<Pr[1][12]<<endl;
//	
//	txtOutput <<"Output File"<< endl;
//    txtOutput <<"p0=compose(pOrigin,world,{"<<Pr[0][0]<<","<<Pr[1][0]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p1=compose(pOrigin,world,{"<<Pr[0][1]<<","<<Pr[1][1]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p2=compose(pOrigin,world,{"<<Pr[0][2]<<","<<Pr[1][2]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p3=compose(pOrigin,world,{"<<Pr[0][3]<<","<<Pr[1][3]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p4=compose(pOrigin,world,{"<<Pr[0][4]<<","<<Pr[1][4]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p5=compose(pOrigin,world,{"<<Pr[0][5]<<","<<Pr[1][5]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p6=compose(pOrigin,world,{"<<Pr[0][6]<<","<<Pr[1][6]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p7=compose(pOrigin,world,{"<<Pr[0][7]<<","<<Pr[1][7]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p8=compose(pOrigin,world,{"<<Pr[0][8]<<","<<Pr[1][8]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p9=compose(pOrigin,world,{"<<Pr[0][9]<<","<<Pr[1][9]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p10=compose(pOrigin,world,{"<<Pr[0][10]<<","<<Pr[1][10]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p11=compose(pOrigin,world,{"<<Pr[0][11]<<","<<Pr[1][11]<<",300,0,180,0})"<<endl;
//    txtOutput <<"p12=compose(pOrigin,world,{"<<Pr[0][12]<<","<<Pr[1][12]<<",300,0,180,0})"<<endl;
//    txtOutput <<"movej(p0,flange,mFast)"<<endl;
//    txtOutput <<"movel(p1,flange,mSlow)"<<endl;
//    txtOutput <<"movec(p2,p3,flange,mSlow)"<<endl;
//    txtOutput <<"movec(p4,p5,flange,mSlow)"<<endl;
//    txtOutput <<"movec(p6,p7,flange,mSlow)"<<endl;
//    txtOutput <<"movec(p8,p9,flange,mSlow)"<<endl;
//    txtOutput <<"movec(p10,p11,flange,mSlow)"<<endl;
//    txtOutput <<"movel(p12,flange,mSlow)"<<endl;
//    txtOutput <<"movel(p0,flange,mSlow)"<<endl;
//    txtOutput <<"waitEndMove()"<<endl;
//
//	f.close();
   //cvReleaseMemStorage(&storage_Result);
////}
//	
//	//QFile f("d:\\test.txt");
//	//if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
//	//{
//	//	cout << "Open failed." << endl;
//	//	//return -1;
//	//}
// //   QTextStream txtOutput(&f);
//	//QString s1("123");
//	//quint32 n1(123);
//
//	//txtOutput << s1 << endl;
//	//txtOutput << n1 << endl;
//
//	//f.close();
//	//std::cout<<p->x<<","<<p->y<<endl;
//}

//计算点上的法向量

CvPoint2D32f **ImageProcess:: methdvector(const CvPoint* a,CvPoint* vector1,CvPoint* vector2)
{
	
	CvPoint2D32f npoint=cvPoint2D32f((double)a->x,(double)a->y);
	
	CvPoint2D32f *vect1meth=&cvPoint2D32f(((double)vector1->y+(double)vector2->y)*(1)/2,(double)(-1)*(((double)vector1->x+(double)vector2->x)/2));
	double m=sqrt(vect1meth->x*vect1meth->x+vect1meth->y*vect1meth->y);
	vect1meth->x=(vect1meth->x)/m;
	vect1meth->y=(vect1meth->y)/m;

	return &vect1meth;
}

float ImageProcess::angle(float x,float y,float a ,float b)
{
	float line1=sqrt(x*x+y*y);
    float line2=sqrt(a*a+b*b);
    float total=x*a+y*b;
    float cosb=total/(line1*line2);
    return cosb;
}


//除去轮廓上过于尖锐的点
CvSeq* ImageProcess::deleteangle(CvSeq* dst1,float ang)
{ 	 CvSeq* src1=cvCloneSeq(dst1);
     CvPoint*  apre=(CvPoint*)cvGetSeqElem(src1,src1->total);
	 CvPoint*  anow=(CvPoint*)cvGetSeqElem(src1,0);
	 CvPoint*  anext=(CvPoint*)cvGetSeqElem(src1,1);
		for(int i=0;i<(src1->total);)
{	
	
	float cosangle=angle((apre->x-anow->x),(apre->y-anow->y),(anext->x-anow->x),(anext->y-anow->y));
	
	if (cosangle>ang)
		{ 
			cvSeqRemove(src1,i);

		}
		else

		{
			i++;

		}
	if(i==(src1->total-1))
	{
      apre=(CvPoint*)cvGetSeqElem(src1,i-1);
	  anow=(CvPoint*)cvGetSeqElem(src1,i);
	 anext=(CvPoint*)cvGetSeqElem(src1,0);
	}
	else
	{
		  apre=(CvPoint*)cvGetSeqElem(src1,i-1);
	  anow=(CvPoint*)cvGetSeqElem(src1,i);
	  anext=(CvPoint*)cvGetSeqElem(src1,i+1);
	}

	}
	
		return  src1;
}

//手动改变图形
CvSeq* ImageProcess::replaceContourPoint(CvSeq* src,CvPoint* rePoint)   //找出轮廓上距离鼠标点最近的点，并将其由鼠标点替换
{  
   CvSeq * dst=cvCloneSeq(src);
   CvPoint *seqPoint=(CvPoint*)cvGetSeqElem(dst,0);
   int min=(rePoint->x-seqPoint->x)*(rePoint->x-seqPoint->x)+(rePoint->y-seqPoint->y)*(rePoint->y-seqPoint->y);
   int distance;  //=small;
   int flag=0;
   int testint=(rePoint->x-seqPoint->x)^2;
  for(int i=0;i<dst->total;i++)
{
	seqPoint=(CvPoint*)cvGetSeqElem(dst,i);
	testint=(rePoint->x-seqPoint->x)^2;
    distance=(rePoint->x-seqPoint->x)*(rePoint->x-seqPoint->x)+(rePoint->y-seqPoint->y)*(rePoint->y-seqPoint->y);
	
	if(min>distance)
	{
		min=distance;
		flag=i;

	}

}
        cvSeqRemove(dst,flag);
		cvSeqInsert(dst,flag,(void*)rePoint);
	
	return dst ;
}


CvSeq* ImageProcess::deleteContourPoint(CvSeq* src,CvPoint* rePoint)   //找出轮廓上距离鼠标点最近的点，并将其由鼠标点替换
{  
   CvSeq * dst=cvCloneSeq(src);
   CvPoint *seqPoint=(CvPoint*)cvGetSeqElem(dst,0);
   int min=(rePoint->x-seqPoint->x)*(rePoint->x-seqPoint->x)+(rePoint->y-seqPoint->y)*(rePoint->y-seqPoint->y);
   int distance;  //=small;
   int flag=0;
   int testint=(rePoint->x-seqPoint->x)^2;
  for(int i=0;i<dst->total;i++)
{
	seqPoint=(CvPoint*)cvGetSeqElem(dst,i);
	testint=(rePoint->x-seqPoint->x)^2;
    distance=(rePoint->x-seqPoint->x)*(rePoint->x-seqPoint->x)+(rePoint->y-seqPoint->y)*(rePoint->y-seqPoint->y);
	
	if(min>distance)
	{
		min=distance;
		flag=i;

	}

}
        cvSeqRemove(dst,flag);
		//cvSeqInsert(dst,flag,(void*)rePoint);
	
	return dst ;
}


CvSeq* ImageProcess::sequenceContourPoint(CvSeq* src,CvPoint* rePoint,CvSeq* src_Seq)   //找出轮廓上距离鼠标点最近的点，并将其由鼠标点替换
{  
	//CvMemStorage* storage_0=cvCreateMemStorage(0);
   CvSeq* dst=cvCloneSeq(src);
   CvSeq* output=cvCloneSeq(src_Seq);
   
   CvPoint *seqPoint=(CvPoint*)cvGetSeqElem(dst,0);
   //CvPoint *outputPoint;
  
   int min=(rePoint->x-seqPoint->x)*(rePoint->x-seqPoint->x)+(rePoint->y-seqPoint->y)*(rePoint->y-seqPoint->y);
   int distance;  //=small;
   int flag=0;
   int testint=(rePoint->x-seqPoint->x)^2;
  for(int i=0;i<dst->total;i++)
{
	seqPoint=(CvPoint*)cvGetSeqElem(dst,i);
	testint=(rePoint->x-seqPoint->x)^2;
    distance=(rePoint->x-seqPoint->x)*(rePoint->x-seqPoint->x)+(rePoint->y-seqPoint->y)*(rePoint->y-seqPoint->y);
	
	if(min>distance)
	{
		min=distance;
		flag=i;

	}

}
         //CvPoint *outputPoint=CV_GET_SEQ_ELEM(CvPoint,dst,flag);
         CvPoint *outputPoint=(CvPoint*)cvGetSeqElem(dst,flag);
		 
		 cvSeqPush(output,outputPoint);
		 //cvSeqPush(output,&outputPoint);这种是错误的
       
	
	return output;
}

CvSeq* ImageProcess::contourOffset(CvSeq* src,int offsetval)
{
    CvSeq* mcont1=cvCloneSeq(src);
	int cont=mcont1->total;

	CvPoint*  pre=(CvPoint*)cvGetSeqElem(src,(src->total));
	CvPoint*  now=(CvPoint*)cvGetSeqElem(src,0);
	CvPoint*  next=(CvPoint*)cvGetSeqElem(src,1);

	    CvPoint vec1;
		vec1.x=(now->x-pre->x);
		vec1.y=(now->y-pre->y);
		CvPoint vec2;
		vec2.x=(next->x-now->x);
		vec2.y=(next->y-next->y);
		CvPoint2D32f* pointver;
		pointver=*methdvector(now,&vec1,&vec2);
		CvPoint *pointoffset=&cvPoint((int)(now->x+(offsetval*(pointver->x))),(int)(now->y+(offsetval*(pointver->y))));
		
		int arrow_ang=135;
		
		CvPoint intvector=cvPoint(pointoffset->x-now->x,pointoffset->y-now->y);
		
		
		double  x1=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)+(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		double  y1=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)-(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		CvPoint arrow1=cvPoint((int)x1,(int)y1);
		double  x2=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)-(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		double  y2=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)+(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		CvPoint arrow2=cvPoint((int)x2,(int)y2);
		////////////////////////////////////////////////////////
		cvSeqInsert(mcont1,0,(void*)pointoffset);
		
		cvSeqRemove(mcont1,1);
	/////////////////////////////////////////////////////////////
	for(int i=0;i<(src->total);i++)
       {
	
		vec1.x=(now->x-pre->x);
		vec1.y=(now->y-pre->y);

		vec2.x=(next->x-now->x);
		vec2.y=(next->y-next->y);

		pointver=*methdvector(now,&vec1,&vec2);
		pointoffset=&cvPoint((int)(now->x+(offsetval*(pointver->x))),(int)(now->y+(offsetval*(pointver->y))));
		//cvLine(src_temp,*now,*pointoffset,cvScalar(255),1,8,0);
		
		CvPoint intvector=cvPoint(pointoffset->x-now->x,pointoffset->y-now->y);
		
		
	     x1=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)+(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		 y1=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)-(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		 arrow1=cvPoint((int)x1,(int)y1);
	     x2=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)-(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		 y2=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)+(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		 arrow2=cvPoint((int)x2,(int)y2);
		
	   // cvLine(src_temp,*pointoffset,arrow1,cvScalar(255),1,8,0);
		//cvLine(src_temp,*pointoffset,arrow2,cvScalar(255),1,8,0);
		cvSeqInsert(mcont1,i,(void*)pointoffset);
		
		cvSeqRemove(mcont1,i+1);
	 if(i!=src->total-1)
	 {    pre=(CvPoint*)cvGetSeqElem(src,i);
	      now=(CvPoint*)cvGetSeqElem(src,i+1);
	      next=(CvPoint*)cvGetSeqElem(src,i+2);
	 }

	 else
	 {
		 pre=(CvPoint*)cvGetSeqElem(src,i);
		 now=(CvPoint*)cvGetSeqElem(src,i+1);
		 next=(CvPoint*)cvGetSeqElem(src,0);

	 }

	}
	
	return mcont1;
}


void ImageProcess::drawVector(CvSeq* src,int offsetval,void *para)   //第二个参数为箭头长度
{
	IplImage* src_temp=(IplImage*)para;
		
	CvSeq* mcont1=cvCloneSeq(src);
	int cont=mcont1->total;

	CvPoint*  pre=(CvPoint*)cvGetSeqElem(src,(src->total));
	CvPoint*  now=(CvPoint*)cvGetSeqElem(src,0);
	CvPoint*  next=(CvPoint*)cvGetSeqElem(src,1);

	    CvPoint vec1;
		vec1.x=(now->x-pre->x);
		vec1.y=(now->y-pre->y);
		CvPoint vec2;
		vec2.x=(next->x-now->x);
		vec2.y=(next->y-next->y);
		CvPoint2D32f* pointver;
		pointver=*methdvector(now,&vec1,&vec2);
		CvPoint *pointoffset=&cvPoint((int)(now->x+(offsetval*(pointver->x))),(int)(now->y+(offsetval*(pointver->y))));
		cvLine(src_temp,*now,*pointoffset,cvScalar(150),1,8,0);
		int arrow_ang=135;
		
		CvPoint intvector=cvPoint(pointoffset->x-now->x,pointoffset->y-now->y);
		
		
		double  x1=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)+(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		double  y1=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)-(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		CvPoint arrow1=cvPoint((int)x1,(int)y1);
		double  x2=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)-(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		double  y2=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)+(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		CvPoint arrow2=cvPoint((int)x2,(int)y2);
		///////////////////////////////////////////
	    cvLine(src_temp,*pointoffset,arrow1,cvScalar(150),1,8,0);
		cvLine(src_temp,*pointoffset,arrow2,cvScalar(150),1,8,0);
		////////////////////////////////////////////
		cvSeqInsert(mcont1,0,(void*)pointoffset);
		
		cvSeqRemove(mcont1,1);
	////
	for(int i=0;i<(src->total);i++)
       {
	
		vec1.x=(now->x-pre->x);
		vec1.y=(now->y-pre->y);

		vec2.x=(next->x-now->x);
		vec2.y=(next->y-next->y);

		pointver=*methdvector(now,&vec1,&vec2);
		pointoffset=&cvPoint((int)(now->x+(offsetval*(pointver->x))),(int)(now->y+(offsetval*(pointver->y))));
		cvLine(src_temp,*now,*pointoffset,cvScalar(150),1,8,0);
		
		CvPoint intvector=cvPoint(pointoffset->x-now->x,pointoffset->y-now->y);
		
		
	     x1=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)+(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		 y1=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)-(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		 arrow1=cvPoint((int)x1,(int)y1);
	     x2=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)-(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		 y2=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)+(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		 arrow2=cvPoint((int)x2,(int)y2);
		
	       cvLine(src_temp,*pointoffset,arrow1,cvScalar(150),1,8,0);
		   cvLine(src_temp,*pointoffset,arrow2,cvScalar(150),1,8,0);
		
	 if(i!=src->total-1)
	 {    pre=(CvPoint*)cvGetSeqElem(src,i);
	      now=(CvPoint*)cvGetSeqElem(src,i+1);
	      next=(CvPoint*)cvGetSeqElem(src,i+2);
	 }

	 else
	 {
		 pre=(CvPoint*)cvGetSeqElem(src,i);
		 now=(CvPoint*)cvGetSeqElem(src,i+1);
		 next=(CvPoint*)cvGetSeqElem(src,0);
      }
	
	}

}




void ImageProcess::mousehandler(int event,int x,int y,int flags,void* param)
{
	/*cvNamedWindow("src3",1);
    cvShowImage("src3",src3);*/
	//IplImage *src1=cvLoadImage("E://photo//TurbineBlade.png",0);
	//IplImage *src1;
	//cvReleaseImage(&src1);
	//IplImage *src1=cvCloneImage(src3);  //src3为全局变量，src3与src相同，随之改变
	/*if(mouSeq_Offset==NULL){mouSeq_Adjust=cvCloneSeq(mouSeq);}
	else{
		mouSeq_Adjust=cvCloneSeq(mouSeq_Offset);}*/
	//IplImage *src1=cvCloneImage(src); 
	
	if(event==CV_EVENT_LBUTTONDOWN)
	{
        IplImage *src1=cvCloneImage(src3); 
		mouPoint=&cvPoint(x,y);
		 isclicked=true;
		 mouSeq_Adjust=replaceContourPoint(mouSeq_Adjust,mouPoint);
		// cvZero(image1);
		// IplImage* image1=cvCloneImage(src1);
	cvDrawContours(
		src1,
		mouSeq_Adjust,
		cvScalar(100),
		cvScalar(50),
		1
		);
	drawVector(mouSeq_Adjust,-20,src1);  //点一下就会画出矢量图
	cvShowImage("Manual Adjustment",src1);
	///////
	cvReleaseImage(&src_Adjust);
	src_Adjust=cvCloneImage(src1);
	//cvWaitKey(0);
	 
    //Qt中实时更新调节后的图像
	
	//改变图像尺寸大小为1/4
	/*IplImage *temp01;
	IplImage *temp02=NULL;
	temp01=cvCreateImage(cvSize(src1->width/2,src1->height/2),src1->depth,src1->nChannels);
	cvCopyImage(src1,temp02);
	cvResize(temp02,temp01,CV_INTER_LINEAR);*/
	
	cv::Mat img;
	img=cv::Mat(src1);

	//改变图像尺寸大小为1/4
	cv::Mat img01;
	cv::Size dsize=cv::Size(img.cols*0.5,img.rows*0.5);
	resize(img,img01,dsize); 

	//cv::cvtColor(img,img,CV_BGR2RGB);
	QImage image=QImage((unsigned char*)(img01.data),img01.cols,img01.rows,img01.step,QImage::Format_Indexed8);

	manager* pmanager=manager::instance();
	pmanager->GetMVS()->ui.label_ProcessResult->setPixmap(QPixmap::fromImage(image));

	/*cvReleaseImage(&temp01);
	cvReleaseImage(&temp02);*/
	//cvReleaseImage(&src1);
	
	cvReleaseImage(&src1);
	}
  
	if(event==CV_EVENT_LBUTTONUP)
	{
		
		 isclicked=false;
		// cvReleaseImage(&src1);
	}
  
}


void ImageProcess::mousehandler_removepoints(int event,int x,int y,int flags,void* param)
{
	
	if(event==CV_EVENT_LBUTTONDOWN)
	{
         IplImage *src1=cvCloneImage(src3); 
		mouPoint=&cvPoint(x,y);
		 isclicked=true;
		 mouSeq_Remove=deleteContourPoint(mouSeq_Remove,mouPoint);
		
	cvDrawContours(
		src1,
		mouSeq_Remove,
		cvScalar(100),
		cvScalar(50),
		1
		);
	drawVector(mouSeq_Remove,-20,src1);  //点一下就会画出矢量图
	cvShowImage("Remove Points",src1);
	
	cvReleaseImage(&src_Remove);
	src_Remove=cvCloneImage(src1);
	
	 
    //Qt中实时更新调节后的图像
	
	
	cv::Mat img;
	img=cv::Mat(src1);

	//改变图像尺寸大小为1/4
	cv::Mat img01;
	cv::Size dsize=cv::Size(img.cols*0.5,img.rows*0.5);
	resize(img,img01,dsize); 

	//cv::cvtColor(img,img,CV_BGR2RGB);
	QImage image=QImage((unsigned char*)(img01.data),img01.cols,img01.rows,img01.step,QImage::Format_Indexed8);

	manager* pmanager=manager::instance();
	pmanager->GetMVS()->ui.label_ProcessResult->setPixmap(QPixmap::fromImage(image));

	/*cvReleaseImage(&temp01);
	cvReleaseImage(&temp02);*/
	cvReleaseImage(&src1);
	}
  
	if(event==CV_EVENT_LBUTTONUP)
	{
		
		 isclicked=false;
		 //cvReleaseImage(&src1);
	}
  
}

//////////////////////////////////////
void ImageProcess::mousehandler_setsequence(int event,int x,int y,int flags,void* param)
{
	if(mouSeq_Result->total<38){
	if(event==CV_EVENT_LBUTTONDOWN)
	{
         IplImage *src1=cvCloneImage(src3); 
		mouPoint=&cvPoint(x,y);
		 isclicked=true;
		 mouSeq_Result=sequenceContourPoint(mouSeq_Sequence,mouPoint,mouSeq_Result);
		
		/* for(int i=0;i<mouSeq_Result->total;++i)
        {
         CvPoint* p = CV_GET_SEQ_ELEM(CvPoint,mouSeq_Result,i);
         std::cout<<p->x<<","<<p->y<<endl;*/
		// printf("p->x,p->y\n");
       // }
		
	//cvDrawContours(
	//	src1,
	//	mouSeq_Result,
	//	cvScalar(100),
	//	cvScalar(50),
	//	1
	//	);
	//drawVector(mouSeq_Result,-20,src1);  //点一下就会画出矢量图
	//
	//
	// 
 //   //Qt中实时更新调节后的图像
	//
	//
	//cv::Mat img;
	//img=cv::Mat(src1);

	////改变图像尺寸大小为1/4
	//cv::Mat img01;
	//cv::Size dsize=cv::Size(img.cols*0.5,img.rows*0.5);
	//resize(img,img01,dsize); 

	////cv::cvtColor(img,img,CV_BGR2RGB);
	//QImage image=QImage((unsigned char*)(img01.data),img01.cols,img01.rows,img01.step,QImage::Format_Indexed8);

	//manager* pmanager=manager::instance();
	//pmanager->GetMVS()->ui.label_ProcessResult->setPixmap(QPixmap::fromImage(image));

	//
	/////////////////////////////////////
	//cvReleaseImage(&src1);
	
	}
  
	if(event==CV_EVENT_LBUTTONUP)
	{
		
		 isclicked=false;
	}
	}
  
}

