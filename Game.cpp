#include "Player.cpp"
#include "AssetManager.cpp"

//SDL2
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <string>
#include <time.h> 
#include <dirent.h>
#include <sys/types.h>

#define scale 3.5

using namespace std;

SDL_Renderer *s;

struct chunk {
	char x,y = 0;
	char data[32][32];
	int chunkid = 0;
}loadarea;

class STRUCTURE_MANAGER {
	struct structure {
		char rarity = 0; //How many to place per world
		chunk chunkdata;
		string name = "_STRUCTURE_NULL_";
	};

	public:
	//Structures
	structure null;
	structure structures[1];

	void load() {
		//Temp hard coding for home
		structure home;
		for(int i = 0;i<32;++i) {
			home.chunkdata.data[0][i] = 1;
		}
		for(int i = 0;i<32;++i) {
			home.chunkdata.data[31][i] = 1;
		}
		for(int i = 0;i<32;++i) {
			home.chunkdata.data[i][0] = 1;
		}
		for(int i = 0;i<32;++i) {
			home.chunkdata.data[i][31] = 1;
		}
		home.chunkdata.data[15][31] = 0;
		home.chunkdata.data[16][31] = 0;
		home.chunkdata.data[15][0] = 0;
		home.chunkdata.data[16][0] = 0;
		home.chunkdata.data[31][15] = 0;
		home.chunkdata.data[31][16] = 0;
		home.chunkdata.data[0][15] = 0;
		home.chunkdata.data[0][16] = 0;
		home.chunkdata.chunkid = 1;
		home.name = "House";
		
		structures[0] = home;
	}
}StructureManager;

int WHEIGHT = 700;
int WWIDTH = 1240;

chunk world[70][70];

int chunkassign = 1;

void generateWoodland(int chunkx,int chunky) {
	chunk temp;
	int prevx = 0;
	int prevy = 0;
	for(int iy = 0;iy < 32;++iy) {
		for(int ix = 0;ix < 32;++ix) {
			if(rand() % 100 + 1 == 1 && sqrt((prevx*prevy)+(ix*iy))>2) {
				temp.data[iy][ix] = 2;
				prevx = ix;
				prevy = iy;
			}
			else {
				temp.data[iy][ix] = 0;
			}
		}
	}
	++chunkassign;
	temp.chunkid = chunkassign;
	world[chunkx][chunky] = temp;
}

chunk getChunk(int x, int y) {
	return world[int(floor(x/32))][int(floor(y/32))];
}

char getTileAt(int x, int y) {
	if(x<0) {
		x=0-x;
	}
	if(y<0) {
		y=0-y;
	}

	chunk readChunk = getChunk(x, y);
	if(readChunk.chunkid == 0) {
		generateWoodland(int(floor(x/32)),int(floor(y/32)));
		return 0;
	}
	return(readChunk.data[int((y - (floor(y/32)*32)))][int((x - (floor(x/32)*32)))]);
}

void ray(float x,float y,double dir,int binit = 255) {
	SDL_RenderSetScale(s,scale,scale);
	x=(x/scale);
	y=(y/scale);
	dir = ((dir / 180) * 3.1415);
    //Cast ray
    for(float brightness = binit;brightness > 1;brightness-=1.75) {
		SDL_SetRenderDrawColor(s, brightness, brightness, brightness, 0xFF);
		x+=(sin(dir)*2.5)/scale;
		y+=(cos(dir)*2.5)/scale;
		SDL_RenderDrawPoint(s, round(x), round(y));
		
		//Handle collision
		if(y > (WHEIGHT/scale) || x > (WWIDTH/scale) || x < 0 || y < 0) {
			return;
		}
		else if(getTileAt(int((( y - Player.y )/6)),int((( x + Player.x )/6))) != 0) {
			return;
		}
	}
}

int InputX = 0;
int InputY = 0;

bool quit = false;

void handle_input(float dt) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch(event.type) {
				case SDL_KEYDOWN: switch( event.key.keysym.sym ){
					case SDLK_a:
						InputX = -1*Player.speed;
						break;
					case SDLK_d:
						InputX = 1*Player.speed;
						break;
					case SDLK_w:
						InputY = 1*Player.speed;
						break;
					case SDLK_s:
						InputY = -1*Player.speed;
						break;
					default:
						break;
					}
					break;
				case SDL_KEYUP: switch( event.key.keysym.sym ){
					case SDLK_a:
						if(InputX<0) {
							InputX = 0;
						}
						break;
					case SDLK_d:
						if(InputX>0) {
							InputX = 0;
						}
						break;
					case SDLK_w:
						if(InputY>0) {
							InputY = 0;
						}
						break;
					case SDLK_s:
						if(InputY<0) {
							InputY = 0;
						}
						break;
					default:
						break;
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;
				default:
					break;
			}
	}
	Player.xv+=InputX*(dt*5);
	Player.yv+=InputY*(dt*5);
}

void render_overlay() {
	int x = (WWIDTH / 2)/scale;
	int y = (WHEIGHT / 2)/scale;
	for(int iy = -16;iy < 16;++iy) {
		for(int ix = -16;ix < 16;++ix) {
			if(getTileAt(iy+int(((int(Player.y))/4)),ix+int(((x+int(Player.x))/4))) == 2){
				SDL_Rect dstrect = { int((ix+(((x-(Player.x))/4)))*4), int((iy+(((y-(Player.y))/4)))*4), 50, 50 };
				SDL_RenderCopy(s, AssetManager.texture_tree, NULL, &dstrect);
			}
		}
	}
}

int main(int argv, char ** argc) {
	std::cout << "Loading structures... (build/structures/*)\n";
	srand (time(NULL));
	StructureManager.load();
	
	std::cout << "Loaded Structures.\n";
	
	world[0][0] = StructureManager.structures[0].chunkdata;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to initialize the SDL2 library\n";
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2 Window",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WWIDTH, WHEIGHT,
                                          0);
    if(!window)
    {
        std::cout << "Failed to create window\n";
        return -1;
    }
    SDL_SetWindowResizable(window, SDL_TRUE);

    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    if(!window_surface)
    {
        std::cout << "Failed to get the surface from the window\n";
        return -1;
    }

    SDL_UpdateWindowSurface(window);

	s = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) ;

	Uint32 start,end;
	float deltatime = 0.0;
	
	AssetManager.load_assets(s);
	
	float swing = 0.0;

    while (!quit) {
		//Get deltatime
		end = SDL_GetTicks();
		deltatime = (end - start) / 1000.0f;
		start = SDL_GetTicks();

		//Update everything
		handle_input(deltatime);
		Player.Update(WWIDTH,WHEIGHT,deltatime);
		
		//Collision
		int x = (WWIDTH / 2)/scale;
		int y = (WHEIGHT / 2)/scale;

		Player.x += Player.xv;
		if(getTileAt(int(((y-int(Player.y))/6)),int(((x+int(Player.x))/6))) != 0) {
			Player.x -= Player.xv;
		}
		Player.y += Player.yv;
		if(getTileAt(int(((y-int(Player.y))/6)),int(((x+int(Player.x))/6))) != 0) {
			Player.y -= Player.yv;
		}
        
		SDL_GetWindowSize(window, &WWIDTH, &WHEIGHT);
        // We clear what we draw before
        SDL_RenderClear(s);
        
		if(abs(InputX)+abs(InputY) > 0) {
			swing+=(5)*deltatime;
		}

		if(swing > 3.1415*2) {
			swing = 0.0;
		}

        for(double i = 70;i<110;i+=0.5) {
			double b = 15 * sin((i / 60)*3.1415);
			b+= 240;
			if(b > 255) {
				b = 255;
			}
			ray(WWIDTH / 2,WHEIGHT / 2,i - Player.dir + (sin(swing)*3),b-(rand() % 2));
		}
		
		//render_overlay();

        // Set the color to what was before
        SDL_SetRenderDrawColor(s, 0x00, 0x00, 0x00, 0xFF);
        // .. you could do some other drawing here
        // And now we present everything we draw after the clear.
        SDL_RenderPresent(s);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(s);
    SDL_Quit();
}