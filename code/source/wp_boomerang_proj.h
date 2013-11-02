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

#ifndef _WPBOOMPROJ_H
#define _WPBOOMPROJ_H

#include <PA9.h>
#include "macros.h"
#include "massObject.h"

//forward declaration
class Player;
class wp_boomerang;

class wp_boomerang_proj : public spriteObject
{
	public:
		wp_boomerang_proj(spriteManager *newsm, Player *p, wp_boomerang *w);
		virtual ~wp_boomerang_proj();
		
		virtual void updateSprite();
		
		void returnToPlayer() {/*vx = 0;*/ bReturning = true;}
	
		wp_boomerang* weapon;
		Player* owner;
	private:
		
		bool bReturning;
		s16 returnx, returny;
		
		static const u8 RETURNSTR = 14;
		
		void returnTick();	//moves the boomerang incrementally toward its owner
};

#endif
