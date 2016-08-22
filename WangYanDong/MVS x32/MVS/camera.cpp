#include "camera.h"
#include <QFileDialog>
#include <QtGui>
#include<HVDAILT.H>
#include<HVDef.h>
//#include<HVUtil.h>
//#include<HVDevDetect.h>
//#include<GDeviceFinder.h>
//#include<HVImageProcess.h>
#include<Raw2Rgb.h>
//#include<ImageLoad.h>
//#include<HVError.h>
#include<assert.h>
#include<Windows.h>
#include <QtWidgets>
//#include "messagebox.h"
#include "manager.h"
//#include "mvs.h"

using namespace mvs;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

const HV_BAYER_CONVERT_TYPE ConvertType = BAYER2RGB_NEIGHBOUR;   ///English:    ///Chinese: Raw2RGB�㷨

Camera::Camera()
{
  
}

Camera::~Camera()
{
}
	
void Camera::InitCamera()
{ if(m_bOpen==TRUE||m_bStart==TRUE){}
else{
    HVSTATUS status = BeginHVDevice(1, &m_hhv);   ///English:    ///Chinese: �������
	//HV_VERIFY(status);
	/*HVSTATUS*/ //status =STATUS_OK;
	//status=HVSetResolution(m_hhv,RES_MODE3);
	//HV_RESOLUTION
	m_bOpen			= FALSE; ///English:    ///Chinese: ���������־
	m_bStart        = FALSE; ///English:    ///Chinese: ��ʼ�ɼ���־

	m_pBmpInfo		= NULL;  ///English:    ///Chinese: BMPͼ����Ϣ
	m_pRawBuffer	= NULL;  ///English:    ///Chinese: ԭʼͼ�����ݻ����ʼ��
	m_pImageBuffer	= NULL;  ///English:    ///Chinese: Bayerת����ͼ�����ݻ�������ʼ��

	m_nMaxWid =0;
	m_nMaxHei =0;
	
	for(int i=0;i<256;i++)   ///English:    ///Chinese: ��ɫ���ұ��ʼ��
	{
		m_pLutR[i] = i;
		m_pLutG[i] = i;
		m_pLutB[i] = i;
	}

	HVGetDeviceInfo_Resolution();  ///English:    ///Chinese: ���������ֱ���
	//HVGetDeviceInfoResolution();
	m_BayerType = HVGetDeviceInfo_Bayer_Layout();///English:    ///Chinese: Bayer��ʽ
	
	m_strDeviceName = HVGetDeviceInfo_Name();

	m_pBmpInfo								= (BITMAPINFO *)m_chBmpBuf;
	///English:    ///Chinese: ��ʼ��BITMAPINFO �ṹ���˽ṹ�ڱ���bmp�ļ�����ʾ�ɼ�ͼ��ʱʹ��
	m_pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	///English:    ///Chinese: ͼ���ȣ�һ��Ϊ������ڿ��
	m_pBmpInfo->bmiHeader.biWidth			= m_nMaxWid;
	///English:    ///Chinese: ͼ���ȣ�һ��Ϊ������ڸ߶�
	m_pBmpInfo->bmiHeader.biHeight			= m_nMaxHei;
	
	
	m_pBmpInfo->bmiHeader.biPlanes			= 1;
	m_pBmpInfo->bmiHeader.biBitCount		= 24;
	m_pBmpInfo->bmiHeader.biCompression		= BI_RGB;//δѹ��
	m_pBmpInfo->bmiHeader.biSizeImage		= 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biClrUsed			= 0;
	m_pBmpInfo->bmiHeader.biClrImportant	= 0;
	
	
	////�����forѭ���Ӳ��Ӷ���
	for (int i = 0; i < 256; i++){
		m_pBmpInfo->bmiColors[i].rgbBlue		= (BYTE)i;
		m_pBmpInfo->bmiColors[i].rgbGreen		= (BYTE)i;
		m_pBmpInfo->bmiColors[i].rgbRed			= (BYTE)i;
		m_pBmpInfo->bmiColors[i].rgbReserved	= 0;	
	}
	/*
	///English:    ///Chinese: ����ԭʼͼ�񻺳�����һ�������洢�ɼ�ͼ��ԭʼ����
	///English:    ///Chinese: һ��ͼ�񻺳�����С��������ڴ�С����Ƶ��ʽȷ����
	*/
	m_pRawBuffer = new BYTE[m_nMaxWid * m_nMaxHei];
	//m_pRawBuffer = new BYTE[1280 * 800];
	assert(m_pRawBuffer);
	
	/*
	///English:    ///Chinese: ����Bayerת����ͼ�����ݻ���
	*/
	m_pImageBuffer = new BYTE[m_nMaxWid * m_nMaxHei * 3];
	//m_pImageBuffer = new BYTE[1280 * 800 * 3];
	assert(m_pImageBuffer);
	
	////���ø���Ȥ����
	m_AOIWid=1392;
	m_AOIHei=820;
	status = HVSetOutputWindow(m_hhv,0,110,m_AOIWid,m_AOIHei);
	
	//return 0;
	emit Init_Run();}
}



void Camera::SetExposure()
{
	HVSTATUS status = STATUS_OK;
	status = HVAECControl(m_hhv,AEC_SHUTTER_UNIT,1);  //0:us; 1:ms; 2:s  �����ع�ʱ�䵥λ
	
	manager* pmanager=manager::instance();
	
	Exposure=pmanager->GetMVS()->ui.lineEdit_Exposure->text().toInt();   //�����ع�ʱ��ֵ (ms)
	status = HVAECControl(m_hhv,AEC_SHUTTER_SPEED,Exposure);
}

void Camera::SetGain()
{
	HVSTATUS status = STATUS_OK;

	manager* pmanager=manager::instance();
	Gain =pmanager->GetMVS()->ui.lineEdit_Gain->text().toInt();
	
	status = HVAGCControl(m_hhv,AGC_GAIN,Gain);
}

void Camera::HVGetDeviceInfo_Resolution()  ///English:    ///Chinese: ���������ֱ���
{ 
	/*m_nMaxWid =1280;
	m_nMaxHei =800;*/
    int nResolution_Size; ///English:    ///Chinese: ���DESC_RESOLUTION����ռ�Ĵ�С  
  
    status = HVGetDeviceInfo(m_hhv, DESC_RESOLUTION, NULL,&nResolution_Size);               
    if (STATUS_OK != status)  
    {  
        //QMessageBox::information(this,tr("HVGetErrorString(status)"),tr("HVGetErrorString(status)"));  
    }  
    else  
    {  
       BYTE  *pbContext = NULL;  
        pbContext = new BYTE[nResolution_Size];  
		DWORD *pdContext = (DWORD *)pbContext;  
        status = HVGetDeviceInfo(m_hhv, DESC_RESOLUTION, pdContext,&nResolution_Size);  
        if (STATUS_OK != status)  
        {  
            if (NULL !=pbContext) {  
                delete []pbContext;  ///English:    ///Chinese: �ͷſռ�  
                pbContext=NULL;  
            }             
           // AfxMessageBox(HVGetErrorString(status));
			//QMessageBox::information(this,tr("HVGetErrorString(status)"),tr("HVGetErrorString(status)")); 
        }  
        else  
        {         
            int nWid,nHei;  
            int nCon = 0;  
              
            for(int i = 0;i < (nResolution_Size/8);i++) /*///English:    ///Chinese: (nResolution_Size/8)��ʾ������ֱ��ʵ�����*/  
            {                                           /*///English:    ///Chinese: CCD�����ֻ��һ�ֱַ��� */  
                nWid = *(pdContext + 2*nCon + 0 );  
                nHei = *(pdContext + 2*nCon + 1 );  
                  
                m_nMaxWid = (m_nMaxWid>nWid)? m_nMaxWid:nWid;  ///English:    ///Chinese:  ��÷ֱ�������   
                m_nMaxHei = (m_nMaxHei>nHei)? m_nMaxHei:nHei;  ///English:    ///Chinese:  ��÷ֱ�������  
                
                nCon+=1;  

            }  
			/*m_nMaxWid =m_nMaxWid-112;
	        m_nMaxHei =m_nMaxHei-240;*/
		/*m_nMaxWid=*(pbContext+2*RES_MODE1);
		m_nMaxHei=*(pbContext+2*RES_MODE1+1);*/
        }  
        if (NULL !=pbContext) {  
            delete []pbContext;  ///English:    ///Chinese: �ͷſռ�  
            pbContext=NULL;  
        }  
    }  
	            /*m_nMaxWid =m_nMaxWid -114;
				m_nMaxHei =m_nMaxHei -240;*/
}  

HV_BAYER_LAYOUT Camera::HVGetDeviceInfo_Bayer_Layout() ///English:    ///Chinese: ����������BAYER���ݸ�ʽ  
{  
    int nBayerLayout_Size;  ///English:    ///Chinese: ���DESC_DEVICE_BAYER_LAYOUT����ռ�Ĵ�С  
  
    HV_BAYER_LAYOUT Layout; ///English:    ///Chinese: Bayer��ʽ  
    status = HVGetDeviceInfo(m_hhv, DESC_DEVICE_BAYER_LAYOUT, NULL,&nBayerLayout_Size);             
    if(STATUS_OK != status)  
    {  
        //AfxMessageBox(HVGetErrorString(status));  
		//QMessageBox::information(this,tr("HVGetErrorString(status)"),tr("HVGetErrorString(status)")); 
    }  
    else  
    {  
        BYTE *pbBayerLayout = NULL;  
        pbBayerLayout = new BYTE[nBayerLayout_Size];  
        DWORD *pdBayerLayout = (DWORD *)pbBayerLayout;  
        status = HVGetDeviceInfo(m_hhv, DESC_DEVICE_BAYER_LAYOUT, pdBayerLayout,&nBayerLayout_Size);  
        if(STATUS_OK != status)  
        {  
            if (NULL != pbBayerLayout) {  
                delete []pbBayerLayout; ///English:    ///Chinese: �ͷſռ�  
                pbBayerLayout=NULL;  
            }  
              
           // AfxMessageBox(HVGetErrorString(status));  
			//QMessageBox::information(this,tr("HVGetErrorString(status)"),tr("HVGetErrorString(status)")); 
        }  
        else  
        {             
            Layout = (HV_BAYER_LAYOUT)*pdBayerLayout;///English:    ///Chinese: �õ������Bayer��ʽ��Ϣ  
        }  
          
        if (NULL != pbBayerLayout) {  
            delete []pbBayerLayout; ///English:    ///Chinese: �ͷſռ�  
            pbBayerLayout=NULL;  
        }  
    }  
    return Layout;  
}  

QString Camera::HVGetDeviceInfo_Name()///English:    ///Chinese: ������������
{
	int nDevName_Size; ///English:    ///Chinese: ���DESC_DEVICE_BAYER_LAYOUT����ռ�Ĵ�С

    QString str_DevName;  ///English:    ///Chinese: ������������
	status = HVGetDeviceInfo(m_hhv, DESC_DEVICE_NAME, NULL,&nDevName_Size);	          
    if(STATUS_OK != status)
	{
		//AfxMessageBox(HVGetErrorString(status));
		//QMessageBox::information(this,tr("HVGetErrorString(status)"),tr("HVGetErrorString(status)")); 
	}
	else
	{
		BYTE *pbName = NULL;
		pbName = new BYTE[nDevName_Size+1];///English:    ///Chinese: ���ڵײ��������㳤��ʱû�м��Ͻ���λ���ʴ˴���Ҫ+1
		DWORD *pdName = (DWORD *)pbName;
        status = HVGetDeviceInfo(m_hhv, DESC_DEVICE_NAME, pdName,&nDevName_Size);
		if(STATUS_OK != status)
		{
			if (NULL != pbName) {
				delete []pbName; ///English:    ///Chinese: �ͷſռ�
				pbName=NULL;
			}
			
			//AfxMessageBox(HVGetErrorString(status));
			//QMessageBox::information(this,tr("HVGetErrorString(status)"),tr("HVGetErrorString(status)")); 
		}
		else
		{	
			str_DevName = (char *)pdName;	///English:    ///Chinese: �õ���������������
		}
		
		if (NULL != pbName) {
			delete []pbName; ///English:    ///Chinese: �ͷſռ�
			pbName=NULL;
		}
	}
	return str_DevName;
}
LRESULT Camera::ChangeSnap(WPARAM wParam, LPARAM lParam)
{
	HVSTATUS status = STATUS_OK;
	 
	////��������Ӳ��Ӿ���
	m_pBmpInfo->bmiHeader.biWidth	= m_AOIWid;
	m_pBmpInfo->bmiHeader.biHeight	= m_AOIHei;	
	 
	ConvertBayer2Rgb( m_pImageBuffer, ///English:    ///Chinese: Bayerת���󻺳���(���)  
        m_pRawBuffer,   ///English:    ///Chinese: ԭʼ���ݻ�����(����)  
        m_AOIWid,      ///English:    ///Chinese: �ֱ��������  
        m_AOIHei,      ///English:    ///Chinese: �ֱ������߶�  
        ConvertType,    ///English:    ///Chinese: Raw2RGB�㷨  
        m_pLutR,        ///English:    ///Chinese: ��ɫ���ұ�(��)  
        m_pLutG,        ///English:    ///Chinese: ��ɫ���ұ�(��)  
        m_pLutB,        ///English:    ///Chinese: ��ɫ���ұ�(��)  
        true,          ///English:    ///Chinese: �Ƿ�ת  
        m_BayerType     ///English:    ///Chinese: Bayer��ʽ  
        );  
	 
	 CvSize n_size = cvSize(m_AOIWid,m_AOIHei);
     IplImage* frame = cvCreateImageHeader( n_size, IPL_DEPTH_8U, 3 );
     frame->imageData = (char *)m_pImageBuffer;
	 //cv::Mat img(m_nMaxWid,m_nMaxHei,CV_8UC3,m_pImageBuffer );  
    
	cv::Mat img;
	img=cv::Mat(frame);

	///////////////////////
	cv::flip(img,img,0);//����ˮƽ��ת��0��ֱ��ת������ˮƽ��ֱ��ת
	
	img.copyTo(result);//result=frame

	  //�ı���ʾͼ���СΪԭ��1/4
	/*IplImage* frame01=NULL;
	IplImage* frame02=NULL;
	cvCopyImage(frame,frame02);
	frame01=cvCreateImage(cvSize(frame->width/2,frame->height/2),frame->depth,frame->nChannels);
	cvResize(frame02,frame01,CV_INTER_LINEAR); */
	
	//img01=cvCreateImage(cvSize(frame->width/2,frame->height/2),frame->depth,frame->nChannels);
	cv::Mat img01;
	cv::Size dsize=cv::Size(img.cols*0.5,img.rows*0.5);
	resize(img,img01,dsize); 
	//img=cv::Mat(frame02);//IplImage convert to Mat
	 //QImage qimage(frame->imageData,frame->width,frame->height,frame->widthStep,QImage::Format_RGB888);
	
	//cv::flip(img,img,1);//����ˮƽ��ת��0��ֱ��ת������ˮƽ��ֱ��ת
	
	//img.copyTo(result);//create new copy����ʹimg�ı䣬result����ı�
	                   //result=img;//img�ı䣬result����֮�ı�
	
	

	
	//img01=cvCreateImage(cvSize(img->width/2,img->height/2),img->depth,img->nChannels);
	//cvResize(img,img01,CV_INTER_LINEAR); 
	
	cv::cvtColor(img01,img01,CV_BGR2RGB);//opencv convert to qt
    QImage image=QImage((const unsigned char*)(img01.data),img01.cols,img01.rows,img01.step,QImage::Format_RGB888);

  //// 
	manager* pmanager=manager::instance();
	
	pmanager->GetMVS()->ui.label_Display->setPixmap(QPixmap::fromImage(image));
	//emit setpixmap(QPixmap::fromImage(image));

	// ui.label->resize(ui.label->pixmap()->size());  ���ϻ��߳����д���

	 //cvShowImage("Display",img);
     //cvShowImage("Display",frame);
	 //cv::imshow("Diaplay",img);
     //cvWaitKey(66);

	 //QImage image;
	 //QByteArray bytearrary((char*)m_pImageBuffer);
	// image.loadFromData(bytearrary);



    // delete []m_pImageBuffer; //�ͷ�ͼ�񻺳�,���ϻ����д���
     cvReleaseImage(&frame);
	// cvReleaseImage(&frame01);
	// cvReleaseImage(&frame02);
	 return 1;
}

int CALLBACK Camera::SnapThreadCallback(HV_SNAP_INFO *pInfo)   ///English:    ///Chinese: �ص�����
{
	//	CMainFrame* pFrame = (CMainFrame*)(pInfo->pParam);
	//pFrame->OnSnapChange(0,0);
	//
	//return 1;

	Camera* camera = (Camera*)(pInfo->pParam);
	camera->ChangeSnap(NULL,NULL);
	
	return 1;
}

/*LRESULT Camera::ChangeSnap(WPARAM wParam, LPARAM lParam) 
{
	HVSTATUS status = STATUS_OK;
	CView *pView		= GetActiveView();		///English:    ///Chinese: ��ȡ��ǰVIEW��ͼ
	CDC *pDC			= pView->GetDC();		///English:    ///Chinese: �õ�VIEW��DC
    
	ConvertBayer2Rgb(m_pImageBuffer, ///English:    ///Chinese: Bayerת���󻺳���(���)  
        m_pRawBuffer,   ///English:    ///Chinese: ԭʼ���ݻ�����(����)  
        m_nMaxWid,      ///English:    ///Chinese: �ֱ��������  
        m_nMaxHei,      ///English:    ///Chinese: �ֱ������߶�  
        ConvertType,    ///English:    ///Chinese: Raw2RGB�㷨  
        m_pLutR,        ///English:    ///Chinese: ��ɫ���ұ�(��)  
        m_pLutG,        ///English:    ///Chinese: ��ɫ���ұ�(��)  
        m_pLutB,        ///English:    ///Chinese: ��ɫ���ұ�(��)  
        true,          ///English:    ///Chinese: �Ƿ�ת  
        m_BayerType     ///English:    ///Chinese: Bayer��ʽ  
        );  
   StretchDIBits(pDC->GetSafeHdc(),
		0,						
		0,
		m_nMaxWid,			///English:    ///Chinese: ��ʾ���ڿ��
		m_nMaxHei,			///English:    ///Chinese: ��ʾ���ڸ߶�
		0,
		0,
		m_nMaxWid,			///English:    ///Chinese: ͼ����
		m_nMaxHei,			///English:    ///Chinese: ͼ��߶�
		m_pImageBuffer,		///English:    ///Chinese: ͼ�񻺳���
		m_pBmpInfo,			///English:    ///Chinese: BMPͼ��������Ϣ
		DIB_RGB_COLORS,
		SRCCOPY
		);	
	 pView->ReleaseDC(pDC);
      return 1;
}*/
void Camera::OpenSnap() 
{
	// TODO: Add your command handler code here
	HVSTATUS status = STATUS_OK;

	status = HVOpenSnap(m_hhv, SnapThreadCallback, this);					
	//HV_VERIFY(status);
	if (HV_SUCCESS(status))
	{
		m_bOpen = TRUE;		///English:    ///Chinese: ��־�Ѿ���Snap����
	}
	emit RunCamera();
}	
    
void Camera::StartSnap()
{
	/*
	///English:    ///Chinese: ��������������ɼ�ͼ���ڴ�
	*/

	HVSTATUS status = STATUS_OK;
	BYTE *ppBuf[1];

	ppBuf[0] = m_pRawBuffer;
	status = HVStartSnap(m_hhv, ppBuf,1);
	//HV_VERIFY(status);

	if (HV_SUCCESS(status)) 
	{
		m_bStart = TRUE;
	}

}



/*void Camera::StartSnap() 
{
	// TODO: Add your command handler code here
	HVSTATUS status = STATUS_OK;
	
    
	///English:    ///Chinese: ��������������ɼ�ͼ���ڴ�
	
	BYTE *ppBuf[1];

	ppBuf[0] = m_pRawBuffer;
	status = HVStartSnap(m_hhv, ppBuf,1);
	//HV_VERIFY(status);

	if (HV_SUCCESS(status)) 
	{
		m_bStart = TRUE;
	}

}*/



/*void Camera::StopCamera() 
{
	// TODO: Add your command handler code here
	HVSTATUS status =STATUS_OK;
	
	///English:    ///Chinese: ֹͣ�ɼ�ͼ���ڴ�
	status = HVStopSnap(m_hhv);
	//HV_VERIFY(status);
	
	if (HV_SUCCESS(status)) 
	{
		m_bStart = FALSE;
	}
	//HVSTATUS status = STATUS_OK;
	
	
	///English:    ///Chinese: ��ֹ����������ɼ�ͼ���ڴ棬ͬʱ�ͷ����вɼ�������
	///English:    ///Chinese: �ٴ���������������ɼ����������³�ʼ��	
	
	status = HVCloseSnap(m_hhv);
	//HV_VERIFY(status);
	
	if (HV_SUCCESS(status))
	{
		m_bOpen		= FALSE;
		m_bStart	= FALSE;
	}

	status = EndHVDevice(m_hhv);///English:    ///Chinese: �ر�������������ͷ�����������ڲ���Դ
	//HV_VERIFY(status);
	
	if (NULL != m_pRawBuffer) {
		delete []m_pRawBuffer;   ///English:    ///Chinese: �ͷ�ԭʼ���ݻ�����
	}
	if (NULL != m_pImageBuffer) {
		delete []m_pImageBuffer; ///English:    ///Chinese: �ͷ�ת�������ݻ�����
	}

}*/



/*void Camera::CloseSnap()    
{
	// TODO: Add your command handler code here
	HVSTATUS status = STATUS_OK;
	
	
	///English:    ///Chinese: ��ֹ����������ɼ�ͼ���ڴ棬ͬʱ�ͷ����вɼ�������
	///English:    ///Chinese: �ٴ���������������ɼ����������³�ʼ��	
	
	status = HVCloseSnap(m_hhv);
	//HV_VERIFY(status);
	
	if (HV_SUCCESS(status))
	{
		m_bOpen		= FALSE;
		m_bStart	= FALSE;
	}

}*/




/*void Camera::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CString str;
	str.Format("HVStore  %s",m_strDeviceName);
	SetWindowText(str); ///English:    ///Chinese: ���������ƺ��豸������ʾ�ڱ�����
	// Do not call CFrameWnd::OnPaint() for painting messages
}*/

void Camera::StopCamera() 
{
	//CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	HVSTATUS status = STATUS_OK;
	
	///English:    ///Chinese: ���ֱ�ӹرճ���Ҫȷ����ͣ��
	if(m_bStart)
	{
		status = HVStopSnap(m_hhv);
		//HV_VERIFY(status);
		if (HV_SUCCESS(status)) 
		{
			m_bStart = FALSE;
		}
	}
	if(m_bOpen)
	{
		status = HVCloseSnap(m_hhv);
		//HV_VERIFY(status);
		
		if (HV_SUCCESS(status)) 
		{
			m_bOpen	= FALSE;
		}
	}
	
	status = EndHVDevice(m_hhv);///English:    ///Chinese: �ر�������������ͷ�����������ڲ���Դ
	//HV_VERIFY(status);
	
	if (NULL != m_pRawBuffer) {
		delete []m_pRawBuffer;   ///English:    ///Chinese: �ͷ�ԭʼ���ݻ�����
	}
	if (NULL != m_pImageBuffer) {
		delete []m_pImageBuffer; ///English:    ///Chinese: �ͷ�ת�������ݻ�����
	}

}

void Camera::SaveSnap() 
{
	// TODO: Add your command handler code here
	//typedef const char* LPCTSTR;
	//string lpfilename = "C:\\Users\\admin\\Desktop";
	//QString::toLocal8Bit().constData;
	//QString fromLocal8Bit(const char*str,int size=-1);
	//QString::fromLocal8Bit(const char*SavePathName,int size=-1);
	//const char* lpfilename=SavePathName.constData;
	//LPCTSTR lpfilename=SavePathName.constData;
    //LPCTSTR lpfilename=(LPCTSTR)SavePathName;

	//wstring SavePathName = L"C:/Users/Administrator/Desktop";
	//LPCWSTR lpfilename=SavePathName.c_str();
	//QString SavePath="C:/Users/Administrator/Desktop";
	QString fileName;
	//fileName=QFileDialog::getSaveFileName(this,tr("Save Image as"),"C:/Users/Administrator/Desktop",tr("Images ( *.bmp )"));
	fileName=QFileDialog::getSaveFileName(NULL,tr("Save Image as"),".",tr("BMP Files ( *.bmp )"));

	if (fileName.isNull())  
       return ;
	else
	{
     // if (m_pImageBuffer && m_pRawBuffer)
	  //{
		  
       //SaveBMPFile(lcp , m_pBmpInfo, m_pImageBuffer); 

      string SavePathName = fileName.toStdString();
	  cv::imwrite(SavePathName, result);
	  
	 // }
	}

}
	
	/*if (dlg.DoModal() == IDOK) //���������������壬��һ��dlg.DoModal()�����ǵ���CPortDlg�Ի��򣬵ڶ�����dlg.DoModal()==IDOK�������˶Ի����ϵ�OK��ť������˵��ͬʱ��������������ʱ��ִ��if������ĳ���
	{
		if (m_pImageBuffer && m_pRawBuffer)
		{
			int nFileExt = GetFileExt(dlg.GetPathName());
			switch(nFileExt) 
			{
			case 0:
				SaveBMPFile(dlg.GetPathName(), m_pBmpInfo, m_pImageBuffer);  
				break;
			case 1:
				///English:    ///Chinese:����JPEGͼ������
				HVSaveJPEG((char *)(LPCSTR)dlg.GetPathName(), m_pImageBuffer, (int)(m_pBmpInfo->bmiHeader.biWidth), 
					(int)(m_pBmpInfo->bmiHeader.biHeight), (int)(m_pBmpInfo->bmiHeader.biBitCount), TRUE, 100);
				break;
			default:
				break;
			}	
		}
	}
}*/

/*int Camera::GetFileExt(LPCSTR lpFileName)   //�õ��ļ���ʽ
{
	int nRVal           = 0;	
	int nLength			= 0;
	char cFileExt[5]	= {0};
	
	nLength = strlen(lpFileName);
	CopyMemory(cFileExt, (lpFileName + nLength - 4), 4);
	strupr(cFileExt);
	
	if (strcmp(cFileExt, ".BMP") == 0)
	{
		nRVal = 0;
	}
	else if(strcmp(cFileExt, ".JPG") == 0)
	{
		nRVal = 1;
	}
	return nRVal;
}*/

 /*
 ///English:    ///Chinese:����BMPͼ������  */

/*BOOL Camera::SaveBMPFile(LPCTSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pImageBuffer)
{
	
	BOOL bRVal				= TRUE;
	DWORD dwBytesRead		= 0;
	DWORD dwSize			= 0;
	BITMAPFILEHEADER bfh	= {0};
	int nTable				= 0;
	DWORD dwImageSize		= 0;
	
	if (pBmpInfo->bmiHeader.biBitCount > 8)
	{
		nTable = 0;
	}
	else
	{
		nTable = 256;
	}
	
	dwImageSize =  WIDTH_BYTES(pBmpInfo->bmiHeader.biWidth * pBmpInfo->bmiHeader.biBitCount) * 
		pBmpInfo->bmiHeader.biHeight;
	
	if (dwImageSize <= 0) 
	{
		bRVal = FALSE;
	}
	else
	{
		bfh.bfType		= (WORD)'M' << 8 | 'B';
		bfh.bfOffBits	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nTable * sizeof(RGBQUAD);
		bfh.bfSize		= bfh.bfOffBits + dwImageSize;			
		//QString::toStdWString();
		//qstring::utf16();
		//string fromUtf16(const char*unicode,int size=-1);
		HANDLE hFile = ::CreateFile(lpFileName,
			GENERIC_WRITE ,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (hFile == INVALID_HANDLE_VALUE) 
		{
			bRVal = FALSE;
		}
		else
		{
			dwSize = sizeof(BITMAPFILEHEADER);
			::WriteFile(hFile, &bfh, dwSize, &dwBytesRead, NULL );
			
			dwSize = sizeof(BITMAPINFOHEADER) + nTable * sizeof(RGBQUAD);
			::WriteFile(hFile, pBmpInfo, dwSize, &dwBytesRead, NULL );
			
			dwSize = dwImageSize;
			WriteFile(hFile, pImageBuffer, dwSize, &dwBytesRead, NULL );
			
			CloseHandle(hFile);
		}
	}	
	return bRVal;
}*/