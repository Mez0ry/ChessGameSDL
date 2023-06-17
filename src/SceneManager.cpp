#include "SceneManager.hpp"

SceneManager::SceneManager(): m_pCurrentScene(nullptr), m_CurrentSceneHash(0) {

}

SceneManager::~SceneManager() {}

void SceneManager::OnResize(){
    m_pCurrentScene->OnResize();
}

void SceneManager::HandleInput(const Base::Ref<EventHandler> event_handler) { 
    m_pCurrentScene->HandleInput(event_handler); 
}

void SceneManager::Update(float dt) { 
    m_pCurrentScene->Update(dt); 
}

void SceneManager::Render() { 
    m_pCurrentScene->Render(); 
}
