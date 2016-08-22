
#include "BladeWindow.h"
#include "BladePlugin.h"

#include <coreplugin/minisplitter.h>

#include "TaskView/TaskView/DeviceDockFrame.h"

#include "TaskDlgBlade.h"
#include "BladeView.h"
#include "ImageObject.h"
#include "Camera/CameraObject.h"

using namespace Blade;

BladeWindow::BladeWindow(QWidget *parent)
	: QWidget(parent)
{
	m_pCameraObject = new Camera::CameraObject(Camera::CameraObject::eDaheng);
	m_pCameraObject->ReadSettings(Core::ICore::settings(), "BladeCameraParameters");
	m_pCameraObject->SetupMenu(BladePlugin::instance()->GetPluginMenu()->menu());
	m_pCameraObject->SimulationStart(true, "image002.bmp"/*"TurbineBlade.png"*/);
	
	setWindowTitle(tr("Blade View"));

	QBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0); 

	Core::MiniSplitter *splitter1 = new Core::MiniSplitter(Qt::Orientation::Horizontal);

	m_pCameraView = new BladeView(this);
	m_pCameraView->SetCamera(m_pCameraObject);
	splitter1->addWidget(m_pCameraView);

	Gui::DeviceDockFrame* device = new Gui::DeviceDockFrame;
	device->setMinimumWidth(300);
	device->AddTaskDialog(new TaskDlgBlade(this), "Blade");
	splitter1->addWidget(device);
	splitter1->setStretchFactor(0, 4);
	//splitter1->setStretchFactor(1, 1);

	//splitter1->addWidget(CLP::Internal::CLPPlugin::instance()->SetupToolbar(this));

	layout->addWidget(splitter1);
}

BladeWindow::~BladeWindow()
{
	if(m_pCameraObject)
		delete m_pCameraObject;
}

void BladeWindow::WriteSettings()
{
	if(theImageObj)
		theImageObj->writeSettings();
	if(m_pCameraObject)
		m_pCameraObject->WriteSettings(Core::ICore::settings(), "BladeCameraParameters");
	if(m_pCameraView)
		m_pCameraView->writeSettings();
}

#include "GeneratedFiles/moc_BladeWindow.cpp"
#include "GeneratedFiles/qrc_PluginBlade.cpp"