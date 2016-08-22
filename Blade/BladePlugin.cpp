#include "BladePlugin.h"
#include "BladeMode.h"

using namespace Blade;

BladePlugin* BladePlugin::m_instance;

BladePlugin::BladePlugin()
{
	m_instance = this;
}

BladePlugin::~BladePlugin()
{
	m_instance = NULL;
}

bool BladePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

	SetupMenu();

    Core::IMode *BladeMode = new Blade::BladeMode;
    addAutoReleasedObject(BladeMode);
	
    return true;
}

void BladePlugin::extensionsInitialized()
{
}

void BladePlugin::SetupMenu()
{
    m_pPluginMenu =	Core::ActionManager::createMenu("Blade.BladeMenu");
    QMenu *menu = m_pPluginMenu->menu();
    menu->setTitle(tr("&Blade"));
    menu->setEnabled(true);

    Core::ActionContainer *toolsMenu = Core::ActionManager::actionContainer(Core::Constants::M_TOOLS);
    Core::ActionContainer *mainMenu = Core::ActionManager::actionContainer(Core::Constants::MENU_BAR);
    mainMenu->addMenu(toolsMenu, m_pPluginMenu);
}

QToolBar* BladePlugin::SetupToolbar(QWidget* parent)
{
    QToolBar* m_toolbar = new QToolBar(parent);
	//m_toolbar->setIconSize(QSize(32,32));
    m_toolbar->setContentsMargins(0, 0, 0, 0);

	//m_pCameraObject->m_actCameraStart->setIcon(QIcon(":/CLPPlugin/resource/camera_start.png"));
 //   m_toolbar->addAction(m_pCameraObject->m_actCameraStart);
	//m_pCameraObject->m_actCameraStop->setIcon(QIcon(":/CLPPlugin/resource/camera_stop.png"));
 //   m_toolbar->addAction(m_pCameraObject->m_actCameraStop);
	//m_toolbar->addSeparator();

	//m_pCameraObject->m_actStorePicture->setIcon(QIcon(":/CLPPlugin/resource/storage_picture.png"));
 //   m_toolbar->addAction(m_pCameraObject->m_actStorePicture);
	//m_pCameraObject->m_actStoreVideo->setIcon(QIcon(":/CLPPlugin/resource/storage_video.png"));
 //   m_toolbar->addAction(m_pCameraObject->m_actStoreVideo);
	//m_pCameraObject->m_actStoreLogfile->setIcon(QIcon(":/CLPPlugin/resource/storage_log.png"));
 //   m_toolbar->addAction(m_pCameraObject->m_actStoreLogfile);
	//m_pCameraObject->m_actStoreSetting->setIcon(QIcon(":/CLPPlugin/resource/storage_setting.png"));
 //   m_toolbar->addAction(m_pCameraObject->m_actStoreSetting);
	//m_toolbar->addSeparator();

	//m_pCameraObject->m_actDisplayZoomin->setIcon(QIcon(":/CLPPlugin/resource/display_zoomin.png"));
 //   m_toolbar->addAction(m_pCameraObject->m_actDisplayZoomin);
	//m_pCameraObject->m_actDisplayZoomout->setIcon(QIcon(":/CLPPlugin/resource/display_zoomout.png"));
 //   m_toolbar->addAction(m_pCameraObject->m_actDisplayZoomout);
	//m_pCameraObject->m_actDisplayNormal->setIcon(QIcon(":/CLPPlugin/resource/display_normal.png"));
 //   m_toolbar->addAction(m_pCameraObject->m_actDisplayNormal);
	//m_pCameraObject->m_actDisplayFitting->setIcon(QIcon(":/CLPPlugin/resource/display_fitting.png"));
 //   m_toolbar->addAction(m_pCameraObject->m_actDisplayFitting);

	////m_toolbar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	//m_toolbar->setFixedHeight(32);

	return m_toolbar;
}

Q_EXPORT_PLUGIN(Blade::BladePlugin)

#include "GeneratedFiles/moc_BladePlugin.cpp"