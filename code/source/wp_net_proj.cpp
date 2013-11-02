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

#include "wp_net_proj.h"
#include "player.h"
#include "floorTile.h"
#include "gameManager.h"

wp_net_proj::wp_net_proj(spriteManager* newsm, Player* p) : spriteObject(newsm), owner(p)
{
	#ifdef __MDDEBUG
	className = "wp_net_proj";
	macros::debugMessage(className, "constructor");
	#endif
	gameManager *gm = dynamic_cast<gameManager*>(sm);
	setPallete(gm->FXSprite.palleteID);
	//GFX warning!
	giveSprite(gm->FXSprite.spriteData, OBJ_SIZE_32X32, 16, 16, 60);
	setBounds(-8, 8, 6, -8);
	setFrame(32);
	setCollision(COL_SOLID);
	setLayer(2);
	owner->setIgnored(this);
}

wp_net_proj::~wp_net_proj()
{
	
}

void wp_net_proj::updateSprite()
{
	if (getx() < -16 || getx() > 271)	//out of the screen now, dont wanna see the death FX either
	{
		destroy();
		return;
	}
	spriteObject::updateSprite();
}
