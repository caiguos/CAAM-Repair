
#include "BladePlugin.h"
#include "BladeView.h"
#include "Camera\CameraObject.h"
#include "ImageObject.h"

using namespace Blade;

BladeView::BladeView(QWidget *parent)
	: CameraView(parent, false)
	, m_pImageDisplay(NULL)
	, m_nMouseMode(0)
	, m_bGrabing(false)
	, m_bMoveHorizontal(false)
	, m_bMoveVertical(false)
	, m_bShowCrosshair(true)
	, m_bShowScale(true)
	, m_bShowCircle(true)
	, m_dScalePixelToMM(50)
	, m_dCircleDiameter(1)
	, m_bDetectCompleted(false)
{
	cvSetMouseCallback(m_szWindowName, on_mouse, this);
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->addWidget(m_pWidDisplay);
	setLayout(mainLayout);

	readSettings();

	m_actCrossHair = new QAction(tr("Crosshair"), this);
	connect(m_actCrossHair, SIGNAL(triggered()), 
		this, SLOT(slotSetCrosshair()));
	BladePlugin::instance()->GetPluginMenu()->addAction(
		Core::ActionManager::registerAction(
		m_actCrossHair, "Blade.BladeAction", 
		Core::Context("Blade.MainView")));

	m_actMeasureDistance = new QAction(tr("Measure Distance"), this);
	connect(m_actMeasureDistance, SIGNAL(triggered()), 
		this, SLOT(slotSetMeasureDistance()));
	BladePlugin::instance()->GetPluginMenu()->addAction(
		Core::ActionManager::registerAction(
		m_actMeasureDistance, "Blade.MeasureAction", 
		Core::Context("Blade.MainView")));

	//Core::ICore::statusBar()->addWidget(new QLabel("Display: 0 fps\tProcessing: 0 fps"));
	//QString str = "Display: 0 fps\tProcessing: 0 fps";
	//Core::ICore::statusBar()->showMessage(str, 20000);

	new ImageObject();
}

BladeView::~BladeView()
{
	if(m_pImageDisplay)
	{
		cvReleaseImage(&m_pImageDisplay);
		m_pImageDisplay = NULL;
	}
}

void BladeView::readSettings()
{
	QSettings *settings = Core::ICore::settings();
	settings->beginGroup("BladeCrossHair");
	m_ptCrossHair.x = settings->value("PointX", 0).toInt();
	m_ptCrossHair.y = settings->value("PointY", 0).toInt();
	m_bShowCrosshair = settings->value("ShowCrosshair", true).toBool();
	m_bShowScale = settings->value("ShowScale", true).toBool();
	m_bShowCircle = settings->value("ShowCircle", true).toBool();
	m_dScalePixelToMM = settings->value("Scale", 50).toDouble();
	m_dCircleDiameter = settings->value("CicleDiameter", 1).toDouble();
	settings->endGroup();
}

void BladeView::writeSettings()
{
	QSettings *settings = Core::ICore::settings();
	settings->beginGroup("BladeCrossHair");
	settings->setValue("PointX", m_ptCrossHair.x);
	settings->setValue("PointY", m_ptCrossHair.y);
	settings->setValue("ShowCrosshair", m_bShowCrosshair);
	settings->setValue("ShowScale", m_bShowScale);
	settings->setValue("ShowCircle", m_bShowCircle);
	settings->setValue("Scale", m_dScalePixelToMM);
	settings->setValue("CicleDiameter", m_dCircleDiameter);
	settings->endGroup();
}

void BladeView::DisplayImage(const IplImage*image)
{
	if(theImageObj->m_bDetection)
	{
		if(m_bDetectCompleted)
			return;
		
		if(m_pImageDisplay==NULL
			|| m_pImageDisplay->nChannels!=3
			|| m_pImageDisplay->width!=image->width
			|| m_pImageDisplay->height!=image->height)
		{
			if(m_pImageDisplay!=NULL)
				cvReleaseImage(&m_pImageDisplay);
			m_pImageDisplay = cvCreateImage(cvSize(image->width,
				image->height), 8, 3);
		}

		theImageObj->DetectBoundary(image, m_pImageDisplay);
		cvShowImage_QT(m_pWidDisplay, m_pImageDisplay);
	}
	else
		CameraView::DisplayImage(image);

	m_bDetectCompleted = theImageObj->m_bDetection;
}

void BladeView::on_mouse( int event, int x, int y, int flags, void* param )
{
	BladeView* me = (BladeView*)param;
	if(me==NULL)
		return;

	me->OnHitTest(event, x, y);
}

void BladeView::OnHitTest(int event, int x, int y)
{
	switch( event )
	{
	case CV_EVENT_MOUSEMOVE:
		switch(m_nMouseMode)
		{
		case 1:
			if(m_bGrabing)
			{
				if(m_bMoveHorizontal)
					m_ptCrossHair.x = x;
				if(m_bMoveVertical)
					m_ptCrossHair.y = y;
			}
			else
			{
				m_bMoveHorizontal = abs(m_ptCrossHair.x - x) < 3;
				m_bMoveVertical = abs(m_ptCrossHair.y - y) < 3;
				if(m_bMoveHorizontal && m_bMoveVertical)
					setCursor(Qt::SizeAllCursor);
				else if(m_bMoveHorizontal)
					setCursor(Qt::SplitHCursor);
				else if(m_bMoveVertical)
					setCursor(Qt::SplitVCursor);
				else
					setCursor(Qt::ArrowCursor);
			}
			emit signalCrosshairPosChanged();
			break;
		case 2:
			if(!m_bFirstPoint)
			{
				m_bMoveHorizontal = true;
				m_ptMeasureEnd.x = x;
				m_ptMeasureEnd.y = y;
			}
			break;
		}
		break;
	case CV_EVENT_LBUTTONDOWN:
		m_bGrabing = true;
		if(m_nMouseMode==2)
		{
			if(m_bFirstPoint)
			{
				m_bMoveHorizontal = false;		// avoid draw line immediately
				m_ptMeasureStart.x = x;
				m_ptMeasureStart.y = y;
			}
			else
			{
				m_ptMeasureEnd.x = x;
				m_ptMeasureEnd.y = y;
			}
			m_bFirstPoint = !m_bFirstPoint;
		}
		else
		break;
	case CV_EVENT_LBUTTONUP:
		m_bGrabing = false;
		break;
	case CV_EVENT_LBUTTONDBLCLK:
		break;
	}
}

void BladeView::slotSetCrosshair()
{
	m_nMouseMode = m_nMouseMode==1 ? 0 : 1;
	m_actCrossHair->setChecked(m_nMouseMode==1);
}

void BladeView::slotSetMeasureDistance()
{
	m_nMouseMode = m_nMouseMode==2 ? 0 : 2;
	m_bFirstPoint = true;
	m_actCrossHair->setChecked(m_nMouseMode==2);
}

#include "GeneratedFiles/moc_BladeView.cpp"