#include "TaskRobot6Axis.h"

#include "IOCenter\IOCenter.h"

using namespace Robot;

TaskRobot6Axis::TaskRobot6Axis(QWidget *parent)
    : TaskBox("Robot_CreateRobot",tr("Robot"),true, parent)
{
    TASKBOX_INIT

	m_tmAxis.start(200);
	connect(&m_tmAxis, SIGNAL(timeout()), this, SLOT(slotUpdateAxis()));
}

TaskRobot6Axis::~TaskRobot6Axis()
{
	m_tmAxis.stop();
}

void TaskRobot6Axis::slotUpdateAxis()
{
	std::vector<double> axis;

	if(IOCenter::GetRobotAxis(axis, true))
		setAxisFrame(axis[0], axis[1], axis[2],
			axis[3], axis[4], axis[5]);

	if(IOCenter::GetRobotAxis(axis, false))
		setAxisJoint(axis[0], axis[1], axis[2],
			axis[3], axis[4], axis[5]);
}

void TaskRobot6Axis::setAxisFrame(float A1,float A2,float A3,float A4,float A5,float A6/*,const Base::Placement &Tcp*/)
{
    horizontalSlider_AxisX->setSliderPosition((int)A1);
    lineEdit_AxisX->setText(QString::fromLatin1("%1").arg(A1,0,'f',3));

    horizontalSlider_AxisY->setSliderPosition((int)A2);
    lineEdit_AxisY->setText(QString::fromLatin1("%1").arg(A2,0,'f',3));

    horizontalSlider_AxisZ->setSliderPosition((int)A3);
    lineEdit_AxisZ->setText(QString::fromLatin1("%1").arg(A3,0,'f',3));

    horizontalSlider_AxisRx->setSliderPosition((int)A4);
    lineEdit_AxisRx->setText(QString::fromLatin1("%1").arg(A4,0,'f',3));

    horizontalSlider_AxisRy->setSliderPosition((int)A5);
    lineEdit_AxisRy->setText(QString::fromLatin1("%1").arg(A5,0,'f',3));

    horizontalSlider_AxisRz->setSliderPosition((int)A6);
    lineEdit_AxisRz->setText(QString::fromLatin1("%1").arg(A6,0,'f',3));
}

void TaskRobot6Axis::setAxisJoint(float A1,float A2,float A3,float A4,float A5,float A6/*,const Base::Placement &Tcp*/)
{
    horizontalSlider_Axis1->setSliderPosition((int)A1);
    lineEdit_Axis1->setText(QString::fromLatin1("%1").arg(A1,0,'f',3));

    horizontalSlider_Axis2->setSliderPosition((int)A2);
    lineEdit_Axis2->setText(QString::fromLatin1("%1").arg(A2,0,'f',3));

    horizontalSlider_Axis3->setSliderPosition((int)A3);
    lineEdit_Axis3->setText(QString::fromLatin1("%1").arg(A3,0,'f',3));

    horizontalSlider_Axis4->setSliderPosition((int)A4);
    lineEdit_Axis4->setText(QString::fromLatin1("%1").arg(A4,0,'f',3));

    horizontalSlider_Axis5->setSliderPosition((int)A5);
    lineEdit_Axis5->setText(QString::fromLatin1("%1").arg(A5,0,'f',3));

    horizontalSlider_Axis6->setSliderPosition((int)A6);
    lineEdit_Axis6->setText(QString::fromLatin1("%1").arg(A6,0,'f',3));
}

#include "GeneratedFiles/moc_TaskRobot6Axis.cpp"
