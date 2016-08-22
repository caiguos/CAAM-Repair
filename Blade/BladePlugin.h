#ifndef BladePLUGIN_H
#define BladePLUGIN_H

#include <extensionsystem/iplugin.h>

namespace Core
{
class ActionContainer;
}

namespace Blade {

class BladeMode;

class BladePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Blade.json")

public:
    BladePlugin();
    ~BladePlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage);
    void extensionsInitialized();

protected:
	static BladePlugin*	m_instance;
	Core::ActionContainer*	m_pPluginMenu;

	void SetupMenu();
	QToolBar* SetupToolbar(QWidget* parent);

public:
	static BladePlugin*	instance() {return m_instance;}
	Core::ActionContainer*	GetPluginMenu() {return m_pPluginMenu;}
};

} // namespace Blade

#endif // BladePLUGIN_H
