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

#ifndef _WP_DUNCE_H
#define _WP_DUNCE_H

#include "macros.h"
#include "equipWeapon.h"

//forward declaration
class spriteObject;
class Player;

class wp_dunce : public equipWeapon
{
	public:
		wp_dunce(Player* p) : equipWeapon(p)
		{
			#ifdef __MDDEBUG
			className = "wp_dunce";
			macros::debugMessage(className, "constructor");
			#endif
			canFireStanding = false;
			canFireInAir = false;
			canFireCrouching = false;
			ammo = -1;
			bHeldFire = false;
			myType = Pickup::PT_DUNCE;
			equipAction();
		}
		virtual ~wp_dunce();
	
	protected:
		virtual void equipAction();
		virtual void unEquipAction();

		virtual void weaponFireAction() {}	//bleh
};

#endif
