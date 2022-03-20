#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"

class assetManager {
	public:
		SDL_Texture * texture_tree;
		void load_assets(SDL_Renderer *s) {
			SDL_Surface * image = SDL_LoadBMP("images/overlays/tree_overlay.bmp");
			texture_tree = SDL_CreateTextureFromSurface(s, image);
		}
};

assetManager AssetManager;