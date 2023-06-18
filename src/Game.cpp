#include "Game.hpp"

Game::Game() : m_bIsRunning(true) {
  ServiceLocator::RegisterService<Window>("ChessGameStellar",0,0,800,700, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  ServiceLocator::RegisterService<Renderer>(ServiceLocator::GetService<Window>(),-1,SDL_RENDERER_ACCELERATED);
  ServiceLocator::RegisterService<EventHandler>();

  m_SceneManager.AddScene<Menu>(ServiceLocator::GetService<Renderer>(),ServiceLocator::GetService<Window>(),&m_SceneManager);
  m_SceneManager.AddScene<Playing>(ServiceLocator::GetService<Renderer>(),ServiceLocator::GetService<Window>(),&m_SceneManager);
  m_SceneManager.AddScene<Exit>(ServiceLocator::GetService<Renderer>(),ServiceLocator::GetService<Window>(),m_bIsRunning);
  m_SceneManager.TransitionTo<Menu>();
  
}

Game::~Game() {}

void Game::Run() {

  //The frames per second timer
  Timestamp fps_timer;

  //The frames per second cap timer
  Timestamp cap_timer;
  
  Timestamp step_timer;

  int counted_frames = 0;
  fps_timer.Start();
  
  while (m_bIsRunning)
  {
    cap_timer.Start();
    ServiceLocator::GetService<Renderer>()->RenderClear();

    HandleInput();
    float dt = step_timer.GetTicks() / 1000.0f;
    Update(dt * ((m_fps / 2) - 0.1f));
    step_timer.Start();
    Render();
    
    ServiceLocator::GetService<Renderer>()->RenderPresent();
    ++counted_frames;
    int frame_ticks = cap_timer.GetTicks();
    if(m_frameDelay > frame_ticks)
    {
      SDL_Delay(m_frameDelay - frame_ticks);
    }
  }
}

void Game::HandleInput() {
 auto event_handler = ServiceLocator::GetService<EventHandler>();
 event_handler->PollEvents();
 
 auto& window_input = event_handler->GetWindowInput();
 auto& keyboard_input = event_handler->GetKeyboardInput();

 if(event_handler->IsQuit() || keyboard_input.IsPressed(SDLK_ESCAPE)){
  m_bIsRunning = false;
 }
 
 if(window_input.OnEvent(SDL_WINDOWEVENT_RESIZED)){
  m_SceneManager.OnResize();
 }

 m_SceneManager.HandleInput(event_handler);
}

void Game::Update(float dt) {   
  m_SceneManager.Update(dt);
}

void Game::Render() {
 m_SceneManager.Render();
}
