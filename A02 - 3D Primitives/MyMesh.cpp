#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triang A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triang A->B->C and then the triang C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> vertices;

	// Set vertex info
	vertices.push_back(vector3(0.0f, -a_fHeight/2.0f, 0.0f));

	GLfloat theta = 0;

	for (int i = 0; i < a_nSubdivisions; i++) {
		vertices.push_back(vector3(static_cast<GLfloat>(sin(theta)) * a_fRadius,
			-a_fHeight / 2.0f,
			-static_cast<GLfloat>(cos(theta)) * a_fRadius));

		theta += ((2 * PI) / a_nSubdivisions);
	}

	vertices.push_back(vector3(0.0f, a_fHeight / 2.0f, 0.0f));

	// Bottom faces
	for (int i = 0; i < a_nSubdivisions; i++) {
		AddTri(vertices[0], vertices[i + 1], vertices[i + 2]);
	}
	AddTri(vertices[0], vertices[a_nSubdivisions], vertices[1]);

	// Vertical faces
	for (int i = 0; i < a_nSubdivisions; i++) {
		AddTri(vertices[a_nSubdivisions + 1], vertices[i + 2], vertices[i + 1]);
	}
	AddTri(vertices[a_nSubdivisions], vertices[a_nSubdivisions + 1], vertices[1]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	// Bottom vertices
	std::vector<vector3 > verticesBot;
	verticesBot.push_back(vector3(0.0f, -a_fHeight / 2.0f, 0.0f));
	GLfloat theta = 0;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		theta += static_cast<GLfloat>(2 * PI / a_nSubdivisions);
		vector3 temp = vector3(static_cast<GLfloat>(cos(theta)) * a_fRadius,
			-a_fHeight / 2.0f,
			-static_cast<GLfloat>(sin(theta)) * a_fRadius);
		verticesBot.push_back(temp);
	}

	// Top vertices
	std::vector<vector3 > verticesTop;
	verticesTop.push_back(vector3(0.0f, a_fHeight / 2.0f, 0.0f));
	vector3 vHeight = vector3(0.0f, a_fHeight, 0.0f);
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		verticesTop.push_back(verticesBot[i + 1] + vHeight);
	}

	// Bottom Faces
	for (int i = 1; i < a_nSubdivisions; i++)
	{
		AddTri(verticesBot[0], verticesBot[i + 1], verticesBot[i]);
	}
	AddTri(verticesBot[0], verticesBot[1], verticesBot[a_nSubdivisions]);
	
	// Top Faces
	for (int i = 1; i < a_nSubdivisions; i++)
	{
		AddTri(verticesTop[0], verticesTop[i], verticesTop[i + 1]);
	}
	AddTri(verticesTop[0], verticesTop[a_nSubdivisions], verticesTop[1]);

	// Side Faces
	for (int i = 1; i < a_nSubdivisions; i++)
	{
		AddTri(verticesBot[i], verticesBot[i + 1], verticesTop[i]);

		AddTri(verticesTop[i], verticesBot[i + 1], verticesTop[i + 1]);
	}
	AddTri(verticesBot[a_nSubdivisions], verticesBot[1], verticesTop[a_nSubdivisions]);

	AddTri(verticesTop[a_nSubdivisions], verticesBot[1], verticesTop[1]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	// Top vertices
	std::vector<vector3 > verticesTopOuter;
	GLfloat theta = 0;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		theta += static_cast<GLfloat>(2 * PI / a_nSubdivisions);
		vector3 temp = vector3(static_cast<GLfloat>(cos(theta)) * a_fOuterRadius,
			a_fHeight / 2.0f,
			-static_cast<GLfloat>(sin(theta)) * a_fOuterRadius);
		verticesTopOuter.push_back(temp);
	}

	std::vector<vector3 > verticesTopInner;
	theta = 0;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		theta += static_cast<GLfloat>(2 * PI / a_nSubdivisions);
		vector3 temp = vector3(static_cast<GLfloat>(cos(theta)) * a_fInnerRadius,
			a_fHeight / 2.0f,
			-static_cast<GLfloat>(sin(theta)) * a_fInnerRadius);
		verticesTopInner.push_back(temp);
	}

	// Bottom vertices
	std::vector<vector3 > verticesBotOuter;
	theta = 0;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		theta += static_cast<GLfloat>(2 * PI / a_nSubdivisions);
		vector3 temp = vector3(static_cast<GLfloat>(cos(theta)) * a_fOuterRadius,
			-a_fHeight / 2.0f,
			-static_cast<GLfloat>(sin(theta)) * a_fOuterRadius);
		verticesBotOuter.push_back(temp);
	}

	std::vector<vector3 > verticesBotInner;
	theta = 0;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		theta += static_cast<GLfloat>(2 * PI / a_nSubdivisions);
		vector3 temp = vector3(static_cast<GLfloat>(cos(theta)) * a_fInnerRadius,
			-a_fHeight / 2.0f,
			-static_cast<GLfloat>(sin(theta)) * a_fInnerRadius);
		verticesBotInner.push_back(temp);
	}

	// Top faces
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(verticesTopInner[i], verticesTopOuter[i], verticesTopOuter[i + 1]);
	}
	AddTri(verticesTopInner[a_nSubdivisions - 1], verticesTopOuter[a_nSubdivisions - 1], verticesTopOuter[0]);

	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(verticesTopOuter[i + 1], verticesTopInner[i + 1], verticesTopInner[i]);
	}
	AddTri(verticesTopOuter[0], verticesTopInner[0], verticesTopInner[a_nSubdivisions - 1]);

	// Bottom faces
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(verticesBotOuter[i + 1], verticesBotOuter[i], verticesBotInner[i]);
	}
	AddTri(verticesBotOuter[0], verticesBotOuter[a_nSubdivisions - 1], verticesBotInner[a_nSubdivisions - 1]);

	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(verticesBotInner[i], verticesBotInner[i + 1], verticesBotOuter[i + 1]);
	}
	AddTri(verticesBotInner[a_nSubdivisions - 1], verticesBotInner[0], verticesBotOuter[0]);

	// Vertical Inside faces
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(verticesTopInner[i], verticesBotInner[i + 1], verticesBotInner[i]);
	}
	AddTri(verticesTopInner[a_nSubdivisions - 1], verticesBotInner[0], verticesBotInner[a_nSubdivisions - 1]);

	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(verticesTopInner[i + 1], verticesBotInner[i + 1], verticesTopInner[i]);
	}
	AddTri(verticesTopInner[0], verticesBotInner[0], verticesTopInner[a_nSubdivisions - 1]);

	// Vertical outside faces
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(verticesBotOuter[i], verticesBotOuter[i + 1], verticesTopOuter[i]);
	}
	AddTri(verticesBotOuter[a_nSubdivisions - 1], verticesBotOuter[0], verticesTopOuter[a_nSubdivisions - 1]);

	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(verticesTopOuter[i + 1], verticesTopOuter[i], verticesBotOuter[i + 1]);
	}
	AddTri(verticesTopOuter[0], verticesTopOuter[a_nSubdivisions - 1], verticesBotOuter[0]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	std::vector<std::vector<vector3>> vertices;

	// Initialize the vector of vectors
	for (int i = 0; i < a_nSubdivisions; i++) {
		std::vector<vector3> temp;
		vertices.push_back(temp);
	}

	// Vertices. Each row of vertices is its own vector
	for (int i = 0; i < a_nSubdivisions; i++) {
		for (int j = 0; j < a_nSubdivisions; j++) {
			float y = 2.0 * i / a_nSubdivisions - 1;
			float r = sqrt(1.0f - (pow(y,2)));
			float x = r * sin(2.0f * PI * j / a_nSubdivisions);
			float z = r * cos (2.0 * PI * j / a_nSubdivisions);

			vertices[i].push_back(vector3(x * a_fRadius, y * a_fRadius, z * a_fRadius));
		}
	}

	// The top vertex
	std::vector<vector3> topVert;
	topVert.push_back(vector3(0.0f, a_fRadius, 0.0f));
	vertices.push_back(topVert);

	// Bottom
	for (int i = 0; i < a_nSubdivisions-1; i++)
	{
		AddTri(vertices[0][0], vertices[1][i + 1], vertices[1][i]);
	}
	AddTri(vertices[0][0], vertices[1][0], vertices[1][a_nSubdivisions-1]);

	// In between
	for (int i = 1; i < a_nSubdivisions-1; i++) {
		for (int j = 0; j < a_nSubdivisions-1; j++) {
			AddQuad(vertices[i][j], vertices[i][j + 1], vertices[i + 1][j], vertices[i + 1][j+1]);
		}
		AddQuad(vertices[i][a_nSubdivisions-1], vertices[i][0], vertices[i + 1][a_nSubdivisions - 1], vertices[i + 1][0]);
	}

	// Top
	for (int i = 0; i < a_nSubdivisions-1; i++)
	{
		AddTri(vertices[a_nSubdivisions][0], vertices[a_nSubdivisions-1][i], vertices[a_nSubdivisions-1][i + 1]);
	}
	AddTri(vertices[a_nSubdivisions][0], vertices[a_nSubdivisions - 1][a_nSubdivisions-1], vertices[a_nSubdivisions - 1][0]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}