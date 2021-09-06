#include "Mode.hpp"

std::shared_ptr< Mode > Mode::current;

void Mode::set_current(std::shared_ptr< Mode > const &new_current, SDL_Window * window) {
	current = new_current;
	//NOTE: may wish to, e.g., trigger resize events on new current mode.
    SDL_SetWindowSize(window, 1280, 720);
}
