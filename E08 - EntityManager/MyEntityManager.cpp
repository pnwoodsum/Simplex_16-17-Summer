#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
}
void MyEntityManager::Release(void)
{
	// Delete each of the entity pointers
	for (int i = 0; i < m_uEntityCount; i++) {
		MyEntity* eEntity = m_entityList[i];
		SafeDelete(eEntity);
	}
	// Set entiy count to zero and wipe the list
	m_uEntityCount = 0;
	m_entityList.clear();
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	// Delete the instance if it exists
	if (m_pInstance != nullptr) {
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	// Check the id of each index until the proper index is found
	for (int i = 0; i < m_uEntityCount; i++) {
		if (a_sUniqueID == m_entityList[i]->GetUniqueID()) 
		{
			return i;
		}
	}
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	// Return the last one if index is out of range
	if (a_uIndex >= m_uEntityCount)
	{
		return m_entityList[m_uEntityCount - 1]->GetModel();;
	}

	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	return MyEntity::GetEntity(a_sUniqueID)->GetModel();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	// Return the last one if index is out of range
	if (a_uIndex >= m_uEntityCount)
	{
		return m_entityList[m_uEntityCount - 1]->GetRigidBody();;
	}

	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	return MyEntity::GetEntity(a_sUniqueID)->GetRigidBody();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	// Return the last one if index is out of range
	if (a_uIndex >= m_uEntityCount)
	{
		return m_entityList[m_uEntityCount - 1]->GetModelMatrix();;
	}

	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	return MyEntity::GetEntity(a_sUniqueID)->GetModelMatrix();
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	return MyEntity::GetEntity(a_sUniqueID)->SetModelMatrix(a_m4ToWorld);
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	// Set target index to the last index if it is out of range
	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	// Check for collisions between each model in the entity list
	for (int i = 0; i < m_uEntityCount - 1; i++) {
		for (int j = i + 1; j < m_uEntityCount; j++) {
			m_entityList[i]->IsColliding(m_entityList[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	MyEntity* tempEntity = new MyEntity(a_sFileName, a_sUniqueID);

	// Push the temporary entity to the back of the entity list
	if (tempEntity->IsInitialized()) {
		m_entityList.push_back(tempEntity);
		m_uEntityCount++;
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	// If the index is out of range then return
	if (a_uIndex >= m_uEntityCount) {
		return;
	}
	
	// Switch  the target index to the back so pop_back can be used to remove entity
	if (a_uIndex != m_uEntityCount - 1) {
		std::swap(m_entityList[a_uIndex], m_entityList[m_uEntityCount - 1]);
	}
	
	SafeDelete(m_entityList[m_uEntityCount - 1]);
	m_entityList.pop_back();
	m_uEntityCount--;
	return;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	RemoveEntity((uint)GetEntityIndex(a_sUniqueID));
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	// Return if index is out of range
	if (a_uIndex >= m_uEntityCount) {
		return "Index out of range";
	}

	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	// Set target index to the last index if it is out of range
	if (a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	// Add entity to the end of list if it's out of the range
	if (a_uIndex >= m_entityList.size() || a_uIndex == -1) {
		for (int i = 0; i < m_entityList.size(); i++) {
			m_entityList[i]->AddToRenderList(a_bRigidBody);
		}
	}

	else {
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	MyEntity::GetEntity(a_sUniqueID)->AddToRenderList(a_bRigidBody);
}