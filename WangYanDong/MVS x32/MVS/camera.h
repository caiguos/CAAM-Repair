#pragma once

#include <QtWidgets/QMainWindow>
#include<string>
using namespace std;
#include<HVDAILT.H>
#include<HVDef.h>
#include<Raw2Rgb.h>
#include<Windows.h>
#include <QObject>
//#include "OpenCV/cv.h"
//#include "OpenCV/highGUI.h"
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>


#define WIDTH_BYTES(bits)	(((bits) + 31) / 32 * 4)

namespace mvs
{
class Camera : public QObject
{
	Q_OBJECT

public:
	Camera();
	~Camera(void);

protected:
	HHV	      m_hhv;     ///English:    ///Chinese: 数字摄像机句柄
	HVSTATUS  status;    ///English:    ///Chinese: 设备状态
	
private:
	//Ui::CameraClass ui;
	
	int Exposure;
	int Gain;

	BOOL		m_bOpen;           ///English:    ///Chinese: 初始化标志
    BOOL		m_bStart;		   ///English:    ///Chinese: 启动标志

	char        Error[256];        ///English:    ///Chinese: 为错误类型指针分配的空间，用以显示错误信息
	char        *m_pErrorType;     ///English:    ///Chinese: 错误类型指针
	int         m_nBayerLayout;    ///English:    ///Chinese: Bayer类型

	BITMAPINFO	*m_pBmpInfo;       ///English:    ///Chinese: BITMAPINFO 结构指针，显示图像时使用
	BYTE		*m_pImageBuffer;   ///English:    ///Chinese: Bayer转换后缓冲区
	BYTE		*m_pRawBuffer;     ///English:    ///Chinese: 采集图像原始数据缓冲区
	char		m_chBmpBuf[2048];  ///English:    ///Chinese: BIMTAPINFO 存储缓冲区，m_pBmpInfo即指向此缓冲区

	int m_nMaxWid;  ///English:    ///Chinese: 分辨率最大宽度
	int m_nMaxHei;  ///English:    ///Chinese: 分辨率最大高度

	HV_BAYER_LAYOUT  m_BayerType;  ///English:    ///Chinese: Bayer格式

	QString  m_strDeviceName;      ///English:    ///Chinese: 设备名称

	BYTE		m_pLutR[256];
	BYTE		m_pLutG[256];
	BYTE		m_pLutB[256];

	QString HVGetDeviceInfo_Name();
	HV_BAYER_LAYOUT HVGetDeviceInfo_Bayer_Layout();
	void HVGetDeviceInfo_Resolution();
	static int CALLBACK SnapThreadCallback(HV_SNAP_INFO *pInfo);  ///English:    ///Chinese: 回调函数声明
   
	cv::Mat result;

	int m_AOIWid;
	int m_AOIHei;

public:
	//CStatusBar  m_wndStatusBar;
	//BOOL SaveBMPFile(LPCSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pImageBuffer);
	//int GetFileExt(LPCSTR);
	// int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// void OnSnapOpen();
	 //void OnSnapStart();
	// void OnSnapStop();
	// void OnSnapClose();
	 //void OnUpdateSnapOpen(CCmdUI* pCmdUI);
	 //void OnUpdateSnapStart(CCmdUI* pCmdUI);
     //void OnUpdateSnapStop(CCmdUI* pCmdUI);
	 //void OnUpdateSnapClose(CCmdUI* pCmdUI);
	// LRESULT OnSnapChange(WPARAM wParam, LPARAM lParam);
	// void OnDestroy();
	// void OnPaint();
	// void OnSnapSave();
	//LRESULT ChangeSnap(WPARAM wParam, LPARAM lParam);
	//void SetGain();
	//void SetExposure();
	//void InitCamera();
	LRESULT ChangeSnap(WPARAM wParam, LPARAM lParam); 
	//void OpenSnap();
	//void StartSnap();
	//void StopSnap();
	//void CloseSnap();
	//void SaveSnap();
	//void DestroySnap();
	//BOOL SaveBMPFile(LPCTSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pImageBuffer);

	public Q_SLOTS:
	
	void InitCamera();
	//void RunCamera();
	void StopCamera();
	void SetExposure();
	void SetGain();
	void SaveSnap();
	void StartSnap();
	void OpenSnap();

  Q_SIGNALS:
	//void StartSnap();
	void RunCamera();
	void Init_Run();
    //void OpenSnap();
	//void StartSnap();
	//void StopSnap();
	//void CloseSnap();
	//void DestroySnap();
	

};
}





