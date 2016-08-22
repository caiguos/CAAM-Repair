#pragma once

#include "TaskView\TaskView\TaskView.h"

#include "GeneratedFiles\Ui_TaskBoxImage.h"

namespace Blade {

class TaskBoxImage : public Gui::TaskView::TaskBox, Ui_TaskBoxImage
{
	Q_OBJECT
		
public:
    TaskBoxImage(QWidget *parent = 0);
    ~TaskBoxImage();

private Q_SLOTS:
	void ClickDetect();
	void ClickLive(bool);
	void SetThreshold(int value);
	void SetThreshold(QString value);
};

} 