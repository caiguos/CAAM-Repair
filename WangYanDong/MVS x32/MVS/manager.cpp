#include "manager.h"

manager*manager::m_instance=NULL;

manager::manager(void)
{
	m_instance=this;
	m_Camera=new Camera();
	m_Process=new ImageProcess();
	m_mvs=new MVS();

	
	m_mvs->ConnectCamera(m_Camera);
	m_mvs->ConnectImageProcess(m_Process);
	
}


manager::~manager(void)
{
	//if(m_instance!=NULL)
	//	delete m_instance;
	//if(m_window!=NULL)
	//	delete m_window;
	//if(m_RobotComm!=NULL)
	//	delete m_RobotComm;
	//if(m_Controller!=NULL)
		//delete m_Controller;

}

void manager::show()
 {
	 m_mvs->show();
 }