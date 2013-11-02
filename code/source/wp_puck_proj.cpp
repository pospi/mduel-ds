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

#include "wp_puck_proj.h"
#include "player.h"
#include "wp_mine_proj.h"
#include "floorTile.h"
#include "gameManager.h"
#include "wp_weasel_proj.h"

wp_puck_proj::wp_puck_proj(spriteManager* newsm, Player* p) : massObject(newsm), owner(p)
{
	#ifdef __MDDEBUG
	className = "wp_puck_proj";
	macros::debugMessage(className, "constructor");
	#endif
	gameManager *gm = dynamic_cast<gameManager*>(sm);
	setPallete(gm->pickupSprite.palleteID);
	//GFX warning!
	giveSprite(gm->pickupSprite.spriteData, OBJ_SIZE_16X16, 8, 8, 60);
	setBounds(6, 2, 7, -2);
	setAnim(24, 25, ANIMSPEED);
	setCollision(COL_SOLID);
	setCheckCollision(true);
	setLayer(2);
}

wp_puck_proj::~wp_puck_proj()
{
	gameManager *gm = dynamic_cast<gameManager *>(sm);
	
	if (gm->isResetting())
		return;
	
	u8 dieAnim[3] = {10, 11, 12};
	vector<u8> temp(dieAnim, dieAnim+3);
	#ifdef __WITHSOUND
	playSound(&owner->gm->mineExp);
	#endif
	owner->gm->createSingleFireSprite(owner->gm->FXSprite.palleteID, (owner == owner->gm->player1 ? owner->gm->spawnGFX[3] : owner->gm->spawnGFX[2]), temp, TICKSPERFRAME*2, getx(), getBottom()-16, OBJ_SIZE_32X32, 16, 16);
}

void wp_puck_proj::updateSprite()
{
	if (!isOnGround() && wasOnGround())
	{
		vy = -150;	//jump up to clear a small gap
	}
	if (getx() < -16 || getx() > 271)	//out of the screen now, dont wanna see the death FX either
	{
		destroy();
		return;
	} else if (y >= (s32)SCREENH+4096)	//fell out of bottom so make a splash, go low enough to hide death FX
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
	
	massObject::updateSprite();
}

void wp_puck_proj::collidingWith(spriteObject *other)
{
	if (dynamic_cast<Player*>(other) != NULL)
	{
		Player *o = dynamic_cast<Player*>(other);
		if (o->hasShield() && o->isFacing(this))		//shield reflects!
		{
			vx *= -1;
		} else {
			o->gm->playerPucked(o);
			destroy();
		}
	} else if (dynamic_cast<wp_mine_proj*>(other) != NULL)
	{
		other->destroy();
	} else if (dynamic_cast<wp_weasel_proj*>(other) != NULL)
	{
		other->destroy();
		destroy();
	}
	
	massObject::collidingWith(other);
}
