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

#include "wp_gren_proj.h"
#include "player.h"
#include "floorTile.h"
#include "gameManager.h"

wp_gren_proj::wp_gren_proj(spriteManager* newsm, Player* p) : massObject(newsm), owner(p)
{
	#ifdef __MDDEBUG
	className = "wp_gren_proj";
	macros::debugMessage(className, "constructor");
	#endif
	gameManager* gm = dynamic_cast<gameManager*>(sm);
	setPallete(gm->pickupSprite.palleteID);
	//GFX warning!
	giveSprite(gm->pickupSprite.spriteData, OBJ_SIZE_16X16, 8, 8, 60);
	setBounds(2, 2, 5, -1);
	setAnim(26, 27, ANIMSPEED);
	setCollision(COL_SOLID);
	setCheckCollision(true);
	setLayer(2);
}

wp_gren_proj::~wp_gren_proj()
{
	gameManager *gm = dynamic_cast<gameManager *>(sm);
	
	if (gm->isResetting())
		return;
	
	u8 dieAnim[4] = {0, 1, 2, 3};
	vector<u8> temp(dieAnim, dieAnim+4);
	#ifdef __WITHSOUND
	playSound(&owner->gm->platformExplode);
	#endif
	owner->gm->createSingleFireSprite(owner->gm->FXSprite.palleteID, (owner == owner->gm->player1 ? owner->gm->spawnGFX[2] : owner->gm->spawnGFX[3]), temp, TICKSPERFRAME*2, getx(), getBottom(), OBJ_SIZE_32X32, 16, 16);
}

void wp_gren_proj::updateSprite()
{
	if (isOnGround())
	{
		setBounds(2, 8, 5, -4);	//bigger collision to take out neighbouring platforms
		for (vector<spriteObject*>::iterator it = sm->gameSprites.begin(); it != sm->gameSprites.end(); ++it)
		{
			if (dynamic_cast<floorTile*>(*it) != NULL &&
				isColliding(*it))
			{
				//check if the destroyed floor was under a player
				gameManager *gm = dynamic_cast<gameManager *>(sm);
				if (gm->player1->isStandingOn(*it))
					gm->player1->groundDeleted(Player::CS_GRENFALL);
				if (gm->player2->isStandingOn(*it))
					gm->player2->groundDeleted(Player::CS_GRENFALL);
				
				(*it)->destroy();
			}
		}
		destroy();
		return;
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
