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

#include "wp_boomerang_proj.h"
#include "player.h"
#include "gameManager.h"
#include "wp_boomerang.h"

#include <math.h>

wp_boomerang_proj::wp_boomerang_proj(spriteManager* newsm, Player* p, wp_boomerang* w) :
	spriteObject(newsm), weapon(w), owner(p), bReturning(false), returnx(0), returny(0)
{
	#ifdef __MDDEBUG
	className = "wp_boomerang_proj";
	macros::debugMessage(className, "constructor");
	#endif
	gameManager *gm = dynamic_cast<gameManager *>(sm);
	setPallete(gm->pickupSprite.palleteID);
	//GFX warning!
	giveSprite(gm->pickupSprite.spriteData, OBJ_SIZE_16X16, 8, 8, 60);
	setBounds(-2, 3, 3, -2);
	setAnim(20, 23, ANIMSPEED);
	setCollision(COL_SOLID);
	setLayer(2);
	#ifdef __WITHSOUND
	playSound(&gm->boomerangLoop, true);
	#endif
}

wp_boomerang_proj::~wp_boomerang_proj()
{
	if (weapon != NULL)
		weapon->boomerangReturned();
}

void wp_boomerang_proj::updateSprite()
{
	if (getx() < -16 || getx() > 271)
	{
		if (!bReturning || owner->isDisabled() || !owner->hasBoomerang() || weapon == NULL)	//out of screen and not returning
		{
			destroy();
			return;
		}
		vx = 0;
	} else if ((getx() <= 0) || (getx() >= 255))
		returnToPlayer();
	else if (y >= (s32)SCREENH+4096)	//fell out of bottom so make a splash, go low enough to hide death FX
	{
		#ifdef __WITHSOUND
		playSound(&owner->gm->smallSplash);
		#endif
		//GFX warning!
		u8 f[3] = {13, 14, 15};
		vector<u8> temp(f, f+3);
		sm->createSingleFireSprite(owner->gm->FXSprite.palleteID, owner->gm->FXSprite.spriteData, temp, TICKSPERFRAME*2, getx(), gameManager::MALLOWYPOS, OBJ_SIZE_32X32, 16, 16);
		destroy();
		return;
	}
	
	if (bReturning)
		returnTick();
	
	spriteObject::updateSprite();
}

void wp_boomerang_proj::returnTick()
{
	if (!owner->isDisabled() && owner->hasBoomerang() && weapon != NULL)
	{
		returnx = owner->getx();
		returny = owner->gety();
	} else {								//player has died, so just fly away inconspicuously
		returnx = (vx > 0 ? 306 : -50);		//basically move it off the screen is the point here
		returny = gety();
	}
	
	s32 myx = getx();
	s32 myy = gety();
	
	s32 dist = (s32)sqrt((myx-returnx)*(myx-returnx)+(myy-returny)*(myy-returny));
	
	setRawSpeed(getRawvx()-((myx-returnx)/(dist/RETURNSTR)), getRawvy()-((myy-returny)/(dist/RETURNSTR)));
}
