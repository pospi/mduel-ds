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

#include "gameManager.h"
#include "player.h"
#include "pickup.h"
#include "menuGBottom.h"
#include "menuBottom.h"

gameManager::~gameManager()
{
	
}

/**
 * Reset the game in order to start a new round. This function performs the actions of 
 * spriteManager::clearOutSprites() in addition to resetting game objects, Player%s and scores.
 * 
 * @param	ending	whether or not the game is ending and going back to the frontend menus. If ending,
 * 					draw the game summary screen with victory dancing guy etc.
 */
void gameManager::resetGame(bool ending)
{
	for (u8 i=0; i<3; ++i)
		pickupSpawners[i] = NULL;
				
	//clear the bottom screen first.
	//resetting its spawner counters should stop it NULLing while we're resetting.
	menuGBottom * bottomScreen = dynamic_cast<menuGBottom*>(gameBottom);
	//bottomScreen->deletePickups(); //makes it crash some
	for (u8 i=0; i<3; ++i)
		bottomScreen->resetIdleTime(i);
	
	//clear out old stuff
	clearOutSprites();	
		
	//handle scores and round counter
	if (!(p1RoundScore && p2RoundScore) && !(!p1RoundScore && !p2RoundScore))
	{
		p1score += p1RoundScore;
		p2score += p2RoundScore;
	}
	p1RoundScore = p2RoundScore = 0;
	++roundNumber;
	
	refreshPalletes();
	preloadGFX();
	
	if (!ending)		//add new stuff for next round
	{
		if (player1 == NULL)
		{
			player1 = new Player(this);
			player1->setPallete(player1Sprite.palleteID);
			u16 gfxId = loadGFX(player1Sprite.spriteData, OBJ_SIZE_32X32, 1);
			player1->giveGFX(gfxId, OBJ_SIZE_32X32, 16, 16, 0, 32, 192-32);
			player1->setStartPos(32, 192-32);
			player1->setStartFlipped(false);
			player1->setControls(1);
			player1->setRotSet(0);
			
			#ifdef __WITHSOUND
			player1->playSound(&spawnPuff);
			#endif
		}
		if (player2 == NULL)
		{
			player2 = new Player(this);
			player2->setPallete(player2Sprite.palleteID);
			u16 gfxId = loadGFX(player2Sprite.spriteData, OBJ_SIZE_32X32, 1);
			player2->giveGFX(gfxId, OBJ_SIZE_32X32, 16, 16, 0, 256-32, 192-32);
			player2->setStartPos(256-32, 192-32);
			player2->setStartFlipped(true);
			player2->setControls(2);
			player2->setRotSet(1);
			
			#ifdef __WITHSOUND
			player2->playSound(&spawnPuff);
			#endif
		}
		
		generateRopes();
		generateBricks();
		generateSpawners();
		drawMallow();
		
		u8 f[3] = {21, 22, 23};
		vector<u8> temp(f, f+3);
		createSingleFireSprite(FXSprite.palleteID, spawnGFX[0], temp, spriteObject::TICKSPERFRAME*2, player1->getx(), player1->gety(), OBJ_SIZE_32X32, 16, 16);
		createSingleFireSprite(FXSprite.palleteID, spawnGFX[1], temp, spriteObject::TICKSPERFRAME*2, player2->getx(), player2->gety(), OBJ_SIZE_32X32, 16, 16);
	} else {
		//victory dancing guy!!
		spriteObject *dancy = new spriteObject(this);
		dancy->setPallete((p1score > p2score ? player1Sprite.palleteID : player2Sprite.palleteID));
		dancy->giveSprite((p1score > p2score ? player1Sprite.spriteData : player2Sprite.spriteData), OBJ_SIZE_32X32, 16, 16, 0, 127, 95);
		dancy->setLayer(1);
		u8 f[10] = {58, 59, 60, 61, 62, 63, 63, 63, 63, 63};
		vector<u8> temp(f, f+10);
		dancy->setArbitraryAnim(temp, true, spriteObject::TICKSPERFRAME);
		//dancy->makeStatic();
		
		bottomScreen->gameOver = true;
	}
}

/**
 * Check whether or not the game is paused. When paused, the main loop is not executed, which is
 * a very efficient way of stopping the action.
 * 
 * @return true if the game is paused, false otherwise
 */
bool gameManager::isGamePaused()
{ 
	return dynamic_cast<menuGBottom*>(gameBottom)->gamePaused;
}

/**
 * Preload commonly used GFX for the gameManager%'s sprites to share. These GFX encompass
 * Player effects (gameManager::spawnGFX), boots (gameManager::bootsGFX) and dunce hats (gameManager::dunceGFX).
 */
void gameManager::preloadGFX()
{
	//pickup graphics
	for (u8 i=0; i < menuBase->numPickups+2; ++i)		//load 2 extra lots of GFX for potential magneted Pickups
	{
		pickupGraphics p;
		p.spawnGFX = loadGFX(FXSprite.spriteData, OBJ_SIZE_32X32);
		p.dieGFX = loadGFX(FXSprite.spriteData, OBJ_SIZE_32X32);
		p.blobGFX = loadGFX(pickupSprite.spriteData, OBJ_SIZE_16X16);
		p.iconGFX = loadGFX(pickupSprite.spriteData, OBJ_SIZE_16X16);
		p.bInUse = false;
		pickupGFX.push_back(p);
	}
	//player spawn & splash effects
	spawnGFX[0] = loadGFX(FXSprite.spriteData, OBJ_SIZE_32X32, 1);
	spawnGFX[1] = loadGFX(FXSprite.spriteData, OBJ_SIZE_32X32, 1);
	//player persistent effects (lightning aura, etc)
	spawnGFX[2] = loadGFX(FXSprite.spriteData, OBJ_SIZE_32X32, 1);
	spawnGFX[3] = loadGFX(FXSprite.spriteData, OBJ_SIZE_32X32, 1);
	//dunce hats!
	dunceGFX[0] = loadGFX(dunceHatSprite.spriteData, OBJ_SIZE_32X32, 1);
	dunceGFX[1] = loadGFX(dunceHatSprite.spriteData, OBJ_SIZE_32X32, 1);
	//totally awesome boots!
	bootsGFX[0] = loadGFX(bootsSprite.spriteData, OBJ_SIZE_32X32, 1);
	bootsGFX[1] = loadGFX(bootsSprite.spriteData, OBJ_SIZE_32X32, 1);
}

/**
 * Unload all preloaded GFX from memory. Not currently in use.
 * @see gameManager::preloadGFX()
 */
void gameManager::unloadGFX()
{
	pickupGFX.clear();
	//TODO: the above wont actually clear those GFX from memory
	PA_DeleteGfx(screen, spawnGFX[0]);
	PA_DeleteGfx(screen, spawnGFX[1]);
	PA_DeleteGfx(screen, spawnGFX[2]);
	PA_DeleteGfx(screen, spawnGFX[3]);
	PA_DeleteGfx(screen, dunceGFX[0]);
	PA_DeleteGfx(screen, dunceGFX[1]);
	PA_DeleteGfx(screen, bootsGFX[0]);
	PA_DeleteGfx(screen, bootsGFX[1]);
}

/**
 * Find the next set of Pickup GFX that are not in use and return them.
 * @return the pickupGraphics set that is next available, or NULL if we have run out
 */
gameManager::pickupGraphics* gameManager::getNextAvailablePickupGFXSet()
{
	vector<pickupGraphics>::iterator it = pickupGFX.begin();
	while (it != pickupGFX.end())
	{
		if (!(*it).bInUse)
			return &(*it);
		++it;
	}
	return NULL;
}

//==============================================================================

/**
 * Generate a playfield made out of floorTile%s.
 * 
 * The same number of tiles are generated each time, even though due to the random nature
 * of the game, some are not used. These extra tiles are simply placed offscreen for the round.
 */
void gameManager::generateBricks()
{
	u16 tileGFX = loadGFX(tileSprite.spriteData, OBJ_SIZE_16X16, 1);
	for (int i=0; i < 51; ++i)
	{
		floorTile *temp = new floorTile(this);
		temp->setPallete(tileSprite.palleteID);
		temp->giveGFX(tileGFX, OBJ_SIZE_16X16, 8, 8, 0, OFFX, OFFY);
		
		if (i<3)
			temp->setPos(i*16+24, 192-32+8);
		else if (i<6)
			temp->setPos(256-24-((i-3)*16), 192-32+8);
		else if (i>47)
			temp->setPos(256-16-8-((i-47)*16), 192-32+8+4*-32);
		else if (i>44)
			temp->setPos((i-45)*16+16+16+8, 192-32+8+4*-32);
		else {
			u8 j = i-6;
			u8 col = j%13;
			u8 row = j/13;
			if (PA_RandMax(10) < 7 || (
				(gameSprites.size()-2 > 0 && gameSprites[gameSprites.size()-2]->getx() > 0) && 
				(gameSprites.size()-3 > 0 && gameSprites[gameSprites.size()-3]->getx() < 0) ))
				temp->setPos((col+1)*16+16, 192-32+8+((row+1)*-32));
		}
		temp->makeStatic();
	}
}

/**
 * Generate the gameManager'%s Rope%s which Player%s can use to navigate up and down the playfield.
 */
void gameManager::generateRopes()
{
	u16 ropeHeadGFX = loadGFX(tileSprite.spriteData, OBJ_SIZE_16X16, 1);
	u16 ropeGFX = loadGFX(tileSprite.spriteData, OBJ_SIZE_16X16, 1);
	
	Rope *r1 = new Rope(this);
	r1->setPallete(tileSprite.palleteID);
	r1->giveGFX(ropeHeadGFX, OBJ_SIZE_16X16, 8, 8);
	r1->childGFX = ropeGFX;
	r1->setPos(56, 3);
	r1->setLength(9);
	r1->makeStatic();
	
	Rope *r2 = new Rope(this);
	r2->setPallete(tileSprite.palleteID);
	r2->giveGFX(ropeHeadGFX, OBJ_SIZE_16X16, 8, 8);
	r2->childGFX = ropeGFX;
	r2->setPos(256-56, 3);
	r2->setLength(9);
	r2->makeStatic();
}

/**
 * Generate the game%'s pickup spawners and position them at the correct places on the screen.
 */
void gameManager::generateSpawners()
{
	pickupSpawners[0] = new pickupSpawner(this);
	pickupSpawners[0]->setPallete(tileSprite.palleteID);
	pickupSpawners[0]->giveSprite(tileSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
	pickupSpawners[0]->setFrame(1);
	pickupSpawners[0]->setPos(8, 96);
	pickupSpawners[0]->setDirection(pickupSpawner::SDIR_RIGHT);
	//p1->makeStatic();
	
	pickupSpawners[1] = new pickupSpawner(this);
	pickupSpawners[1]->setPallete(tileSprite.palleteID);
	pickupSpawners[1]->giveSprite(tileSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
	pickupSpawners[1]->setFrame(3);
	pickupSpawners[1]->setPos(256-8, 96);
	pickupSpawners[1]->setDirection(pickupSpawner::SDIR_LEFT);
	//p2->makeStatic();
	
	pickupSpawners[2] = new pickupSpawner(this);
	pickupSpawners[2]->setPallete(tileSprite.palleteID);
	pickupSpawners[2]->giveSprite(tileSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
	pickupSpawners[2]->setFrame(2);
	pickupSpawners[2]->setPos(128, 8);
	pickupSpawners[2]->setDirection(pickupSpawner::SDIR_DOWN);
	//p3->makeStatic();
}

/**
 * Generate the marshmallow sea that sits at the bottom of the screen and looks nice.
 */
void gameManager::drawMallow()
{
	for (int i=0; i<16; ++i)
	{
		spriteObject *goo = new spriteObject(this);
		goo->setPallete(tileSprite.palleteID);
		goo->giveSprite(tileSprite.spriteData, OBJ_SIZE_16X16, 8, 8, 0, i*16+8, 192-8);
		goo->setLayer(1);
		goo->setFrame(6);
		goo->updateSprite();
		goo->setAnim(6,9,PA_RandMax(3)+1);
		goo->makeStatic();
	}
}

//==============================================================================

/**
 * The game's update function, called once per frame. In addition to the spriteManager%'s purpose
 * of updating and redrawing spriteObject%s, this function also performs additional high-level game
 * logic such as scoring, checking Player inputs and computing collision.
 * 
 * The order of main execution loops is as follows:
 * - Player inputs are checked
 * - all spriteObject positions & displays are updated
 * - spriteObject collision calculation is performed and acted upon
 * 
 * This does make some things a little tricky, but it leaves things in a very predictable order
 * with spriteObjects at very specific states after each loop.
 */
void gameManager::gameTick()
{
	#ifdef __MDDEBUG
	char buffer[40];
	sprintf ( buffer, "%d %d %d %d %d ", p1score, p1RoundScore, p2score, p2RoundScore, pointsToWin );
	macros::debugMessage("Scores", buffer, 2);
		
	sprintf ( buffer, "%d \nTaken  : %d \nNext ID: %d ", gameSprites.size(), getNumSpritesTaken(), getNextSpriteID() );
	macros::debugMessage("Sprites", buffer, 6);
	#endif
	
	if (!dynamic_cast<menuGBottom*>(gameBottom)->gamePaused)
	{
		//round end timer (set when a player dies)
		if (gameEndTimer == 0)
		{
			if (player1 != NULL)
				player1->clearWeapon();
			if (player2 != NULL)
				player2->clearWeapon();
			
			//add this round to stats
			if (p1RoundScore > 0 || p2RoundScore > 0)	//dont want rounds where nobody has died!
			{
				u8 winner = (p1RoundScore > p2RoundScore ? menuBase->player1id : menuBase->player2id);
				u8 loser = (p1RoundScore > p2RoundScore ? menuBase->player2id : menuBase->player1id);
				menuBase->scoreRound(winner, loser, (p1RoundScore == p2RoundScore));
			}
			
			resetGame(((p1score + p1RoundScore >= pointsToWin) || (p2score + p2RoundScore >= pointsToWin)) && ((p1score + p1RoundScore) != (p2score + p2RoundScore)));
			gameEndTimer = -1;
			pauseSpawning = false;
		} else if (gameEndTimer > 0)
		{
			pauseSpawning = true;
			if ((player1->isDisabled() && player2->readyForVictory()) || 
				(player1->readyForVictory() && player2->isDisabled()) ||
				(player1->isDisabled() && player2->isDisabled()))
			{
				//this ternary is ok since you can't see the players at this point anyway
				if (gameEndTimer == ROUNDENDTIMEOUT && !(player1->isDisabled() && player2->isDisabled()))
				{
					//frozen player will be the loser at this point
					player1->isFrozen() ? player2->playVictory() : player1->playVictory();
				}
				gameEndTimer--;
			}
		}
		
		//read player input before screen updates
		if (player1 != NULL)
			player1->checkPad();
		if (player2 != NULL)
			player2->checkPad();
		
		spriteManager::gameTick();
		
		//all are updated to new positions, so run collision detection
		for (vector<spriteObject*>::iterator it = gameSprites.begin(); it != gameSprites.end(); ++it)
			(*it)->checkCollisions();
	}
	
	if (Pad.Newpress.Start)		//escape key!
	{
		dynamic_cast<menuGBottom*>(gameBottom)->gamePaused = true;
		
		//stop primary timer
		PauseTimer(statTimerID);
		
		//this stops any animations from sticking (example: player 2 walking right without animating)
		player1->updateAnimation();
		player2->updateAnimation();
	}
}

//==============================================================================

/**
 * Called when a Player bites the dust. Freezes the action and awards kill & death
 * statistics to the proper Player%s.
 * 
 * @param	p	a reference to the Player who died
 * @param	weaponID	ID of the Weapon the Player was killed by. Relates to Pickup::pickupType.
 * 						type 18 is a death by drowning (no Weapon)
 * @param	skipKill	if true, the death was a suicide and no kill points are awarded
 */
void gameManager::playerDied(Player *p, u8 weaponID, bool skipKill)
{
	if (player1 == p)
	{
		player1->freezePlayer();	//stop moving and colliding
		player2->disableControls();	//let gravity take over
		p2RoundScore = 1;
		
		//add this death to stats
		if (!skipKill)
			menuBase->scoreKill(menuBase->player2id, weaponID);
		menuBase->scoreDeath(menuBase->player1id, weaponID);
	} else {
		player2->freezePlayer();
		player1->disableControls();
		p1RoundScore = 1;
		
		//add this death to stats
		if (!skipKill)
			menuBase->scoreKill(menuBase->player1id, weaponID);
		menuBase->scoreDeath(menuBase->player2id, weaponID);
	}
	player1->stopNetted();
	player2->stopNetted();
	gameEndTimer = ROUNDENDTIMEOUT;
}

/**
 * this death isn't associated with any particular Weapon, it's just for completely silent 
 * ones if ever needed
 * 
 * @param	p	reference to the Player who died
 */
void gameManager::playerDiedSilently(Player *p)
{
	player1 == p ? player1->playInvis() : player2->playInvis();
	playerDied(p);
}

/**
 * Player was hit by a skull. Award a kill if the other Player was using a magnet, or they just bumped
 * into the Player who died.
 * 
 * @param	p	reference to the Player who died
 */
void gameManager::playerSkulled(Player *p)
{
	player1 == p ? player1->playSkulled() : player2->playSkulled();
	
	//if other player is using the magnet, attribute this to a magnet kill
	//otherwise if you werent hit at all, its your own dumb fault
	bool isMagnet = (player1 == p ? player2->usingMagnet() : player1->usingMagnet());
	playerDied(p, (isMagnet ? Pickup::PT_MAGNET : Pickup::PT_SKULL), !(p->lastCollision > Player::CS_NONE));
}

/**
 * A player drowned in the marshmallow. Award a kill only if the other Player just hit them
 * with their body or a Weapon.
 * 
 * @param	p	reference to the Player who died
 */
void gameManager::playerSank(Player *p)
{
	player1 == p ? player1->playSplashEffect() : player2->playSplashEffect();
	
	//check last collision state to determine cause of death
	if (p->isNetted())
		playerDied(p, Pickup::PT_NET);
	else if (p->lastCollision == Player::CS_INVISPLAYER)
		playerDied(p, Pickup::PT_INVIS);
	else if (p->lastCollision == Player::CS_TNTFALL)
		playerDied(p, Pickup::PT_TNT);
	else if (p->lastCollision == Player::CS_GRENFALL)
		playerDied(p, Pickup::PT_GRENADE);
	else if (p->lastCollision == Player::CS_WARPEDPLAYER)
		playerDied(p, Pickup::PT_WARP);
	else if (p->lastCollision == Player::CS_SHIELDPLAYER)
		playerDied(p, Pickup::PT_SHIELD);
	else if (p->lastCollision == Player::CS_BOOMERANG)
		playerDied(p, Pickup::PT_BOOMERANG);
	else if (p->lastCollision == Player::CS_BASICHIT)
		playerDied(p, 18);
	else
		playerDied(p, 18, true);
}

/**
 * A Player was touched by someone with 1000 volts
 * 
 * @param	p	reference to the Player who died
 */
void gameManager::playerLightningd(Player *p)
{
	playerDied(p, Pickup::PT_1000V);
	player1 == p ? player1->play1000VHitEffect() : player2->play1000VHitEffect();
}

/**
 * A Player stepped on a mine
 * 
 * @param	p	reference to the Player who died
 */
void gameManager::playerMined(Player *p)
{
	playerDied(p, Pickup::PT_MINE);
	player1 == p ? player1->playMineHitEffect() : player2->playMineHitEffect();
}

/**
 * A Player was shot by a gun
 * 
 * @param	p	reference to the Player who died
 */
void gameManager::playerDisintegrated(Player *p)
{
	player1 == p ? player1->playDisintegrated() : player2->playDisintegrated();
	playerDied(p, Pickup::PT_GUN);
}

/**
 * A Player got bitten by a weasel
 * 
 * @param	p	reference to the Player who died
 */
void gameManager::playerWeaseled(Player *p)
{
	player1 == p ? player1->playDisintegrated() : player2->playDisintegrated();
	playerDied(p, Pickup::PT_WEASEL);
}

/**
 * A Player was hit by a nuclear puck
 * 
 * @param	p	reference to the Player who died
 */
void gameManager::playerPucked(Player *p)
{
	playerDied(p, Pickup::PT_PUCK);
	player1 == p ? player1->playMineHitEffect() : player2->playMineHitEffect();
}

//==============================================================================

///load the sprite data for the first Player
void gameManager::loadPlayer1SpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&player1Sprite, p, s);
}

///load the sprite data for the second Player
void gameManager::loadPlayer2SpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&player2Sprite, p, s);
}

///load the sprite data for the environment (floor, marshmallow and Pickup spawners)
void gameManager::loadEnvSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&tileSprite, p, s);
}

///load the sprite data for the Pickup%s 
void gameManager::loadPickupSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&pickupSprite, p, s);
}

///load the sprite data for the game's effects
void gameManager::loadFXSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&FXSprite, p, s);
}

///load the sprite data for the dunce hat
void gameManager::loadDunceSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&dunceHatSprite, p, s);
}

///load the sprite data for the boots
void gameManager::loadBootsSpriteSet(const unsigned short *p, const unsigned char *s)
{
	loadSpriteSet(&bootsSprite, p, s);
}

/**
 * Reload all palletes that the game's spriteObject%s needs to operate. This should
 * be called after flushing the sprite system so that newly created sprites display
 * properly.
 */
void gameManager::refreshPalletes()
{
	loadPlayer1SpriteSet(player1Sprite.palleteData, player1Sprite.spriteData);
	loadPlayer2SpriteSet(player2Sprite.palleteData, player2Sprite.spriteData);
	loadEnvSpriteSet(tileSprite.palleteData, tileSprite.spriteData);
	loadPickupSpriteSet(pickupSprite.palleteData, pickupSprite.spriteData);
	loadFXSpriteSet(FXSprite.palleteData, FXSprite.spriteData);
	loadDunceSpriteSet(dunceHatSprite.palleteData, dunceHatSprite.spriteData);
	loadBootsSpriteSet(bootsSprite.palleteData, bootsSprite.spriteData);
	
	player1000VPallete = loadPallete((void*)player1000VPalleteData);
}

/**
 * Flush the sprite system and reset everything for a new round.
 * Sets all spriteSet%s palleteID's back to -1 so that they will be reloaded when
 * gameManager::refreshPalletes() is called. Also clears the Pickup%s array and resets
 * the number of active Pickup%s. 
 */
void gameManager::clearOutSprites(bool keepSound)
{
	player1Sprite.palleteID = -1;
	player2Sprite.palleteID = -1;
	tileSprite.palleteID = -1;
	pickupSprite.palleteID = -1;
	FXSprite.palleteID = -1;
	dunceHatSprite.palleteID = -1;
	bootsSprite.palleteID = -1;
	
	pickups.clear();
	dynamic_cast<menuGBottom*>(gameBottom)->pickupsActive = 0;
	
	player1 = NULL;
	player2 = NULL;
	
	//unloadGFX();	//<- spriteManager resets sprite system to clean out GFX
	//however we still need to clear preloaded GFX indices array
	pickupGFX.clear();
	
	spriteManager::clearOutSprites(keepSound);
}

//==============================================================================

/**
 * Adds a Pickup to the array of available Pickup%s so that they can be retrieved later.
 * 
 * @param	p	the Pickup to add to the gameManager::pickups array
 */
void gameManager::addPickup(Pickup *p)
{
	pickups.push_back(p);
}

/**
 * Removes a Pickup from the array of available Pickup%s when it has died or is no longer needed.
 * 
 * @param	p	the Pickup to remvoe from the gameManager::pickups array
 */
void gameManager::removePickup(Pickup *p)
{
	vector<Pickup*>::iterator it = pickups.begin();
	
	while (it != pickups.end())
	{
		if ((*it) == p)
		{
			pickups.erase(it);
			break;
		}
		++it;
	}
}
