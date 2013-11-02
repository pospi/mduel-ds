/*
 * Marshmallow Duel DS v2
 * Copyright � 2007 Sam Pospischil http://pospi.spadgos.com
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

#include "wp_1000V.h"
#include "player.h"
#include "gameManager.h"

wp_1000V::~wp_1000V()
{
	#ifdef __MDDEBUG
	className = "wp_1000V";
	macros::debugMessage(className, "constructor");
	#endif
	pawn->updatePallete(playerNormPallete, OBJ_SIZE_32X32);
}

void wp_1000V::weaponTick()
{
	skipTick = !skipTick;

	if (skipTick)
		return;
	
	//lightningy flicker of palletes
	if (pawn->getPalleteID() == playerNormPallete)
		pawn->updatePallete(pawn->gm->player1000VPallete, OBJ_SIZE_32X32);
	else
		pawn->updatePallete(playerNormPallete, OBJ_SIZE_32X32);
}
