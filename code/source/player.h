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

#ifndef _PLAYER_H
#define _PLAYER_H

#include <PA9.h>
#include <dswifi9.h>
#include <vector>
	using std::vector;
#include <time.h>
#include "macros.h"
#include "massObject.h"

#include "rope.h"

//forward declaration
class gameManager;
class Weapon;

/**
 * A Player is a spriteObject that responds to input events on the DS.
 * 
 * Players can run, jump, crouch, roll, bounce into each other, trip over, climb ropes, use Weapon%s
 * and die. Oh yes, they can die in quite a plethora of ways.
 * 
 * Once a Player dies, a round is over. The winning Player must wait until it comes to rest before
 * scores are allocated however, so ties can easily happen.
 * 
 * When in the air, Player%s have two distinct behaviours - stable and unstable. Stable Player%s land
 * on their feet nicely and don't go anywhere. Unstable Player%s however, flail around wildly in the air
 * and roll upon landing. Pushing another Player by running into them the right way makes them unstable.
 * 
 * @author pospi
 */
class Player : public massObject
{
	public:
		Player(gameManager *newgm);
		virtual ~Player();
		
		void setControls(u8 nplayerNum);
		
		///return this Player%s game ID, which also determines which controls they are using
		u8 getPlayerNum() {return playerNum;}
		
		void checkPad();
		void basicInput(bool hr, bool hl, bool hu, bool hd, bool nr, bool nl, bool nu, bool nd, bool hf, bool nf);
		
		///move this Player back to their spawning position and halt their movement
		void resetPlayer() {setPos(startx, starty); setBasePos(starty); setFlipped(startFlipped); setSpeed(0,0); playIdle();}
		
		///TOTALLY FREEZE the Player, so that they cannot move at all
		void freezePlayer() {vx=0;vy=0;playerNum=-1;bStasis=true;setCollision(COL_NONE);setCheckCollision(false);}
		
		///HALT INPUT, but retain motion and animation updates
		void disableControls() {bIgnoreInput=true;}
		
		/**
		 * check if this Player is frozen
		 * @return true if the Player cannot be moved, false otherwise
		 */ 
		bool isDisabled() {return (playerNum == -1 && bStasis);}
		bool readyForVictory();
		
		virtual void updateSprite();
		
		/**
		 * Set the round starting position for this player. The Player will move to this position
		 * every time a new round begins.
		 * 
		 * @param	x	the x screen position (in pixels) where this Player will spawn
		 * @param	y	the y screen position (in pixels) where this Player%'s BASE (feet) will be
		 */
		void setStartPos(s16 x, s16 y) {setPos(x,y); setBasePos(y); startx = x; starty = y;}
		
		/**
		 * Set whether or not this Player starts off flipped horizontally
		 * @param	f	true if the Player should start flipped, false otherwise
		 */ 
		void setStartFlipped(bool f) {setFlipped(f); startFlipped = f;}
		
		virtual void collidingWith(spriteObject *other);
		virtual bool isColliding(const spriteObject *other, bool checkReverse = true);
		void collideNormally(Player *o);
		
		//higher-level player control functions
		bool jump(bool bootsJump = false);
		bool crouch();
		void bounce(bool pushedForwards = false);
		bool roll(bool backwards = false);
		void uncrouch(bool hr, bool hl);	//call to reset to standing position
		void climbRope(bool hd);				//grab on to touchingRope
		void justlanded(bool hr, bool hl);	//called after landing to reset collisions, etc
		void justfell();		//called after falling
		
		void setUnstable(bool unst) {bUnstable = unst;}
		bool isUnstable() {return bUnstable;}
		
		//animation functions
		///play our idle animation when we're just standing still
		void playIdle()			{setFrame(0);}
		///play a running animation loop when the Player is moving
		void playRunning()		{setAnim(1, 4, ANIMSPEED);}
		void playCrouching();
		void playRolling();
		void playRollingBack();
		void playJumpedStanding();
		void playJumpedMoving();
		///play an animation for falling straight down
		void playFalling() {setFrame(21);}
		///play an animation for falling when unstable and leaning forwards
		void playPushedForward() {setAnim(26, 27, ANIMSPEED/2);}
		///play an animation for falling when unstable and leaning backwards
		void playPushedBackward() {setAnim(28, 29, ANIMSPEED/2);}
		void playClimbingUp();
		void playClimbingDown();
		void playVictory();
		
		///make this Player invisible but retain all collision etc
		void playInvis() {setFrame(66);}
		void playSkulled();
		void playDisintegrated();
		
		void playWarpEffect();
		void playSplashEffect();
		void play1000VHitEffect();
		void playMineHitEffect();
		
		Weapon* getWeapon() {return currWeapon;}
		void setWeapon(Weapon* w) {clearWeapon(); currWeapon = w;}
		
		/**
		 * tells the Player to stay still until this ticker expires. Mostly used by Weapon%s to play
		 * firing animations and keep the Player fixed until they have performed whatever action it is.
		 * 
		 * @param	frames	the number of frames the Player should stay still for
		 */
		void setInputInterrupt(u8 frames) {inputInterrupt = frames; forceAnimUpdate = true;}
		///forces an animation update on the Player. Useful for releasing a Weapon and making the player find whatever animation it needs for the current situation.
		void updateAnimation() {forceAnimUpdate = true;}
		
		///Weapon calls this to break the reference and record statistics when deleted
		void weaponDestroyed() { if (!weaponJustCleared) weaponChange(); currWeapon = NULL;}
		//non-Weapon calls this to destroy the Weapon
		void clearWeapon();
		//called when Weapon changes, to save time stats
		void weaponChange();
		
		/**
		 * check if this Player is low to the ground
		 * @return	true if crouching or rolling, false otherwise
		 */
		bool isCrouched() {return (bCrouching || bRolling);}
		/**
		 * check if this Player is currently climbing on a Rope
		 * @return	true if currently on a Rope, false otherwise
		 */
		bool isOnRope() {return bClimbingRope;}
		
		/**
		 * Check if the Player was touching a rope in the tick just before this one
		 * This can be used to check for Rope%s in spriteManager::gameTick(), since Player::touchingRope is always NULL there.
		 * @return true if just touching a rope, false otherwise
		 */
		bool justTouchingRope() {return wasTouchingRope != NULL;}
		
		///another pointer to spriteObject::sm, since we will be using a lot of gameManager functions.
		gameManager* gm;
		
		///walking speed - 1 pixel per frame in fixed point format
		static const u16 WALKSPEED = 256;
		///climbing speed - 1 pixel per frame in fixed point format
		static const u16 CLIMBSPEED = 256;
		///amount of instantaneous yvel to give when jump happens in fixed point format
		static const short JUMPIMPULSE = -640;
		///speed the Player is flung out of the screen at when killed, in fixed point format
		static const s16 POWERHITSPEED = 2048;
		
		/**
		 * Change the pallete ID of this Player. To be used by the 1000 Volts pickup.
		 * @param	id	the spriteManager preloaded ID of the new pallete
		 */
		virtual void setPallete(u8 id) {defaultPalleteID = id; palleteID = id;}
		///1000V needs a default pallete to be sure to return us to the right one
		u8 defaultPalleteID;
		
		//functions for net input
		void handleNetted(bool upPressed);
		void startNetted();
		void stopNetted();
		/**
		 * Check if the Player is currently captured in a net and unable to move properly
		 * @return true if netted, false otherwise
		 */
		bool isNetted() {return nettedStrength > 0;}
		
		//weapon checks
		bool has1000V();
		bool usingChut();
		bool hasShield();
		bool hasBoomerang();
		bool usingMagnet();
		bool usingInvis();
		
		/**
		 * The state a Player was in when they last left the ground.
		 * A Player%'s collision state is very important for statistics recording, so that the
		 * last thing that happened to them before they drowned can be recorded.
		 */
		enum collisionState {
			CS_NONE, CS_BASICHIT, CS_INVISPLAYER, CS_TNTFALL, CS_GRENFALL, 
			CS_WARPEDPLAYER, CS_SHIELDPLAYER, CS_BOOMERANG
		};
		///the state of this player when they last left the ground. Reset to CS_NONE when they land again.
		collisionState lastCollision;
		
		//called when the ground under a player is deleted
		void groundDeleted(collisionState newCollState = CS_NONE);
		
		///set by warp when used, so that warp stats can be recorded for kills if they magically fluke to happen
		void justWarped() {bJustUnwarped = true;}
	protected:		
		static const s8 standingBounds[];
		static const s8 fallingBounds[];
		static const s8 crouchingBounds[];
		
		///this Player%'s game ID, which also determines their controls
		s8 playerNum;
		
		///true if this Player is crouched
		bool bCrouching;
		///true if this Player is rolling
		bool bRolling;
		///if true, the Player must roll when landing
		bool bUnstable;
		///true when leaving the ground by normal means, to prevent Player::justFell() from executing
		bool justJumped;
		
		///if true, no keypresses will have an effect (used for round end)
		bool bIgnoreInput;
		
		s16 startx, starty;
		bool startFlipped;
		
		///the PARENT Rope being climbed - NULL when not climbing
		Rope* touchingRope;
		///same as Player::touchingRope, but this can be checked against in the spriteManager::gameTick() cycle
		Rope* wasTouchingRope;
		///true if the Player is touching a Rope as well as climbing it
		bool bClimbingRope;
		
		///the current Weapon this Player has equipped
		Weapon* currWeapon;
		
		//weapon animation of the player
		///this is the numer of ticks until the Player can move again
		u8 inputInterrupt;
		///force the Player to restart potentially old animations, to kill weapon anims
		bool forceAnimUpdate;
		
		///time that the last Weapon change happened
		u32 lastWeaponChangeTime;
		///Player::currWeapon was just cleared - stops a double check when deleting old weapon
		bool weaponJustCleared;
		
		//weapon specific variables
		///how many clicks are still needed to break free from net (if 0, not netted)
		u8 nettedStrength;
		///true if warp was used and haven't touched the ground yet
		bool bJustUnwarped;
		
		///barely a jump, just something to let the player know they pressed it.
		static const s8 NETJUMPIMPULSE = -100;
		///relation between sprite rotation and movement velocity whilst in the net
		static const s8 NETSPINSPEED = -20;
		///x-axis deceleration to be applied every tick whilst in the net
		static const u8 NETROLLDECEL = 2;
		
};

#endif
