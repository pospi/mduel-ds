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

#ifndef _WEAPON_H
#define _WEAPON_H

#include <PA9.h>
#include <dswifi9.h>
#include <vector>
#include <string>
	using namespace std;
#include "macros.h"
#include "pickup.h"

//forward declaration
class Player;

/**
 * An abstract class to be extended to handle any sort of inventory logic a Player requires when holding
 * a specific Pickup.
 * 
 * Weapon%s are not sprite-driven and have no visual display. They perform high-level game logic and
 * respond to a variety of events from the player carrying them.
 * 
 * A Weapon operates either in single-fire or held-fire mode. Single-fire mode refers to one 'shot'
 * per keypress, whilst held-fire refers to a constant effect while the fire button is held down.
 * 
 * For single-fire Weapon%s, weaponFireAction() is executed when the fire button is pressed.
 * For held-fire weapons, weaponFireAction() is executed every frame until the button is released.
 * In both cases, stopFiring() is called to stop the weapon from firing.
 * weaponFireAction() automaticlly decrements ammo and handles object destruction when it runs out.
 * An ammo of -1 means infinite ammo.
 * 
 * @author pospi
 */
class Weapon
{
	public:
		Weapon(Player* p);
		virtual ~Weapon();
		
		virtual bool fire();
		virtual void stopFiring();
		virtual void weaponTick();
		
		/**
		 * check if the Weapon is currently firing
		 * @return true if firing, false otherwise
		 */
		bool isFiring() {return bFiring;}
		/**
		 * check if the Weapon was firing in the previous tick (useful for animation notifications etc)
		 * @return true if the Weapon was firing the frame before this one, false otherwise
		 */
		bool wasFiring() {return bWasFiring;}
		
		/**
		 * check if the Weapon can be fired whilst the player is in the air
		 * @return true if the Weapon is able to be fired in the air, false otherwise
		 */
		bool midairWeapon() {return canFireInAir;}
		
		/**
		 * find out how much ammo this Weapon has left
		 * @return the number of shots remaining, or -1 if the Weapon has unlimited ammo
		 */ 
		s8 getAmmo() {return ammo;}
		
		/**
		 * Determine the type of Pickup that this Weapon corresponds to
		 * @return the Pickup::pickupType of this Weapon
		 */
		Pickup::pickupType getType() {return myType;}
	
	protected:
		/**
		 * Actions to perform when the Weapon is fired. This may be only executed once for single-fire Weapon%s
		 * or every frame for held-fire Weapon%s.
		 */
		virtual void weaponFireAction() = 0;

		void handleMovementAndAmmo();
		
		///the Player who owns this Weapon and is currently holding it
		Player* pawn;
		
		///the type of Pickup that this Weapon corresponds to
		Pickup::pickupType myType;
		
		///specifies whether or not this Weapon can be fired while the player is standing upright
		bool canFireStanding;
		///specifies whether or not this Weapon can be fired while the player is in midair
		bool canFireInAir;
		///specifies whether or not this Weapon can be fired while the player is crouching
		bool canFireCrouching;
		///specifies whether or not the Player can move whilst firing this Weapon
		bool canMoveWhileFiring;
		///the amount of shots remaining in this Weapon. When ammo is depleted, the Weapon is deleted.
		s8 ammo;
		
		///specifies if this Weapon operates in 'held-fire' mode or 'single-fire' mode
		bool bHeldFire;
		///the animation to play once for single-fire Weapon%s when fired
		vector<u8> firingAnim;
		///the speed to play Weapon::firingAnim at
		u8 fireAnimSpeed;
		
		///true while the Weapon is firing
		bool bFiring;
		///true if the Weapon was firing in the previous tick
		bool bWasFiring;
		
		#ifdef __MDDEBUG
		string className;
		#endif
};

#endif
