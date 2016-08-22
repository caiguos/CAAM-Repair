#ifndef BladeMODE_H
#define BladeMODE_H

#include <coreplugin/imode.h>

namespace Blade {

class BladeWindow;

class BladeMode : public Core::IMode
{
public:
    BladeMode();
    ~BladeMode();

protected:
	BladeWindow* window;
};

} // namespace Blade

#endif // BladeMODE_H
