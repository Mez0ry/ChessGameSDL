#ifndef __CHESS_GAME_HPP__
#define __CHESS_GAME_HPP__
#include <iostream>
#include <cstdint>

#include "ServiceLocator.hpp"
#include "EventHandler.hpp"
#include "Timestamp.hpp"

#include "Menu.hpp"
#include "Playing.hpp"
#include "Exit.hpp"
#include "SceneManager.hpp"

class Game
{
private:
  uint32_t m_fps = 60;
  uint32_t m_frameDelay = 3000 / m_fps;
  uint32_t m_frameStart;
  uint32_t m_frameTime;
public:
    Game();
    ~Game();

    void Run();
private:
    void HandleInput();
    void Update(float dt);
    void Render();

private:
    bool m_bIsRunning;
    SceneManager m_SceneManager;
};
#endif //!__CHESS_GAME_HPP__