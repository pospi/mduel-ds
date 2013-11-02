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

#ifndef _GAMEMANAGER_H
#define _GAMEMANAGER_H

#include <PA9.h>
#include <vector>
#include <string>
	using std::vector;
	using std::string;
#include "macros.h"

#include "spriteObject.h"
#include "floorTile.h"
#include "rope.h"
#include "pickupSpawner.h"

//forward declaration
class Player;
class Pickup;
class menuBottom;

/**
 * Class to manage game sprites and handle high-level game logic.
 * 
 * The gameManager works in tandem with a menuGBottom in order to run the game itself.
 * The gameManager is responsible for the 'important' game logic and display, whilst the 
 * menuGBottom is responsible for controlling Pickup spawns, showing scores etc and pausing
 * the game.
 * 
 * The game is played in rounds, and a configurable number of rounds make up a match. The gameManager
 * is in control of its screen for the entire duration of the match.
 * 
 * @author pospi
 */
class gameManager : public spriteManager
{
	public:
		/**
		 * Create a new gameManager instance and assign it a screen to manage.
		 * 
		 * @param 	newscreen	the screen this gameManager should handle. 1 is the top screen, 0
		 * 						is the bottom one.
		 */
		gameManager(u8 newscreen = 0) : spriteManager(newscreen), 
			p1score(0), p2score(0), pointsToWin(3), p1RoundScore(0), p2RoundScore(0), 
			player1(NULL), player2(NULL), menuTop(NULL), gameBottom(NULL), menuBase(NULL), 
			gameEndTimer(0), roundNumber(0), pauseSpawning(false)
		{
			player1Sprite.palleteID = -1;
			player2Sprite.palleteID = -1;
			tileSprite.palleteID = -1;
			pickupSprite.palleteID = -1;
			FXSprite.palleteID = -1;
			dunceHatSprite.palleteID = -1;
			bootsSprite.palleteID = -1;
		}
		virtual ~gameManager();
		
		/**
		 * In addition to giving this gameManager control of its screen, reset all scores.
		 * We do this because the gameManager is only activated at the start of a match.
		 */
		virtual void activate()
		{
			resetScores(); 
			gameEndTimer = 0;
			pickupMask.clear();
			spriteManager::activate(); 
		}
		
		/**
		 * In addition to relinquishing control of this gameManager%'s screen, reset all scores.
		 */
		virtual void deactivate()
		{
			resetScores();
			spriteManager::deactivate();
		}
		
		virtual void clearOutSprites(bool keepSound = false);
		
		//initial loadings
		void loadPlayer1SpriteSet(const unsigned short *p, const unsigned char *s);
		void loadPlayer2SpriteSet(const unsigned short *p, const unsigned char *s);
		void loadEnvSpriteSet(const unsigned short *p, const unsigned char *s);
		void loadPickupSpriteSet(const unsigned short *p, const unsigned char *s);
		void loadFXSpriteSet(const unsigned short *p, const unsigned char *s);
		void loadDunceSpriteSet(const unsigned short *p, const unsigned char *s);
		void loadBootsSpriteSet(const unsigned short *p, const unsigned char *s);
		
		//reload all pallete IDs for sprite sets
		void refreshPalletes();
		
		void preloadGFX();
		void unloadGFX();
		
		virtual void gameTick();					//called every frame while game is in progress
		void resetGame(bool ending = false);		//reset everything to initial state (unless ending, then just delete)
		
		void generateBricks();
		void generateRopes();
		void drawMallow();
		void generateSpawners();
		
		void playerSank(Player *p);		//player calls these when they die, increments scores and resets etc
		void playerDiedSilently(Player *p);
		void playerSkulled(Player *p);
		void playerLightningd(Player *p);
		void playerMined(Player *p);
		void playerDisintegrated(Player *p);	//gun
		void playerWeaseled(Player *p);
		void playerPucked(Player *p);
		
		u8 p1score, p2score, pointsToWin;
		///the score that Player 1 has accumulated for this round
		u8 p1RoundScore;
		///the score that Player 2 has accumulated for this round
		u8 p2RoundScore;
		
		///reset all score-related variables in preparation for a new match
		void resetScores() {p1score = 0; p2score = 0; p1RoundScore = 0; p2RoundScore = 0; roundNumber = 0;}
		
		bool isGamePaused();
		
		#ifdef __WITHSOUND
		//public sound handles that MUST BE SET dammit
		soundData bigSplash, platformExplode, spawnPuff, gotPickup, teleport, skullLoop, skullPickup, lightningHit, lightningLoop, invisOn, invisOff, mineExp, mineDrop, gunFire, bootsJump, smallSplash, chutClose, chutOpen, hookHit, duncePickup, magnetLoop, netThrow, bulletReflect, boomerangLoop, throwSound;
		
		soundData weaselSounds[7];
		#endif
		
		spriteSet player1Sprite, player2Sprite, tileSprite, pickupSprite, FXSprite, dunceHatSprite, bootsSprite;
		u8 player1000VPallete;
		const short unsigned int* player1000VPalleteData;
		
		///holds preloaded graphical data for the game's Pickup%s, so that we don't have to load them up freshly each time
		struct pickupGraphics
		{
			///GFX to be used for the 'spawn puff' effect
			u16 spawnGFX;
			///GFX to be used for the dieing 'pop' effect
			u16 dieGFX;
			///GFX to be used for the purple blob
			u16 blobGFX;
			///GFX to be used for the icon in front
			u16 iconGFX;
			///true if this lot of pickup graphics are currently being used by a Pickup
			bool bInUse;
		};
		///all the preloaded GFX that Pickup%s will hopefully need (maxPickups + 2)
		vector<pickupGraphics> pickupGFX;
		
		pickupGraphics* getNextAvailablePickupGFXSet();

		u16 spawnGFX[4];
		u16 dunceGFX[2];
		u16 bootsGFX[2];
		
		///the pickupSpawner%s that generate Pickup%s for the game
		pickupSpawner* pickupSpawners[3];
		///The Player who spawns on the left hand side of the battlefield
		Player *player1;
		///The Player who spawns on the right hand side of the battlefield
		Player *player2;
		///a vector of Pickup%s saves us from having to iterate through all actors to find them
		vector <Pickup*> pickups;
		
		//some management functions for the Pickup%s array
		void addPickup(Pickup* p);
		void removePickup(Pickup* p);
		
		/**
		 * Check whether or not it is allowable for Pickup%s to be created.
		 * 
		 * @return true if Pickup spawning is ok, false otherwise.
		 */
		bool allowPickupSpawning() {return !pauseSpawning;}
		
		///the Y screen position (in pixels) to play splash effects at
		static const u8 MALLOWYPOS = 169;
		///spriteManager which handles the top screen during frontend
		spriteManager *menuTop;
		///spriteManager which handles the bottom screen during gameplay
		spriteManager *gameBottom;
		///spriteManager which handles the bottom screen during frontend
		menuBottom *menuBase;
		///an array of Pickup IDs that are ok to use. These correspond to Pickup::pickupType.
		vector<u8> pickupMask;
		///the timer id of the main timer that is used to record Weapon time statistics
		u8 statTimerID;
	private:
		void playerDied(Player* p, u8 weaponID = 18, bool skipKill = false);		//weaponID used for statistics. Defaults to FIDs suicide
		///count down from here (in ticks) when round ends, then reset
		s8 gameEndTimer;
		///the number of the round that is currently being played
		u8 roundNumber;
		///time in frames that the round will restart after a Player is killed
		static const u8 ROUNDENDTIMEOUT = 60;
		///Set to true to stop Pickup%s from spawning. Pickup%s shouldn't spawn when the round is over, as a safety measure.
		bool pauseSpawning;
};

#endif
