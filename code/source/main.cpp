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

// Includes
#include <PA9.h>       // Include for PA_Lib
#include <dswifi9.h>
#include <vector>
#include <string>
	using std::vector;
	using std::string;

#include "macros.h"
#include "spriteManager.h"
#include "gameManager.h"
#include "menuTitle.h"
#include "menuBottom.h"
#include "menuGBottom.h"

//graphics
#ifndef _ALLGRAPHICS_
#define _ALLGRAPHICS_
#include "../data/all_gfx_ed.c"
#endif
//sounds
#ifdef __WITHSOUND
#include "boomflyF.h"
#include "bootsF.h"
#include "BulletReflect.h"
#include "dunceF.h"
#include "HookHit.h"
#include "invisibleF.h"
#include "lazerF.h"
#include "lightningarmedF.h"
#include "lightninghitF.h"
#include "magnetF.h"
#include "mineDrop.h"
#include "mineExp.h"
#include "net.h"
#include "paraCloseF.h"
#include "paraOpenF.h"
#include "gotPickup.h"
#include "skull.h"
#include "skullPickup.h"
#include "splashBig.h"
#include "splashSmall.h"
#include "teleF.h"
#include "thrown.h"
#include "visableF.h"
#include "WeasAttack2.h"
#include "WeasAttackF.h"
#include "WeasRelease1.h"
#include "WeasRelease2.h"
#include "WeasRun1.h"
#include "WeasRun2.h"
#include "WeasRun3.h"
#include "platformExplode.h"
#include "spawnPuff.h"
#include "menuMove.h"
#include "menuOk.h"
#include "menuCancel.h"
/*#ifdef __WITHMENUTRACK
	#include "opening.h"
#endif*/
#endif

//end asset loading
//start actual program

//==============================================================================================
//==============================================================================================
//HERE IS SOME AWESOME DEBUGGING WHICH IS REALLY NOT MUCH USE TO ME. MAYBE SOME OTHER TIME.

/*#ifdef __MDDEBUG
	
	#include <list>
	#include <iterator>
	
	typedef struct {
		int	address;
		int	size;
		char file[64];
		int	line;
	} ALLOC_INFO;

	typedef list<ALLOC_INFO*> AllocList;
	
	AllocList *allocList;
	
	void AddTrack(int addr,  int asize,  const char *fname, int lnum)
	{
		ALLOC_INFO *info;
		
		if(!allocList) {
			allocList = new(AllocList);
		}
		
		info = new(ALLOC_INFO);
		info->address = addr;
		strncpy(info->file, fname, 63);
		info->line = lnum;
		info->size = asize;
		allocList->insert(allocList->begin(), info);
	};
	
	void RemoveTrack(int addr)
	{
		AllocList::iterator i;
		
		if(!allocList)
			return;
		for(i = allocList->begin(); i != allocList->end(); i++)
		{
			if((*i)->address == addr)
			{
				allocList->remove((*i));
				break;
			}
		}
	};
	
	void DumpUnfreed()
	{
		AllocList::iterator i;
		int totalSize = 0;
		string buf;
		
		if(!allocList)
		  return;
		
		for(i = allocList->begin(); i != allocList->end(); i++) {
			char temp[256];
			sprintf(temp, "%-50s: LINE %d, ADDRESS %d, %d unfreed\n",
			  (*i)->file, (*i)->line, (*i)->address, (*i)->size);
			buf += temp;
			totalSize += (*i)->size;
		}
		//sprintf(buf, "-----------------------------------------------------------\n");
		//PA_OutputText(0, 0, 0, buf);
		//sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
		
		PA_OutputText(0, 0, 0, const_cast<char *>(buf.c_str()));
	};
	
	inline void * operator new(unsigned int size, const char *file, int line)
	{
	  void *ptr = (void *)malloc(size);
	  AddTrack((int)ptr, size, file, line);
	  return(ptr);
	};
	inline void operator delete(void *p)
	{
	  RemoveTrack((int)p);
	  free(p);
	};
	
	#define DEBUG_NEW new(__FILE__, __LINE__)
	#define new DEBUG_NEW
#endif*/

//==============================================================================================
//==============================================================================================

int main(int argc, char ** argv)
{
	PA_Init();    	// Initializes PA_Lib
	PA_InitRand();	// random numbers are go!
	#ifdef __WITHSOUND
	PA_InitSound();	// sounds!
	#endif
	PA_InitVBL(); 	// Initializes a standard VBL

	#ifdef __MDDEBUG
	PA_WaitForVBL();  PA_WaitForVBL();  PA_WaitForVBL();  // wait a few VBLs
	fatInitDefault();  // FAT driver init
	FILE* logFile = fopen(LOGFILE, "a+");
	if (logFile)
		fprintf(logFile, "\n\n\n\n=========================================\nNEW GAME!\n\n\n\n");
	fclose(logFile);
	#endif
	
	//start timer system. We get roughly 1,100 hours of play before the timer overflows. Should be alright :P
	StartTime(true);
	
	PA_InitCustomText(0, 2, Font);	//initialise text subsystems
	PA_InitCustomText(1, 2, Font);
	
	//sprite managers
	gameManager *gm = new gameManager(1);
	menuTitle *menuTop = new menuTitle(1);
	menuBottom *menuBase = new menuBottom(0);
	menuGBottom *gameBase = new menuGBottom(0);
	
	gm->statTimerID = NewTimer(true);		//timer for stats recording
	
	gm->loadEnvSpriteSet(envSprites_Pal, envSprites_Sprite);
	gm->loadPickupSpriteSet(pickupSprites_Pal, pickupSprites_Sprite);
	gm->loadFXSpriteSet(effectSprites_Pal, effectSprites_Sprite);
	gm->loadDunceSpriteSet(dunceHatSprite_Pal, dunceHatSprite_Sprite);
	gm->loadBootsSpriteSet(bootsSprite_Pal, bootsSprite_Sprite);
	
	gm->player1000VPalleteData = player1000V_Pal;
	
	menuBase->loadCursorSpriteSet(menuCursors_Pal, menuCursors_Sprite);
	menuBase->loadPickupSpriteSet(pickupSprites_Pal, pickupSprites_Sprite);
	menuBase->loadMenuSpriteSet(menuSprites_Pal, menuSprites_Sprite);
	menuBase->playerPallete1 = playerSprite1_Pal;
	menuBase->playerPallete2 = playerSprite3_Pal;
	menuBase->playerPallete3 = playerSprite2_Pal;
	menuBase->playerPallete4 = playerSprite4_Pal;
	menuBase->playerSprite1 = playerSprite1_Sprite;
	menuBase->playerSprite2 = playerSprite2_Sprite;
	menuBase->playerSprite3 = playerSprite3_Sprite;
	
	gameBase->loadMenuSpriteSet(menuSprites_Pal, menuSprites_Sprite);
	gameBase->loadSpawnerSpriteSet(spawnerSpriteX_Pal, spawnerSpriteX_Sprite);
	gameBase->loadPickupSpriteSet(pickupSprites_Pal, pickupSprites_Sprite);
	
	#ifdef __WITHSOUND
	/*#ifdef __WITHMENUTRACK
	menuBase->loadSound(&menuBase->titleMusic, opening, opening_size);
	#endif*/
	gm->loadSound(&gm->bigSplash, splashBig, splashBig_size);
	gm->loadSound(&gm->smallSplash, splashSmall, splashSmall_size);
	gm->loadSound(&gm->platformExplode, platformExplode, platformExplode_size);
	gm->loadSound(&gm->spawnPuff, spawnPuff, spawnPuff_size);
	gm->loadSound(&gm->gotPickup, gotPickup, gotPickup_size);
	gm->loadSound(&gm->teleport, teleF, teleF_size);
	gm->loadSound(&gm->skullLoop, skull, skull_size);
	gm->loadSound(&gm->skullPickup, skullPickup, skullPickup_size);
	gm->loadSound(&gm->lightningHit, lightninghitF, lightninghitF_size);
	gm->loadSound(&gm->lightningLoop, lightningarmedF, lightningarmedF_size);
	gm->loadSound(&gm->invisOn, invisibleF, invisibleF_size);
	gm->loadSound(&gm->invisOff, visableF, visableF_size);
	gm->loadSound(&gm->mineExp, mineExp, mineExp_size);
	gm->loadSound(&gm->mineDrop, mineDrop, mineDrop_size);
	gm->loadSound(&gm->gunFire, lazerF, lazerF_size);
	gm->loadSound(&gm->bootsJump, bootsF, bootsF_size);
	gm->loadSound(&gm->chutClose, paraCloseF, paraCloseF_size);
	gm->loadSound(&gm->chutOpen, paraOpenF, paraOpenF_size);
	gm->loadSound(&gm->hookHit, HookHit, HookHit_size);
	gm->loadSound(&gm->duncePickup, dunceF, dunceF_size);
	gm->loadSound(&gm->magnetLoop, magnetF, magnetF_size);
	gm->loadSound(&gm->netThrow, net, net_size);
	gm->loadSound(&gm->bulletReflect, BulletReflect, BulletReflect_size);
	gm->loadSound(&gm->boomerangLoop, boomflyF, boomflyF_size);
	gm->loadSound(&gm->throwSound, thrown, thrown_size);
	
	gm->loadSound(&gm->weaselSounds[0], WeasAttack2, WeasAttack2_size);
	gm->loadSound(&gm->weaselSounds[1], WeasAttackF, WeasAttackF_size);
	gm->loadSound(&gm->weaselSounds[2], WeasRelease1, WeasRelease1_size);
	gm->loadSound(&gm->weaselSounds[3], WeasRelease2, WeasRelease2_size);
	gm->loadSound(&gm->weaselSounds[4], WeasRun1, WeasRun1_size);
	gm->loadSound(&gm->weaselSounds[5], WeasRun2, WeasRun2_size);
	gm->loadSound(&gm->weaselSounds[6], WeasRun3, WeasRun3_size);

	menuBase->loadSound(&menuBase->menuMove, menuMove, menuMove_size);
	menuBase->loadSound(&menuBase->menuOk, menuOk, menuOk_size);
	menuBase->loadSound(&menuBase->menuCancel, menuCancel, menuCancel_size);
	#endif
	
	//spriteManager associations
	menuBase->menuTop = menuTop;
	menuBase->gameTop = gm;
	menuBase->gameBottom = gameBase;
	
	gameBase->menuTop = menuTop;
	gameBase->menuBase = menuBase;
	gameBase->gameTop = gm;
	
	gm->menuTop = menuTop;
	gm->menuBase = menuBase;
	gm->gameBottom = gameBase;
	
	//ok - turn on the spriteManagers we want to use now.
	menuTop->activate();
	menuBase->activate();
	
	// Unfortunately try/catch blocks don't seem to work on the DS. Boo!
	//try {
		while (1)
		{
			if (menuTop->isActive())
				menuTop->gameTick();
			if (menuBase->isActive())
				menuBase->gameTick();
			if (gm->isActive())
				gm->gameTick();
			if (gameBase->isActive())
				gameBase->gameTick();
			
			PA_CheckLid();	//check for lid pausing
			PA_WaitForVBL();
		} 
	/*} catch (...) {
		DumpUnfreed();
	}*/
	
	return 0;
}

/*! \mainpage Marshmallow Duel DS
 *
 * \section secIntro About
 *
 * Welcome to the Marshmallow Duel DS source code. MDuel DS is a free game built for the Nintendo DS
 * console, using <a href="http://www.devkitpro.org/">DevKitPro</a> and 
 * <a href="http://www.palib.info">PALib</a>.
 * 
 * It is a sequel to / extension of <a href="http://www.marshmallowduel.com">Marshmallow Duel</a>, a game originally
 * created by Duncan and Roger Gill in 1996 using TurboC and some archaic PCX libraries. Ah, those were
 * the days.
 * 
 * To read more about MDuel DS and to download the latest official version, please go to
 * <a href="http://mduel2k5.spadgos.com/mduel96/mduelDSdownload.php">http://mduel2k5.spadgos.com/mduel96/mduelDSdownload.php</a>.
 * 
 * \section secCopy Legal
 * 
 * MDuel DS was created entirely by pospi, just for a bit of fun really. You can view more of his
 * projects or contact him by going to <a href="http://pospi.spadgos.com/">http://pospi.spadgos.com</a>.
 * 
 * The source code is released under the GPL, which means that it is free for anyone to use or modify so
 * long as you respect a few simple guidelines. Basically, you must provide the full source code whenever
 * you release something based on this code, and you must document all your changes to the original source
 * code with the appropriate \@author and \@date tags.
 * A copy of this license is included in <a href="../gpl.txt">gpl.txt</a>.<br/>
 * License summary follows:
 * 
 * <pre>Marshmallow Duel DS v2
Copyright &copy; 2007 Sam Pospischil <a href="http://pospi.spadgos.com">http://pospi.spadgos.com</a>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA</pre>
 * 
 * \section	secCode The Code
 * 
 * The MDuel DS source code is generated with <a href="http://www.doxygen.org/">doxyGen</a> and
 * <a href="http://www.graphviz.org/">GraphViz</a>, which should make it very easy to follow, so if you
 * plan on editing it please stick to some quality documentation. Methods, 
 * classes etc are displayed with their comment headers and are hyperlinked to all locations in the source
 * code that reference them. To view the documentation for functions linked to a member, click on the
 * name of the function in the member's call graph.
 *
 * For testing debug builds, I recommend <a href="http://nocash.emubase.de/gba.htm">no$gba</a> for quick
 * testing on Windows and a DS flashcart like the <a href="http://www.m3adapter.com/">M3 adapter</a> for
 * running the game on hardware. When compiled with the compiler directive __MDDEBUG (see macros.h), the
 * game attempts to output debugging info to a log file if the flashcart supports FAT drivers. If this is
 * unavailable, the information is printed to the bottom screen instead.
 * 
 * The game engine is fairly simplistic and features only a handful of base classes. These are:
 * - spriteManager, a screen and sprite memory management class
 * - spriteObject, a single sprite on the screen and all its related information
 * - Weapon, an inventory class for Player%s to utilise.
 * 
 * All objects in the game are derived from these and implement game logic at higher levels. Note that
 * most specific Weapon subclasses are not documented fully as a good understanding of their behaviour can
 * be obtained via analysis of the Weapon class itself.
 * 
 * There are 18 weapons in this version, and please note that the parachut is not a typo and should
 * definitely never have its spelling corrected.
 *
 * \subsection secBugs Current Bugs
 *
 * - Stability leaves something to be desired
 * - Boots occaisonally appear behind the player
 * - Walk animation sometimes doesn't play in a DS emulator or when intensive calculations are taking place
 *
 * \subsection secWish Wishlist
 *
 * Here are some things I, personally, would like to see happen one day:
 * - Wifi multiplayer
 * - AI opponents
 * - Shift the sprite routines to a 3D sprite system (newer versions of PALib support this)
 *   - Would allow up to 1024 sprites, instead of 128
 *   - Which means there could be larger, scrolling maps...
 *   - and more than two players...
 *   - and custom built levels, level editors, tilesets etc. The possibilities are endless.
 *
 * \section secInstall Compilation
 *
 * \subsection secIns1 Required libraries
 *  
 * If you plan on compiling MDuel DS, you're going to need to install some libraries first:
 * - <a href="http://downloads.sourceforge.net/devkitpro/devkitProUpdater-1.3.5.exe">DevKitPro v1.3.5</a>
 *   (newer versions seem to have issues with timers and SRAM saving)
 * - <a href="http://palib.info/forum/modules/PDdownloads/visit.php?cid=2&lid=2">PALib 060917</a>
 *   (again, newer versions seem bad)
 *
 * With DevKitPro, only the core files and ARM libraries are necessary. Other stuff is for development
 * on different platforms.
 *
 * Newer versions of both may well work for you with little or no fiddling, so if you're feeling
 * adventurous then by all means knock yourself out. After all, it can only make the game more stable.
 * If you do upgrade, the weapon time statistics and SRAM saving should be checked on emulators and DS 
 * hardware as these were the problematic areas with more recent libraries.
 *
 * \subsection secIns1b PALib library edits
 *
 * If you are using PALib 060917 then an additional change is required to its internals to allow for resetting
 * screen VRAM independently. If you decided to try your luck with an upgrade, then this is not necessary.
 * To find the required changes, open up the 'libraryEdits.cpp' file included under the 'notsource' directory.
 * After making the changes listed, simply recompile PALib by navigating to DevKitPro/PALib and running build.bat.
 * 
 * \subsection secIns2 Choose your compiler
 * 
 * So all that stuff is installed, and you're ready to compile. But how?
 *
 * The project is set up to be compiled under Visual Studio 2005, but if you want to compile 'normally' under GCC
 * because you hate Microsoft, then that's perfectly doable. Here are the steps to follow:
 * - Copy the PALib template (should be included in your PALib download) to a new folder inside DevKitPro. Let's
 *   call it 'MDuelGCC' for the purposes of these instructions. It really doesn't matter what you call it though.
 * - Copy everything inside the MDuel 'source' and 'data' folders to the corresponding folders in MDuelGCC
 * - Copy the supplied 'Makefile-gcc' into MDuelGCC and rename it to plain old 'Makefile'. You might have to play
 *   around with this a little, especially if you're using a different version of DevKitPro / PALib. I suggest
 *   you compare its contents with those of the PALib template makefile.
 * - That's it! Run build.bat and everything should _hopefully_ compile. If not, consult a 
 *   <a href="http://www.palib.info/forum/">forum</a> someplace ;)
 * 
 * If you have a copy of Microsoft Visual Studio 2005 Pro then you're in luck, because that's what I used.
 * I don't know if VS2005 Express will work or not but you'll probably get lucky.
 * Note that to make VS2005 recognise DevKitPro properly, you'll need to add a DKP_PATH environment variable 
 * with the path to the main DevKitPro folder.
 * You should be able to open up the project file and compile straight away after that last step.
 *
 * Just for reference, here are the environment variables you need to have set in either case. They should be
 * set automatically by the DKP and PALib installers, but just in case...
 * - PAPATH		c:/devkitPro/PAlib/
 * - DEVKITARM	/c/devkitPro/devkitARM
 * - DEVKITPRO	/c/devkitPro
 * - PATH		c:\\devkitPro\\msys\\bin
 * - DKP_PATH	c:\\DevKitPro
 */
