
#ifndef BladeWINDOW_H
#define BladeWINDOW_H

namespace Camera {
	class CameraObject;
}

namespace Blade {

class BladeView;

class BladeWindow : public QWidget
{
    Q_OBJECT

public:
    BladeWindow(QWidget *parent = 0);
    ~BladeWindow();

	Camera::CameraObject* m_pCameraObject;
	BladeView* m_pCameraView;

	void WriteSettings();
};

} // namespace Blade

#endif // BladeWINDOW_H
