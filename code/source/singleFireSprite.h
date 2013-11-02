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

#ifndef _SINGLEFIRESPRITE_H
#define _SINGLEFIRESPRITE_H

#include "macros.h"
#include "spriteObject.h"

/**
 * A spriteObject that plays through an animation cycle once and then deletes itself.
 * 
 * singleFireSprite%s are designed to be low-maintenance effects that you can simply assign a graphic
 * and animation frames to and leave the rest up to them. Once the animation is completed, they destroy
 * themselves and the spriteManager handling them cleans up the rest.
 * 
 * @author pospi
 */
class singleFireSprite : public spriteObject
{
	public:
		singleFireSprite(spriteManager* newsm, u8 palID, const unsigned char* spriteData, vector<u8> frames, u8 ticksPerFrame, u8 sizex, u8 sizey, u8 cx = 16, u8 cy = 16, s16 nx = OFFX, s16 ny = OFFY, u8 minIndex = 0, u8 colorMode = 1);
		singleFireSprite(spriteManager* newsm, u8 palID, u16 gfxID, vector<u8> frames, u8 ticksPerFrame, u8 sizex, u8 sizey, u8 cx = 16, u8 cy = 16, s16 nx = OFFX, s16 ny = OFFY, u8 minIndex = 0, u8 colorMode = 1);
		
		virtual void updateSprite();
		virtual ~singleFireSprite();
		
	private:
		
};

#endif
