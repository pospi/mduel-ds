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

#include "spriteManager.h"
#include "spriteObject.h"
#include "singleFireSprite.h"

/**
 * Load some GFX data into memory for it to be later used by a spriteObject.
 * 
 * @param	newGFX	pointer to the GFX data to load
 * @param	sizex	memory constant to specify x size of the sprite
 * @param	sizey	memory constant to specify y size of the sprite
 * @param	colorMode	colour mode, using PALib constants. 1 = 8bit.
 * 
 * @return id of the loaded GFX
 */
u16 spriteManager::loadGFX(const unsigned char *newGFX, u8 sizex, u8 sizey, u8 colorMode)
{
	u16 temp = PA_CreateGfx(screen, static_cast<void *>(const_cast<unsigned char *>(newGFX)), sizex, sizey, colorMode);
	//GFXids.push_back(temp);
	return temp;
}

/**
 * Loads a sprite into OAM, using a previously created GFX and pallete.
 * 
 * @param	GFXid	id of the GFX previously loaded via spriteManager::loadGFX()
 * @param	sizex	memory constant to specify x size of the sprite
 * @param	sizey	memory constant to specify y size of the sprite
 * @param	palleteID	id of the pallete previously created via spriteManager::loadPallete()
 * @param	minIndex	minimum sprite index in OAM array, which can be used as a depth hierachy
 * @param	colorMode	colour mode, using PALib constants. 1 = 8bit.
 * @param	x	initial x position of created sprite
 * @param	y	initial y position of created sprite
 * 
 * @return OAM index of the newly created sprite, or -1 if out of OAM slots
 */
s8 spriteManager::loadSpriteFromGFX(u16 GFXid, u8 sizex, u8 sizey, u8 palleteID, u8 minIndex, u8 colorMode, s16 x, s16 y)
{
	s8 next = getNextSpriteID(minIndex);
	if (next != -1)
	{
		PA_CreateSpriteFromGfx(screen, next, GFXid, sizex, sizey, colorMode, palleteID, x, y);
		sprites[next] = 1;
	}
	return next;
}

/**
 * Update the graphics of a sprite without the need to delete, recreate or otherwise modify it.
 * 
 * @param	spriteID	the OAM index of the sprite to modify
 * @param	GFXid		id of the loaded GFX to make this sprite use
 */
void spriteManager::updateSpriteGFX(s8 spriteID, void* GFXid)
{
	PA_UpdateSpriteGfx(screen, spriteID, GFXid);
}

/**
 * Update the pallete of a sprite without the need to delete, recreate or otherwise modify it.
 * Note that the sprite's GFX is not deleted, so if changing the GFX as well the old one should be 
 * deleted first.
 * 
 * @param	spriteID	the OAM index of the sprite to modify
 * @param	gfxID	id of the loaded GFX to use for the sprite
 * @param	palleteID	id of the loaded pallete to use for the sprite
 * @param	sizex	memory constant to specify x size of the sprite
 * @param	sizey	memory constant to specify y size of the sprite
 * @param	colorMode	colour mode, using PALib constants. 1 = 8bit.
 * @param	x	new x position of modified sprite
 * @param	y	new y position of modified sprite
 */
void spriteManager::updateSpritePallete(u8 spriteID, u16 gfxID, u8 palleteID, u8 sizex, u8 sizey, u8 colorMode, u8 x, u8 y)
{
	PA_CreateSpriteFromGfx(screen, spriteID, gfxID, sizex, sizey, colorMode, palleteID, x, y);
}

/**
 * Set the global transparency level for the screen this spriteManager belongs to.
 * Note that this is due to DS hardware limitations, where only 1 transparency level may be set
 * for each screen.
 * 
 * @see spriteObject::setTransparency()
 * @param translevel	the transparency level to set. Levels range from 0 (invisible) to 15 (opaque)
 */
void spriteManager::setAlpha(u8 transLevel)
{
	u8 fxon;
	if (transLevel < 15)
		fxon = SFX_ALPHA;
	else
		fxon = SFX_NONE;
	PA_SetSFXAlpha(screen, transLevel, 15);
	PA_EnableSpecialFx(screen, fxon, 0, SFX_BG0 | SFX_BG1 | SFX_BG2 | SFX_BG3 | SFX_BD);
}

/**
 * Load a sprite into OAM by passing GFX directly to the sprite. This will implicitly create
 * a GFX as well, which is externally linked to the sprite and will delete itself when the sprite
 * is deleted.
 * 
 * @param	newSprite	pointer to the GFX data to load into the sprite
 * @param	sizex	memory constant to specify x size of the sprite
 * @param	sizey	memory constant to specify y size of the sprite
 * @param	minIndex	minimum sprite index in OAM array, which can be used as a depth hierachy
 * @param	palleteID	id of the loaded pallete to use for the sprite
 * @param	x	new x position of modified sprite
 * @param	y	new y position of modified sprite
 * @param	colorMode	colour mode, using PALib constants. 1 = 8bit.
 * 
 * @return	OAM index of the newly created sprite, or -1 if out of OAM slots
 */
s8 spriteManager::loadSprite(const unsigned char *newSprite, u8 sizex, u8 sizey, u8 minIndex, u8 palleteID, s16 x, s16 y, u8 colorMode)
{
	s8 next = getNextSpriteID(minIndex);
	if (next != -1)
	{
		PA_CreateSprite(screen, next, static_cast<void *>(const_cast<unsigned char *>(newSprite)), sizex, sizey, colorMode, palleteID, x, y);
		sprites[next] = 1;
	}
	return next;
}

/**
 * Replace the sprite's GFX at the specified OAM slot with new GFX.
 * 
 * @param	id	OAM id of the sprite to replace
 * @param	newSprite	new GFX data to give that sprite
 */
void spriteManager::replaceSprite(u8 id, void *newSprite)
{
	PA_UpdateSpriteGfx(screen, id, newSprite);
}

/**
 * Load a pallete into memory, to later be used by sprite GFX.
 * 
 * @param newpallete	new pallete data to load
 * 
 * @return	id of the newly created pallete, or -1 if no pallete slots were free.
 */
s8 spriteManager::loadPallete(void *newpallete)
{
	s8 next = getNextPalleteID();
	if (next != -1)
	{
		PA_LoadSpritePal(screen, next, newpallete);
		palletes[next] = 1;
	}
	return next;
}

/**
 * Find out the next available pallete id for a new pallete to be loaded into
 * 
 * @return	the next pallete ID available (between 0 and 15), or -1 if there were no free slots
 */
s8 spriteManager::getNextPalleteID()
{
	for(int i=0;i<16;++i)
	{
		if(palletes[i]==0)
			return i;
	}
	#ifdef __MDDEBUG
	macros::debugMessage(ERRTEXT, "out of palletes!", ERRORPOS);
	#endif
	return -1;	//out of palletes!
}

/**
 * Find out the next available OAM slot for a new sprite to be loaded into
 * 
 * @return	the next OAM slot available (between 0 and 127), or -1 if there were no free slots
 */
s8 spriteManager::getNextSpriteID(u8 minIndex)
{
	for(int i=minIndex;i<128;++i)
	{
		if(sprites[i]==0)
			return i;
	}
	#ifdef __MDDEBUG
	macros::debugMessage(ERRTEXT, "out of sprites!", ERRORPOS);
	#endif
	return -1;	//out of sprites!
}

/**
 * Check that a spriteSet has its pallete loaded into memory
 * 
 * @param	s	a pointer to the spriteSet to check up on
 * @return	true if the spriteSet%'s pallete was loaded, false otherwise
 */
bool spriteManager::spriteSetHasPallete(spriteSet *s)
{
	return (s->palleteID != -1);
}

/**
 * Load a spriteSet%'s pallete (if needed) so that the spriteSet can successfully be used 
 * to create sprites
 * 
 * @param	s	a pointer to the spriteSet to load palletes for
 */
void spriteManager::spriteSetLoadPallete(spriteSet *s)
{
	if (!spriteSetHasPallete(s))
		s->palleteID = loadPallete((void*)s->palleteData);
}

/**
 * Load raw pallete and sprite data into a spriteSet. The data passed in comes straight from 
 * the PAGfx converter. The spriteSet%'s pallete will automatically be loaded as well.
 * 
 * @param	target	a pointer to the spriteSet to fill
 * @param	p	a pointer to a raw array of pallete data
 * @param	s	a pointer to a raw array of sprite date
 */
void spriteManager::loadSpriteSet(spriteSet *target, const unsigned short *p, const unsigned char *s)
{
	target->palleteData = p;
	target->spriteData = s;
	spriteSetLoadPallete(target);
}

//=====================================================

/**
 * Create a 'single effect' sprite, which will play once, then delete itself.
 * 
 * @see singleFireSprite
 * 
 * @param	palID	id of the pallete to give the effect
 * @param	gfx		GFX data to give the effect
 * @param	frames	a vector of frames that the effect should play through before dieing
 * @param	ticksPerFrame	animation speed for the effect
 * @param	nx	x position for the effect to play at
 * @param	ny	y position for the effect to play at
 * @param	objSize1	memory constant to specify x size of the sprite
 * @param	objSize2	memory constant to specify y size of the sprite
 * @param	cx	x center offset (from the top left corner) for the sprite
 * @param	cy	y center offset (from the top left corner) for the sprite
 * @param	minIndex	minimum sprite index in OAM array, which can be used as a depth hierachy
 * @param	colorMode	colour mode, using PALib constants. 1 = 8bit.
 */
void spriteManager::createSingleFireSprite(u8 palID, const unsigned char *gfx, vector<u8> frames, u8 ticksPerFrame, s16 nx, s16 ny, u8 objSize1, u8 objSize2, u8 cx, u8 cy, u8 minIndex, u8 colorMode)
{
	if (!totalReset)
	{
		frames.push_back(frames[frames.size()-1]);	//this makes the last frame actually show... hax.
		new singleFireSprite(this, palID, gfx, frames, ticksPerFrame, objSize1, objSize2, cx, cy, nx, ny, minIndex, colorMode);
	}
}

/**
 * Another utility function to create 'single effect' sprites, but for effects with preloaded GFX.
 * 
 * @see spriteManager::createSingleFireSprite() 
 */
void spriteManager::createSingleFireSprite(u8 palID, u16 gfxID, vector<u8> frames, u8 ticksPerFrame, s16 nx, s16 ny, u8 objSize1, u8 objSize2, u8 cx, u8 cy, u8 minIndex, u8 colorMode)
{
	if (!totalReset)
	{
		frames.push_back(frames[frames.size()-1]);	//this makes the last frame actually show... hax.
		new singleFireSprite(this, palID, gfxID, frames, ticksPerFrame, objSize1, objSize2, cx, cy, nx, ny, minIndex, colorMode);
	}
}

/**
 * Clean up all spriteObject%'s and reset the screen along with all video memory for that screen.
 * 
 * @param	keepSound	if true, non-looping sounds will continue playing until they expire.
 */
void spriteManager::clearOutSprites(bool keepSound)
{
	totalReset = true;
	#ifdef __MDDEBUG
	char buffer[255];
	sprintf(buffer, "%d: CLEARING ALL SPRITES", screen);
	macros::debugMessage("spriteManager", buffer);
	#endif
	for (vector<spriteObject*>::iterator it = gameSprites.begin(); it != gameSprites.end(); ++it)
	{
		delete *it;
	}
	gameSprites.clear();
	totalReset = false;
	
	//reset all sprite indexes
	for (u8 i = 0; i < 128; ++i)
		sprites[i] = 0;
	//clear out all palletes
	for (u8 i = 0; i < 16; ++i)
		palletes[i] = 0;
	
	#ifdef __WITHSOUND
	if (!keepSound)
	{
		for (u8 i = 0; i < 8; ++i)		//stop all sounds as well
			stopSound(i);
	}
	#endif
	setAlpha(15);
	PA_ResetSpriteSysScreen(screen);
}

#ifdef __WITHSOUND
/**
 * Play a sound effect. There are 8 sound channels for use. Once a sound has completed, it will automatically
 * be removed from its sound channel and free it up for other sounds to use.
 * 
 * @param	data	the sound data to play
 * @param	sizePointer	length of the sound
 * @param	repeat	if true, the sound will loop until the spriteObject responsible for playing it is destroyed
 * @param	vol	volume to play the sound at
 * 
 * @return the channel the sound has begun playing in
 */
s8 spriteManager::playSound(const u8 *data, const u32* sizePointer, bool repeat, u8 vol)
{
	s8 channel = PA_GetFreeSoundChannel();
	u32 size = (u32)sizePointer;
	if (channel != -1)
	{
		PA_PlaySoundEx2(channel, (void*)data, size, vol, 11025, 0, repeat, 0);
		soundLoopings[channel] = repeat;
	}
	return channel;
}

/**
 * Stop playing a sound effect. Required to halt looping sounds.
 * 
 * @param	channel	channel of the sound to halt prematurely.
 */
void spriteManager::stopSound(s8 channel)
{
	if (channel <= -1 || channel > 7)
		return;
	PA_StopSound(channel);
	soundLoopings[channel] = false;
}

/**
 * Checks whether the sound playing in the specified channel is looping or not
 * 
 * @param	channel	the sound channel to check
 * @return	true if that sound is looping, false otherwise.
 */
bool spriteManager::soundLooping(s8 channel)
{
	if (channel <= -1 || channel > 7) return false;
	return soundLoopings[channel];
}
#endif

/**
 * The game's 'tick' function, run once per frame to update all spriteObject%s, sounds etc.
 * This function should be extended in child spriteManager classes to implement higher level game
 * logic, menu navigation etc.
 */
void spriteManager::gameTick()
{
	//update screen
	vector<spriteObject*>::iterator it = gameSprites.begin();
	
	while ( it != gameSprites.end() )
	{
		if ((*it)->markedForDeletion())
		{
			#ifdef __MDDEBUG
			char buffer[255];
			sprintf(buffer, "%d: DELETING SPRITEOBJECT #%d", screen, it-gameSprites.begin());
			macros::debugMessage("spriteManager", buffer);
			#endif
			delete *it;
			it = gameSprites.erase(it);
		} else {
			(*it)->updateSprite();
			++it;
		}
	}
}

/**
 * Reset the background system to its original state (all backgrounds off).
 * This function does not destroy any background data that may be loaded.
 */
void spriteManager::resetBackground()
{
	PA_ResetBg(screen);
	PA_SetVideoMode(screen, 0);
}

/**
 * Reset the background system to its original state and delete any background images that may
 * be in memory.
 */
void spriteManager::unloadBackground()
{
	PA_DeleteBg(screen, 3);
	PA_SetVideoMode(screen, 0);
}

/**
 * Reset the text output system for this screen.
 * Note that the text system acts through a background layer, so the text layer (layer 2) must
 * not be used for other purposes while in use.
 */
void spriteManager::resetText()
{
	PA_ClearTextBg(screen);
	PA_InitCustomText(screen, 2, Font);
	PA_SetTextCol(screen, 20, 0, 31);
}
