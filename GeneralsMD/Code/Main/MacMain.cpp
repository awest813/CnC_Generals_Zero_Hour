/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if defined(__APPLE__)

#include <cstdio>

#ifdef CNC_HAS_SDL3
#include <SDL3/SDL.h>
#endif

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

#ifdef CNC_HAS_SDL3
	const char* product_name = "Command & Conquer Generals: Zero Hour";
	if (!SDL_SetAppMetadata(product_name, "0.0.0", "com.ea.generals.zero-hour")) {
		std::fprintf(stderr, "%s: SDL_SetAppMetadata failed: %s\n", product_name, SDL_GetError());
	}

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		std::fprintf(stderr, "%s: SDL_Init failed: %s\n", product_name, SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow(product_name, 1280, 720, SDL_WINDOW_RESIZABLE);
	if (window == nullptr) {
		std::fprintf(stderr, "%s: SDL_CreateWindow failed: %s\n", product_name, SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_ShowWindow(window);
	SDL_PumpEvents();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
#else
	std::fprintf(stderr, "Command & Conquer Generals: Zero Hour: SDL3 is required for the macOS bootstrap target.\n");
	return 1;
#endif
}

#endif
