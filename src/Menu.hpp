#ifndef __MENU_HPP__
#define __MENU_HPP__
#include "GameScene.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

#include "Text.hpp"
#include <array>
#include "EventHandler.hpp"

class SceneManager;

class Menu : virtual public GameScene {
private:

struct MenuLabel{
  MenuLabel(const char* label_name) : label(label_name){}
  Text text[2];
  uint8_t text_type_index = 0;
  const char* label;
};

public:
  Menu(const Base::Ref<Renderer> renderer, const Base::Ref<Window> window, SceneManager* scene_manager);
  ~Menu();
  
  void OnResize() override;
  void OnCreate() override;
  void HandleInput(const Base::Ref<EventHandler> event_handler) override;
  void Update(float dt) override;
  void Render() override;

private:
  std::array<MenuLabel, 2> m_MenuOptions = {MenuLabel("Play"),MenuLabel("Exit")};
  
  const SDL_Color m_RedColor = {255,0,0,255};
  const SDL_Color m_WhiteColor = {255,255,255,255};

  Texture m_TitlePanelTexture;
  Text m_TitleText;
  std::array<Texture,2> m_ButtonsTexture;
  SceneManager* m_SceneManager;
};

#endif //! __MENU_HPP__