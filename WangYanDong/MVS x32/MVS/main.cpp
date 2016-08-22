#include "mvs.h"
#include <QtWidgets/QApplication>
#include "manager.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//MVS w;
	//w.show();
	manager Mymanager;
	Mymanager.show();
	return a.exec();
}
