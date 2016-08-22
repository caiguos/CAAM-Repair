
#include "TaskDlgBlade.h"
#include "TaskView\TaskView\TaskView.h"
#include "Camera\TaskBoxCameraControl.h"
#include "Camera\TaskBoxCameraPreview.h"
#include "TaskBoxImage.h"
#include "TaskBoxCameraDisplay.h"
#include "TaskRobot6Axis.h"
#include "BladeWindow.h"

using namespace Blade;

TaskDlgBlade::TaskDlgBlade(BladeWindow* window)
: TaskDialog()
{
	Camera::TaskBoxCameraControl* boxControl = new Camera::TaskBoxCameraControl();
	Content.push_back(boxControl);
	boxControl->SetCamera(window->m_pCameraObject);
	boxControl->hideGroupBox();
	
	TaskBoxImage* box = new TaskBoxImage();
	Content.push_back(box);	
	//box->hideGroupBox();

	TaskBoxCameraDisplay* boxDisplay = new TaskBoxCameraDisplay();
	Content.push_back(boxDisplay);
	boxDisplay->hideGroupBox();
	boxDisplay->SetView(window->m_pCameraView);

	Robot::TaskRobot6Axis* boxRobot = new Robot::TaskRobot6Axis();
	Content.push_back(boxRobot);
	boxRobot->hideGroupBox();
}

TaskDlgBlade::~TaskDlgBlade()
{
}

#include "GeneratedFiles/moc_TaskDlgBlade.cpp"