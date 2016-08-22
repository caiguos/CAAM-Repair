
#include "TaskBoxImage.h"
#include "ImageObject.h"

using namespace Blade;

TaskBoxImage::TaskBoxImage(QWidget *parent)
: TaskBox("Robot_CreateRobot",tr("Image"),true, parent)
{
	TASKBOX_INIT

	if(theImageObj!=NULL)
	{
		sliderThreshold->setValue(theImageObj->m_dBoundThres);
		edtThreshold->setText(QString::fromAscii("%1").arg(theImageObj->m_dBoundThres));
	}
	cbLive->setChecked(true);

	connect(pbDetect, SIGNAL(clicked()), this, SLOT(ClickDetect()));
	connect(cbLive, SIGNAL(clicked(bool)), this, SLOT(ClickLive(bool)));
	connect(sliderThreshold,SIGNAL(/*QSlider::*/valueChanged(int)),this,SLOT(SetThreshold(int)));
	connect(edtThreshold,SIGNAL(/*QLineEdit::*/textChanged(const QString&)),this,SLOT(SetThreshold(QString)));
}

TaskBoxImage::~TaskBoxImage()
{
}

void TaskBoxImage::ClickDetect()
{
	if(theImageObj!=NULL)
	{
		theImageObj->m_bDetection = true;
		cbLive->setChecked(false);
	}
}

void TaskBoxImage::ClickLive(bool checked)
{
	if(theImageObj!=NULL && checked)
		theImageObj->m_bDetection = false;
}

void TaskBoxImage::SetThreshold(int value)
{
	edtThreshold->setText(QString::fromAscii("%1").arg(value));
}

void TaskBoxImage::SetThreshold(QString value)
{
	bool bOK;
	int exp = value.toInt(&bOK);
	if(bOK)	
	{
		disconnect(sliderThreshold,SIGNAL(/*QSlider::*/valueChanged(int)),this,SLOT(SetThreshold(int)));
		sliderThreshold->setValue(exp);
		connect(sliderThreshold,SIGNAL(/*QSlider::*/valueChanged(int)),this,SLOT(SetThreshold(int)));

		if(theImageObj!=NULL)
		{
			theImageObj->m_dBoundThres = exp;
		}
	}
}

#include "GeneratedFiles/moc_TaskBoxImage.cpp"