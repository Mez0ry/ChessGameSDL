#ifndef __GAME_SCENE_HPP__
#define __GAME_SCENE_HPP__
#include "Base.hpp"

class Renderer;
class Window;
class EventHandler;

class GameScene {
public:
  virtual ~GameScene() {}

  virtual void OnCreate() {};
  virtual void OnResize() {}
  virtual void OnDestroy() {};

  
  virtual void HandleInput(const Base::Ref<EventHandler> event_handler) = 0;
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;


protected:
  Base::Ref<Renderer> m_Renderer;
  Base::Ref<Window> m_Window;
};

#endif //! __GAME_SCENE_HPP__