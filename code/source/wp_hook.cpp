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

#include "wp_hook.h"
#include "player.h"
#include "gameManager.h"
#include "floorTile.h"

wp_hook::~wp_hook()
{
	if (isFiring() || wasFiring())
		pawn->updateAnimation();
}

void wp_hook::weaponTick()
{
	Weapon::weaponTick();
	
	if (pawn->isOnRope() || pawn->isOnGround() && isFiring())
		stopFiring();
	
	bWasFiring = bFiring;
	
	if (!isFiring() && wasFiring())
		pawn->updateAnimation();
}

void wp_hook::weaponFireAction()
{
	if (floorGrabTimer > 0)
	{
		pawn->setFrame(38);
		floorGrabTimer--;
	} else
		pawn->setFrame(37);
	
	//rope grabbing first
	if (pawn->justTouchingRope())
	{
		pawn->climbRope(false);
		pawn->setvx(0);
	} else {
		s16 xpos = (pawn->getFlippedh() ? pawn->getLeft() : pawn->getRight());
		s16 ypos = pawn->gety();
		for (vector<spriteObject*>::iterator it = pawn->gm->gameSprites.begin(); it != pawn->gm->gameSprites.end(); ++it)
		{
			if (dynamic_cast<floorTile*>(*it) != NULL &&
				(*it)->pointCollision(xpos, ypos))
			{
				pawn->setvy((Player::JUMPIMPULSE>>8)*2/3);
				floorGrabTimer = spriteObject::TICKSPERFRAME*3;
				break;
			}
		}
	}
}

void wp_hook::stopFiring()
{
	Weapon::stopFiring();
	floorGrabTimer = 0;
}
