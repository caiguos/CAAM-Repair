#include "mvs.h"
#include <QtCore>
#include "manager.h"
#include "robotlib.h"

MVS::MVS(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//connect(&_camera,SIGNAL(setpixmap(const QPixmap &)),this,SLOT(DisplayPixmap(const QPixmap &)));
}

MVS::~MVS()
{

}

void MVS::ConnectCamera(Camera* camera)
{
	if(camera!=NULL)
	{
		/*bool ok=*///connect(ui.pushButton_InitCamera,SIGNAL(clicked()),camera,SLOT(InitCamera()));
		connect(ui.pushButton_SetExposure,SIGNAL(clicked()),camera,SLOT(SetExposure()));
		connect(ui.pushButton_SetGain,SIGNAL(clicked()),camera,SLOT(SetGain()));
		connect(ui.pushButton_RunCamera,SIGNAL(clicked()),camera,SLOT(InitCamera()));
		connect(camera,SIGNAL(Init_Run()),camera,SLOT(OpenSnap()));
		connect(camera,SIGNAL(RunCamera()),camera,SLOT(StartSnap()));
		connect(ui.pushButton_StopCamera,SIGNAL(clicked()),camera,SLOT(StopCamera()));
		connect(ui.pushButton_SaveImage,SIGNAL(clicked()),camera,SLOT(SaveSnap()));
	}
}

void MVS::ConnectImageProcess(ImageProcess* process)
{
	if(process!=NULL)
	{
		connect(ui.pushButton_OpenImage,SIGNAL(clicked()),process,SLOT(OpenImage()));
		//connect(ui.pushButton_SetThreshold,SIGNAL(clicked()),process,SLOT(SetThresholdandProcess()));
		connect(ui.pushButton_SetThreshold,SIGNAL(clicked()),process,SLOT(SetThreshold()));
		connect(ui.pushButton_ImageProcess,SIGNAL(clicked()),process,SLOT(imageProcess()));
		connect(ui.pushButton_ManualAdjustment,SIGNAL(clicked()),process,SLOT(ManualAdjustment()));
		connect(ui.pushButton_GenerateVal3,SIGNAL(clicked()),process,SLOT(GenerateVal3File()));
		connect(ui.pushButton_SetOffset,SIGNAL(clicked()),process,SLOT(SetOffset()));
		connect(ui.pushButton_Remove,SIGNAL(clicked()),process,SLOT(RemovePoints()));
		connect(ui.pushButton_SetSequence,SIGNAL(clicked()),process,SLOT(SetPointSequence()));
	}
}

//void MVS::SetGain()
//{
//    HVSTATUS status=STATUS_OK;
//    int Gain=ui.lineEdit_Gain->text().toInt();
//	//_camera.SetGain(Gain);
//}
//void MVS::DisplayPixmap(const QPixmap & pixmap)
//{
//	ui.label_Display->setPixmap(pixmap);
//
//}