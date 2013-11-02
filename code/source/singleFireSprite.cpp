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

#include "singleFireSprite.h"

/**
 * Create an 'effect' sprite that will play once by giving it raw sprite data.
 * 
 * @param	newsm		the spriteManager which should manage this spriteObject
 * @param	palID		ID of the pallete (preloaded by newsm) to use
 * @param	spriteData	the raw sprite data to use to draw this spriteObject
 * @param	frames		a vector of frame numbers, to be played in sequence
 * @param	ticksPerFrame	the animation speed of the effect
 * @param	sizex		memory constant to specify x size of the sprite
 * @param	sizey		memory constant to specify y size of the sprite
 * @param	cx			x sprite center relative to top left of the graphic
 * @param	cy			y sprite center relative to top left of the graphic 
 * @param	nx			x screen position (in pixels) to play the effect
 * @param	ny			y screen position (in pixels) to play the effect
 * @param	minIndex	minimum sprite index in the spriteManager OAM array, which can be used as a depth hierachy
 * @param	colorMode	colour mode, using PALib constants. 1 = 8bit.
 */
singleFireSprite::singleFireSprite(spriteManager* newsm, u8 palID, const unsigned char* spriteData, vector<u8> frames, u8 ticksPerFrame, u8 sizex, u8 sizey, u8 cx, u8 cy, s16 nx, s16 ny, u8 minIndex, u8 colorMode) : spriteObject(newsm)
{
	#ifdef __MDDEBUG
	className = "singleFireSprite";
	macros::debugMessage(className, "cs-giveSprite");
	#endif
	setPallete(palID);
	giveSprite(spriteData, sizex, sizey, cx, cy, minIndex, nx, ny, colorMode);
	setFrame(frames[0]);
	setLayer(0);
	setArbitraryAnim(frames, false, ticksPerFrame);
}

/**
 * Create an 'effect' sprite that will play once by giving it a preloaded GFX id.
 * 
 * @param	newsm		the spriteManager which should manage this spriteObject
 * @param	palID		ID of the pallete (preloaded by newsm) to use
 * @param	gfxID		ID of the GFX (preloaded by newsm) to use
 * @param	frames		a vector of frame numbers, to be played in sequence
 * @param	ticksPerFrame	the animation speed of the effect
 * @param	sizex		memory constant to specify x size of the sprite
 * @param	sizey		memory constant to specify y size of the sprite
 * @param	cx			x sprite center relative to top left of the graphic
 * @param	cy			y sprite center relative to top left of the graphic 
 * @param	nx			x screen position (in pixels) to play the effect
 * @param	ny			y screen position (in pixels) to play the effect
 * @param	minIndex	minimum sprite index in the spriteManager OAM array, which can be used as a depth hierachy
 * @param	colorMode	colour mode, using PALib constants. 1 = 8bit.
 */
singleFireSprite::singleFireSprite(spriteManager* newsm, u8 palID, u16 gfxID, vector<u8> frames, u8 ticksPerFrame, u8 sizex, u8 sizey, u8 cx, u8 cy, s16 nx, s16 ny, u8 minIndex, u8 colorMode) : spriteObject(newsm)
{
	#ifdef __MDDEBUG
	className = "singleFireSprite";
	macros::debugMessage(className, "cs-giveGFX");
	#endif
	setPallete(palID);
	giveGFX(gfxID, sizex, sizey, cx, cy, minIndex, nx, ny, colorMode);
	setFrame(frames[0]);
	setLayer(0);
	setArbitraryAnim(frames, false, ticksPerFrame);
}

/**
 * Update the animation frame for this spriteObject and flag for deletion if the animation has finished.
 */
void singleFireSprite::updateSprite()
{
	spriteObject::updateSprite();
	if (arbitraryAnim == 0)
	{
		destroy();
		return;
	}
}

singleFireSprite::~singleFireSprite()
{
	
}
