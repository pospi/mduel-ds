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

#ifndef _WPWEASELPROJ_H
#define _WPWEASELPROJ_H

#include <PA9.h>
#include "macros.h"
#include "massObject.h"

//forward declaration
class Player;

class wp_weasel_proj : public massObject
{
	public:
		wp_weasel_proj(spriteManager* newsm, Player* p);
		virtual ~wp_weasel_proj();
		
		virtual void updateSprite();
	
	private:
		Player* owner;
		
		static const s8 standingBounds[];
		static const s8 hangingBounds[];
		static const s8 fallingBounds[];
		
		static const u8 INITIALDELAY = 45;
		static const u16 WALKSPEED = 384;
		static const u8 WALKANIMSPEED = 6;		//like ticksperframe
		static const u8 HANGDELAY = 30;
		
		u16 idleTime;
		bool justStoppedIdling;	//animation helper
		
		void setIdlePeriod(u16 framesToWait) {idleTime = framesToWait;}
		void runLeft() {setFlipped(false, false); vx = WALKSPEED*-1;}
		void runRight() {setFlipped(true, false); vx = WALKSPEED;}
		void changeDirection()
		{
			if (!isOnGround())
				return;
			if (vx < 0)
				runRight();
			else if (vx > 0)
				runLeft();
			else if (vx == 0)
			{
				if (!flippedh)
					runLeft();
				else
					runRight();
			}
		}
		bool atEdge();	//detect if we are about to fall off a platform
		
		void playWalking();
		void playFalling();
		void hangDown();
		
		void playRandomSnarl();
};

#endif
