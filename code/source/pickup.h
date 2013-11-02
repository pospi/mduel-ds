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

#ifndef _PICKUP_H
#define _PICKUP_H

#include <PA9.h>
#include <dswifi9.h>
#include <vector>
	using std::vector;
#include "macros.h"
#include "spriteObject.h"
#include "gameManager.h"

//forward declaration
class Player;

/**
 * Pickup%s are a spriteObject%s that collide with Player%s, performing actions on them. They move
 * randomly around the level and bounce off 
 * the edges of the screen. They have a finite lifetime and handle all their own creation & destruction
 * effect sprites internally.
 * 
 * @author pospi
 */
class Pickup : public spriteObject
{
	public:
		Pickup(spriteManager *newgm, pickupSpawner *mySpawner, u8 lifeSeconds, s16 nx, s16 ny, u8 type, gameManager::pickupGraphics* graphicSet);
		virtual ~Pickup();
		
		virtual void updateSprite();
		virtual void collidingWith(spriteObject *other);
		virtual bool isColliding(spriteObject *other, bool checkReverse = true) const;
		
		/**
		 * The type identifier for a Pickup. Mostly used to specify which Weapon to give a Player
		 * when they touch that Pickup, or what behaviour to run in such a case.
		 * 
		 * The Pickup icon given is set to the frame given by Pickup::pickupType%+3
		 */
		enum pickupType {
			PT_SKULL, PT_1000V, PT_INVIS, PT_MINE, PT_GUN, PT_TNT,
			PT_BOOT, PT_GRENADE, PT_PUCK, PT_CHUT, PT_HOOK, PT_WARP,
			PT_MAGNET, PT_NET, PT_SHIELD, PT_DUNCE, PT_WEASEL, PT_BOOMERANG
		};
		void setType(u8 newType);
		///return the type of this Pickup from Pickup::pickupType
		pickupType getType() const {return myType;}
		///set the spawner that fired this Pickup so that it may be told to spawn another Pickup when this one dies
		void setSpawner(pickupSpawner *p) { spawner = p; }
		///used for the TNT Pickup to set which animation and sound to play when destroyed
		bool shouldExplode() { return exploding; }
		
		///the total number of possible Pickup%s in the game
		static const u8 NUMPICKUPVARIATIONS = 18;

		void playerTouchAction(Player* other);
	protected:
		///another reference to this Pickup%s spriteManager so that game-specific function calls can be done more easily
		gameManager* gm;
	private:
		///the pickupSpawner that was responsible for firing this Pickup
		pickupSpawner* spawner;
		///if true, play explosion on death instead of 'pop' animation
		bool exploding;
		///the type of this Pickup from Pickup::pickupType
		pickupType myType;
		///the graphics set for this pickup to use for its related effects. If a free slot from the gameManager cannot be found, we cope by spawning with non-loaded GFX
		gameManager::pickupGraphics* myGFXset;
		
		void makeIcon(u8 frame);
		///the spriteObject that this Pickup is using for its distinguishing icon
		spriteObject* myIcon;
		
		///minimum x screen position (in pixels) that the Pickup will reflect off if it moves outside of
		static const u16 MINX = 8;
		///minimum y screen position (in pixels) that the Pickup will reflect off if it moves outside of
		static const u16 MINY = 8;
		///maximum x screen position (in pixels) that the Pickup will reflect off if it moves outside of
		static const u16 MAXX = 248;
		///maximum y screen position (in pixels) that the Pickup will reflect off if it moves outside of
		static const u16 MAXY = 176;
};

#endif
