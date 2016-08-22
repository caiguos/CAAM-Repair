#ifndef ADDITIVEGUI_TaskBoxCameraDisplay_H
#define ADDITIVEGUI_TaskBoxCameraDisplay_H

#include "TaskView\TaskView\TaskView.h"
#include "GeneratedFiles\Ui_TaskBoxCameraDisplay.h"

namespace Blade {

class BladeView;

class TaskBoxCameraDisplay
	: public Gui::TaskView::TaskBox
	, Ui_TaskBoxCameraDisplay
{
	Q_OBJECT
		
public:
    TaskBoxCameraDisplay(QWidget *parent = 0);
    ~TaskBoxCameraDisplay();

	void SetView(BladeView*);

protected:
	BladeView* m_pView;

private Q_SLOTS:
	void slotCrosshairPosChanged();
};

} 

#endif // ADDITIVEGUI_TaskBoxCameraDisplay_H
