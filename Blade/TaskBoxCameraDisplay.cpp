
#include "TaskBoxCameraDisplay.h"
#include "BladeView.h"

using namespace Blade;

TaskBoxCameraDisplay::TaskBoxCameraDisplay(QWidget *parent)
: TaskBox("Robot_CreateRobot",tr("Display"),true, parent)
, m_pView(NULL)
{
	TASKBOX_INIT
}

TaskBoxCameraDisplay::~TaskBoxCameraDisplay()
{
}

void TaskBoxCameraDisplay::SetView(BladeView* view)
{
	m_pView = view;
	if(view)
	{
		slotCrosshairPosChanged();
		cbShowCross->setChecked(view->IsShowCrosshair());
		connect(cbShowCross,SIGNAL(clicked(bool)),view,SLOT(slotShowCrosshair(bool)));
		connect(sbCrossX,SIGNAL(valueChanged(int)),view,SLOT(slotSetCrosshairX(int)));
		connect(sbCrossY,SIGNAL(valueChanged(int)),view,SLOT(slotSetCrosshairY(int)));
		connect(view,SIGNAL(signalCrosshairPosChanged()),this,SLOT(slotCrosshairPosChanged()));
		connect(pbSelect,SIGNAL(clicked()),view,SLOT(slotSetCrosshair()));

		cbShowScale->setChecked(view->IsShowScale());
		connect(cbShowScale,SIGNAL(clicked(bool)),view,SLOT(slotShowScale(bool)));
		sbScale->setValue(view->GetScale());
		connect(sbScale,SIGNAL(valueChanged(double)),view,SLOT(slotSetScale(double)));

		cbShowCircle->setChecked(view->IsShowCircle());
		connect(cbShowCircle,SIGNAL(clicked(bool)),view,SLOT(slotShowCircle(bool)));
		sbDiameter->setValue(view->GetCircleDiameter());
		connect(sbDiameter,SIGNAL(valueChanged(double)),view,SLOT(slotSetCircleDiameter(double)));
	}
}

void TaskBoxCameraDisplay::slotCrosshairPosChanged()
{
	if(m_pView)
	{
		sbCrossX->setValue(m_pView->GetCrosshairX());
		sbCrossY->setValue(m_pView->GetCrosshairY());
	}
}

#include "GeneratedFiles/moc_TaskBoxCameraDisplay.cpp"