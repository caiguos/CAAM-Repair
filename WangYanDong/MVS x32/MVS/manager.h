#pragma once
#ifndef MANAGER_H
#define MANAGER_H

#include "mvs.h"
#include "camera.h"
#include "ImageProcess.h"


using namespace mvs;
class manager
{
public:
	manager(void);
	~manager(void);

public:
	static manager*	m_instance;
	MVS* m_mvs;
	Camera* m_Camera;
	ImageProcess* m_Process;
	

public:
	static  manager* instance() {return m_instance;}
	Camera* GetCamera() {return m_Camera;}
	MVS*    GetMVS() {return m_mvs;}
	ImageProcess*     GetProcess()  {return m_Process;}
	

	void show();
	//void ConnectRobot();
};

#endif 
