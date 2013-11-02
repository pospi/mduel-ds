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

#include "wp_gun.h"
#include "player.h"
#include "gameManager.h"
#include "wp_shield.h"

wp_gun::wp_gun(Player* p) : Weapon(p)
{
	#ifdef __MDDEBUG
	className = "wp_gun";
	macros::debugMessage(className, "constructor");
	#endif
	canFireStanding = true;
	canFireInAir = false;
	canFireCrouching = false;
	canMoveWhileFiring = false;
	ammo = 5;
	bHeldFire = false;
	
	u8 f[4] = {30, 31, 31, 30};
	firingAnim = vector<u8>(f, f+4);
	myType = Pickup::PT_GUN;
}

wp_gun::~wp_gun()
{
}

void wp_gun::weaponFireAction()
{
	#ifdef __WITHSOUND
	pawn->playSound(&pawn->gm->gunFire);
	#endif
	
	Player *opponent = (pawn == pawn->gm->player1 ? pawn->gm->player2 : pawn->gm->player1);
		
	if ((pawn->getFlippedh() && pawn->getx() > opponent->getx()) ||
		(!pawn->getFlippedh() && pawn->getx() < opponent->getx()))
	{
		if (dynamic_cast<wp_shield*>(opponent->getWeapon()) != NULL && 
			((pawn->getx() < opponent->getx() && opponent->getFlippedh()) || 
			(pawn->getx() > opponent->getx() && !opponent->getFlippedh())))
		{
			#ifdef __WITHSOUND
			opponent->playSound(&pawn->gm->bulletReflect);
			#endif
		} else
		{
			s16 xpos = opponent->getx();
			s16 ypos = pawn->getBottom() + GUNHEIGHT;
			
			if (opponent->pointCollision(xpos, ypos))
			{
				//call animation changes first (same as in weapon class), then return to stop dangling pointers crashing stuff
				if (!canMoveWhileFiring)
					pawn->setvx(0);
				if (firingAnim.size() > 0 && !bHeldFire)
				{
					pawn->setArbitraryAnim(firingAnim, false, fireAnimSpeed);
					pawn->setInputInterrupt(fireAnimSpeed * firingAnim.size());
				}
				pawn->gm->playerDisintegrated(opponent);
				return;	//VERY IMPORTANT !
			}
		}
	}
}
