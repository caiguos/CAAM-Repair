#pragma once

namespace Blade
{

class ImageObject : public QObject
{
	Q_OBJECT
public:
    ImageObject(void);
    virtual ~ImageObject();

	bool	m_bDetection;
	double	m_dBoundThres;

	void		readSettings();
	void		writeSettings();

	void DetectBoundary(const IplImage* grayImage, IplImage* displayImage);

public Q_SLOTS:
	void slotCapture();

Q_SIGNALS:
	void signalDataUpdated();
};

extern ImageObject* theImageObj;

} // namespace Blade