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

//resource management and allocation for sprites

#ifndef _SPRITEMANAGER_H
#define _SPRITEMANAGER_H

#include <PA9.h>
#include <vector>
#include <string>
	using std::vector;
	using std::string;
#include "macros.h"
#ifndef _ALLGRAPHICS_
#define _ALLGRAPHICS_
#include "../data/all_gfx_ed.c"
#endif

//forward declaration
class spriteObject;

/**
 * The spriteManger is responsible for managing low-level memory allocation for the needs of spriteObject%s.
 * This includes sprite graphics, palletes and of course the sprites themselves. There is always 1 spriteManager
 * active for each screen at a time, as the different screens on a DS occupy entirely separate memory spaces.
 * 
 * There are 4 main areas managed here which it is important to differentiate between:
 * - Sprites - locations in the DS' Object Address Memory which hold low-level position, rotation etc information
 * - GFX or sprite graphics - the actual graphic data that a sprite uses to draw itself
 * - Palletes - give colour meaning to the values stored in sprite graphics
 * - Active spriteObject%s, which each require a sprite, pallete and GFX to draw themselves as a whole
 * 
 * Sprites are stored in an 128 element array (spriteManager::sprites) - this is referred to as Object Address Memory (OAM). Each
 * element in this array is a boolean toggle to show whether the sprite at that index is in use. Once a
 * sprite is no longer needed, its position is set back to 0 so that future sprites can reuse it. Sprites
 * reference GFX in order to draw themselves.
 * 
 * GFX are allowed rather more memory space (1024 slots iirc) and manage themselves rather more effectively,
 * so such an array is not necessary for them. A sprite must be assigned a GFX in order for it to be visible.
 * 
 * Palletes are stored similarly to sprites, in a 16 element array of toggles (spriteManager::palletes). All palletes are generally
 * loaded at the start of a screen refresh so that they are available on demand.
 * 
 * spriteObject%s are the sum of these three parts, and are stored in a dynamic array (spriteManager::gameSprites). 
 * Each references a sprite ID in OAM, which it uses to
 * manage its position variables. When a spriteObject is deleted, the sprite is freed up and the GFX the
 * spriteObject was using is destroyed, but only if it is not being shared by other spriteObject%s.
 * spriteObject%s also require a pallete in order to display their GFX.
 * 
 * spriteManager%s also handle the use of background layers. The DS supports 4 sprite layers, any of which
 * can be set as background layers to display tiled images instead of sprites.
 * 
 * A spriteManager is intended to stay in memory for the entire execution of the game. The methods
 * spriteManager::activate() and spriteManager::deactivate() are used to change which spriteManager is
 * active for its screen.
 * 
 * @author pospi
 */
class spriteManager {
	public:
		/**
		 * Create a new spriteManager instance and assign it a screen to manage.
		 * 
		 * @param 	newscreen	the screen this spriteManager should handle. 1 is the top screen, 0
		 * 						is the bottom one.
		 */
		spriteManager(u8 newscreen = 0) : bActive(false), screen(newscreen), totalReset(false)
		{
			for (u8 i=0; i<128; ++i)
				sprites[i] = 0;
			for (u8 i=0; i<16; ++i)
				palletes[i] = 0;
			#ifdef __WITHSOUND
			for (u8 i=0; i<8; ++i)
				soundLoopings[i] = false;
			#endif
		}
		/**
		 * Upon destroying this spriteManager, deactivate it first. This will likely never be called
		 * as spriteManager objects should be used in global program scope.
		 */
		virtual ~spriteManager() {deactivate();}
		
		/**
		 * Set this spriteManager as the active manager for the screen it is assigned to.
		 * spriteManager::gameTick() is only executed for the active spriteManager.
		 * NEVER set two different spriteManager%s to be active on the same screen at the same time.
		 */
		virtual void activate() {bActive = true; clearOutSprites();}
		/**
		 * Put this spriteManager into a hibernation state so that tick events are no longer executed for it.
		 */
		virtual void deactivate() {bActive = false; clearOutSprites();}
		///specifies whether or not this spriteManager is in control of its screen
		bool bActive;
		/**
		 * Detect whether or not this spriteManager should be currently executing tick events and controlling
		 * its screen.
		 * @return true if the spriteManager is active, false otherwise
		 */
		virtual bool isActive() {return bActive;}
		
		virtual void gameTick();
		
		//background and text resetting
		void resetBackground();		//reset only
		void unloadBackground();	//delete properly
		void resetText();			//clear text
		
		/**
		 * A set of raw data that spriteObject%s may use to easily load themselves.
		 * Contains both pallete and GFX data. Note that a spriteSet must have its pallete loaded
		 * into memory before it can be useful, this is done via spriteManager::spriteSetLoadPallete()
		 */
		struct spriteSet
		{
			s8 palleteID;
			const unsigned short* palleteData;
			const unsigned char* spriteData;
		};
		
		/**
		 * A set of raw data that tiled image backgrounds may use to be more easily loaded.
		 * Contains pallete data, tile data and the tile mapping.
		 * 
		 * @see spriteManager::spriteSet
		 */
		struct bgSprite
		{
			const unsigned short* bg_Pal;
			const unsigned char* bg_Map;
			const unsigned char* bg_Tiles;		//for backgrounds
		};
		
		bgSprite backGroundSprite;
		
		void loadSpriteSet(spriteSet* target, const unsigned short* p, const unsigned char* s);
		bool spriteSetHasPallete(spriteSet *s);
		void spriteSetLoadPallete(spriteSet *s);
		
		//create a sprite that plays an animation once, then destroys itself
		void createSingleFireSprite(u8 palID, const unsigned char* gfx, vector<u8> frames, u8 ticksPerFrame, s16 nx = OFFX, s16 ny = OFFY, u8 objSize1 = 0, u8 objSize2 = 2, u8 cx = 16, u8 cy = 16, u8 minIndex = 0, u8 colorMode = 1);
		void createSingleFireSprite(u8 palID, u16 gfxID, vector<u8> frames, u8 ticksPerFrame, s16 nx = OFFX, s16 ny = OFFY, u8 objSize1 = 0, u8 objSize2 = 2, u8 cx = 16, u8 cy = 16, u8 minIndex = 0, u8 colorMode = 1);
		
		s8 loadPallete(void* newpallete);
		s8 loadSprite(const unsigned char* newSprite, u8 sizex, u8 sizey, u8 minIndex = 0, u8 palleteID = 0, s16 x = OFFX, s16 y = OFFY, u8 colorMode = 1);
		void replaceSprite(u8 id, void* newSprite);
		
		void updateSpritePallete(u8 spriteID, u16 gfxID, u8 palleteID, u8 sizex, u8 sizey, u8 colorMode, u8 x, u8 y);
		
		void setAlpha(u8 transLevel);	//set alpha blending for this screen's SFX
		
		u16 loadGFX(const unsigned char* newGFX, u8 sizex, u8 sizey, u8 colorMode = 1);
		s8 loadSpriteFromGFX(u16 GFXid, u8 sizex, u8 sizey, u8 palleteID = 0, u8 minIndex = 0, u8 colorMode = 1, s16 x = OFFX, s16 y = OFFY);
		void updateSpriteGFX(s8 spriteID, void* GFXid);
		
		//deletes all gameSprite objects and frees their sprites
		virtual void clearOutSprites(bool keepSound = false);
		
		///the array of spriteObject%s for this spriteManager. This contains all active spriteObject%s for the screen. 
		vector<spriteObject*> gameSprites;
		
		///which screen this spriteManager relates to - 0 (bottom) or 1 (top)
		u8 screen;
		
		/**
		 * Checks if a sprite in OAM is currently active
		 * @param	index	the OAM index of the sprite to check
		 * @return	true if the sprite slot is in use, false otherwise
		 */
		bool isSprite(s8 index) const {return (index != -1 && sprites[index]);}
		/**
		 * Removes a sprite from OAM so that it may be reused
		 */
		void removeSprite(s8 index) {if (index >= 0) sprites[index] = 0;}
		
		#ifdef __WITHSOUND
		/**
		 * A set of raw data that sounds may use to be more easily played.
		 * Contains raw sound data and the length of the sound.
		 */
		struct soundData
		{
			const u8* data;
			const u32* size;
		};
		/**
		 * Fill a soundData with raw sound data and the size of that sound.
		 * 
		 * @see soundData
		 * @param	ref	the soundData to fill
		 * @param	data	pointer to the data that the soundData will be filled with
		 * @param	size	the length of the sound
		 */
		void loadSound(soundData* ref, const u8* data, const u32* size) {ref->data = data; ref->size = size;}
		
		s8 playSound(const u8* data, const u32* size, bool repeat = false, u8 vol = 127);
		void stopSound(s8 channel);
		bool soundLooping(s8 channel);
		void soundTick();
		#endif
		
		/**
		 * Check whether or not the spriteManager is clearing out all data for its screen
		 * 
		 * @return true if the spriteManager is resetting, false otherwise.
		 */
		bool isResetting() const {return totalReset;}
	protected:		
		s8 getNextPalleteID();
		s8 getNextSpriteID(u8 minIndex = 0);
		#ifdef __MDDEBUG
		u8 getNumSpritesTaken() { u8 ret = 0; for (u8 i=0; i<128; ++i) ret+= sprites[i]; return ret; }
		#endif
		
		///array of sprite OAM toggles (0=no sprite, 1=allocated)
		u8 sprites[128];
		///array of pallete toggles (0=no pallete, 1=pallete loaded)
		u8 palletes[16];
		
		///true only when the entire screen is being destroyed and refreshed. In such a case, there's no need for sprites to clean themselves up properly.
		bool totalReset;
		
		#ifdef __WITHSOUND
		///sound channels, and whether or not the sound playing in that channel is looping
		bool soundLoopings[8];
		#endif
};

#endif
