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

#include "wp_weasel.h"
#include "wp_weasel_proj.h"
#include "player.h"
#include "gameManager.h"

wp_weasel::wp_weasel(Player *p) : Weapon(p)
{
	#ifdef __MDDEBUG
	className = "wp_weasel";
	macros::debugMessage(className, "constructor");
	#endif
	canFireStanding = true;
	canFireInAir = false;
	canFireCrouching = false;
	canMoveWhileFiring = false;
	ammo = 1;
	bHeldFire = false;
	
	u8 f[2] = {6, 35};
	firingAnim = vector<u8>(f, f+2);
	myType = Pickup::PT_WEASEL;
}

wp_weasel::~wp_weasel()
{
	if (isFiring() || wasFiring())
		pawn->updateAnimation();
}

void wp_weasel::weaponFireAction()
{
	s16 xpos;
	s16 ypos = pawn->getBottom()-17;
	
	xpos = (pawn->getFlippedh() ? pawn->getLeft()-10 : pawn->getRight()+10);
	
	wp_weasel_proj *m = new wp_weasel_proj(pawn->gm, pawn);
	m->setPos(xpos, ypos);
}
