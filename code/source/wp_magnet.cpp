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

#include "wp_magnet.h"
#include "player.h"
#include "gameManager.h"
#include "pickup.h"
#include "pickupSpawner.h"
#include "weaponIncludes.h"

#include <math.h>

wp_magnet::~wp_magnet()
{
	if (isFiring() || wasFiring())
		pawn->updateAnimation();
}

void wp_magnet::weaponTick()
{	
	if ((!pawn->isOnGround() || pawn->isCrouched()) && isFiring())
		stopFiring();
	
	if (!isFiring() && wasFiring())
		pawn->updateAnimation();
		
	Weapon::weaponTick();
}

void wp_magnet::weaponFireAction()
{
	#ifdef __WITHSOUND
	if (!pawn->hasSoundLooping())
		pawn->playSound(&pawn->gm->magnetLoop, true);
	#endif
	
	if (!wasFiring())
		pawn->setAnim(64, 65, spriteObject::ANIMSPEED);
	pawn->setInputInterrupt(1);
	
	//use global pickups array for a much faster loop than if we went through all gameSprites
	for (unsigned int i=0; i < pawn->gm->pickups.size(); ++i)
	{
		Pickup *p = pawn->gm->pickups[i];
		if (p->getLifetime() == -1)			//disabled so dont touch it
			continue;

		s32 magnetx = pawn->getx();
		s32 magnety = pawn->getTop();
		s32 pickupx = p->getx();
		s32 pickupy = p->gety();
		
		s32 dist = (s32)sqrt((pickupx-magnetx)*(pickupx-magnetx)+(pickupy-magnety)*(pickupy-magnety));
		
		p->setRawSpeed(p->getRawvx()-((pickupx-magnetx)/(dist/PULLSTR)), p->getRawvy()-((pickupy-magnety)/(dist/PULLSTR)));
	}
	
	//now for player weapon grabbing
	Player *p;
	if (pawn == pawn->gm->player1)
		p = pawn->gm->player2;
	else
		p = pawn->gm->player1;
	if (p->pointCollision(p->getx(), pawn->gety()) && p->getWeapon() != NULL &&
		(pawn->getFlippedh() ? pawn->getx() > p->getx() : pawn->getx() < p->getx()))
		popOutPawnWeapon(p);
}

void wp_magnet::stopFiring()
{
	Weapon::stopFiring();
	#ifdef __WITHSOUND
	if (pawn->hasSoundLooping())
		pawn->stopSound();
	#endif
}

void wp_magnet::popOutPawnWeapon(Player *p)
{
	if (p->getWeapon() == NULL)
		return;
	u8 lifetime = PA_RandMinMax(pickupSpawner::PICKUPLIFEMIN, pickupSpawner::PICKUPLIFEMAX);
	s8 offset = 13;
	s16 speed = Player::WALKSPEED>>8;
	if (p->getx() > pawn->getx())
	{
		offset *= -1;
		speed *= -1;
	}
	
	Pickup *pk = new Pickup(pawn->gm, NULL, lifetime, p->getx()+offset, p->gety(), 
							p->getWeapon()->getType(), p->gm->getNextAvailablePickupGFXSet());

	if (pk != NULL)
	{
		pk->setSpeed(speed, 0);
	}
	p->clearWeapon();
}
