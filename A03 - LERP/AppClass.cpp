#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Peter Woodsum - pnw8434@rit.edu";

	//Alberto needed this at this position for software recording.
	m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndView(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength

		m_sphereList.push_back(m_pMeshMngr->GenerateSphere(0.1f, 10, C_WHITE)); // Initialize the spheres

		// Initialize pathSTopList vector
		std::vector<vector3> tempVect;
		m_pathStopList.push_back(tempVect);

		// Set each of the stop positions for each point in each ring
		for (uint j = 0; j < i; j++) 
		{
			float theta = ((j * 2.0f * PI) / i);
			float xPos = cos(theta) * (fSize - 0.55f);
			float yPos = sin(theta) * (fSize - 0.55f);
			m_pathStopList[i - 3].push_back(vector3(xPos, yPos, 0.0f));
		}

		// Initialize current path list
		m_currentPathList.push_back(0);
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
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	// The map value is the same for each circle
	float fMappedVal = MapValue(fTimer, 0.0f, 0.5f, 0.0f, 1.0f);

	// Reset the mapped value when it is greater than 1
	if (fMappedVal > 1.0f)
	{
		fTimer = 0.0f;
		fMappedVal = 0.0f;
		for (uint i = 0; i < m_uOrbits; i++)
		{
			m_currentPathList[i]++; // Increment the current point
			if (m_currentPathList[i] > i+2) { // Loop back for the 0th index
				m_currentPathList[i] = 0;
			}
		}
	}

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));

		// Find the position of the next stop
		vector3 nextStop;
		if (m_currentPathList[i] == i + 2) 
		{
			nextStop = m_pathStopList[i][0]; // IF it's at the final index, then the next stop is the first index [0]
		}
		else 
		{
			nextStop = m_pathStopList[i][m_currentPathList[i]+1]; // The next stop of the 0 index if the 
		}

		//calculate the current position
		vector3 v3CurrentPos = glm::lerp(m_pathStopList[i][m_currentPathList[i]], nextStop, fMappedVal);
		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddMeshToRenderList(m_sphereList[i], m4Model);
	}

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
}