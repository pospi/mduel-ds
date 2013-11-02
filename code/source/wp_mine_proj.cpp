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

#include "wp_mine_proj.h"
#include "player.h"
#include "spriteManager.h"
#include "gameManager.h"

wp_mine_proj::wp_mine_proj(gameManager* newsm, Player* own) : spriteObject(newsm), owner(own)
{
	#ifdef __MDDEBUG
	className = "wp_mine_proj";
	macros::debugMessage(className, "constructor");
	#endif
	//this one doesnt need any graphics yey
	setCollision(COL_SOLID);
	setLayer(1);
	setBounds(-1, 1, 1, -1);	//teenie weenie
	#ifdef __WITHSOUND
	playSound(&dynamic_cast<gameManager *>(sm)->mineDrop);
	#endif
}

wp_mine_proj::~wp_mine_proj()
{
	gameManager *gm = dynamic_cast<gameManager *>(sm);
	
	if (gm->isResetting())
		return;
	
	u8 f[3] = {10, 11, 12};
	vector<u8> temp(f, f+3);
	gm->createSingleFireSprite(gm->FXSprite.palleteID, (owner == gm->player1 ? gm->spawnGFX[0] : gm->spawnGFX[1]), temp, TICKSPERFRAME*2, getx(), gety()-16, OBJ_SIZE_32X32, 16, 16);
	#ifdef __WITHSOUND
	playSound(&gm->mineExp);
	#endif
}
