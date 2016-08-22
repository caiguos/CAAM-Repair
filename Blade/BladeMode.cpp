#include "BladeMode.h"
#include "BladeWindow.h"

using namespace Blade;

BladeMode::BladeMode()
{
	setWidget(window = new BladeWindow());
	setContext(Core::Context("Blade.MainView"));
	setDisplayName(tr("Blade"));
	setIcon(QIcon(QLatin1String(":/BladePlugin/mode_Blade.png")));
	setPriority(4);
	setId("Blade.BladeMode");
	setType("Blade.BladeMode");
	setContextHelpId(QString());
}

BladeMode::~BladeMode()
{
	window->WriteSettings();
}

//#include "GeneratedFiles/moc_BladeMode.cpp"