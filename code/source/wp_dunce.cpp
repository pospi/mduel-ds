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

#include "wp_dunce.h"
#include "player.h"
#include "gameManager.h"

wp_dunce::~wp_dunce()
{
	unEquipAction();
}
	
void wp_dunce::equipAction()
{
	effect = new spriteObject(pawn->gm);
	effect->setPallete(pawn->gm->dunceHatSprite.palleteID);
	effect->giveGFX((pawn == pawn->gm->player1 ? pawn->gm->dunceGFX[0] : pawn->gm->dunceGFX[1]), OBJ_SIZE_32X32, 16, 16, 60, 0, 0);
	effect->setFrame(pawn->getFrame());
	pawn->addChild(effect);
	pawn->setAttachment(effect);
	#ifdef __WITHSOUND
	pawn->playSound(&pawn->gm->duncePickup);
	#endif
}

void wp_dunce::unEquipAction()
{
	if (effect != NULL)
	{
		pawn->removeChild(/*effect*/);
		if (pawn->getAttachment() == effect)	//this prevents an overwrite when we get 2 dunces in a row
			pawn->setAttachment(NULL);
	}
}
