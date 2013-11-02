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

#include "menuGBottom.h"
#include "gameManager.h"
#include "menuBottom.h"
#include "pickupSpawner.h"

/**
 * create the status screen and give it a screen to display on
 * 
 * @param	newScreen	the screen this spriteManager should handle. 1 is the top screen, 0 is the bottom.
 */
menuGBottom::menuGBottom(u8 newScreen) : spriteManager(newScreen), gameTop(NULL), menuTop(NULL), menuBase(NULL), 
	gameOver(false), gamePaused(false), pickupsActive(0), continueMsg(NULL), quitMsg(NULL), spawnerSpriteL(NULL),
	spawnerSpriteR(NULL)
{
	
}
	
menuGBottom::~menuGBottom()
{
	
}

/**
 * 'Tick' function for the status screen that redraws scores and animates pre-emptive Pickup spawning.
 * It also handles the 'game over' screen and pausing of the game.
 */
void menuGBottom::gameTick()
{
	if (!gamePaused)
		spriteManager::gameTick();
	
	PA_OutputText(screen, (gameTop->pointsToWin > 10 ? 1 : 2), 22, "%d / %d", gameTop->p1score, gameTop->pointsToWin);
	PA_OutputText(screen, (gameTop->pointsToWin > 10 ? 22 : 23), 22, "%d / %d", gameTop->p2score, gameTop->pointsToWin);
		
	if (gameOver)
	{
		if (continueMsg == NULL)
		{
			continueMsg = new spriteObject(this);
			continueMsg->setPallete(menuSprite.palleteID);
			continueMsg->giveSprite(menuSprite.spriteData, OBJ_SIZE_64X32, 32, 16, 0, 128, 191-8);
			continueMsg->setLayer(0);
			continueMsg->setFrame(7);
			continueMsg->makeStatic();
		}
		
		if (gameTop->p1score == gameTop->p2score)
			PA_OutputText(screen, 9, 20, "Game Tied!");
		else
			PA_OutputText(screen, 9, 20, "%s wins!", (gameTop->p1score > gameTop->p2score ? const_cast<char*>(macros::getPlayerName(menuBase->player1id, menuBase->playerNames, false).c_str()) : const_cast<char*>(macros::getPlayerName(menuBase->player2id, menuBase->playerNames, false).c_str())));
		if (Pad.Released.A)
		{
			gameTop->deactivate();
			deactivate();
			PA_WaitForVBL();
			menuTop->activate();
			menuBase->activate();
		}
	}
	else if (gamePaused)
	{
		if (continueMsg == NULL)
		{
			continueMsg = new spriteObject(this);
			continueMsg->setPallete(menuSprite.palleteID);
			continueMsg->giveSprite(menuSprite.spriteData, OBJ_SIZE_64X32, 32, 16, 0, 96, 191-8);
			continueMsg->setLayer(0);
			continueMsg->setFrame(7);
			continueMsg->makeStatic();
			PA_OutputText(screen, 10, 20, "Game Paused");
		}
		if (quitMsg == NULL)
		{
			quitMsg = new spriteObject(this);
			quitMsg->setPallete(menuSprite.palleteID);
			quitMsg->giveSprite(menuSprite.spriteData, OBJ_SIZE_64X32, 32, 16, 0, 160, 191-8);
			quitMsg->setLayer(0);
			quitMsg->setFrame(8);
			quitMsg->makeStatic();
		}
		
		if (Pad.Released.B)
		{
			//have to do all this stuff first or the game's goin to crash bigtime
			for (u8 i=0; i<3; ++i)
				gameTop->pickupSpawners[i] = NULL;
			//gameTop->clearOutSprites();
			if (gameTop->player1 != NULL)
				gameTop->player1 = NULL;
			if (gameTop->player2 != NULL)
				gameTop->player2 = NULL;
			//gameTop->refreshPalletes();
			
			gameTop->deactivate();
			deactivate();
			PA_WaitForVBL();
			menuTop->activate();
			menuBase->activate();
			//start main timer
			UnpauseTimer(gameTop->statTimerID);
		} else if (Pad.Released.A)		//with release means player2 doesnt move afterwards
		{
			gamePaused = false;
			//start main timer
			UnpauseTimer(gameTop->statTimerID);
		}
	} else {
		if (continueMsg != NULL)
		{
			continueMsg->destroy();
			continueMsg = NULL;
			PA_OutputText(screen, 10, 20, "           ");
		}
		if (quitMsg != NULL)
		{
			quitMsg->destroy();
			quitMsg = NULL;
		}
		for (u8 i=0; i<3; ++i)
		{
			if (spawnerCounters[i] == 0 && gameTop->pickupMask.size() > 0 && pickupsActive < menuBase->numPickups && gameTop->allowPickupSpawning())
				spawnPickup(i);
			if (spawnerCounters[i] > -1)
				spawnerCounters[i]--;
		}
	}
}

/**
 * Reset the spawn counter for a pickupSpawner to a random number. When this counter reaches zero again,
 * a Pickup is ready to spawn.
 * 
 * @param	spawnerNum	the id of the pickupSpawner in gameManager that should be reset to fire again
 */
void menuGBottom::resetIdleTime(u8 spawnerNum)
{
	if (menuBase->numPickups < 3) {		//pick a random one if we have less Pickups than spawners
		spawnerNum = PA_RandMax(2);
		
		u8 i = 0;
		while (spawnerCounters[spawnerNum] > 0 && i < 3) {
			++spawnerNum;
			spawnerNum %= 3;
			++i;
		}
	}
	spawnerCounters[spawnerNum] = PA_RandMinMax(pickupSpawner::IDLEMIN, pickupSpawner::IDLEMAX)*spriteObject::TICKSPERFRAME;
}

/**
 * Pick a random Pickup icon to spawn and make it travel down whichever spawner tube is ready
 * @see gameManager::pickupSpawners
 * @see gameManager::pickupMask
 * 
 * @param	i	id of the spawner tube to travel down
 */
void menuGBottom::spawnPickup(u8 i)
{
	//spawn a pickup icon on the bottom screen
	u8 pType = PA_RandMax(gameTop->pickupMask.size()-1);
	
	spriteObject *icon = new spriteObject(this);
	icon->setPallete(pickupSprite.palleteID);
	icon->giveSprite(pickupSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
	icon->setFrame(gameTop->pickupMask[pType]+3);
	icon->setLayer(1);
	spriteObject *icons = new spriteObject(this);
	icons->setPallete(pickupSprite.palleteID);
	icons->giveSprite(pickupSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
	icons->setAnim(0,2,spriteObject::ANIMSPEED);
	icons->setLayer(1);
	icons->addChild(icon);
	icons->setPos(128, 100);
	float speedx, speedy;
	if (i==2)
	{
		speedx = 0;
		speedy = -1;
		icons->setLifetime(2);
	} else if (i==1) {
		speedx = 1;
		speedy = 0;
		icons->setLifetime(3);
	} else {
		speedx = -1;
		speedy = 0;
		icons->setLifetime(3);
	}
	icons->setSpeed(speedx, speedy);
	
	//now tell top screen to spawn it soon
	if (gameTop->pickupSpawners[i] != NULL)
		gameTop->pickupSpawners[i]->addSpawnTime(3, gameTop->pickupMask[pType]);
	
	++pickupsActive;
	resetIdleTime(i);
	
	
	//make pretty flashy animations
	if (menuBase->showPickups) {
		u8 f[3] = {3, 3, 5};
		vector<u8> temp(f, f+3);
		spawnerSpriteL->setArbitraryAnim(temp, false);
		u8 g[3] = {4, 4, 6};
		vector<u8> temp2(g, g+3);
		spawnerSpriteR->setArbitraryAnim(temp2, false);
	}
}

/**
 * Load the background graphics into this screen. These graphics combine to form the Pickup
 * generator in the center of the screen, and scoreboard at the bottom.
 */
void menuGBottom::loadBackground()
{
	//PA_SetVideoMode(screen, 1);
	PA_EasyBgLoad(screen, 3, ::spawnerBG);
		
	u8 spriteOffset = (menuBase->showPickups ? _SPAWNERSPRITEMININDEX : 0);
	
	spriteObject * spawnerSprites[8];
	
	for (u8 i=0; i<10; ++i)
	{
		spawnerSprites[i] = new spriteObject(this);
		spawnerSprites[i]->setPallete(spawnerSprite.palleteID);
		spawnerSprites[i]->giveSprite(spawnerSprite.spriteData, OBJ_SIZE_64X64, 32, 32, spriteOffset);
		
		if (i<4)
			spawnerSprites[i]->setLayer(0);
		else {
			spawnerSprites[i]->setLayer(1);
			if (menuBase->showPickups)		//just dont transparentify the glass to hide :P
				spawnerSprites[i]->setTransparency(6);
		}
	}
	
	spawnerSprites[0]->setFrame(0);
	spawnerSprites[0]->setPos(128, -27);
	spawnerSprites[1]->setFrame(1);
	spawnerSprites[1]->setPos(128, 37);
	spawnerSprites[2]->setFrame(2);
	spawnerSprites[2]->setPos(32, 101);
	spawnerSprites[3]->setFrame(2);
	spawnerSprites[3]->setPos(224, 101);
	spawnerSprites[3]->setFlipped(true);
	
	spawnerSprites[4]->setFrame(9);
	spawnerSprites[4]->setPos(32, 101);
	spawnerSprites[5]->setFrame(10);
	spawnerSprites[5]->setPos(96, 101);
	spawnerSprites[6]->setFrame(10);
	spawnerSprites[6]->setPos(160, 101);
	spawnerSprites[6]->setFlipped(true);
	spawnerSprites[7]->setFrame(9);
	spawnerSprites[7]->setPos(224, 101);
	spawnerSprites[7]->setFlipped(true);
	spawnerSprites[8]->setFrame(7);
	spawnerSprites[8]->setPos(128, -27);
	spawnerSprites[9]->setFrame(8);
	spawnerSprites[9]->setPos(128, 37);
	
	spawnerSpriteL = new spriteObject(this);
	spawnerSpriteL->setPallete(spawnerSprite.palleteID);
	spawnerSpriteL->giveSprite(spawnerSprite.spriteData, OBJ_SIZE_64X64, 32, 32, spriteOffset, 96, 101);
	spawnerSpriteL->setLayer(0);
	spawnerSpriteL->setFrame(5);
	spawnerSpriteR = new spriteObject(this);
	spawnerSpriteR->setPallete(spawnerSprite.palleteID);
	spawnerSpriteR->giveSprite(spawnerSprite.spriteData, OBJ_SIZE_64X64, 32, 32, spriteOffset, 160, 101);
	spawnerSpriteR->setLayer(0);
	spawnerSpriteR->setFrame(6);

	spawnerSpriteL->updateSprite();
	spawnerSpriteR->updateSprite();
	spawnerSpriteL->makeStatic();		//maybe not..?
	spawnerSpriteR->makeStatic();
	
	for (u8 i=0; i<10; ++i)
	{
		spawnerSprites[i]->updateSprite();
		spawnerSprites[i]->makeStatic();
	}
	
	//this text goes here because we only need to draw it once
	PA_OutputText(screen, 1, 21, const_cast<char*>(macros::getPlayerName(menuBase->player1id, menuBase->playerNames).c_str()));
	PA_OutputText(screen, 22, 21, const_cast<char*>(macros::getPlayerName(menuBase->player2id, menuBase->playerNames).c_str()));
	PA_OutputText(screen, 15, 21, "VS");
}

///load the sprite data for the menu instructions
void menuGBottom::loadMenuSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&menuSprite, p, s);
}

///load the sprite data for the Pickup generator
void menuGBottom::loadSpawnerSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&spawnerSprite, p, s);
}

///load the sprite data for the Pickup icons
void menuGBottom::loadPickupSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&pickupSprite, p, s);
}
