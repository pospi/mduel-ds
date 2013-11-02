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

#ifndef _MENUBOTTOM_H
#define _MENUBOTTOM_H

#include <PA9.h>
#include <string>
#include <vector>
	using namespace std;
#include "spriteManager.h"
#include "macros.h"
#ifndef _ALLGRAPHICS_
#define _ALLGRAPHICS_
#include "../data/all_gfx_ed.c"
#endif

//forward declaration
class gameManager;

/**
 * The interactive menu screen manager which manages an interface to all game configuration options.
 * 
 * This spriteManager is responsible for updating and saving all data into SRAM, so that it is stored
 * between game sessions. When SRAM is not or cannot be used, the game variables are stored temporarily
 * and lost when the DS powers off.
 * 
 * It maintains several menu screens to modify this data. 
 * These screens are primarily interfaced via the control pad, however some of
 * them respond to stylus taps as well.
 * 
 * @author pospi
 */
class menuBottom : public spriteManager
{
	public:
		menuBottom(u8 newScreen = 0);
		virtual ~menuBottom();
		
		void loadBackground();
		
		virtual void clearOutSprites(bool keepSound = false);
		
		virtual void gameTick();
		
		/**
		 * Give this spriteManager control of its screen, and display the main menu.
		 */
		virtual void activate()
		{
			spriteManager::activate(); 
			resetText();			
			loadBackground();
			mainMenu();	//initially jump to main menu
			/*#ifdef __WITHSOUND
			#ifdef __WITHMENUTRACK
			playSound(titleMusic.data, titleMusic.size, true);
			#endif
			#endif*/
			loadSRAM(true);
		}
		virtual void deactivate() {spriteManager::deactivate(); unloadBackground();}
		
		///spriteManager which handles the top screen during frontend
		spriteManager *menuTop;
		///spriteManager which handles the bottom screen during gameplay
		spriteManager *gameBottom;
		///spriteManager which handles the top screen during gameplay
		gameManager* gameTop;
		
		void loadCursorSpriteSet(const unsigned short *p, const unsigned char *s);
		void loadPickupSpriteSet(const unsigned short *p, const unsigned char *s);
		void loadMenuSpriteSet(const unsigned short *p, const unsigned char *s);
		
		//player character selections, needed by the gameManager
		u8 player1id, player2id;
		//character pallete and sprite data
		const unsigned short *playerPallete1, *playerPallete2, *playerPallete3, *playerPallete4;
		const unsigned char *playerSprite1, *playerSprite2, *playerSprite3;
		
		///a string of all the character names. Each name is 8 characters, * 6 names = 48 length.
		string playerNames;
		///number of pickups that can be active at a time, used by menuGBottom
		u8 numPickups;
		bool dieWhileDancing, showPickups;
		
		void scoreKill(u8 player, u8 weaponID = 18);		//weaponID == Pickup::pickupType. default to FIDs.
		void scoreDeath(u8 player, u8 weaponID = 18);
		void scoreTime(u8 player, u8 weaponID, u16 timeSecs);
		void scoreRound(u8 winner, u8 loser, bool tied);
		
		void saveStats();	//silently save statistics so SRAM (or not, if not yet formatted)
		
		#ifdef __WITHSOUND
			soundData menuMove, menuOk, menuCancel;
		#endif
private:
		///the screen navigation function that will be currently executed every tick
		void (menuBottom::*navFunction)();
		///the menu item display changing function that manipulates each menuItem in the current menu.
		void (menuBottom::*renderFunction)(u8);
		///the action function that will be currently executed every tick
		void (menuBottom::*actionFunction)(bool, bool);

		//different menu behaviours to assign to navFunction, renderFunction & actionFunction
		void normalNav();
		void statsNav();
		void miscNav();
		void charNav();
		void charNameNav();
		void roundsNav();
		void normalRender(u8 i);
		void statsRender(u8 i);
		void pickupsRender(u8 i);
		void charRender(u8 i);
		void charNameRender(u8 i);
		void mainAction(bool stylusTouched = false, bool stylusActivated = false);
		void charAction(bool stylusTouched = false, bool stylusActivated = false);
		void pickupsAction(bool stylusTouched = false, bool stylusActivated = false);
		void roundsAction(bool stylusTouched = false, bool stylusActivated = false);
		void sramAction(bool stylusTouched = false, bool stylusActivated = false);
		void controlsAction(bool stylusTouched = false, bool stylusActivated = false);
		void charNameAction(bool stylusTouched = false, bool stylusActivated = false);
		void creditsAction(bool stylusTouched = false, bool stylusActivated = false);
		void miscAction(bool stylusTouched = false, bool stylusActivated = false);
		void statsAction(bool stylusTouched = false, bool stylusActivated = false);
		
		u8 playerPal1, playerPal2, playerPal3, playerPal4;
		
		spriteSet cursorSprite, pickupSprite, menuSprite;
		
		/*#ifdef __WITHSOUND
		#ifdef __WITHMENUTRACK
		soundData titleMusic;
		#endif
		#endif*/
		
		//menu resettings
		void clearMenus();
		void makeCursor();
		
		void autoMenuClickRegions();
		s8 touchingWhichItem(u16 x, u16 y);
		
		//menu changings
		void mainMenu(u8 prevScreen = 0);
		void characterMenu();
		void pickupMenu();
		void roundsMenu();
		void saveWarningMenu();
		void controlsMenu();
		void playerRenameMenu(u8 playerID);
		void creditsMenu();
		void miscMenu();
		void statsMenu();
		
		void alterPlayerName(u8 playerID, u8 letterID, s8 modBy);
		///id of the character name being edited, for the character selection screen
		u8 editingName;
		
		bool isSRAMSaved();
		
		/**
		 * Properties needed to display a specific menu element.
		 * Contains a text string, text position, spriteObject and sprite position.
		 * May also contain an extra spriteObject and hitbox for stylus checking.
		 */
		struct menuItem
		{
			u8 cursorx, cursory;
			u8 textx, texty;
			string text;
			u8 objx, objy;
			spriteObject *obj;
			///an extra spriteObject for the Pickup%s screen
			spriteObject *obj2;
			u16 x1, x2, y1, y2;
		};
		///index of the selected menu item
		s8 selectedItem;
		///size of menu columns (if present)
		u8 menuColSize;
		///all the active menuItem%s for a menu
		vector<menuItem> menuItems;
		///the cursor sprite that shows us what we have selected
		spriteObject* cursor;
		
		///Rather nasty hack for the settings screen and its two cursors. I could do it properly, but im impatient.
		s8 selectedItem2;
		u8 settingsPlayer, settingsPage;
		void drawStatsFor(u8 player, u8 page);
		
		//SRAM variables
		string prevPlayerNames;
		u8 roundNum, prevRoundNum;
		u32 pickupMask, prevPickupMask;
		u8 prevPlayer1id, prevPlayer2id;	//1 indexed
		u8 playerSprs[6], prevPlayerSprs[6];
		u8 playerPals[6], prevPlayerPals[6];
		bool prevDieWhileDancing, prevShowPickups;
		u8 prevNumPickups;
		
		/**
		 * A GIANT array of statistical data in SRAM format.
		  - 16 bit values (ie 2 slots)
		  - 19 values for kills, deaths and time (per weapon, and FIDs/no weapon)
		  - 1 value each for wins, losses and ties
		  - 7 lots of this (1 for each character save slot)
		  - = 840 bytes of data!
		  - -42 unneeded fields for 'all players' save slot
		  - = 798 bytes of data!
		 *
		 * STORED AS:<br/>
		 * for characters - {[KILLS][TIME][DEATHS][wins][losses][ties]}<br/>
		 * for global  - {[KILLS][TIME][rounds]}<br/>
		 * Use the mutator functions to manipulate it in meaningful ways.
		 * 
		 * @see menuBottom::writeStatValueAt()
		 * @see menuBottom::readStatValueAt()
		 * @see menuBottom::scoreKill()
		 * @see menuBottom::scoreDeath()
		 * @see menuBottom::scoreTime()
		 * @see menuBottom::scoreRound()
		 * @see menuBottom::erasePlayer()
		 */
		u8 statistics[798];
		void writeStatValueAt(u16 offset, u16 value);
		u16 readStatValueAt(u16 offset) const;
		void erasePlayer(u8 player);
		//for these:
		// if player == -1 then get stat for all players
		// if weaponID == -1, get stat for all weapons
		u16 getKillsForPlayer(s8 player, s8 weaponID) const;
		u16 getDeathsForPlayer(s8 player, s8 weaponID) const;
		u16 getTimeForPlayer(s8 player, s8 weaponID) const;
		u16 getWins(s8 player) const;
		u16 getLosses(s8 player) const;
		u16 getTies(s8 player) const;
		///size of an individual character's slot in the menuBottom::statistics array.
		static const u8 PLAYERSLOTSIZE = 120;
		///size of the global statistics slot in the menuBottom::statistics array.
		static const u8 GLOBALSLOTSIZE = 78;
		///size of a weapon statistics block in the menuBottom::statistics array.
		static const u8 WEAPONBLOCKSIZE = 38;

		///ticks for changing text and round numbers
		u8 menuRepeater;
		
		void loadSRAM(bool firstLoad = false);
		void saveSRAM(bool dontCheck = false, u8 menuNumber = 0);
		
		void sramSavePrevVars();
		void sramLoadPrevVars();
		void sramLoadDefaults();
		
		bool pickupSelected(u8 pickupID);
		void makePickupIcon(string text, u8 num, u16 curx, u16 cury);
		void makeInstructions(u8 frame, u8 xpos);
		const unsigned char *getPlayerSpriteData(u8 idNum);
		const unsigned short *getPlayerPalleteData(u8 idNum);
		u8 getPlayerPalleteID(u8 idNum);
		void updatePlayerIcon(u8 i);
		
		void loadPlayerGFX(u8 dataNum, u8 playerNum);
		void pickupToggle(u8 i);
		void selectPlayer(u8 i, u8 playerNum);
		void playerColorCycle(u8 i);
		void playerGFXCycle(u8 i);
		
		///the SRAM offset to save the format check string (menuBottom::_SRAMID) in
		static const u8 _SRAMIDOFFSET = 0;
		///the SRAM offset to save the number of rounds to play (menuBottom::roundNum) at
		static const u8 _ROUNDSOFFSET = 7;
		///the SRAM offset to save the active Pickup%s mask (menuBottom::pickupMask) at
		static const u8 _PICKUPOFFSET = 8;
		///the SRAM offset to save the selected character for Player 1 (menuBottom::player1id) at
		static const u8 _PLAYER1IDOFFSET = 12;
		///the SRAM offset to save the selected character for Player 2 (menuBottom::player2id) at
		static const u8 _PLAYER2IDOFFSET = 13;
		///the SRAM offset to save the character GFX ids (menuBottom::playerSprs) at
		static const u8 _PLAYERSPRSOFFSET = 14;
		///the SRAM offset to save the character pallete ids (menuBottom::playerPals) at
		static const u8 _PLAYERPALSOFFSET = 20;
		///the SRAM offset to save the character names (menuBottom::playerNames) at
		static const u8 _NAMESOFFSET = 26;
		///the SRAM offset to save the game's miscellaneous settings (menuBottom::dieWhileDancing, menuBottom::numPickups and menuBottom::showPickups) at
		static const u8 _SETTINGSOFFSET = 76;
		///the SRAM offset to save character statistics (menuBottom::statistics) at
		static const u8 _STATSOFFSET = 78;
		///the length of menuBottom::statistics, to manage SRAM size
		static const u16 _STATSLEN = 798;
		
		///the ID to save at the base address in SRAM that tells us if SRAM has been formatted for this game
		static const char *_SRAMID;
		///the number of Pickup%s in the game
		static const u8 _NUMPICKUPS = 18;
		///the maximum number of Pickup%s that can be configured. Must be low enough so that we do not run out of sprites.
		static const u8 MAXPICKUPSONSCREEN = 10;
		///a bit for every Pickup type turned on
		static const u32 _ALLPICKUPS = 262143;
		///number of character save slots
		static const u8 _NUMSAVESLOTS = 6;
		///number of frames to wait before changing the selected menu item when a scrolling button is held down
		static const u8 menuRepeatDelay = 10;
};

#endif
