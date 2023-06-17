#ifndef __SCENE_MANAGER_HPP__
#define __SCENE_MANAGER_HPP__
#include "Base.hpp"
#include "GameScene.hpp"
#include <unordered_map>
#include <algorithm>

class SceneManager {
private:
  using hash_code = std::size_t;
public:
  SceneManager();
  ~SceneManager();
  
  void OnResize();
  void HandleInput(const Base::Ref<EventHandler> event_handler);
  void Update(float dt);
  void Render();

  template <typename T, typename... Args> 
  void AddScene(Args &&...args) {
    if (!HasScene<T>()) {
      m_ScenesMap.insert(std::make_pair(GetHashCode<T>(), Base::CreateRef<T, Args...>(std::forward<Args>(args)...)));
    }
  }

  template <typename T> 
  bool HasScene() {

    auto it = std::find_if(m_ScenesMap.begin(), m_ScenesMap.end(), [&](const auto &scene_pair) {
        return (scene_pair.first == GetHashCode<T>());
    });

    return (it != m_ScenesMap.end()) ? true : false;
  }

  template <typename... _Ts, class = typename std::enable_if_t< (sizeof...(_Ts) > 1), std::nullptr_t>>
  void AddScene() {
    (AddScene<_Ts>(), ...);
  }

  template <typename _Scene> 
  void TransitionTo() {
    hash_code scene_hash = GetHashCode<_Scene>();
    if (m_CurrentSceneHash == scene_hash)
      return;

    auto it = std::find_if(m_ScenesMap.begin(), m_ScenesMap.end(), [&](const auto &scene_pair) {
        return (scene_pair.first == scene_hash);
    });

    if (m_pCurrentScene != nullptr) {
      m_pCurrentScene->OnDestroy();
      m_pCurrentScene = nullptr;
    }

    if (it != m_ScenesMap.end()) {
      m_pCurrentScene = std::dynamic_pointer_cast<_Scene>(it->second);
      m_pCurrentScene->OnCreate();
    }
  }

private:
  template <typename T> 
  hash_code GetHashCode() {
    return typeid(decltype(std::declval<T>())).hash_code();
  }

private:
  std::unordered_map<hash_code, Base::Ref<GameScene>> m_ScenesMap;
  std::shared_ptr<GameScene> m_pCurrentScene;
  hash_code m_CurrentSceneHash;
};

#endif //!__SCENE_MANAGER_HPP__