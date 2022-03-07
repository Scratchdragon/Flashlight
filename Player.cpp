#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"

#include <cmath>

int mouseX, mouseY;

class playerCls {
	public:
		float speed = 1;
		float xv, yv = 0;
		float x = -30;
		float y = 0;
		int dir = 0;
		double dest = 0.0;
		void Update(int WWIDTH,int WHEIGHT,float dt) {
			//Update velocity
			yv = yv / 1.25;
			xv = xv / 1.25;
			
			//Look towards mouse
			SDL_GetMouseState(&mouseX, &mouseY);
			mouseX-=WWIDTH / 2;
			mouseY-=WHEIGHT / 2;
			dest = round((atan2(mouseY,mouseX) / 3.1415)*180);
			dir = ((dir*(0.1/dt)) + dest) / (1+(0.1/dt));
			if(cos((dest/180)*3.1415) < -0.95) {
				dir = dest;
			}
		}
};

playerCls Player;
