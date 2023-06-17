#ifndef __PLAYING_HPP__
#define __PLAYING_HPP__

#include "GameScene.hpp"
#include "EventHandler.hpp"
#include "Text.hpp"
#include "Timestamp.hpp"

#include "Board.hpp"
#include "Player.hpp"
#include "PieceBuilder.hpp"
#include "DragEntity.hpp"

class SceneManager;

class Playing : virtual public GameScene {
public:
  Playing(const Base::Ref<Renderer> renderer, const Base::Ref<Window> window,SceneManager* scene_manager);
  ~Playing();

  void OnResize() override;
  void OnCreate() override;
  void OnDestroy() override;
  
  void HandleInput(const Base::Ref<EventHandler> event_handler) override;
  void Update(float dt) override;
  void Render() override;
private:
  MouseInput m_MouseInput;
  SceneManager* m_SceneManager;
  Board m_Board;
  std::vector<Player> m_Players;
};

#endif //! __PLAYING_HPP__