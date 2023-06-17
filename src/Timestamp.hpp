#ifndef __TIMER_HPP__
#define __TIMER_HPP__
#include <chrono>
#include <iostream>
#include <cstdint>
#include "SDL2/SDL.h"

class Timestamp{
  public:
      Timestamp();
      ~Timestamp() = default;

      void Start();
      void Stop();
      void Pause();
      void UnPause();

      Uint32 GetTicks();

      //Checks the status of the timer
      bool isStarted();
      bool isPaused();

  private:
      //The clock time when the timer started
      Uint32 m_StartTicks;

      //The ticks stored when the timer was paused
      Uint32 m_PausedTicks;

      //The timer status
      bool m_IsPaused;
      bool m_IsStarted;
};

#endif  //! __TIMER_HPP__
