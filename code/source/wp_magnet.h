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

#ifndef _WP_MAGNET_H
#define _WP_MAGNET_H

#include "macros.h"
#include "weapon.h"

class wp_magnet : public Weapon
{
	public:
		wp_magnet(Player* p) : Weapon(p)
		{
			#ifdef __MDDEBUG
			className = "wp_magnet";
			macros::debugMessage(className, "constructor");
			#endif
			canFireStanding = true;
			canFireInAir = false;
			canFireCrouching = false;
			canMoveWhileFiring = false;
			ammo = -1;
			bHeldFire = true;
			myType = Pickup::PT_MAGNET;
		}
		virtual ~wp_magnet();
		
		virtual void weaponTick();
		
	protected:
		virtual void weaponFireAction();
		virtual void stopFiring();
		
		void popOutPawnWeapon(Player *p);
		
		static const u8 PULLSTR = 5;
};

#endif
