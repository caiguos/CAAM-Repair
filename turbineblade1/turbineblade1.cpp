#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <opencv2\opencv.hpp>
#include <iostream>
using namespace std;
//ͼ������ƫ�õ���
int offset=30;
float pi=3.14159;
CvPoint *replacePoint=&cvPoint(0,0);
bool isclicked=false;
CvSeq* mouSeq=NULL;   //������ȡ���ĵ��������������
CvPoint *mouPoint=NULL;
IplImage *src3=NULL;
//IplImage* src1=NULL;
//CvSeq* seqtemp=0;

//������ϵķ�����

CvPoint2D32f ** methdvector(const CvPoint* a,CvPoint* vector1,CvPoint* vector2);


//�������˳�����е��������γɵ�����ֱ�߽Ƕ�
float angle(float x,float y,float a ,float b);

//��ȥ�����Ϲ��ڼ���ĵ�
CvSeq* deleteangle(CvSeq* dst1,float ang);

//�ֶ��ı�ͼ��
CvSeq* replaceContourPoint(CvSeq* src,CvPoint* rePoint);

CvSeq* contourOffset(CvSeq* src,int offsetval);

//CvSeq* drawVectorAndOffset(CvSeq* src,int offsetval,void *para);

void drawVector(CvSeq* src,int offsetval,void *para);

void mousehandler(int event,int x,int y,int flags,void* param);

int main(int arg,char *argv[])
{   CvMemStorage* storagetemp=cvCreateMemStorage(0);    //�ȷ���opencv��ȡ������ʱ�򣬾�Ҫ����һ���ڴ����洢�ҵ�����������
	
	//IplImage *src=cvLoadImage("TurbineBlade.png",0);
	IplImage *src=cvLoadImage("Image001.bmp",0);    //����Ĭ��λ��ͼ��
	IplImage *src_temp=cvCreateImage(cvGetSize(src),src->depth,1);   //src_tempΪ��ͨ���Ҷ�ͼ
	cvThreshold(src,src,110,255,CV_THRESH_BINARY);  //��ֵ�ָӦ����ʽ����ֵ
	//cvAdaptiveThreshold(src,src,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,3,5);
	//cvAdaptiveThreshold(src,src,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,7,5);
	src3=cvCloneImage(src);   //����src��src3��src�ı䣬src3Ҳ����֮�ı䣬��ʱsrc3Ϊ��ֵ�ָ��Ķ�ֵ��ͼ��
	CvMemStorage* storage1=cvCreateMemStorage(0);
	CvMemStorage* storage2=cvCreateMemStorage(0);//����block_size��Ӧ�ڴ�����ÿ���ڴ��Ĵ�С��Ϊ0ʱ�ڴ��Ĭ�ϴ�СΪ64k
	CvSeq *contour=NULL,*mcont=NULL;
	IplImage *temp=cvCreateImage(cvGetSize(src),src->depth,src->nChannels);//tempΪ��ֵ�ָ���src����������ı�
	
	cvNamedWindow("src",1);
	cvCopyImage(src,src_temp);  //��ǰ�߸�������
//	double t=(double)cvGetTickCount();
	IplImage *src1=cvCloneImage(src);
	cvSetMouseCallback("src",mousehandler,(void*)src);
	for(int i=1;i<=10;i++)
	{
	cvMorphologyEx(src_temp,src_temp,temp,NULL,CV_MOP_OPEN,4);
    cvMorphologyEx(src_temp,src_temp,temp,NULL,CV_MOP_CLOSE,4);

     //smooth the image
    cvSmooth(src_temp,src_temp,CV_BLUR,3,3);
    cvSmooth(src_temp,src_temp,CV_GAUSSIAN,3,3);
	cvSmooth(src_temp,src_temp,CV_MEDIAN,3,3);
	}	
	cvFindContours(src_temp,storage1,&contour,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);    //CV_CHAIN_APPROX_SIMPLEѹ��ˮƽ���򣬴�ֱ���򣬶Խ��߷����Ԫ�أ�ֻ�����÷�����յ����꣬����һ����������ֻ��4����������������Ϣ

	
	CvSeq* contourtemp=cvCloneSeq(contour,storagetemp);

	cout<<"temp"<<contour->total<<endl;
	CvSeq* contour1=deleteangle(contour,-0.93);

	//cout<<contour1->total<<endl;
	cvZero(src_temp);
	mcont=cvApproxPoly(contour1,sizeof(CvContour),storage2,CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.001,0);  //cvContourPerimeter�������ܳ�
     //����ʸ��ͼ

	//ͼ��ƫ��

	
	cout<<"or"<<mcont->total<<endl;
//	CvSeq*mcont1=contourOffset(mcont,offset);
	mcont=contourOffset(mcont,offset);


	CvSeq* mcont2=deleteangle(mcont,-0.2);
	mouSeq=cvCloneSeq(mcont2);
for(int i=0;i<mouSeq->total;++i)

{

CvPoint* p = CV_GET_SEQ_ELEM(CvPoint,mouSeq,i);
cout<<p->x<<","<<p->y<<endl;


}
	//CvSeq* mcont3=replaceContourPoint(mcont2,&cvPoint(53,827));
	//CvSeq* mcont3=drawVectorAndOffset(mcont2,20,src_temp);
    drawVector(mcont2,-20,src_temp);

	//cout<<"now"<<mcont3->total;
	cvDrawContours(
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
		);

	
	cvDrawContours(
		src,
		mcont2,
		cvScalar(100),
		cvScalar(50),
		1
		);
	

	//t=(double)cvGetTickCount()-t;
	IplImage* smallimg=cvCreateImage(cvSize(src_temp->width/2,src_temp->height/2),src_temp->depth,src->nChannels);    //cvCreateImage�����׵�ַ������洢�ռ�
	cvResize(src_temp,smallimg,CV_INTER_LINEAR);    //ƥ��src_temp��smallimg�Ĵ�С�Ȳ���
	cvShowImage("src",src);
	cvNamedWindow("src_temp",1);

	/*cvNamedWindow("src3",1);
	cvShowImage("src3",src3);*/
	
	cvShowImage("src_temp",smallimg);
	cvWaitKey(0);
	cvReleaseMemStorage(&storage1);
	cvReleaseMemStorage(&storage2);
	
	cvReleaseMemStorage(&storagetemp);
	cvReleaseImage(&src);
	cvDestroyWindow("src");




}

//������ϵķ�����

CvPoint2D32f ** methdvector(const CvPoint* a,CvPoint* vector1,CvPoint* vector2)
{
	
	CvPoint2D32f npoint=cvPoint2D32f((double)a->x,(double)a->y);
	
	CvPoint2D32f *vect1meth=&cvPoint2D32f(((double)vector1->y+(double)vector2->y)*(1)/2,(double)(-1)*(((double)vector1->x+(double)vector2->x)/2));
	double m=sqrt(vect1meth->x*vect1meth->x+vect1meth->y*vect1meth->y);
	vect1meth->x=(vect1meth->x)/m;
	vect1meth->y=(vect1meth->y)/m;

	
	return &vect1meth;

}

float angle(float x,float y,float a ,float b)
{float line1=sqrt(x*x+y*y);
float line2=sqrt(a*a+b*b);
float total=x*a+y*b;
float cosb=total/(line1*line2);
return cosb;
}


//��ȥ�����Ϲ��ڼ���ĵ�
CvSeq* deleteangle(CvSeq* dst1,float ang)
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
	if(i==(src1->total-1)){
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

//�ֶ��ı�ͼ��
CvSeq* replaceContourPoint(CvSeq* src,CvPoint* rePoint)   //�ҳ������Ͼ�����������ĵ㣬�������������滻
{  
CvSeq * dst=cvCloneSeq(src);
CvPoint *seqPoint=(CvPoint*)cvGetSeqElem(dst,0);
int small=(rePoint->x-seqPoint->x)*(rePoint->x-seqPoint->x)+(rePoint->y-seqPoint->y)*(rePoint->y-seqPoint->y);
int distance;  //=small;
int flag=0;
int testint=(rePoint->x-seqPoint->x)^2;
for(int i=0;i<dst->total-1;i++)
{
	seqPoint=(CvPoint*)cvGetSeqElem(dst,i);
	testint=(rePoint->x-seqPoint->x)^2;
    distance=(rePoint->x-seqPoint->x)*(rePoint->x-seqPoint->x)+(rePoint->y-seqPoint->y)*(rePoint->y-seqPoint->y);
	cout<<"seq x"<<seqPoint->x<<"seq y"<<seqPoint->y<<"num"<<i+1<<"distance"<<distance<<"xxxx"<<testint<<endl;
	if(small>distance)
	{
		small=distance;
		flag=i;

	}

	   
}
        cvSeqRemove(dst,flag);
		cvSeqInsert(dst,flag,(void*)rePoint);
		
		if(flag==0){
		//cvSeqRemove(dst,flag+1);
		//  cvSeqRemove(dst,dst->total-1);
		}

		else if(flag==dst->total-1)
		{

			//cvSeqRemove(dst,0);
		  //cvSeqRemove(dst,dst->total-1);
		}

		else
		{

			//cvSeqRemove(dst,flag+1);
		//  cvSeqRemove(dst,flag-1);

		}
	return dst ;
}
CvSeq* contourOffset(CvSeq* src,int offsetval)
{

		//CvMemStorage* storage3=cvCreateMemStorage(0);
	
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
		
	

		cvSeqInsert(mcont1,0,(void*)pointoffset);
		
		cvSeqRemove(mcont1,1);
	
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


/*CvSeq* drawVectorAndOffset(CvSeq* src,int offsetval,void *para)
{
	IplImage* src_temp=(IplImage*)para;
		//CvMemStorage* storage3=cvCreateMemStorage(0);
	
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
		cvLine(src_temp,*now,*pointoffset,cvScalar(255),1,8,0);
		int arrow_ang=135;
		
		CvPoint intvector=cvPoint(pointoffset->x-now->x,pointoffset->y-now->y);
		
		
		double  x1=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)+(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		double  y1=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)-(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		CvPoint arrow1=cvPoint((int)x1,(int)y1);
			double  x2=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)-(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		double  y2=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)+(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		CvPoint arrow2=cvPoint((int)x2,(int)y2);
		
	    cvLine(src_temp,*pointoffset,arrow1,cvScalar(255),1,8,0);
		cvLine(src_temp,*pointoffset,arrow2,cvScalar(255),1,8,0);

		cvSeqInsert(mcont1,0,(void*)pointoffset);
		
		cvSeqRemove(mcont1,1);
	
	for(int i=0;i<(src->total);i++)
{
	
		vec1.x=(now->x-pre->x);
		vec1.y=(now->y-pre->y);

		vec2.x=(next->x-now->x);
		vec2.y=(next->y-next->y);

		pointver=*methdvector(now,&vec1,&vec2);
		pointoffset=&cvPoint((int)(now->x+(offsetval*(pointver->x))),(int)(now->y+(offsetval*(pointver->y))));
		cvLine(src_temp,*now,*pointoffset,cvScalar(255),1,8,0);
		
		CvPoint intvector=cvPoint(pointoffset->x-now->x,pointoffset->y-now->y);
		
		
	     x1=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)+(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		 y1=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)-(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		 arrow1=cvPoint((int)x1,(int)y1);
	     x2=pointoffset->x+((intvector.x)*cos(arrow_ang*pi/180)-(intvector.y)*sin(arrow_ang*pi/180))/2.5;
		 y2=pointoffset->y+((intvector.y)*cos(arrow_ang*pi/180)+(intvector.x)*sin(arrow_ang*pi/180))/2.5;
		 arrow2=cvPoint((int)x2,(int)y2);
		
	    cvLine(src_temp,*pointoffset,arrow1,cvScalar(255),1,8,0);
		cvLine(src_temp,*pointoffset,arrow2,cvScalar(255),1,8,0);
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
}*/



void drawVector(CvSeq* src,int offsetval,void *para)   //�ڶ�������Ϊ��ͷ����
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
		
	    cvLine(src_temp,*pointoffset,arrow1,cvScalar(150),1,8,0);
		cvLine(src_temp,*pointoffset,arrow2,cvScalar(150),1,8,0);

		cvSeqInsert(mcont1,0,(void*)pointoffset);
		
		cvSeqRemove(mcont1,1);
	
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

void mousehandler(int event,int x,int y,int flags,void* param)
{
	
	//IplImage *src1=cvLoadImage("E://photo//TurbineBlade.png",0);
	IplImage *src1=cvCloneImage(src3);  //src3Ϊȫ�ֱ�����src3��src��ͬ����֮�ı�
	if(event==CV_EVENT_LBUTTONDOWN)
	{
		//cout<<x<<" "<<y<<endl;
		mouPoint=&cvPoint(x,y);
		 isclicked=true;
		 mouSeq=replaceContourPoint(mouSeq,mouPoint);
		// cvZero(image1);
		// IplImage* image1=cvCloneImage(src1);
	cvDrawContours(
		src1,
		mouSeq,
		cvScalar(100),
		cvScalar(50),
		1
		);
	drawVector(mouSeq,-20,src1);  //��һ�¾ͻử��ʸ��ͼ
	cvShowImage("src",src1);

	}
  
	if(event==CV_EVENT_LBUTTONUP)
	{
		
		 isclicked=false;
	}
  


}