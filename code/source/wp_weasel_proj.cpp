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

#include "wp_weasel_proj.h"
#include "player.h"
#include "floorTile.h"
#include "gameManager.h"
#include "macros.h"

const s8 wp_weasel_proj::standingBounds[4] = {-13, 2, 0, -7};
const s8 wp_weasel_proj::hangingBounds[4] = {0, 2, 13, -7};
const s8 wp_weasel_proj::fallingBounds[4] = {-7, 2, 7, -7};

wp_weasel_proj::wp_weasel_proj(spriteManager* newsm, Player* p) : 
	massObject(newsm), owner(p), idleTime(0), justStoppedIdling(false)
{
	#ifdef __MDDEBUG
	className = "wp_weasel_proj";
	macros::debugMessage(className, "constructor");
	#endif
	gameManager *gm = dynamic_cast<gameManager *>(sm);
	setPallete(gm->FXSprite.palleteID);
	//GFX warning!
	giveSprite(gm->FXSprite.spriteData, OBJ_SIZE_32X32, 16, 16, 60);
	setBounds(standingBounds[0], standingBounds[1], standingBounds[2], standingBounds[3]);
	setFrame(26);
	setFlipped(!owner->getFlippedh(), false);
	setIdlePeriod(INITIALDELAY);
	setCollision(COL_SOLID);
	setCheckCollision(true);
	setLayer(2);
	//setSoundChannel(SLOT_WEASEL);
	playRandomSnarl();
}

wp_weasel_proj::~wp_weasel_proj()
{
	
}

void wp_weasel_proj::updateSprite()
{
	if (getx() < -16 || getx() > 271)	//out of the screen now
	{
		destroy();
		return;
	} else if (y >= (s32)SCREENH+4096)	//fell out of bottom so make a splash, go low enough to hide death FX
	{
		#ifdef __WITHSOUND
		playSound(&owner->gm->bigSplash);
		#endif
		//GFX warning!
		u8 f[3] = {13, 14, 15};
		vector<u8> temp(f, f+3);
		sm->createSingleFireSprite(owner->gm->FXSprite.palleteID, owner->gm->FXSprite.spriteData, temp, TICKSPERFRAME*2, getx(), gameManager::MALLOWYPOS, OBJ_SIZE_32X32, 16, 16);
		destroy();
		return;
	}
	
	//bouncing off the walls
	if (getLeft() <= 0)
	{
		runRight();
	} else if (getRight() >= 255)
	{
		runLeft();
	}
	
	if (idleTime == 0)
	{
		if (PA_RandMax(30) > 29)
		{
			changeDirection();
			playRandomSnarl();
		}
		
		if (!isOnGround() && wasOnGround())
		{
			hangDown();
			return;
		}
		
		if (justStoppedIdling)
		{
			changeDirection();
			playWalking();
			justStoppedIdling = false;
		}
	}
	else {
		vx = 0;
		if (isOnGround())
			setFrame(frame);
		idleTime--;
		if (idleTime == 0)
			justStoppedIdling = true;
	}
	
	massObject::updateSprite();
}

void wp_weasel_proj::playWalking()
{
	u8 f[4] = {24, 25, 26, 25};
	vector<u8> temp(f, f+4);
	setArbitraryAnim(temp, true, WALKANIMSPEED);
}

void wp_weasel_proj::playFalling()
{
	u8 f[8] = {27, 28, 29, 30, 29, 28, 27, 26};
	vector<u8> temp(f, f+8);
	setArbitraryAnim(temp, true, 30/8);
}

void wp_weasel_proj::hangDown()
{
	vx = 0;
	vy = 0;
	setIdlePeriod(HANGDELAY);
	playFalling();
}

bool wp_weasel_proj::atEdge()
{
	bool aboutToFall = true;
	for (vector<spriteObject*>::iterator it = sm->gameSprites.begin(); it != sm->gameSprites.end(); ++it)
	{
		if (*it == this || !(*it)->isBaseable())
			continue;
		if ((*it)->pointCollision((flippedh ? getRight() : getLeft()), getBottom()+2))
		{
			aboutToFall = false;
			break;
		}
	}
	return aboutToFall;
}

void wp_weasel_proj::playRandomSnarl()
{
	#ifdef __WITHSOUND
	gameManager *gm = dynamic_cast<gameManager*>(sm);
	playSound(&gm->weaselSounds[PA_RandMax(6)]);
	#endif
}
