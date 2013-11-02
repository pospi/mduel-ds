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

#include "menuBottom.h"
#include "spriteObject.h"
#include "gameManager.h"

const char *menuBottom::_SRAMID = "MDUEL";

/**
 * create a menu and give it a screen to display on.
 * 
 * @param	newScreen	the screen this spriteManager should handle. 1 is the top screen, 0 is the bottom.
 */
menuBottom::menuBottom(u8 newScreen) : spriteManager(newScreen), menuTop(NULL), gameBottom(NULL), gameTop(NULL), 
	playerPallete1(NULL), playerPallete2(NULL), playerPallete3(NULL), playerPallete4(NULL), 
	playerSprite1(NULL), playerSprite2(NULL), playerSprite3(NULL),
	navFunction(NULL), renderFunction(NULL), actionFunction(NULL),
	playerPal1(0), playerPal2(0), playerPal3(0), playerPal4(0),
	editingName(0), selectedItem(0), menuColSize(0), cursor(NULL), selectedItem2(0), settingsPlayer(0), 
	settingsPage(0), menuRepeater(0)
{
	
}
	
menuBottom::~menuBottom()
{
	
}

/**
 * Handle all the possible menu input that a user can achieve on this screen.
 * 
 * Detects stylus input on menu items, menu navigation and rendering, and menu changing
 * based on the menu screen and menu item selected.
 * 
 * TODO: deprecate this function and use function pointers to break down menu actions
 * into nicer discrete blocks of logic.
 */
void menuBottom::gameTick()
{
	spriteManager::gameTick();

	#ifdef __WITHSOUND
	u8 oldSelItem = selectedItem;
	#endif
	
	//stylus input
	bool stylusActivated = false;
	bool stylusTouched = false;
	
	if (Stylus.Newpress)
	{
		s8 touchedItem = touchingWhichItem(Stylus.X, Stylus.Y);
		if (touchedItem == selectedItem) {
			stylusActivated = true;
		}
		if (touchedItem != -1) {
			selectedItem = touchedItem;
				//selectedItem2 for stats can suck my balls
			stylusTouched = true;
		}
	}
	
	//button navigation
	if (navFunction != NULL)
		(this->*navFunction)();

	#ifdef __WITHSOUND
	if (oldSelItem != selectedItem)
		playSound(menuMove.data, menuMove.size);
	#endif
	
	//cursor display
	if (cursor != NULL)
	{
		cursor->setPos(menuItems[selectedItem].cursorx, menuItems[selectedItem].cursory);
		if (selectedItem > menuColSize-1 && menuColSize != 0)
			cursor->setFrame(2);
		else
			cursor->setFrame(1);
	}
	//rendering
	for (u8 i=0; i < menuItems.size(); ++i)
	{
		normalRender(i);
		if (renderFunction != NULL)
			(this->*renderFunction)(i);
	}
	
	//actions
	if (actionFunction != NULL)
		(this->*actionFunction)(stylusTouched, stylusActivated);
	
	if (menuRepeater == 0)
		menuRepeater = menuRepeatDelay;
	menuRepeater--;
}

//========================================================================================================
//========================================================================================================

///normal navigation actions. The cursor is moved by the DS's D-Pad intuitively around the screen
void menuBottom::normalNav()
{
	if (Pad.Newpress.Up)
		--selectedItem;
	else if (Pad.Newpress.Down)
		++selectedItem;
	else if (Pad.Newpress.Left && menuColSize > 0)
		selectedItem -= menuColSize;
	else if (Pad.Newpress.Right && menuColSize > 0)
		selectedItem += menuColSize;
	if (selectedItem < 0)
		(menuColSize > 0 ? selectedItem += menuColSize*2 : selectedItem = menuItems.size()-1);
	selectedItem %= menuItems.size();
}
///navigation actions for the statistics page. Up/Down navigates the player, Left/Right navigates the page.
void menuBottom::statsNav()
{
	#ifdef __WITHSOUND
	u8 oldSelectedItem2 = selectedItem2;
	#endif

	if (Pad.Newpress.Up)
		--selectedItem;
	else if (Pad.Newpress.Down)
		++selectedItem;
	else if (Pad.Newpress.Left)
		--selectedItem2;
	else if (Pad.Newpress.Right)
		++selectedItem2;
	//hacked at 6 'cos we only want that column 'navigable'
	if (selectedItem < 0)
		selectedItem = 6;
	else if (selectedItem > 6)
		selectedItem = 0;			
	//hacked at 4 'cos we only want that much 'navigable'. ITS A BIG HACK ANYWAY GAWD
	if (selectedItem2 < 0)
		selectedItem2 = 3;
	else if (selectedItem2 > 3)
		selectedItem2 = 0;
	//general stats has no 4th selectedItem2
	if (selectedItem == 0 && selectedItem2 == 3)
		selectedItem2 = 0;
	drawStatsFor(selectedItem, selectedItem2);

	#ifdef __WITHSOUND
	if (oldSelectedItem2 != selectedItem2)
		playSound(menuMove.data, menuMove.size);
	#endif
}
///navigation actions for the misc settings page. Up/Down changes item only.
void menuBottom::miscNav()
{
	if (Pad.Newpress.Up)
		--selectedItem;
	else if (Pad.Newpress.Down)
		++selectedItem;
	if (selectedItem < 0)
		(menuColSize > 0 ? selectedItem += menuColSize*2 : selectedItem = menuItems.size()-1);
	selectedItem %= menuItems.size();
}
///navigation actions for character selection page. Up/Down changes character, Left/Right makes them active in either player slot.
void menuBottom::charNav()
{
	if (Pad.Newpress.Up)
		--selectedItem;
	else if (Pad.Newpress.Down)
		++selectedItem;
	else if (Pad.Newpress.Left)
	{
		#ifdef __WITHSOUND
		playSound(menuMove.data, menuMove.size);
		#endif
		selectPlayer(selectedItem, 1);
	} else if (Pad.Newpress.Right)
	{
		#ifdef __WITHSOUND
		playSound(menuMove.data, menuMove.size);
		#endif
		selectPlayer(selectedItem, 2);
	}
	if (selectedItem < 0)
		(menuColSize > 0 ? selectedItem += menuColSize*2 : selectedItem = menuItems.size()-1);
	selectedItem %= menuItems.size();
	
	if (playerPals[player1id-1] == playerPals[player2id-1])		//stops same colour happening for both characters
		playerColorCycle(selectedItem);
}
///navigation actions for character rename page. Up/Down changes letter, Left/Right navigates between letters
void menuBottom::charNameNav()
{
	if (Pad.Held.Up && menuRepeater == 0 || Pad.Newpress.Up)
	{
		#ifdef __WITHSOUND
		playSound(menuMove.data, menuMove.size);
		#endif
		alterPlayerName(editingName, selectedItem, -1);
	} else if (Pad.Held.Down && menuRepeater == 0 || Pad.Newpress.Down)
	{
		#ifdef __WITHSOUND
		playSound(menuMove.data, menuMove.size);
		#endif
		alterPlayerName(editingName, selectedItem, 1);
	}
	else if (Pad.Newpress.Left)
		--selectedItem;
	else if (Pad.Newpress.Right)
		++selectedItem;
	if (selectedItem < 0)
		selectedItem = menuItems.size()-1;
	selectedItem %= menuItems.size();
	PA_OutputText(screen, 12, 9, "________\n%s\n--------", macros::getPlayerName(editingName, playerNames, false).c_str());
}
///navigation actions for round number page. Up/Down changes the number of rounds.
void menuBottom::roundsNav()
{
	if ((Pad.Held.Up && menuRepeater == 0 || Pad.Newpress.Up) && roundNum < 50)
	{
		#ifdef __WITHSOUND
		playSound(menuMove.data, menuMove.size);
		#endif
		++roundNum;
	} else if ((Pad.Held.Down && menuRepeater == 0 || Pad.Newpress.Down) && roundNum > 1)
	{
		#ifdef __WITHSOUND
		playSound(menuMove.data, menuMove.size);
		#endif
		--roundNum;
	}
	PA_OutputText(screen, 14, 10, "-%d-  ", roundNum);
	gameTop->pointsToWin = roundNum;
}

/**
 * normal menu item drawing function. Different to the other render functions as it is not used as a delegate and is actually called all the time
 * @param	i	the menuItem to render
 */
void menuBottom::normalRender(u8 i)
{
	if (menuItems[i].text != "")
		PA_OutputSimpleText(screen, menuItems[i].textx, menuItems[i].texty, const_cast<char*>(menuItems[i].text.c_str()));
	if (menuItems[i].obj != NULL)
		menuItems[i].obj->setPos(menuItems[i].objx, menuItems[i].objy);
}
/**
 * screen rendering function for statistics menu items. Handles the extra cursor position.
 * @param	i	the menuItem to render
 */
void menuBottom::statsRender(u8 i)
{
	if (i > 6)		//first six are normal menu items
	{
		if (selectedItem == 0 && i==10)		//no player, dont show deaths
			PA_OutputSimpleText(screen, menuItems[i].textx, menuItems[i].texty, "      ");
		if (selectedItem2 == i-7)
			menuItems[i].obj->setPos(menuItems[i].objx, menuItems[i].objy);	//show cursor
		else
			menuItems[i].obj->setPos(OFFX, OFFY);	//hide cursor
	}
}
/**
 * screen rendering function for Pickup%s menu items. Handles the extra spriteObject%'s for the Pickup circles.
 * @param	i	the menuItem to render
 */
void menuBottom::pickupsRender(u8 i)
{
	//cursor points the opposite way in the pickups menu.
	//this need only be executed once but this is really the best way to do it anyway.
	if (cursor != NULL)
	{
		if (selectedItem > menuColSize-1 && menuColSize != 0)
			cursor->setFrame(1);
		else
			cursor->setFrame(2);
	}

	if (pickupSelected(i))
		menuItems[i].obj2->setPos(menuItems[i].objx, menuItems[i].objy);
	else
		menuItems[i].obj2->setPos(OFFX, OFFY);
}
/**
 * screen rendering function for character menu items. 
 * @param	i	the menuItem to render
 */
void menuBottom::charRender(u8 i)
{
	if (i == (u8)(player1id-1))
	{
		menuItems[i].obj->setFrame(0);
		menuItems[i].obj->setPos(120, menuItems[i].obj->gety());
	} else if (i == (u8)(player2id-1))
	{
		menuItems[i].obj->setFrame(0);
		menuItems[i].obj->setPos(224, menuItems[i].obj->gety());
	} else
	{
		menuItems[i].obj->setFrame(6);
		menuItems[i].obj->setPos(172, menuItems[i].obj->gety());
	}
}
/**
 * screen rendering function for character renaming menu, used to set the cursor frame.
 * @param	i	the menuItem to render
 */
void menuBottom::charNameRender(u8 i)
{
	if (cursor != NULL && cursor->getFrame() != 0)
		cursor->setFrame(0);
}

/**
 * Actions for the main menu. Start a new game, or redirect us to another menu.
 * @param	stylusTouched	true if any menuItem was tapped
 * @param	stylusActivated	true if the currently selected menuItem was tapped
 */
void menuBottom::mainAction(bool stylusTouched, bool stylusActivated)
{
	#ifdef __WITHSOUND
	if (Pad.Released.A || stylusTouched)
		playSound(menuOk.data, menuOk.size);
	#endif
	switch (selectedItem)
	{
		case 0:		//start 2P game
			#ifdef __MDDEBUG //file logging seems to slow the menu down to the point where button hits dont register
			if (Pad.Held.A || stylusTouched)
			#else
			if (Pad.Released.A || stylusTouched)
			#endif
			{
				menuTop->deactivate();
				deactivate();
				PA_WaitForVBL();
				gameTop->activate();
				for (u8 i=0; i < _NUMPICKUPS; ++i)
				{
					if (pickupSelected(i))
						gameTop->pickupMask.push_back(i);
				}
				gameTop->pointsToWin = roundNum;
				loadPlayerGFX(player1id, 1);
				loadPlayerGFX(player2id, 2);
				gameBottom->activate();
			}
		break;
		case 1:		//select characters
			if (Pad.Released.A || stylusTouched)
				characterMenu();
		break;
		case 2:		//choose pickups
			if (Pad.Released.A || stylusTouched)
				pickupMenu();
		break;
		case 3:		//set kill limit
			if (Pad.Released.A || stylusTouched)
				roundsMenu();
		break;
		case 4:		//misc options
			if (Pad.Released.A || stylusTouched)
				miscMenu();
		break;
		case 5:		//statistics
			if (Pad.Released.A || stylusTouched)
				statsMenu();
		break;
		case 6:		//view controls
			if (Pad.Released.A || stylusTouched)
				controlsMenu();
		break;
		case 7:		//credits
			if (Pad.Released.A || stylusTouched)
				creditsMenu();
		break;
		default:
		break;
	}
}
/**
 * Actions for the character selection menu. Save settings, edit names and modify colours/graphics.
 * @param	stylusTouched	true if any menuItem was tapped
 * @param	stylusActivated	true if the currently selected menuItem was tapped
 */
void menuBottom::charAction(bool stylusTouched, bool stylusActivated)
{
	if (Pad.Released.A)
	{
		#ifdef __WITHSOUND
		playSound(menuOk.data, menuOk.size);
		#endif
		saveSRAM(false, 1);
	} else if (Pad.Released.B) {
		#ifdef __WITHSOUND
		playSound(menuCancel.data, menuCancel.size);
		#endif
		loadSRAM();
		mainMenu(1);
	} else if (Pad.Released.X || stylusActivated) {
		#ifdef __WITHSOUND
		playSound(menuOk.data, menuOk.size);
		#endif
		editingName = selectedItem+1;	//used by renameMenu to work out which name to modify
		playerRenameMenu(selectedItem);
	} else if (Pad.Released.R) {
		#ifdef __WITHSOUND
		playSound(menuMove.data, menuMove.size);
		#endif
		playerColorCycle(selectedItem);
	} else if (Pad.Released.L) {
		#ifdef __WITHSOUND
		playSound(menuMove.data, menuMove.size);
		#endif
		playerGFXCycle(selectedItem);
	}
}
/**
 * Actions for the Pickup%s menu. Toggle active Pickup%s and save settings.
 * @param	stylusTouched	true if any menuItem was tapped
 * @param	stylusActivated	true if the currently selected menuItem was tapped
 */
void menuBottom::pickupsAction(bool stylusTouched, bool stylusActivated)
{
	if (Pad.Released.A)
	{
		#ifdef __WITHSOUND
		playSound(menuOk.data, menuOk.size);
		#endif
		saveSRAM(false, 2);
	} else if (Pad.Released.B) {
		#ifdef __WITHSOUND
		playSound(menuCancel.data, menuCancel.size);
		#endif
		loadSRAM();
		mainMenu(2);
	} else if (Pad.Released.X || stylusTouched) {
		#ifdef __WITHSOUND
		playSound(menuMove.data, menuMove.size);
		#endif
		pickupToggle(selectedItem);
	}
}
/**
 * Actions for the rounds menu. Handles settings saving.
 * @param	stylusTouched	true if any menuItem was tapped
 * @param	stylusActivated	true if the currently selected menuItem was tapped
 */
void menuBottom::roundsAction(bool stylusTouched, bool stylusActivated)
{
	if (Pad.Released.A)
	{
		#ifdef __WITHSOUND
		playSound(menuOk.data, menuOk.size);
		#endif
		saveSRAM(false, 3);
	} else if (Pad.Released.B) {
		#ifdef __WITHSOUND
		playSound(menuCancel.data, menuCancel.size);
		#endif
		loadSRAM();
		mainMenu(3);
	}
}
/**
 * Actions for the SRAM warning menu. Allows the user to decide whether to format the SRAM.
 * @param	stylusTouched	true if any menuItem was tapped
 * @param	stylusActivated	true if the currently selected menuItem was tapped
 */
void menuBottom::sramAction(bool stylusTouched, bool stylusActivated)
{
	if (Pad.Released.A || Stylus.Newpress)
	{
		#ifdef __WITHSOUND
		playSound(menuOk.data, menuOk.size);
		#endif
		saveSRAM(true);
	} else if (Pad.Released.B) {
		#ifdef __WITHSOUND
		playSound(menuCancel.data, menuCancel.size);
		#endif
		mainMenu(0);
	}
}
/**
 * Actions for the controls menu. Resets the background on close.
 * @param	stylusTouched	true if any menuItem was tapped
 * @param	stylusActivated	true if the currently selected menuItem was tapped
 */
void menuBottom::controlsAction(bool stylusTouched, bool stylusActivated)
{
	if (Pad.Released.A || Stylus.Newpress)
	{
		#ifdef __WITHSOUND
		playSound(menuCancel.data, menuCancel.size);
		#endif
		resetBackground();
		resetText();
		mainMenu(6);
	}
}
/**
 * Actions for the character names menu. Erase character statistics and save settings.
 * @param	stylusTouched	true if any menuItem was tapped
 * @param	stylusActivated	true if the currently selected menuItem was tapped
 */
void menuBottom::charNameAction(bool stylusTouched, bool stylusActivated)
{
	if (Pad.Released.A)
	{
		#ifdef __WITHSOUND
		playSound(menuOk.data, menuOk.size);
		#endif
		erasePlayer(editingName);
		characterMenu();
	} else if (Pad.Released.B) {
		#ifdef __WITHSOUND
		playSound(menuCancel.data, menuCancel.size);
		#endif
		loadSRAM();
		characterMenu();
	}
}
/**
 * Actions for the credits menu. Simply redirects us back to the main menu.
 * @param	stylusTouched	true if any menuItem was tapped
 * @param	stylusActivated	true if the currently selected menuItem was tapped
 */
void menuBottom::creditsAction(bool stylusTouched, bool stylusActivated)
{
	if (Pad.Released.A || Stylus.Newpress)
	{
		#ifdef __WITHSOUND
		playSound(menuCancel.data, menuCancel.size);
		#endif
		mainMenu(7);
	}
}
/**
 * Actions for the misc settings menu. Modify settings and handle saving.
 * @param	stylusTouched	true if any menuItem was tapped
 * @param	stylusActivated	true if the currently selected menuItem was tapped
 */
void menuBottom::miscAction(bool stylusTouched, bool stylusActivated)
{
	s8 temp;

	if (Pad.Newpress.Left || Pad.Newpress.Right)
	{
		#ifdef __WITHSOUND
		playSound(menuMove.data, menuMove.size);
		#endif
		switch (selectedItem)
		{
			case 0:
				temp = numPickups;
				temp += Pad.Newpress.Right-Pad.Newpress.Left;
				numPickups = temp > MAXPICKUPSONSCREEN ? MAXPICKUPSONSCREEN : (temp < 0 ? 0 : temp);
				char data[24];
				sprintf(data, "Max pickups: %d  ", numPickups);
				menuItems[selectedItem].text = data;
			break;
			case 1:
				showPickups = !showPickups;
				strcpy(data, "Pre-show pickups: ");
				strcat(data, (showPickups ? "ON " : "OFF"));
				menuItems[selectedItem].text = data;
			break;
			case 2:
				dieWhileDancing = !dieWhileDancing;
				strcpy(data, "Die while dancing: ");
				strcat(data, (dieWhileDancing ? "ON " : "OFF"));
				menuItems[selectedItem].text = data;
			break;
		}
	}
	if (Pad.Released.A)
	{
		#ifdef __WITHSOUND
		playSound(menuOk.data, menuOk.size);
		#endif
		saveSRAM(false, 4);
	}
	else if (Pad.Released.B) {
		#ifdef __WITHSOUND
		playSound(menuCancel.data, menuCancel.size);
		#endif
		loadSRAM();
		mainMenu(4);
	}
}
/**
 * Actions for the statistics menu. Simply returns us to the main menu.
 * @param	stylusTouched	true if any menuItem was tapped
 * @param	stylusActivated	true if the currently selected menuItem was tapped
 */
void menuBottom::statsAction(bool stylusTouched, bool stylusActivated)
{
	if (Pad.Released.A)
	{
		#ifdef __WITHSOUND
		playSound(menuCancel.data, menuCancel.size);
		#endif
		mainMenu(5);
	}
}

//========================================================================================================
//========================================================================================================

/**
 * Load the background for this screen.
 */
void menuBottom::loadBackground()
{
	PA_EasyBgLoad(screen, 3, ::menuBGbottom);
}

/**
 * Flush the sprite system and reset everything for a new round.
 * Sets all spriteSet%s palleteID's back to -1 so that they will be reloaded again later.
 */
void menuBottom::clearOutSprites(bool keepSound)
{
	cursorSprite.palleteID = -1;
	pickupSprite.palleteID = -1;
	menuSprite.palleteID = -1;
	spriteManager::clearOutSprites(keepSound);
}

//============================================================================

///load the sprite data for the menu cursor
void menuBottom::loadCursorSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&cursorSprite, p, s);
}

///load the sprite data for Pickup%s 
void menuBottom::loadPickupSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&pickupSprite, p, s);
}

///load the sprite data for menu instructions
void menuBottom::loadMenuSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&menuSprite, p, s);
}

//============================================================================

/**
 * Clear the menu screen entirely so that a new menu can replace the current one.
 * 
 * I've put a bunch of waits in here. The thing seems to crash without them. Dunno why.
 */
void menuBottom::clearMenus()
{
	menuItems.clear();
	PA_WaitForVBL();
	//PA_ClearTextBg(screen);
	clearOutSprites(true);
	PA_WaitForVBL();
	resetText();
	PA_WaitForVBL();
	loadBackground();
	loadCursorSpriteSet(cursorSprite.palleteData, cursorSprite.spriteData);
	loadMenuSpriteSet(menuSprite.palleteData, menuSprite.spriteData);
	PA_WaitForVBL();
	makeCursor();
	selectedItem = 0;
	selectedItem2 = 0;
	navFunction = &menuBottom::normalNav;
	renderFunction = NULL;
	actionFunction = NULL;
}

/**
 * Create a cursor sprite for the currently active menu
 */
void menuBottom::makeCursor()
{
	cursor = new spriteObject(this);
	cursor->setPallete(cursorSprite.palleteID);
	cursor->giveSprite(cursorSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
	cursor->setLayer(0);
	cursor->setFrame(1);
}

//============================================================================

/**
 * load the main menu onto the screen. ID 0.
 * 
 * @param	prevScreen	the previously chosen menu item, which will start in focus
 */
void menuBottom::mainMenu(u8 prevScreen)
{
	clearMenus();
	actionFunction = &menuBottom::mainAction;
	menuColSize = 0;
	selectedItem = prevScreen;
	struct menuItem temp1 = {60, 28, 9, 3, "Start 2P Game", 0,0,NULL,NULL};
	menuItems.push_back(temp1);
	struct menuItem temp2 = {44, 52, 7, 6, "Select Characters", 0,0,NULL,NULL};
	menuItems.push_back(temp2);
	struct menuItem temp3 = {60, 68, 9, 8, "Choose Pickups", 0,0,NULL,NULL};
	menuItems.push_back(temp3);
	struct menuItem temp4 = {60, 84, 9, 10, "Set Kill Limit", 0,0,NULL,NULL};
	menuItems.push_back(temp4);
	struct menuItem temp7 = {68, 100, 10, 12, "Misc Options", 0,0,NULL,NULL};
	menuItems.push_back(temp7);
	struct menuItem temp8 = {76, 124, 11, 15, "Statistics", 0,0,NULL,NULL};
	menuItems.push_back(temp8);
	struct menuItem temp5 = {60, 140, 9, 17, "View Controls", 0,0,NULL,NULL};
	menuItems.push_back(temp5);
	struct menuItem temp6 = {68, 156, 10, 19, "View Credits", 0,0,NULL,NULL};
	menuItems.push_back(temp6);
	
	autoMenuClickRegions();
	
	//instructions
	makeInstructions(0, 128);
}

///load the character selection menu onto the screen. ID 1.
void menuBottom::characterMenu()
{
	clearMenus();
	navFunction = &menuBottom::charNav;
	renderFunction = &menuBottom::charRender;
	actionFunction = &menuBottom::charAction;
	menuColSize = 0;
	
	playerPal1 = loadPallete((void*)playerPallete1);
	playerPal2 = loadPallete((void*)playerPallete2);
	playerPal3 = loadPallete((void*)playerPallete3);
	playerPal4 = loadPallete((void*)playerPallete4);
		
	spriteObject *playerSpr1 = new spriteObject(this);
	playerSpr1->setPallete(getPlayerPalleteID(playerPals[0]));
	playerSpr1->giveSprite(getPlayerSpriteData(playerSprs[0]), OBJ_SIZE_32X32, 16, 16);
	spriteObject *playerSpr2 = new spriteObject(this);
	playerSpr2->setPallete(getPlayerPalleteID(playerPals[1]));
	playerSpr2->giveSprite(getPlayerSpriteData(playerSprs[1]), OBJ_SIZE_32X32, 16, 16);
	spriteObject *playerSpr3 = new spriteObject(this);
	playerSpr3->setPallete(getPlayerPalleteID(playerPals[2]));
	playerSpr3->giveSprite(getPlayerSpriteData(playerSprs[2]), OBJ_SIZE_32X32, 16, 16);
	spriteObject *playerSprA = new spriteObject(this);
	playerSprA->setPallete(getPlayerPalleteID(playerPals[3]));
	playerSprA->giveSprite(getPlayerSpriteData(playerSprs[3]), OBJ_SIZE_32X32, 16, 16);
	//playerSprA->setFlipped(true);
	spriteObject *playerSprB = new spriteObject(this);
	playerSprB->setPallete(getPlayerPalleteID(playerPals[4]));
	playerSprB->giveSprite(getPlayerSpriteData(playerSprs[4]), OBJ_SIZE_32X32, 16, 16);
	//playerSprB->setFlipped(true);
	spriteObject *playerSprC = new spriteObject(this);
	playerSprC->setPallete(getPlayerPalleteID(playerPals[5]));
	playerSprC->giveSprite(getPlayerSpriteData(playerSprs[5]), OBJ_SIZE_32X32, 16, 16);
	//playerSprC->setFlipped(true);
	
	struct menuItem temp1 = {12, 36, 3, 4, macros::getPlayerName(1, playerNames, false).c_str(), 172, 36, playerSpr1, NULL};
	menuItems.push_back(temp1);
	struct menuItem temp2 = {12, 60, 3, 7, macros::getPlayerName(2, playerNames, false).c_str(), 172, 60, playerSpr2, NULL};
	menuItems.push_back(temp2);
	struct menuItem temp3 = {12, 84, 3, 10, macros::getPlayerName(3, playerNames, false).c_str(), 172, 84, playerSpr3, NULL};
	menuItems.push_back(temp3);
	struct menuItem tempA = {12, 108, 3, 13, macros::getPlayerName(4, playerNames, false).c_str(), 172, 108, playerSprA, NULL};
	menuItems.push_back(tempA);
	struct menuItem tempB = {12, 132, 3, 16, macros::getPlayerName(5, playerNames, false).c_str(), 172, 132, playerSprB, NULL};
	menuItems.push_back(tempB);
	struct menuItem tempC = {12, 156, 3, 19, macros::getPlayerName(6, playerNames, false).c_str(), 172, 156, playerSprC, NULL};
	menuItems.push_back(tempC);
	
	autoMenuClickRegions();
	
	//heading
	PA_OutputSimpleText(screen, 14, 1, "P1    OFF    P2");
	
	//instructions
	makeInstructions(5, 32);
	makeInstructions(1, 96);
	makeInstructions(2, 160);
	makeInstructions(3, 224);
}

///load the Pickup%s selection menu onto the screen. ID 2.
void menuBottom::pickupMenu()
{
	clearMenus();
	renderFunction = &menuBottom::pickupsRender;
	actionFunction = &menuBottom::pickupsAction;
	menuColSize = 9;
	loadPickupSpriteSet(pickupSprite.palleteData, pickupSprite.spriteData);
	
	makePickupIcon("Skull", 0, 84, 12);
	makePickupIcon("1000V", 1, 84, 28);
	makePickupIcon("Invisibility", 2, 140, 44);
	makePickupIcon("Mine", 3, 76, 60);
	makePickupIcon("Gun", 4, 68, 76);
	makePickupIcon("TNT", 5, 68, 92);
	makePickupIcon("Boots", 6, 84, 108);
	makePickupIcon("Grenade", 7, 100, 124);
	makePickupIcon("Puck", 8, 76, 140);
	makePickupIcon("Parachut", 9, 148, 12);
	makePickupIcon("Hook", 10, 180, 28);
	makePickupIcon("Warp", 11, 180, 44);
	makePickupIcon("Magnet", 12, 164, 60);
	makePickupIcon("Net", 13, 188, 76);
	makePickupIcon("Shield", 14, 164, 92);
	makePickupIcon("Dunce Hat", 15, 140, 108);	
	makePickupIcon("Weasel", 16, 164, 124);	
	makePickupIcon("Boomerang", 17, 140, 140);
	
	autoMenuClickRegions();
	
	//instructions
	makeInstructions(5, 96);
	makeInstructions(4, 160);
}

/**
 * Generate a menu item that comprises a Pickup icon on the Pickup selection screen.
 * 
 * @param	text	the text for this menu item to show
 * @param	num		the frame number of the Pickup icon
 * @param	curx	the x position (in pixels) to spawn the icon at
 * @param	cury	the y position (in pixels) to spawn the icon at
 */
void menuBottom::makePickupIcon(string text, u8 num, u16 curx, u16 cury)
{
	spriteObject *icon = new spriteObject(this);
	icon->setPallete(pickupSprite.palleteID);
	icon->giveSprite(pickupSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
	icon->setFrame(num+3);
	spriteObject *icons = new spriteObject(this);
	icons->setPallete(pickupSprite.palleteID);
	icons->giveSprite(pickupSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
	icons->setAnim(0,2,spriteObject::ANIMSPEED);
	
	if (num < 9)
	{
		struct menuItem temp = {curx, cury, 4, 1+num*2, text, 20, ((num+1)*16)-4, icon, icons};
		menuItems.push_back(temp);
	}
	else {
		struct menuItem temp = {curx, cury, 28-text.length(), 1+(num-9)*2, text, 236, ((num-8)*16)-4, icon, icons};
		menuItems.push_back(temp);
	}
}

/**
 * Create an instruction sprite and place it at the bottom of the screen.
 * 
 * @param	frame	the frame the icon should show
 * @param	xpos	the x position (in pixels) to show the instruction at
 */
void menuBottom::makeInstructions(u8 frame, u8 xpos)
{
	spriteObject *temp = new spriteObject(this);
	temp->setPallete(menuSprite.palleteID);
	temp->giveSprite(menuSprite.spriteData, OBJ_SIZE_64X32, 32, 16, 0, xpos, 192-8);
	temp->setLayer(0);
	temp->setFrame(frame);
	temp->makeStatic();
}

/**
 * check if a Pickup ID is currently enabled
 * 
 * @param	pickupID	the Pickup number to check for. Relates to Pickup::pickupType.
 * 
 * @return true if the Pickup type is enabled, false otherwise.
 */
bool menuBottom::pickupSelected(u8 pickupID)
{
	u32 pickupNum = 1<<pickupID;
	return pickupMask & pickupNum;
}

///load the round number selection menu onto the screen. ID 3.
void menuBottom::roundsMenu()
{
	clearMenus();
	navFunction = &menuBottom::roundsNav;
	actionFunction = &menuBottom::roundsAction;
	menuColSize = 0;
	
	PA_OutputText(screen, 4, 4, "First player to reach\nthis many points wins:");
	
	cursor->destroy();
	cursor = NULL;
	
	//instructions
	makeInstructions(5, 96);
	makeInstructions(6, 160);
}

///load the SRAM format warning onto the screen. ID 4.
void menuBottom::saveWarningMenu()
{
	clearMenus();
	actionFunction = &menuBottom::sramAction;
	menuColSize = 0;
	PA_OutputText(screen, 2, 4, " WARNING: SRAM has not been\n\nformatted for this game yet.\n\n\n\n\n\n\n  Overwrite SRAM Save Data?");
	
	cursor->destroy();
	cursor = NULL;
	
	//instructions
	makeInstructions(0, 128);
}

///load the controls page onto the screen. ID 5.
void menuBottom::controlsMenu()
{
	resetBackground();
	clearMenus();
	actionFunction = &menuBottom::controlsAction;
	menuColSize = 0;
	
	cursor->destroy();
	cursor = NULL;
	
	//dont need to set video mode for this one :D
	PA_EasyBgLoad(screen, 3, ::controlsBG);
	
	//instructions
	makeInstructions(7, 224);
}

///load the credits page onto the screen. ID 7.
void menuBottom::creditsMenu()
{
	clearMenus();
	actionFunction = &menuBottom::creditsAction;
	menuColSize = 0;
	
	cursor->destroy();
	cursor = NULL;
	
	PA_OutputText(screen, 2, 2, "    Marshmallow Duel DS\n\nby pospi - pospi.spadgos.com\n\n\n Based on Marshmallow Duel\n     created in 1996\n  by Duncan and Roger Gill\n\n     Sounds and music by\n       Liam Alexander\n\n Special thanks to Sam and\nMoles for their beta-testing\n      help and support.");
	
	//instructions
	makeInstructions(7, 128);
}

/**
 * load the rename character menu onto the screen. ID 6.
 * 
 * @param	playerID	ID of the character whose name will be edited
 */
void menuBottom::playerRenameMenu(u8 playerID)
{
	clearMenus();
	navFunction = &menuBottom::charNameNav;
	renderFunction = &menuBottom::charNameRender;
	actionFunction = &menuBottom::charNameAction;
	menuColSize = 0;
	
	struct menuItem temp1 = {100, 84, 0, 0, "", 0, 0, NULL, NULL};
	menuItems.push_back(temp1);
	struct menuItem temp2 = {108, 84, 0, 0, "", 0, 0, NULL, NULL};
	menuItems.push_back(temp2);
	struct menuItem temp3 = {116, 84, 0, 0, "", 0, 0, NULL, NULL};
	menuItems.push_back(temp3);
	struct menuItem temp4 = {124, 84, 0, 0, "", 0, 0, NULL, NULL};
	menuItems.push_back(temp4);
	struct menuItem temp5 = {132, 84, 0, 0, "", 0, 0, NULL, NULL};
	menuItems.push_back(temp5);
	struct menuItem temp6 = {140, 84, 0, 0, "", 0, 0, NULL, NULL};
	menuItems.push_back(temp6);
	struct menuItem temp7 = {148, 84, 0, 0, "", 0, 0, NULL, NULL};
	menuItems.push_back(temp7);
	struct menuItem temp8 = {156, 84, 0, 0, "", 0, 0, NULL, NULL};
	menuItems.push_back(temp8);
	
	PA_OutputText(screen, 1, 3, "    Pressing A will erase \n     this player's stats.\n\nPRESS B IF YOU DON'T WANT THIS");
	
	//instructions
	makeInstructions(0, 96);
	makeInstructions(6, 160);
}

///load the miscellaneous settings menu onto the screen. ID 8.
void menuBottom::miscMenu()
{
	clearMenus();
	navFunction = &menuBottom::miscNav;
	actionFunction = &menuBottom::miscAction;
	menuColSize = 0;
	
	char data[24];
	sprintf(data, "Max pickups: %d  ", numPickups);
	struct menuItem temp2 = {12, 60, 3, 7, data, 0, 0, NULL, NULL};
	menuItems.push_back(temp2);
	
	strcpy(data, "Pre-show pickups: ");
	strcat(data, (showPickups ? "ON " : "OFF"));
	struct menuItem temp3 = {12, 84, 3, 10, data, 0, 0, NULL, NULL};
	menuItems.push_back(temp3);
	
	strcpy(data, "Die while dancing: ");
	strcat(data, (dieWhileDancing ? "ON " : "OFF"));
	struct menuItem tempA = {12, 108, 3, 13, data, 0, 0, NULL, NULL};
	menuItems.push_back(tempA);
	
	//instructions
	makeInstructions(5, 96);
	makeInstructions(6, 160);
}

///load the statistics viewing menu onto the screen. ID 9.
void menuBottom::statsMenu()
{
	clearMenus();
	navFunction = &menuBottom::statsNav;
	renderFunction = &menuBottom::statsRender;
	actionFunction = &menuBottom::statsAction;
	menuColSize = 7;		//first column = players, 2nd = pages
	
	//players
	struct menuItem temp1 = {8, 52, 2, 6, "All", 0, 0, NULL, NULL};
	menuItems.push_back(temp1);
	struct menuItem temp2 = {8, 68, 2, 8, "P1", 0, 0, NULL, NULL};
	menuItems.push_back(temp2);
	struct menuItem temp3 = {8, 84, 2, 10, "P2", 0, 0, NULL, NULL};
	menuItems.push_back(temp3);
	struct menuItem temp4 = {8, 100, 2, 12, "P3", 0, 0, NULL, NULL};
	menuItems.push_back(temp4);
	struct menuItem temp5 = {8, 116, 2, 14, "P4", 0, 0, NULL, NULL};
	menuItems.push_back(temp5);
	struct menuItem temp6 = {8, 132, 2, 16, "P5", 0, 0, NULL, NULL};
	menuItems.push_back(temp6);
	struct menuItem temp7 = {8, 148, 2, 18, "P6", 0, 0, NULL, NULL};
	menuItems.push_back(temp7);
	
	//pages
	for (u8 i=0; i<4; ++i)
	{
		spriteObject *icon = new spriteObject(this);
		icon->setPallete(cursorSprite.palleteID);
		icon->giveSprite(cursorSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
		icon->setFrame(0);
		u16 x=0;			//x pos
		u16 ix=0;			//icon x pos
		string text;		//text for menuItems
		switch (i)
		{
			case 0:
				x=4; ix=60; text="General";
			break;
			case 1:
				x=13; ix=120; text="Time";
			break;
			case 2:
				x=19; ix=172; text="Kills";
			break;
			case 3:
				x=26; ix=232; text="Deaths";
			break;
		}
		struct menuItem temp = {0, 0, x, 0, text, ix, 4, icon, NULL};
		menuItems.push_back(temp);
	}
	
	autoMenuClickRegions();
	
	//instructions
	makeInstructions(7, 96);
	makeInstructions(6, 160);
	
	//draw first screen
	settingsPlayer = 1;	//now it's different, so screen gets drawn.
	drawStatsFor(0, 0);
}

/**
 * fill all menuItem stylus hitboxes out based on text position and length
 */
void menuBottom::autoMenuClickRegions()
{
	for (u8 i=0; i<menuItems.size(); ++i)
	{
		menuItems[i].x1 = menuItems[i].textx*8;
		menuItems[i].x2 = menuItems[i].x1 + (menuItems[i].text.length())*8;
		menuItems[i].y1 = menuItems[i].texty*8;
		menuItems[i].y2 = menuItems[i].y1+8;
	}
}

/**
 * Find out which menuItem is touched at the coordinates given
 * 
 * @param	x	the x position of the coordinate to check (in pixels)
 * @param	y	the y position of the coordinate to check (in pixels)
 * @return ID of menu item being touched by stylus, or -1 if not touching any
 */
s8 menuBottom::touchingWhichItem(u16 x, u16 y)
{
	for (u8 i=0; i<menuItems.size(); ++i)
	{
		if (menuItems[i].x1 != 0 && menuItems[i].x2 != 0
			&& menuItems[i].y1 != 0 && menuItems[i].y2 != 0
			&& x > menuItems[i].x1 && x < menuItems[i].x2 
			&& y > menuItems[i].y1 && y < menuItems[i].y2)
		{
			return i;
		}
	}
	return -1;
}

//================================================================================

/**
 * Checks whether or not the DS's SRAM has been formatted for MDuel DS, as some flashcarts
 * actually share SRAM between programs (!)
 * 
 * @return true if the SRAM is formatted for MDuel, false if not
 */
bool menuBottom::isSRAMSaved()
{
	#ifdef __WITHSAVING
		char testStr[6];
		PA_LoadData(_SRAMIDOFFSET, testStr, 6);
		return PA_CompareText(testStr, const_cast<char*>(_SRAMID));
	#else
	return true;
	#endif
}

/**
 * Load all saveable variables from the DS's SRAM, or from memory if SRAM has not been formatted.
 * 
 * @param	firstLoad	whether or not the SRAM is being loaded for the very first time.
 * 						if true and SRAM has not been saved before, load default values into memory.
 */
void menuBottom::loadSRAM(bool firstLoad)
{
	#ifdef __WITHSAVING
	if (isSRAMSaved())
	{
		pickupMask = PA_Load32bit(_PICKUPOFFSET);
		roundNum = PA_Load8bit(_ROUNDSOFFSET);
		gameTop->pointsToWin = roundNum;
		char pNames[49];
		PA_LoadString(_NAMESOFFSET, pNames);
		playerNames = pNames;
		player1id = PA_Load8bit(_PLAYER1IDOFFSET);
		player2id = PA_Load8bit(_PLAYER2IDOFFSET);
		PA_LoadData(_PLAYERSPRSOFFSET, &playerSprs, _NUMSAVESLOTS);
		PA_LoadData(_PLAYERPALSOFFSET, &playerPals, _NUMSAVESLOTS);
		u8 settings = PA_Load8bit(_SETTINGSOFFSET);
		dieWhileDancing = settings & 32;
		showPickups = settings & 64;
		numPickups = settings & 31;
		PA_LoadData(_STATSOFFSET, &statistics, _STATSLEN);
	} else if (firstLoad) {								//load defaults for first load only!
		sramLoadDefaults();
		sramSavePrevVars();
	} else {
		sramLoadPrevVars();
	}   
	#else
	if (firstLoad) {
		sramLoadDefaults();
		sramSavePrevVars();
	} else {
		sramLoadPrevVars();
	}
	#endif
}

/**
 * Save all persistant variables into the DS's SRAM for retrieval later.
 * 
 * @param	dontCheck	if true, the SRAM will be formatted and overwritten even if not already formatted
 * @param	menuNumber	the menu to hilight in the main menu after saving. Works as in menuBottom::mainMenu()
 */
void menuBottom::saveSRAM(bool dontCheck, u8 menuNumber)
{
	#ifdef __WITHSAVING
	if (isSRAMSaved() || dontCheck)
	{
		PA_SaveString(_SRAMIDOFFSET, const_cast<char*>(_SRAMID));	//SRAM identifier
		PA_Save8bit(_ROUNDSOFFSET, roundNum);
		PA_Save32bit(_PICKUPOFFSET, pickupMask);
		PA_SaveString(_NAMESOFFSET, const_cast<char*>(playerNames.c_str()));
		PA_Save8bit(_PLAYER1IDOFFSET, player1id);
		PA_Save8bit(_PLAYER2IDOFFSET, player2id);
		PA_SaveData(_PLAYERSPRSOFFSET, playerSprs, _NUMSAVESLOTS);
		PA_SaveData(_PLAYERPALSOFFSET, playerPals, _NUMSAVESLOTS);
		u8 settings = numPickups + (dieWhileDancing*32) + (showPickups*64);
		PA_Save8bit(_SETTINGSOFFSET, settings);
		PA_SaveData(_STATSOFFSET, statistics, _STATSLEN);
		sramSavePrevVars();
		mainMenu(menuNumber);
	} else {
		saveWarningMenu();
	}
	#else
		sramSavePrevVars();
	#endif
}

/**
 * Save data by setting backup variables in memory. This is intended to be used as a faux storage
 * engine for when SRAM is disabled.
 */
void menuBottom::sramSavePrevVars()
{
	prevRoundNum = roundNum;
	prevPickupMask = pickupMask;
	prevPlayerNames = playerNames;
	prevPlayer1id = player1id;
	prevPlayer2id = player2id;
	prevDieWhileDancing = dieWhileDancing;
	prevShowPickups = showPickups;
	prevNumPickups = numPickups;
	
	for (u8 i=0; i<_NUMSAVESLOTS; ++i) {
		prevPlayerSprs[i] = playerSprs[i];
		prevPlayerPals[i] = playerPals[i];
	}
}

/**
 * Load persistent variables from backups. This is intended to be used as a faux storage
 * engine for when SRAM is disabled and a user opts to cancel their changes in a particular menu.
 */
void menuBottom::sramLoadPrevVars()
{
	roundNum = prevRoundNum;
	gameTop->pointsToWin = roundNum;
	pickupMask = prevPickupMask;
	playerNames = prevPlayerNames;
	player1id = prevPlayer1id;
	player2id = prevPlayer2id;
	dieWhileDancing = prevDieWhileDancing;
	showPickups = prevShowPickups;
	numPickups = prevNumPickups;
	
	for (u8 i=0; i<_NUMSAVESLOTS; ++i) {
		playerSprs[i] = prevPlayerSprs[i];
		playerPals[i] = prevPlayerPals[i];
	}
}

/**
 * Set default values for persistent variables when the game is loaded for the first time.
 */
void menuBottom::sramLoadDefaults()
{
	roundNum = 3;
	gameTop->pointsToWin = roundNum;
	pickupMask = _ALLPICKUPS;
	playerNames = "Percy   Howard  Mowbray CliffordStanley Beaufort";
	player1id = 1;
	player2id = 2;
	dieWhileDancing = false;
	showPickups = true;
	numPickups = 3;
	
	playerSprs[0] = 1; playerPals[0] = 1;
	playerSprs[1] = 1; playerPals[1] = 3;
	playerSprs[2] = 2; playerPals[2] = 2;
	playerSprs[3] = 2; playerPals[3] = 2;
	playerSprs[4] = 3; playerPals[4] = 4;
	playerSprs[5] = 3; playerPals[5] = 4;
	
	for (u16 i=0; i<_STATSLEN; ++i)
		statistics[i] = 0;
}

//=========================================================================

/**
 * Modify menuBottom::pickupMask by toggling a desired Pickup. The function toggles the
 * nth bit in the mask.
 * 
 * @param	i	the Pickup number to toggle. Relates to Pickup::pickupType.
 */
void menuBottom::pickupToggle(u8 i)
{
	u32 pickupNum = 1<<i;
	
	if (pickupSelected(i))
		pickupMask &= ~pickupNum;
	else
		pickupMask |= pickupNum;
}

/**
 * Modify a specific char in a character's name by the desired amount.
 * The char will have moved that many places in the ASCII table.
 * 
 * @param	playerID	1-indexed id of the character name to change
 * @param	letterID	letter in that character's name to modify
 * @param	modBy	number of places to change the char by
 */
void menuBottom::alterPlayerName(u8 playerID, u8 letterID, s8 modBy)
{
	playerNames[(playerID-1)*8 + letterID] += modBy;
	
	//now take us away from silly characters that nobody likes
	if (playerNames[(playerID-1)*8 + letterID] == 31)
		playerNames[(playerID-1)*8 + letterID] = 122;
	else if (playerNames[(playerID-1)*8 + letterID] == 123)
		playerNames[(playerID-1)*8 + letterID] = 32;
	else if (playerNames[(playerID-1)*8 + letterID] >= 91 && playerNames[(playerID-1)*8 + letterID] <= 96)
	{
		if (modBy > 0)
			playerNames[(playerID-1)*8 + letterID] = 97;
		else
			playerNames[(playerID-1)*8 + letterID] = 90;
	}
	else if (playerNames[(playerID-1)*8 + letterID] == 38)
	{
		if (modBy > 0)
			playerNames[(playerID-1)*8 + letterID] = 39;
		else
			playerNames[(playerID-1)*8 + letterID] = 37;
	}
	
	#ifdef __MDDEBUG
	PA_OutputText(screen, 5, 0, "%d ", playerNames[(playerID-1)*8 + letterID]);
	#endif
}

/**
 * Change the colour of a character sprite by using a different pallete.
 * 
 * @param	i	1-indexed id of the player to modify palletes for
 */
void menuBottom::playerColorCycle(u8 i)
{
	++playerPals[i];
	if (playerPals[i] == 5)
		playerPals[i] = 1;
	updatePlayerIcon(i);
}

/**
 * Change the sprite of a character by using a different GFX.
 * 
 * @param	i	1-indexed id of the player to modify appearance of
 */
void menuBottom::playerGFXCycle(u8 i)
{
	++playerSprs[i];
	if (playerSprs[i] == 4)
		playerSprs[i] = 1;
	updatePlayerIcon(i);
}

/**
 * Destroy and recreate a character sprite, as a visual property of it has been updated.
 * 
 * @param	i	1-indexed id of the character sprite to modify
 */
void menuBottom::updatePlayerIcon(u8 i)
{
	u8 tempx = menuItems[selectedItem].obj->getx();
	u8 tempy = menuItems[selectedItem].obj->gety();
	menuItems[selectedItem].obj->destroy();
	//overwrite, create another
	menuItems[selectedItem].obj = new spriteObject(this);
	menuItems[selectedItem].obj->setPallete(getPlayerPalleteID(playerPals[i]));
	menuItems[selectedItem].obj->giveSprite(getPlayerSpriteData(playerSprs[i]), OBJ_SIZE_32X32, 16, 16, 0, tempx, tempy);
	//if (selectedItem > 2)
		//menuItems[selectedItem].obj->setFlipped(true);
}

/**
 * Pick a character for use by a player.
 * 
 * @param	i	id of the character to select
 * @param	playerNum	which player has selected this character (player 1 or player 2)
 */
void menuBottom::selectPlayer(u8 i, u8 playerNum)
{
	if (player1id == i+1)
		player1id = player2id;
	else if (player2id == i+1)
		player2id = player1id;		//selecting same - SWAP it.
	
	if (playerNum == 1)
		player1id = i+1;
	else
		player2id = i+1;
}

/**
 * Make the gameManager load the graphics for a Player from the character they have selected.
 * Call this just before a match is started.
 * 
 * @param	dataNum	character save slot to get pallete and GFX from
 * @param	playerNum	which Player to load the character graphics for (player 1 or player 2)
 */
void menuBottom::loadPlayerGFX(u8 dataNum, u8 playerNum)
{
	const unsigned char *sprite = getPlayerSpriteData(playerSprs[dataNum-1]);
	const unsigned short *pal = getPlayerPalleteData(playerPals[dataNum-1]);
		
	if (playerNum == 1)
		gameTop->loadPlayer1SpriteSet(pal, sprite);
	else
		gameTop->loadPlayer2SpriteSet(pal, sprite);
}

/**
 * Retrieve the sprite data for a character GFX id
 * 
 * @param	idNum	id of the GFX to retrieve
 * @return	the raw GFX data for that id
 */
const unsigned char *menuBottom::getPlayerSpriteData(u8 idNum)
{
	switch(idNum)
	{
		case 2:	return playerSprite2;
		case 3:	return playerSprite3;
		default: return playerSprite1;
	}
}

/**
 * Retrieve the pallete data for a character pallete id
 * 
 * @param	idNum	id of the pallete to retrieve
 * @return	the raw pallete data for that id
 */
const unsigned short *menuBottom::getPlayerPalleteData(u8 idNum)
{
	switch(idNum)
	{
		case 2:	return playerPallete2;
		case 3:	return playerPallete3;
		case 4:	return playerPallete4;
		default: return playerPallete1;
	}
}

/**
 * Retrieve the preloaded pallete index for a character pallete id
 * 
 * @param	idNum	id of the pallete to retrieve
 * @return	the loaded pallete index for that pallete
 */
u8 menuBottom::getPlayerPalleteID(u8 idNum)
{
	switch(idNum)
	{
		case 2:	return playerPal2;
		case 3:	return playerPal3;
		case 4:	return playerPal4;
		default: return playerPal1;
	}
}

//=====================================================================================
//							STATISTICS ACCESSORS / MUTATORS
//=====================================================================================

/**
 * A utility function to write a statistic at a specific offset in memory.
 * @see menuBottom::statistics
 * 
 * @param	offset	the memory location (from the beginning of the statistics block) to write to
 * @param	value	the 16-bit value to write at that location
 */
void menuBottom::writeStatValueAt(u16 offset, u16 value)
{
	u8 part1 = value>>8;
	u8 part2 = value&255;
	
	statistics[offset] = part1;
	statistics[offset+1] = part2;
	saveStats();
}

/**
 * A utility function to read a statistic from a specific offset in memory.
 * @see menuBottom::statistics
 * 
 * @param	offset	the memory location (form the beginning of the statistics block) to read from
 * @return	the 16-bit numeric value stored at that location
 */
u16 menuBottom::readStatValueAt(u16 offset) const
{
	//what the shitting balls. Somehow, typecasting individually doesn't cut it!?
	u16 good1 = statistics[offset]<<8;
	u16 good2 = statistics[offset+1];
	u16 good = good1 + good2;
	
	return good;
}

/**
 * Save the game statistics. Designed to be called by the gameManager every time a statistic changes.
 * The stats will only be saved if SRAM has been formatted for MDuel.
 */
void menuBottom::saveStats()
{
	if (isSRAMSaved())
		PA_SaveData(_STATSOFFSET, statistics, _STATSLEN);
}

/**
 * A Player scored a kill during a match - save a record of this into SRAM.
 * 
 * @param	player	the 1-indexed id of the character who made the kill
 * @param	weaponID	the id of the weapon the kill was made with. Relates to Pickup::pickupType,
 * 						except for id 18 which denotes a drowning.
 */
void menuBottom::scoreKill(u8 player, u8 weaponID)
{
	player--;	//1 indexed :p
	u16 location = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*player)+(weaponID*2);
	writeStatValueAt(location, readStatValueAt(location)+1);
}

/**
 * A Player died during a match - save a record of this into SRAM.
 * 
 * @param	player	the 1-indexed id of the character who died
 * @param	weaponID	the id of the weapon that caused the kill. Relates to Pickup::pickupType,
 * 						except for id 18 which denotes a drowning.
 */
void menuBottom::scoreDeath(u8 player, u8 weaponID)
{
	player--;	//1 indexed :p
	u16 location = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*player)+(WEAPONBLOCKSIZE*2)+(weaponID*2);
	writeStatValueAt(location, readStatValueAt(location)+1);
	
	//score global kill as well
	location = weaponID*2;
	writeStatValueAt(location, readStatValueAt(location)+1);
}

/**
 * A Player switched weapons - save a record of how long they were holding their old weapon for.
 * 
 * @param	player	the 1-indexed id of the character who changed weapons
 * @param	weaponID	the id of the weapon that was just dropped. 18 denotes the Player had no weapon.
 * @param	timeSecs	the total time the weapon was held for
 */
void menuBottom::scoreTime(u8 player, u8 weaponID, u16 timeSecs)
{
	player--;	//1 indexed :p
	u16 location = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*player)+WEAPONBLOCKSIZE+(weaponID*2);
	writeStatValueAt(location, readStatValueAt(location)+timeSecs);
	
	//score global time as well
	location = WEAPONBLOCKSIZE+weaponID*2;
	writeStatValueAt(location, readStatValueAt(location)+timeSecs);
}

/**
 * A round has ended - accumulate and save the winner and loser's points in SRAM.
 * 
 * @param	winner	1-indexed id of the character who won the round
 * @param	loser	1-indexed id of the character who lost the round
 * @param	tied	if true, both players store ties instead of a win and a loss
 */
void menuBottom::scoreRound(u8 winner, u8 loser, bool tied)
{
	winner--;
	loser--;	//1 indexed :p
	
	u16 winlocation = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*winner)+(WEAPONBLOCKSIZE*3);
	u16 loselocation = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*loser)+(WEAPONBLOCKSIZE*3)+2;
	if (tied)
	{
		winlocation += 4;
		loselocation += 2;
	}
	
	writeStatValueAt(winlocation, readStatValueAt(winlocation)+1);
	writeStatValueAt(loselocation, readStatValueAt(loselocation)+1);
		
	//score global round as well
	u16 location = (WEAPONBLOCKSIZE*2);
	writeStatValueAt(location, readStatValueAt(location)+1);
}

/**
 * Completely clear out all statistical data for a character.
 * 
 * @param	player	1-indexed id of the character whose data should be erased
 */
void menuBottom::erasePlayer(u8 player)
{
	player--;
	u16 startPos = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*player);
	for (u16 i=0; i < (WEAPONBLOCKSIZE*3)+6; ++i)
	{
		statistics[startPos+i] = 0;
		//writeStatValueAt(startPos+i, 0);	//seems liable to run outside the range
	}
}

/**
 * Retrieve the number of kills a character has made with a specific weapon.
 * 
 * @param	player 		1-indexed id of the character to retrieve data for
 * 						If -1, the number of kills in total (for all character) is retrieved.
 * @param	weaponID	the id of the weapon to check data for. 18 denotes drownings.
 * 						If -1, the number of kills with all weapons is retrieved.
 */
u16 menuBottom::getKillsForPlayer(s8 player, s8 weaponID) const
{
	u16 location;
	if (player == -1)
		location = 0;
	else {
		player--;
		location = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*player);
	}
	
	if (weaponID != -1)
		return readStatValueAt(location+weaponID*2);
	
	u16 total = 0;
	for (u8 i=0; i < WEAPONBLOCKSIZE; i+=2)
	{
		total += readStatValueAt(location+i);
	}
	return total;
}

/**
 * Retrieve the number of deaths a character has with a specific weapon.
 * 
 * @param	player 		1-indexed id of the character to retrieve data for
 * 						If -1, the number of deaths in total (for all character) is retrieved.
 * @param	weaponID	the id of the weapon to check data for. 18 denotes drownings.
 * 						If -1, the number of deaths with all weapons is retrieved.
 */
u16 menuBottom::getDeathsForPlayer(s8 player, s8 weaponID) const
{
	u16 location;
	if (player == -1)	//cant get global deaths stat
		return 0;
	else {
		player--;
		location = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*player)+(WEAPONBLOCKSIZE*2);
	}
	
	if (weaponID != -1)
		return readStatValueAt(location+weaponID*2);
	
	u16 total = 0;
	for (u8 i=0; i < WEAPONBLOCKSIZE; i+=2)
	{
		total += readStatValueAt(location+i);
	}
	return total;
}

/**
 * Retrieve the time (in seconds) a character has spent with a specific weapon.
 * 
 * @param	player 		1-indexed id of the character to retrieve data for
 * 						If -1, the playtime in total (for all character) is retrieved.
 * @param	weaponID	the id of the weapon to check data for. 18 denotes drownings.
 * 						If -1, the total playtime for this player is retrieved.
 */
u16 menuBottom::getTimeForPlayer(s8 player, s8 weaponID) const
{
	u16 location;
	if (player == -1)
		location = WEAPONBLOCKSIZE;
	else {
		player--;
		location = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*player)+WEAPONBLOCKSIZE;
	}
	
	if (weaponID != -1)
		return readStatValueAt(location+weaponID*2);
	
	u16 total = 0;
	for (u8 i=0; i < WEAPONBLOCKSIZE; i+=2)
	{
		total += readStatValueAt(location+i);
	}
	return total;
}

/**
 * Retrieve the number of round wins a character has had.
 * 
 * @param	player 		1-indexed id of the character to retrieve data for
 * 						If -1, the number of rounds played in total (for all character) is retrieved.
 */
u16 menuBottom::getWins(s8 player) const
{
	u16 location;
	if (player == -1)
		location = WEAPONBLOCKSIZE*2;
	else {
		player--;
		location = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*player)+(WEAPONBLOCKSIZE*3);
	}
	return readStatValueAt(location);
}

/**
 * Retrieve the number of round losses a character has had.
 * 
 * @param	player 		1-indexed id of the character to retrieve data for
 * 						If -1, 0 is returned as there is no global loss counter (its redundant!)
 */
u16 menuBottom::getLosses(s8 player) const
{
	u16 location;
	if (player == -1)
		return 0;	//cant get global losses stat
	else {
		player--;
		location = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*player)+(WEAPONBLOCKSIZE*3)+2;
	}
	return readStatValueAt(location);
}

/**
 * Retrieve the number of round ties a character has had.
 * 
 * @param	player 		1-indexed id of the character to retrieve data for
 * 						If -1, 0 is returned as there is no global tie counter (its redundant!)
 */
u16 menuBottom::getTies(s8 player) const
{
	u16 location;
	if (player == -1)
		return 0;	//cant get global ties stat
	else {
		player--;
		location = GLOBALSLOTSIZE+(PLAYERSLOTSIZE*player)+(WEAPONBLOCKSIZE*3)+4;
	}
	return readStatValueAt(location);
}


/**
 * This function draws a state page for the character/page combo.
 * Called every tick when on stats page, so it checks that player & page
 * are different than the tick before before doing anything.
 * 
 * @param	player	1-indexed id of the character's stats to be retrieved.
 * 					if -1, the stats for the game as a whole are retrieved.
 * @param	page	the page we are drawing. Page 0 is a global statistics page, page 1 is weapon
 * 					time, page 2 is kills and page 3 is deaths.
 */
void menuBottom::drawStatsFor(u8 player, u8 page)
{
	if (settingsPlayer == player && settingsPage == page)
		return;
	settingsPlayer = player;
	settingsPage = page;
	
	//clear text
	resetText();
	
	//draw this player's name
	if (player != 0)
		PA_OutputText(screen, 4, 2, "Player: %s", macros::getPlayerName(player, playerNames, false).c_str());
	
	u8 playerSlot = (player==0 ? -1 : player);
		
	char buf[21];	//buffer for all text output
	u16 (menuBottom::*statFunction)(s8, s8) const = NULL;
	
	switch (page)			//general stats - different format
	{
		case 0:
			sprintf(buf, "Total playtime:   %5d", getTimeForPlayer(playerSlot, -1)/(player==0 ? 2 : 1));
				//playtime for global value will be half of what is recorded,
				//since recording happens for both players simultaneously
			PA_OutputText(screen, 8,  5, buf);
			sprintf(buf, "Rounds played:    %5d", (player==0 ? getWins(playerSlot) : getWins(playerSlot)+getLosses(playerSlot)+getTies(playerSlot)));
			PA_OutputText(screen, 8,  7, buf);
			sprintf(buf, "Total kills:      %5d", getKillsForPlayer(playerSlot, -1));
			PA_OutputText(screen, 8,  9, buf);
			if (player != 0)
			{
				sprintf(buf, "Total deaths:     %5d", getDeathsForPlayer(playerSlot, -1));
				PA_OutputText(screen, 8, 11, buf);
				sprintf(buf, "Number of wins:   %5d", getWins(playerSlot));
				PA_OutputText(screen, 8, 13, buf);
				sprintf(buf, "Number of losses: %5d", getLosses(playerSlot));
				PA_OutputText(screen, 8, 15, buf);
				sprintf(buf, "Number of ties:   %5d", getTies(playerSlot));
				PA_OutputText(screen, 8, 17, buf);
			}
		break;
		case 1:				//all other pages are segregated by weapon
			PA_OutputText(screen, 7,  5, "Skull   N/A");
			sprintf(buf, "1000V %5d", getTimeForPlayer(playerSlot, 1));
			PA_OutputText(screen, 7,  7, buf);
			sprintf(buf, "Invis %5d", getTimeForPlayer(playerSlot, 2));
			PA_OutputText(screen, 7,  9, buf);
			sprintf(buf, " Mine %5d", getTimeForPlayer(playerSlot, 3));
			PA_OutputText(screen, 7, 11, buf);
			sprintf(buf, "  Gun %5d", getTimeForPlayer(playerSlot, 4));
			PA_OutputText(screen, 7, 13, buf);
			PA_OutputText(screen, 7, 15, "  TNT   N/A");
			sprintf(buf, "Boots %5d", getTimeForPlayer(playerSlot, 6));
			PA_OutputText(screen, 7, 17, buf);
			sprintf(buf, " Gren %5d", getTimeForPlayer(playerSlot, 7));
			PA_OutputText(screen, 7, 19, buf);
			sprintf(buf, " Puck %5d", getTimeForPlayer(playerSlot, 8));
			PA_OutputText(screen, 7, 21, buf);
			sprintf(buf, "   None %5d", getTimeForPlayer(playerSlot, 18));
			PA_OutputText(screen, 19, 3, buf);
			sprintf(buf, "Paracht %5d", getTimeForPlayer(playerSlot, 9));
			PA_OutputText(screen, 19,  5, buf);
			sprintf(buf, "   Hook %5d", getTimeForPlayer(playerSlot, 10));
			PA_OutputText(screen, 19,  7, buf);
			sprintf(buf, "   Warp %5d", getTimeForPlayer(playerSlot, 11));
			PA_OutputText(screen, 19,  9, buf);
			sprintf(buf, " Magnet %5d", getTimeForPlayer(playerSlot, 12));
			PA_OutputText(screen, 19, 11, buf);
			sprintf(buf, "    Net %5d", getTimeForPlayer(playerSlot, 13));
			PA_OutputText(screen, 19, 13, buf);
			sprintf(buf, " Shield %5d", getTimeForPlayer(playerSlot, 14));
			PA_OutputText(screen, 19, 15, buf);
			sprintf(buf, "  Dunce %5d", getTimeForPlayer(playerSlot, 15));
			PA_OutputText(screen, 19, 17, buf);
			sprintf(buf, " Weasel %5d", getTimeForPlayer(playerSlot, 16));
			PA_OutputText(screen, 19, 19, buf);
			sprintf(buf, "Boomrng %5d", getTimeForPlayer(playerSlot, 17));
			PA_OutputText(screen, 19, 21, buf);
		break;
		
		case 2: case 3:
			if (page == 2)
				statFunction = &menuBottom::getKillsForPlayer;
			else
				statFunction = &menuBottom::getDeathsForPlayer;
			
			sprintf(buf, "Skull %5d", (this->*statFunction)(playerSlot, 0));
			PA_OutputText(screen, 7,  5, buf);
			sprintf(buf, "1000V %5d", (this->*statFunction)(playerSlot, 1));
			PA_OutputText(screen, 7,  7, buf);
			sprintf(buf, "Invis %5d", (this->*statFunction)(playerSlot, 2));
			PA_OutputText(screen, 7,  9, buf);
			sprintf(buf, " Mine %5d", (this->*statFunction)(playerSlot, 3));
			PA_OutputText(screen, 7, 11, buf);
			sprintf(buf, "  Gun %5d", (this->*statFunction)(playerSlot, 4));
			PA_OutputText(screen, 7, 13, buf);
			sprintf(buf, "  TNT %5d", (this->*statFunction)(playerSlot, 5));
			PA_OutputText(screen, 7, 15, buf);
			PA_OutputText(screen, 7, 17, "Boots   N/A");
			sprintf(buf, " Gren %5d", (this->*statFunction)(playerSlot, 7));
			PA_OutputText(screen, 7, 19, buf);
			sprintf(buf, " Puck %5d", (this->*statFunction)(playerSlot, 8));
			PA_OutputText(screen, 7, 21, buf);
			sprintf(buf, (page == 3 ? "   FIDs %5d" : " Tackle %5d"), (this->*statFunction)(playerSlot, 18));
			PA_OutputText(screen, 19,  3, buf);
			PA_OutputText(screen, 19,  5, "Paracht   N/A");
			PA_OutputText(screen, 19,  7, "   Hook   N/A");
			sprintf(buf, "   Warp %5d", (this->*statFunction)(playerSlot, 11));
			PA_OutputText(screen, 19,  9, buf);
			sprintf(buf, " Magnet %5d", (this->*statFunction)(playerSlot, 12));
			PA_OutputText(screen, 19, 11, buf);
			sprintf(buf, "    Net %5d", (this->*statFunction)(playerSlot, 13));
			PA_OutputText(screen, 19, 13, buf);
			sprintf(buf, " Shield %5d", (this->*statFunction)(playerSlot, 14));
			PA_OutputText(screen, 19, 15, buf);
			PA_OutputText(screen, 19, 17, "  Dunce   N/A");
			sprintf(buf, " Weasel %5d", (this->*statFunction)(playerSlot, 16));
			PA_OutputText(screen, 19, 19, buf);
			sprintf(buf, "Boomrng %5d", (this->*statFunction)(playerSlot, 17));
			PA_OutputText(screen, 19, 21, buf);
		break;
	}
}
