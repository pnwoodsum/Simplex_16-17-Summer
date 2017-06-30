#include "AppClass.h"
void Application::InitVariables(void)
{
	//Music
	String sRoute = m_pSystem->m_pFolder->GetFolderData();
	sRoute += m_pSystem->m_pFolder->GetFolderAudio();

	//Background music
	m_soundBGM.openFromFile(sRoute + "elementary-wave-11.ogg");
	m_soundBGM.play();
	m_soundBGM.setLoop(true);

	//sound effect
	m_soundBuffer.loadFromFile(sRoute + "12C.wav");
	m_sound.setBuffer(m_soundBuffer);

	// Set default stops
	if (m_uTotalStops < 1) {
		m_vStopList.push_back(vector3(-3.0, -3.0, -3.0));
		m_vStopList.push_back(vector3(-3.0, 3.0, 0.0));
		m_vStopList.push_back(vector3(-2.0, -2.0, -3.0));
		m_vStopList.push_back(vector3(-2.0, 2.0, 0.0));
		m_vStopList.push_back(vector3(-1.0, -1.0, -3.0));
		m_vStopList.push_back(vector3(-1.0, 1.0, 0.0));
		m_vStopList.push_back(vector3(0.0, 0.0, 0.0));
		m_vStopList.push_back(vector3(1.0, -1.0, 0.0));
		m_vStopList.push_back(vector3(1.0, 1.0, -3.0));
		m_vStopList.push_back(vector3(2.0, -2.0, 0.0));
		m_vStopList.push_back(vector3(2.0, 2.0, -3.0));
		m_vStopList.push_back(vector3(3.0, -3.0, -0.0));
		m_vStopList.push_back(vector3(3.0, 3.0, -3.0));
		m_uTotalStops = 13;
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Move light... just for fun...
	static double dTimer = 0.0f; //create a variable to store time
	static uint uClock = m_pSystem->GenClock(); //generate a clock to track time
	m_fMapTracker += m_pSystem->GetDeltaTime(uClock);
	dTimer += m_pSystem->GetDeltaTime(uClock); //get the time difference since last time called
	double dAngle = MapValue(dTimer, 0.0, 5.0, 0.0, 360.0);//map the value so we do not need to wait 360 seconds, only 5
	

	static vector3 v3Color(C_WHITE); //color of the light
	vector3 v3Position(glm::sin(glm::radians(dAngle)) * 5.0f, 2.5f, glm::cos(glm::radians(dAngle)) * 5.0f);//holds position of light
	m_pLightMngr->SetPosition(v3Position, 1); //set the position of first light(0 is reserved for global light)
	m_pLightMngr->SetIntensity(5.0f, 1); //set the intensity of first light
	m_pLightMngr->SetColor(v3Color, 1); //set the color of first light
}

void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	// Draw a sphere at each of the stops based on the .ini file
	for (uint i = 0; i < m_uTotalStops; i++) {
		m_pMeshMngr->AddSphereToRenderList(glm::translate(IDENTITY_M4, m_vStopList[i]) * glm::scale(vector3(0.05f, 0.05f, 0.05f)), C_RED);
	}

	// Find the current position of the model
	float fMappedValue = MapValue(m_fMapTracker, 0.0f, 1.0f, 0.0f, 1.0f);
	if (fMappedValue > 1.0f) {
		fMappedValue = 0.0f;
		m_fMapTracker = 0.0f;
		m_uCurrentStop++;
		if (m_uCurrentStop >= m_uTotalStops) {
			m_uCurrentStop = 0;
		}
	}
	// Increment and loop the current stop
	float nextStop = m_uCurrentStop + 1;
	if (nextStop >= m_uTotalStops) {
		nextStop = 0;
	}
	m_v3CurrentPos = glm::lerp(m_vStopList[m_uCurrentStop], m_vStopList[nextStop], fMappedValue);

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	// Update the rotation
	if (!m_bRotating) {
		m_fRotation = 90.0f;
	}
	else {
		m_fRotation += 1.0f;
	}

	// Set the model matrix
	m_pMeshMngr->AddTorusToRenderList(glm::translate(IDENTITY_M4, m_v3CurrentPos) * glm::rotate(m_fRotation, AXIS_X) * glm::scale(vector3(0.7f)), C_GREEN);
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();
	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}

void Application::Release(void)
{
	//release GUI
	ShutdownGUI();

	//release variables
	SafeDelete(m_pModel);
}