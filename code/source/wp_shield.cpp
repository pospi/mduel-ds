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

#include "wp_shield.h"
#include "player.h"
#include "gameManager.h"

wp_shield::~wp_shield()
{
	unEquipAction();
}

void wp_shield::weaponTick()
{	
	if (pawn->isOnRope())
		effect->setPos(256, 256);	//this will always be offscreen if the player is always onscreen
	else
		effect->setPos(6, -3);
		
	Weapon::weaponTick();
}
	
void wp_shield::equipAction()
{
	effect = new spriteObject(pawn->gm);
	effect->setPallete(pawn->gm->FXSprite.palleteID);
	//GFX warning!
	effect->giveSprite(pawn->gm->FXSprite.spriteData, OBJ_SIZE_32X32, 16, 16, 60, 6, -3);
	effect->setFrame(31);
	effect->setCollision(spriteObject::COL_SOLID);
	effect->setBounds(-4, 3, 11, -2);
	pawn->addChild(effect);
}

void wp_shield::unEquipAction()
{
	pawn->removeChild(/*effect*/);
}
