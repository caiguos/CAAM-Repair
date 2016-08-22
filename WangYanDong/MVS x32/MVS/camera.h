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
	HHV	      m_hhv;     ///English:    ///Chinese: ������������
	HVSTATUS  status;    ///English:    ///Chinese: �豸״̬
	
private:
	//Ui::CameraClass ui;
	
	int Exposure;
	int Gain;

	BOOL		m_bOpen;           ///English:    ///Chinese: ��ʼ����־
    BOOL		m_bStart;		   ///English:    ///Chinese: ������־

	char        Error[256];        ///English:    ///Chinese: Ϊ��������ָ�����Ŀռ䣬������ʾ������Ϣ
	char        *m_pErrorType;     ///English:    ///Chinese: ��������ָ��
	int         m_nBayerLayout;    ///English:    ///Chinese: Bayer����

	BITMAPINFO	*m_pBmpInfo;       ///English:    ///Chinese: BITMAPINFO �ṹָ�룬��ʾͼ��ʱʹ��
	BYTE		*m_pImageBuffer;   ///English:    ///Chinese: Bayerת���󻺳���
	BYTE		*m_pRawBuffer;     ///English:    ///Chinese: �ɼ�ͼ��ԭʼ���ݻ�����
	char		m_chBmpBuf[2048];  ///English:    ///Chinese: BIMTAPINFO �洢��������m_pBmpInfo��ָ��˻�����

	int m_nMaxWid;  ///English:    ///Chinese: �ֱ��������
	int m_nMaxHei;  ///English:    ///Chinese: �ֱ������߶�

	HV_BAYER_LAYOUT  m_BayerType;  ///English:    ///Chinese: Bayer��ʽ

	QString  m_strDeviceName;      ///English:    ///Chinese: �豸����

	BYTE		m_pLutR[256];
	BYTE		m_pLutG[256];
	BYTE		m_pLutB[256];

	QString HVGetDeviceInfo_Name();
	HV_BAYER_LAYOUT HVGetDeviceInfo_Bayer_Layout();
	void HVGetDeviceInfo_Resolution();
	static int CALLBACK SnapThreadCallback(HV_SNAP_INFO *pInfo);  ///English:    ///Chinese: �ص���������
   
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





