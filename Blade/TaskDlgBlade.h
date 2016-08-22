#ifndef ADDITIVEGUI_TaskDlgBlade_H
#define ADDITIVEGUI_TaskDlgBlade_H

#include "TaskView\TaskView\TaskDialog.h"

namespace Blade {

class BladeWindow;

class TaskDlgBlade : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
	TaskDlgBlade(BladeWindow*);
    ~TaskDlgBlade();
};

} 

#endif // ADDITIVEGUI_TaskDlgBlade_H
