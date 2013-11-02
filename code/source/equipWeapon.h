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

#ifndef _EQUIPWEAPON_H
#define _EQUIPWEAPON_H

#include "weapon.h"

//forward declaration
class spriteObject;
class Player;

/**
 * A Weapon that performs an action on its owning Player when it is picked up and dropped.
 * 
 * This is usually used to spawn a spriteObject and attach it to the Player for the duration of
 * them holding the Weapon.
 *
 * @author pospi
 */
class equipWeapon : public Weapon
{
	public:
		equipWeapon(Player* p);
		virtual ~equipWeapon();

	protected:
		///actions to perform when the Weapon is given to a Player
		virtual void equipAction() = 0;
		///actions to perform when the Weapon is removed from a Player%'s inventory
		virtual void unEquipAction() = 0;
		///a spriteObject to spawn and attach to the Player who owns this Weapon, if necessary
		spriteObject* effect;
};

#endif
