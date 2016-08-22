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

const HV_BAYER_CONVERT_TYPE ConvertType = BAYER2RGB_NEIGHBOUR;   ///English:    ///Chinese: Raw2RGB算法

Camera::Camera()
{
  
}

Camera::~Camera()
{
}
	
void Camera::InitCamera()
{ if(m_bOpen==TRUE||m_bStart==TRUE){}
else{
    HVSTATUS status = BeginHVDevice(1, &m_hhv);   ///English:    ///Chinese: 打开摄像机
	//HV_VERIFY(status);
	/*HVSTATUS*/ //status =STATUS_OK;
	//status=HVSetResolution(m_hhv,RES_MODE3);
	//HV_RESOLUTION
	m_bOpen			= FALSE; ///English:    ///Chinese: 打开摄像机标志
	m_bStart        = FALSE; ///English:    ///Chinese: 开始采集标志

	m_pBmpInfo		= NULL;  ///English:    ///Chinese: BMP图像信息
	m_pRawBuffer	= NULL;  ///English:    ///Chinese: 原始图像数据缓冲初始化
	m_pImageBuffer	= NULL;  ///English:    ///Chinese: Bayer转换后图像数据缓冲区初始化

	m_nMaxWid =0;
	m_nMaxHei =0;
	
	for(int i=0;i<256;i++)   ///English:    ///Chinese: 颜色查找表初始化
	{
		m_pLutR[i] = i;
		m_pLutG[i] = i;
		m_pLutB[i] = i;
	}

	HVGetDeviceInfo_Resolution();  ///English:    ///Chinese: 获得摄像机分辨率
	//HVGetDeviceInfoResolution();
	m_BayerType = HVGetDeviceInfo_Bayer_Layout();///English:    ///Chinese: Bayer格式
	
	m_strDeviceName = HVGetDeviceInfo_Name();

	m_pBmpInfo								= (BITMAPINFO *)m_chBmpBuf;
	///English:    ///Chinese: 初始化BITMAPINFO 结构，此结构在保存bmp文件、显示采集图像时使用
	m_pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	///English:    ///Chinese: 图像宽度，一般为输出窗口宽度
	m_pBmpInfo->bmiHeader.biWidth			= m_nMaxWid;
	///English:    ///Chinese: 图像宽度，一般为输出窗口高度
	m_pBmpInfo->bmiHeader.biHeight			= m_nMaxHei;
	
	
	m_pBmpInfo->bmiHeader.biPlanes			= 1;
	m_pBmpInfo->bmiHeader.biBitCount		= 24;
	m_pBmpInfo->bmiHeader.biCompression		= BI_RGB;//未压缩
	m_pBmpInfo->bmiHeader.biSizeImage		= 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biClrUsed			= 0;
	m_pBmpInfo->bmiHeader.biClrImportant	= 0;
	
	
	////下面的for循环加不加都行
	for (int i = 0; i < 256; i++){
		m_pBmpInfo->bmiColors[i].rgbBlue		= (BYTE)i;
		m_pBmpInfo->bmiColors[i].rgbGreen		= (BYTE)i;
		m_pBmpInfo->bmiColors[i].rgbRed			= (BYTE)i;
		m_pBmpInfo->bmiColors[i].rgbReserved	= 0;	
	}
	/*
	///English:    ///Chinese: 分配原始图像缓冲区，一般用来存储采集图像原始数据
	///English:    ///Chinese: 一般图像缓冲区大小由输出窗口大小和视频格式确定。
	*/
	m_pRawBuffer = new BYTE[m_nMaxWid * m_nMaxHei];
	//m_pRawBuffer = new BYTE[1280 * 800];
	assert(m_pRawBuffer);
	
	/*
	///English:    ///Chinese: 分配Bayer转换后图像数据缓冲
	*/
	m_pImageBuffer = new BYTE[m_nMaxWid * m_nMaxHei * 3];
	//m_pImageBuffer = new BYTE[1280 * 800 * 3];
	assert(m_pImageBuffer);
	
	////设置感兴趣区域
	m_AOIWid=1392;
	m_AOIHei=820;
	status = HVSetOutputWindow(m_hhv,0,110,m_AOIWid,m_AOIHei);
	
	//return 0;
	emit Init_Run();}
}



void Camera::SetExposure()
{
	HVSTATUS status = STATUS_OK;
	status = HVAECControl(m_hhv,AEC_SHUTTER_UNIT,1);  //0:us; 1:ms; 2:s  设置曝光时间单位
	
	manager* pmanager=manager::instance();
	
	Exposure=pmanager->GetMVS()->ui.lineEdit_Exposure->text().toInt();   //设置曝光时间值 (ms)
	status = HVAECControl(m_hhv,AEC_SHUTTER_SPEED,Exposure);
}

void Camera::SetGain()
{
	HVSTATUS status = STATUS_OK;

	manager* pmanager=manager::instance();
	Gain =pmanager->GetMVS()->ui.lineEdit_Gain->text().toInt();
	
	status = HVAGCControl(m_hhv,AGC_GAIN,Gain);
}

void Camera::HVGetDeviceInfo_Resolution()  ///English:    ///Chinese: 获得摄像机分辨率
{ 
	/*m_nMaxWid =1280;
	m_nMaxHei =800;*/
    int nResolution_Size; ///English:    ///Chinese: 获得DESC_RESOLUTION所需空间的大小  
  
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
                delete []pbContext;  ///English:    ///Chinese: 释放空间  
                pbContext=NULL;  
            }             
           // AfxMessageBox(HVGetErrorString(status));
			//QMessageBox::information(this,tr("HVGetErrorString(status)"),tr("HVGetErrorString(status)")); 
        }  
        else  
        {         
            int nWid,nHei;  
            int nCon = 0;  
              
            for(int i = 0;i < (nResolution_Size/8);i++) /*///English:    ///Chinese: (nResolution_Size/8)表示摄像机分辨率的数量*/  
            {                                           /*///English:    ///Chinese: CCD摄像机只有一种分辨率 */  
                nWid = *(pdContext + 2*nCon + 0 );  
                nHei = *(pdContext + 2*nCon + 1 );  
                  
                m_nMaxWid = (m_nMaxWid>nWid)? m_nMaxWid:nWid;  ///English:    ///Chinese:  获得分辨率最大宽   
                m_nMaxHei = (m_nMaxHei>nHei)? m_nMaxHei:nHei;  ///English:    ///Chinese:  获得分辨率最大高  
                
                nCon+=1;  

            }  
			/*m_nMaxWid =m_nMaxWid-112;
	        m_nMaxHei =m_nMaxHei-240;*/
		/*m_nMaxWid=*(pbContext+2*RES_MODE1);
		m_nMaxHei=*(pbContext+2*RES_MODE1+1);*/
        }  
        if (NULL !=pbContext) {  
            delete []pbContext;  ///English:    ///Chinese: 释放空间  
            pbContext=NULL;  
        }  
    }  
	            /*m_nMaxWid =m_nMaxWid -114;
				m_nMaxHei =m_nMaxHei -240;*/
}  

HV_BAYER_LAYOUT Camera::HVGetDeviceInfo_Bayer_Layout() ///English:    ///Chinese: 获得摄像机的BAYER数据格式  
{  
    int nBayerLayout_Size;  ///English:    ///Chinese: 获得DESC_DEVICE_BAYER_LAYOUT所需空间的大小  
  
    HV_BAYER_LAYOUT Layout; ///English:    ///Chinese: Bayer格式  
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
                delete []pbBayerLayout; ///English:    ///Chinese: 释放空间  
                pbBayerLayout=NULL;  
            }  
              
           // AfxMessageBox(HVGetErrorString(status));  
			//QMessageBox::information(this,tr("HVGetErrorString(status)"),tr("HVGetErrorString(status)")); 
        }  
        else  
        {             
            Layout = (HV_BAYER_LAYOUT)*pdBayerLayout;///English:    ///Chinese: 得到具体的Bayer格式信息  
        }  
          
        if (NULL != pbBayerLayout) {  
            delete []pbBayerLayout; ///English:    ///Chinese: 释放空间  
            pbBayerLayout=NULL;  
        }  
    }  
    return Layout;  
}  

QString Camera::HVGetDeviceInfo_Name()///English:    ///Chinese: 获得摄像机名称
{
	int nDevName_Size; ///English:    ///Chinese: 获得DESC_DEVICE_BAYER_LAYOUT所需空间的大小

    QString str_DevName;  ///English:    ///Chinese: 存放摄像机名称
	status = HVGetDeviceInfo(m_hhv, DESC_DEVICE_NAME, NULL,&nDevName_Size);	          
    if(STATUS_OK != status)
	{
		//AfxMessageBox(HVGetErrorString(status));
		//QMessageBox::information(this,tr("HVGetErrorString(status)"),tr("HVGetErrorString(status)")); 
	}
	else
	{
		BYTE *pbName = NULL;
		pbName = new BYTE[nDevName_Size+1];///English:    ///Chinese: 由于底层驱动计算长度时没有加上结束位，故此处需要+1
		DWORD *pdName = (DWORD *)pbName;
        status = HVGetDeviceInfo(m_hhv, DESC_DEVICE_NAME, pdName,&nDevName_Size);
		if(STATUS_OK != status)
		{
			if (NULL != pbName) {
				delete []pbName; ///English:    ///Chinese: 释放空间
				pbName=NULL;
			}
			
			//AfxMessageBox(HVGetErrorString(status));
			//QMessageBox::information(this,tr("HVGetErrorString(status)"),tr("HVGetErrorString(status)")); 
		}
		else
		{	
			str_DevName = (char *)pdName;	///English:    ///Chinese: 得到具体的摄像机名称
		}
		
		if (NULL != pbName) {
			delete []pbName; ///English:    ///Chinese: 释放空间
			pbName=NULL;
		}
	}
	return str_DevName;
}
LRESULT Camera::ChangeSnap(WPARAM wParam, LPARAM lParam)
{
	HVSTATUS status = STATUS_OK;
	 
	////下面两句加不加均可
	m_pBmpInfo->bmiHeader.biWidth	= m_AOIWid;
	m_pBmpInfo->bmiHeader.biHeight	= m_AOIHei;	
	 
	ConvertBayer2Rgb( m_pImageBuffer, ///English:    ///Chinese: Bayer转换后缓冲区(输出)  
        m_pRawBuffer,   ///English:    ///Chinese: 原始数据缓冲区(输入)  
        m_AOIWid,      ///English:    ///Chinese: 分辨率最大宽度  
        m_AOIHei,      ///English:    ///Chinese: 分辨率最大高度  
        ConvertType,    ///English:    ///Chinese: Raw2RGB算法  
        m_pLutR,        ///English:    ///Chinese: 颜色查找表(红)  
        m_pLutG,        ///English:    ///Chinese: 颜色查找表(绿)  
        m_pLutB,        ///English:    ///Chinese: 颜色查找表(蓝)  
        true,          ///English:    ///Chinese: 是否翻转  
        m_BayerType     ///English:    ///Chinese: Bayer格式  
        );  
	 
	 CvSize n_size = cvSize(m_AOIWid,m_AOIHei);
     IplImage* frame = cvCreateImageHeader( n_size, IPL_DEPTH_8U, 3 );
     frame->imageData = (char *)m_pImageBuffer;
	 //cv::Mat img(m_nMaxWid,m_nMaxHei,CV_8UC3,m_pImageBuffer );  
    
	cv::Mat img;
	img=cv::Mat(frame);

	///////////////////////
	cv::flip(img,img,0);//整数水平反转，0垂直反转，负数水平垂直反转
	
	img.copyTo(result);//result=frame

	  //改变显示图像大小为原来1/4
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
	
	//cv::flip(img,img,1);//整数水平反转，0垂直反转，负数水平垂直反转
	
	//img.copyTo(result);//create new copy，即使img改变，result不会改变
	                   //result=img;//img改变，result会随之改变
	
	

	
	//img01=cvCreateImage(cvSize(img->width/2,img->height/2),img->depth,img->nChannels);
	//cvResize(img,img01,CV_INTER_LINEAR); 
	
	cv::cvtColor(img01,img01,CV_BGR2RGB);//opencv convert to qt
    QImage image=QImage((const unsigned char*)(img01.data),img01.cols,img01.rows,img01.step,QImage::Format_RGB888);

  //// 
	manager* pmanager=manager::instance();
	
	pmanager->GetMVS()->ui.label_Display->setPixmap(QPixmap::fromImage(image));
	//emit setpixmap(QPixmap::fromImage(image));

	// ui.label->resize(ui.label->pixmap()->size());  加上会线程运行错误

	 //cvShowImage("Display",img);
     //cvShowImage("Display",frame);
	 //cv::imshow("Diaplay",img);
     //cvWaitKey(66);

	 //QImage image;
	 //QByteArray bytearrary((char*)m_pImageBuffer);
	// image.loadFromData(bytearrary);



    // delete []m_pImageBuffer; //释放图像缓冲,加上会运行错误
     cvReleaseImage(&frame);
	// cvReleaseImage(&frame01);
	// cvReleaseImage(&frame02);
	 return 1;
}

int CALLBACK Camera::SnapThreadCallback(HV_SNAP_INFO *pInfo)   ///English:    ///Chinese: 回调函数
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
	CView *pView		= GetActiveView();		///English:    ///Chinese: 获取当前VIEW视图
	CDC *pDC			= pView->GetDC();		///English:    ///Chinese: 得到VIEW的DC
    
	ConvertBayer2Rgb(m_pImageBuffer, ///English:    ///Chinese: Bayer转换后缓冲区(输出)  
        m_pRawBuffer,   ///English:    ///Chinese: 原始数据缓冲区(输入)  
        m_nMaxWid,      ///English:    ///Chinese: 分辨率最大宽度  
        m_nMaxHei,      ///English:    ///Chinese: 分辨率最大高度  
        ConvertType,    ///English:    ///Chinese: Raw2RGB算法  
        m_pLutR,        ///English:    ///Chinese: 颜色查找表(红)  
        m_pLutG,        ///English:    ///Chinese: 颜色查找表(绿)  
        m_pLutB,        ///English:    ///Chinese: 颜色查找表(蓝)  
        true,          ///English:    ///Chinese: 是否翻转  
        m_BayerType     ///English:    ///Chinese: Bayer格式  
        );  
   StretchDIBits(pDC->GetSafeHdc(),
		0,						
		0,
		m_nMaxWid,			///English:    ///Chinese: 显示窗口宽度
		m_nMaxHei,			///English:    ///Chinese: 显示窗口高度
		0,
		0,
		m_nMaxWid,			///English:    ///Chinese: 图像宽度
		m_nMaxHei,			///English:    ///Chinese: 图像高度
		m_pImageBuffer,		///English:    ///Chinese: 图像缓冲区
		m_pBmpInfo,			///English:    ///Chinese: BMP图像描述信息
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
		m_bOpen = TRUE;		///English:    ///Chinese: 标志已经打开Snap环境
	}
	emit RunCamera();
}	
    
void Camera::StartSnap()
{
	/*
	///English:    ///Chinese: 启动数字摄像机采集图像到内存
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
	
    
	///English:    ///Chinese: 启动数字摄像机采集图像到内存
	
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
	
	///English:    ///Chinese: 停止采集图像到内存
	status = HVStopSnap(m_hhv);
	//HV_VERIFY(status);
	
	if (HV_SUCCESS(status)) 
	{
		m_bStart = FALSE;
	}
	//HVSTATUS status = STATUS_OK;
	
	
	///English:    ///Chinese: 终止数字摄像机采集图像到内存，同时释放所有采集环境，
	///English:    ///Chinese: 再次启动数字摄像机采集，必须重新初始化	
	
	status = HVCloseSnap(m_hhv);
	//HV_VERIFY(status);
	
	if (HV_SUCCESS(status))
	{
		m_bOpen		= FALSE;
		m_bStart	= FALSE;
	}

	status = EndHVDevice(m_hhv);///English:    ///Chinese: 关闭数字摄像机，释放数字摄像机内部资源
	//HV_VERIFY(status);
	
	if (NULL != m_pRawBuffer) {
		delete []m_pRawBuffer;   ///English:    ///Chinese: 释放原始数据缓冲区
	}
	if (NULL != m_pImageBuffer) {
		delete []m_pImageBuffer; ///English:    ///Chinese: 释放转换后数据缓冲区
	}

}*/



/*void Camera::CloseSnap()    
{
	// TODO: Add your command handler code here
	HVSTATUS status = STATUS_OK;
	
	
	///English:    ///Chinese: 终止数字摄像机采集图像到内存，同时释放所有采集环境，
	///English:    ///Chinese: 再次启动数字摄像机采集，必须重新初始化	
	
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
	SetWindowText(str); ///English:    ///Chinese: 将程序名称和设备名称显示在标题栏
	// Do not call CFrameWnd::OnPaint() for painting messages
}*/

void Camera::StopCamera() 
{
	//CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	HVSTATUS status = STATUS_OK;
	
	///English:    ///Chinese: 如果直接关闭程序，要确保先停采
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
	
	status = EndHVDevice(m_hhv);///English:    ///Chinese: 关闭数字摄像机，释放数字摄像机内部资源
	//HV_VERIFY(status);
	
	if (NULL != m_pRawBuffer) {
		delete []m_pRawBuffer;   ///English:    ///Chinese: 释放原始数据缓冲区
	}
	if (NULL != m_pImageBuffer) {
		delete []m_pImageBuffer; ///English:    ///Chinese: 释放转换后数据缓冲区
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
	
	/*if (dlg.DoModal() == IDOK) //这个语句有两层意义，第一是dlg.DoModal()作用是弹出CPortDlg对话框，第二层是dlg.DoModal()==IDOK是你点击了对话框上的OK按钮。就是说你同时做了上述两件事时就执行if语句后面的程序。
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
				///English:    ///Chinese:保存JPEG图像数据
				HVSaveJPEG((char *)(LPCSTR)dlg.GetPathName(), m_pImageBuffer, (int)(m_pBmpInfo->bmiHeader.biWidth), 
					(int)(m_pBmpInfo->bmiHeader.biHeight), (int)(m_pBmpInfo->bmiHeader.biBitCount), TRUE, 100);
				break;
			default:
				break;
			}	
		}
	}
}*/

/*int Camera::GetFileExt(LPCSTR lpFileName)   //得到文件格式
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
 ///English:    ///Chinese:保存BMP图像数据  */

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