
#ifndef BladeView_H
#define BladeView_H

#include "Camera\CameraView.h"

namespace Blade {

class BladeView : public Camera::CameraView
{
    Q_OBJECT

public:
    BladeView(QWidget *parent = 0);
    ~BladeView();

protected:
	IplImage*	m_pImageDisplay;

	bool		m_bDetectCompleted;

	int			m_nMouseMode;
	bool		m_bGrabing;
	bool		m_bMoveHorizontal;
	bool		m_bMoveVertical;
	bool		m_bFirstPoint;

	CvPoint		m_ptCrossHair;
	CvPoint		m_ptMeasureStart;
	CvPoint		m_ptMeasureEnd;
	bool		m_bShowCrosshair;
	bool		m_bShowScale;
	bool		m_bShowCircle;
	
	double		m_dScalePixelToMM;
	double		m_dCircleDiameter;

	QAction*	m_actCrossHair;
	QAction*	m_actMeasureDistance;

	void		readSettings();

	static void on_mouse( int event, int x, int y, int flags, void* param );
	void		OnHitTest(int event, int x, int y);

public:
	bool IsShowCrosshair() {return m_bShowCrosshair;}
	bool IsShowScale() {return m_bShowScale;}
	bool IsShowCircle() {return m_bShowCircle;}

	int GetCrosshairX() {return m_ptCrossHair.x;}
	int GetCrosshairY() {return m_ptCrossHair.y;}

	double GetScale() {return m_dScalePixelToMM;}
	double GetCircleDiameter() {return m_dCircleDiameter;}

	void writeSettings();

	virtual void DisplayImage(const IplImage*);

Q_SIGNALS:
	void signalCrosshairPosChanged();

protected Q_SLOTS:
	void slotSetCrosshair();
	void slotSetMeasureDistance();

	void slotShowCrosshair(bool bShow) {m_bShowCrosshair = bShow;}
	void slotShowScale(bool bShow) {m_bShowScale = bShow;}
	void slotShowCircle(bool bShow) {m_bShowCircle = bShow;}

	void slotSetCrosshairX(int x) {m_ptCrossHair.x = x;}
	void slotSetCrosshairY(int y) {m_ptCrossHair.y = y;}

	void slotSetScale(double v) {m_dScalePixelToMM = v;}
	void slotSetCircleDiameter(double v) {m_dCircleDiameter = v;}
};

} // namespace Blade

#endif // BladeView_H
