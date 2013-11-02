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

#ifndef _ROPE_H
#define _ROPE_H

#include <PA9.h>
#include <dswifi9.h>
#include <vector>
	using std::vector;
#include "macros.h"
#include "spriteObject.h"

/**
 * Rope%s are spriteObject%s that a Player can use to climb up and down between platforms.
 * 
 * To create a rope on the playing field, one Rope object is spawned and given a length. 
 * It will spawn as many child Rope sprites as it needs to meet this length. The parent
 * Rope is drawn as a small ball, whilst its children are drawn as a brown line, thus making
 * the rope appear affixed to something.
 * 
 * @author pospi
 */
class Rope : public spriteObject
{
	public:
		Rope(spriteManager *newsm, bool isCap = true);
		virtual ~Rope();
		
		virtual void setPos(s16 nx, s16 ny);
		bool setLength(u8 numRopes);
		
		///the number of pixels each side of a Rope that counts as touching if a Player%'s center is within them
		static const u8 ROPETOLERANCE = 5;
		
		Rope* getParent() const {return parentRope;}
		
		///child Rope sprites if this is a parent Rope
		vector <Rope*> childRopes;
		
		virtual void makeStatic();
		
		///id of GFX for Rope children to load in automatically
		u16 childGFX;
		
	protected:
		///child Rope%s have the parent (non-collidable) one set to this
		Rope* parentRope;
};

#endif
