/*
 * Marshmallow Duel DS v2
 * Copyright © 2007 Sam Pospischil http://pospi.spadgos.com
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _MENUGBOTTOM_H
#define _MENUGBOTTOM_H

#include <PA9.h>
#include "spriteManager.h"
#include "macros.h"
#ifndef _ALLGRAPHICS_
#define _ALLGRAPHICS_
#include "../data/all_gfx_ed.c"
#endif

//forward declaration
class gameManager;
class menuBottom;

/**
 * The 'status' screen manager responsible for drawing scores and the Pickup generator.
 * 
 * This spriteManager is non-interactive and serves only as a 'status' screen of sorts. It draws
 * Player scores; the Pickup generator which acts as a forewarning to what will soon happen on the
 * gameManager%'s screen; and if compiled in debug mode, debug messages.
 * It is also responsible for handling game pausing, since the code to check a 'resume' key needs
 * to be in a separate main loop than that of the game itself.
 * 
 * The main focus of this screen is the Pickup generator, drawn as a metallic cauldron-looking-thing
 * with three tubes coming out of it. Pickups are spawned in the center of the cauldron and travel
 * down the tubes, which correspond to the three Pickup spawners on the top screen. Once a Pickup moves
 * off the screen, it will spawn on the top screen in the game proper. menuGBottom also handles msot of 
 * the logic for spawning Pickup%s on the gameManager screen.
 * 
 * @author pospi
 */
class menuGBottom : public spriteManager
{
	public:
		menuGBottom(u8 newScreen = 0);
		virtual ~menuGBottom();
		
		void loadBackground();
		
		virtual void gameTick();
		
		/**
		 * give this spriteManager control of its screen, and reset all Pickup spawning related variables
		 */
		virtual void activate()
		{
			for (u8 i=0; i<3; ++i)
				spawnerCounters[i] = -1;
			pickupsActive = 0;
			
			gameOver = false;
			gamePaused = false;
			continueMsg = NULL;
			quitMsg = NULL;
			spriteManager::activate();
			resetText();
			
			spawnerSprite.palleteID = -1;
			loadSpawnerSpriteSet(spawnerSprite.palleteData, spawnerSprite.spriteData);
			loadBackground();
			
			menuSprite.palleteID = -1;
			loadMenuSpriteSet(menuSprite.palleteData, menuSprite.spriteData);
			pickupSprite.palleteID = -1;
			loadPickupSpriteSet(pickupSprite.palleteData, pickupSprite.spriteData);
			
			for (u8 i=0; i<3; ++i)
				resetIdleTime(i);
		}
		virtual void deactivate() {spriteManager::deactivate(); gameOver = false; unloadBackground(); for (u8 i=0; i<3; ++i) spawnerCounters[i] = -1;}
			
		void loadMenuSpriteSet(const unsigned short *p, const unsigned char *s);
		void loadSpawnerSpriteSet(const unsigned short *p, const unsigned char *s);
		void loadPickupSpriteSet(const unsigned short *p, const unsigned char *s);
		
		void resetIdleTime(u8 spawnerNum);
		
		void spawnPickup(u8 i);
		
		///spriteManager which handles the top screen during gameplay
		gameManager* gameTop;
		///spriteManager which handles the top screen during frontend
		spriteManager* menuTop;
		///spriteManager which handles the bottom screen during frontend
		menuBottom* menuBase;
		///if true, we are in 'player wins' screen.
		bool gameOver;
		///if true, stop executing the main loop for the game screen
		bool gamePaused;
		
		///countdown timers for Pickup spawners. Set to -1 to disable that spawner.
		s16 spawnerCounters[3];
		///the number of Pickup%s that are in play
		u8 pickupsActive;
		
		spriteObject *continueMsg, *quitMsg, *spawnerSpriteL, *spawnerSpriteR;
		
		spriteSet menuSprite, spawnerSprite, pickupSprite;
		
		///The minimum OAM index for Pickup icons, so that they are neatly sandwiched between the Pickup generator graphics
		static const u8 _SPAWNERSPRITEMININDEX = 64;
};

#endif
