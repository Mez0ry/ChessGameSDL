#ifndef __EXIT_HPP__
#define __EXIT_HPP__
#include "GameScene.hpp"
#include "Text.hpp"
#include "Timestamp.hpp"

class Exit : virtual public GameScene {
public:
  Exit(const Base::Ref<Renderer> renderer, const Base::Ref<Window> window,bool& is_running);
  ~Exit();

  void OnResize() override;
  void OnCreate() override;
  
  void HandleInput(const Base::Ref<EventHandler> event_handler) override;
  void Update(float dt) override;
  void Render() override;

private:
  bool& m_IsRunning;
  Text m_Text;
  
  Color m_BlackColor = {0,0,0,255};
  Timestamp m_TextShowTime;
  bool m_StartTimerOnce = false;
};

#endif //! __EXIT_HPP__