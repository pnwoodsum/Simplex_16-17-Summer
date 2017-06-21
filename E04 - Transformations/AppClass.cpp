#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Peter Woodsum - pnw8434@rit.edu";

	//Alberto needed this at this position for software recording.
	m_pWindow->setPosition(sf::Vector2i(710, 0)); 

	// Initialize the cubes and the positions of each cube
	for (uint i = 0; i < m_iMeshCount; i++) {
		m_pMeshes.push_back(new MyMesh());
		m_pMeshes[i]->GenerateCube(0.5f, C_RED);
		m_v3Positions.push_back(new vector3(0, 0, 0));
	}

	// Position each of the cubes individually
	*m_v3Positions[0] = vector3(0, 0, 0);
	*m_v3Positions[1] = vector3(-0.5, 0, 0);
	*m_v3Positions[2] = vector3(-1.0, 0, 0);
	*m_v3Positions[3] = vector3(-1.5, 0, 0);
	*m_v3Positions[4] = vector3(-2.0, 0, 0);
	*m_v3Positions[5] = vector3(-2.5, 0, 0);
	*m_v3Positions[7] = vector3(0.5, 0, 0);
	*m_v3Positions[6] = vector3(1.0, 0, 0);
	*m_v3Positions[8] = vector3(1.5, 0, 0);
	*m_v3Positions[9] = vector3(2.0, 0, 0);
	*m_v3Positions[10] = vector3(2.5, 0, 0);
	*m_v3Positions[11] = vector3(0.0, -0.5, 0);
	*m_v3Positions[12] = vector3(-0.5, -0.5, 0);
	*m_v3Positions[13] = vector3(-1.0, -0.5, 0);
	*m_v3Positions[14] = vector3(-1.5, -0.5, 0);
	*m_v3Positions[15] = vector3(-2.5, -0.5, 0);
	*m_v3Positions[16] = vector3(0.5, -0.5, 0);
	*m_v3Positions[17] = vector3(1.0, -0.5, 0);
	*m_v3Positions[18] = vector3(1.5, -0.5, 0);
	*m_v3Positions[19] = vector3(2.5, -0.5, 0);
	*m_v3Positions[20] = vector3(0.0, 0.5, 0);
	*m_v3Positions[21] = vector3(-0.5, 0.5, 0);
	*m_v3Positions[22] = vector3(-1.5, 0.5, 0);
	*m_v3Positions[23] = vector3(-2.0, 0.5, 0);
	*m_v3Positions[24] = vector3(0.5, 0.5, 0);
	*m_v3Positions[25] = vector3(1.5, 0.5, 0);
	*m_v3Positions[26] = vector3(2.0, 0.5, 0);
	*m_v3Positions[27] = vector3(-1.5, -1.0, 0);
	*m_v3Positions[28] = vector3(-2.5, -1.0, 0);
	*m_v3Positions[29] = vector3(1.5, -1.0, 0);
	*m_v3Positions[30] = vector3(2.5, -1.0, 0);
	*m_v3Positions[31] = vector3(-0.5, -1.5, 0);
	*m_v3Positions[32] = vector3(-1.0, -1.5, 0);
	*m_v3Positions[33] = vector3(0.5, -1.5, 0);
	*m_v3Positions[34] = vector3(1.0, -1.5, 0);
	*m_v3Positions[35] = vector3(0.0, 1.0, 0);
	*m_v3Positions[36] = vector3(-0.5, 1.0, 0);
	*m_v3Positions[37] = vector3(-1.0, 1.0, 0);
	*m_v3Positions[38] = vector3(-1.5, 1.0, 0);
	*m_v3Positions[39] = vector3(0.5, 1.0, 0);
	*m_v3Positions[40] = vector3(1.0, 1.0, 0);
	*m_v3Positions[41] = vector3(1.5, 1.0, 0);
	*m_v3Positions[42] = vector3(-1.0, 1.5, 0);
	*m_v3Positions[43] = vector3(1.0, 1.5, 0);
	*m_v3Positions[44] = vector3(-1.5, 2.0, 0);
	*m_v3Positions[45] = vector3(1.5, 2.0, 0);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
	static float value = -5.0f;

	value += 0.01f;

	// Set each models translations
	for (uint i = 0; i < m_iMeshCount; i++)
	{
		matrix4 m4Model = glm::translate(IDENTITY_M4, *m_v3Positions[i]);
		m4Model = m4Scale * glm::translate(m4Model, vector3(value, 2.0f, 3.0f));

		m_pMeshes[i]->Render(m4Projection, m4View, m4Model);
	}
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	for (uint i = 0; i < m_iMeshCount; i++) {
		SafeDelete(m_pMeshes[i]);
		SafeDelete(m_v3Positions[i]);
	}

	//release GUI
	ShutdownGUI();
}