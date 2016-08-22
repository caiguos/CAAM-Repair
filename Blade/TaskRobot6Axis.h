#ifndef GUI_TASKVIEW_TaskRobot6Axis_H
#define GUI_TASKVIEW_TaskRobot6Axis_H

#include "TaskView\TaskView\TaskView.h"

#include "GeneratedFiles\ui_TaskRobot6Axis.h"

namespace Robot { 

class TaskRobot6Axis : public Gui::TaskView::TaskBox, Ui_TaskRobot6Axis
{
    Q_OBJECT

public:
    TaskRobot6Axis(QWidget *parent = 0);
    ~TaskRobot6Axis();

private Q_SLOTS:
	void slotUpdateAxis();

protected:
	QTimer m_tmAxis;

    void setAxisFrame(float A1,float A2,float A3,float A4,float A5,float A6);
    void setAxisJoint(float A1,float A2,float A3,float A4,float A5,float A6);
};

} //namespace Robot

#endif // GUI_TASKVIEW_TaskRobot6Axis_H
