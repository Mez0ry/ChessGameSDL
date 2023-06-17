#include "Game.hpp"
#include "init_library.hpp"

int main(int argc, char **argv) {
  init_library<SDL> lib_sdl(SDL_INIT_EVERYTHING);
  init_library<SDL_Image> lib_img(IMG_INIT_PNG | IMG_INIT_JPG);
  init_library<SDL_TTF> lib_ttf;

  Game gm;
  gm.Run();
  
  return 0;
}