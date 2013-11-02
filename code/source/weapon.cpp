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

#include "weapon.h"
#include "player.h"

/**
 * Create a Weapon and assign it to the Player who has it in their inventory. Player%s can only
 * carry a single weapon at a time.
 * 
 * @param	p	a reference to the Player who is carrying this Weapon
 */
Weapon::Weapon(Player *p) : 
	pawn(p), canFireStanding(false), canFireInAir(false), canFireCrouching(false), canMoveWhileFiring(false), 
	ammo(-1), bHeldFire(false), fireAnimSpeed(spriteObject::TICKSPERFRAME), bFiring(false), bWasFiring(false)
{
	#ifdef __MDDEBUG
	className = "weapon";
	macros::debugMessage(className, "constructor");
	#endif
}

Weapon::~Weapon()
{
	#ifdef __MDDEBUG
	macros::debugMessage(className, "destructor");
	#endif
	stopFiring();
	pawn->weaponDestroyed();
}

/**
 * Called by a Player to try and fire this Weapon. The Weapon checks if it can fire before performing
 * any actions.
 * 
 * @return true if the weapon was fired, false otherwise 
 */
bool Weapon::fire()
{
	if (pawn->isOnRope())
		return false;
	
	if (pawn->isOnGround() && pawn->isCrouched())
		bFiring = canFireCrouching;
	else if (pawn->isOnGround())
		bFiring = canFireStanding;
	else
		bFiring = canFireInAir;
	
	if (bFiring && !bHeldFire)
	{
		weaponFireAction();
		if (bFiring)	//this gives weaponFireAction in child classes a chance to override the animation playing
			handleMovementAndAmmo();
	}
	
	return bFiring;
}

/**
 * Called by a Player to stop this Weapon from firing, usually to stop a held-fire Weapon%'s weaponFireAction().
 */
void Weapon::stopFiring()
{
	#ifdef __MDDEBUG
	macros::debugMessage(className, "stopFiring");
	#endif
	bFiring = false;
}

/**
 * Executed every frame to fire the Weapon if it is a held-fire Weapon and firing
 */
void Weapon::weaponTick()
{
	if (bHeldFire && bFiring)
	{
		weaponFireAction();
		handleMovementAndAmmo();
	}
	bWasFiring = bFiring;
}

/**
 * Handle the low-level effects of firing a Weapon, such as halting Player movement when required,
 * playing fire animations and decrementing Weapon::ammo.
 */
void Weapon::handleMovementAndAmmo()
{
	if (!canMoveWhileFiring)
		pawn->setvx(0);
	
	if (firingAnim.size() > 0 && !bHeldFire)
	{
		pawn->setArbitraryAnim(firingAnim, false, fireAnimSpeed);
		pawn->setInputInterrupt(fireAnimSpeed * firingAnim.size());
	}
		
	if (ammo != -1)		//negative 1 means unlimited ammo
		ammo--;
	if (ammo == 0)
		delete this;
}
