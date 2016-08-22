#ifndef MVS_H
#define MVS_H

#include <QtWidgets/QMainWindow>
#include "ui_mvs.h"
#include"camera.h"
#include"ImageProcess.h"


using namespace mvs;

class MVS : public QMainWindow
{
	Q_OBJECT

public:
	MVS(QWidget *parent = 0);
	~MVS();

	void ConnectCamera(Camera* camera);
	void ConnectImageProcess(ImageProcess* process);
	//public slots:
	//void SetGain();
	//void DisplayPixmap(const QPixmap &);


//private: 使用private:的情况下 manager程序无法访问private
 public:
	Ui::MVSClass ui;
	//Camera _camera;
};

#endif // MVS_H
