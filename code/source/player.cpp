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

#include "player.h"
#include "pickup.h"
#include "weapon.h"
#include "gameManager.h"
#include "weaponIncludes.h"
#include "menuBottom.h"

//some shortcuts to make life easier
#define HU (Pad.Held.Up && !bIgnoreInput)
#define HR (Pad.Held.Right && !bIgnoreInput)
#define HD (Pad.Held.Down && !bIgnoreInput)
#define HL (Pad.Held.Left && !bIgnoreInput)
#define HA (Pad.Held.A && !bIgnoreInput)
#define HB (Pad.Held.B && !bIgnoreInput)
#define HX (Pad.Held.X && !bIgnoreInput)
#define HY (Pad.Held.Y && !bIgnoreInput)
#define HRT (Pad.Held.R && !bIgnoreInput)
#define HLT (Pad.Held.L && !bIgnoreInput)
#define NU (Pad.Newpress.Up && !bIgnoreInput)
#define NR (Pad.Newpress.Right && !bIgnoreInput)
#define ND (Pad.Newpress.Down && !bIgnoreInput)
#define NL (Pad.Newpress.Left && !bIgnoreInput)
#define NA (Pad.Newpress.A && !bIgnoreInput)
#define NB (Pad.Newpress.B && !bIgnoreInput)
#define NX (Pad.Newpress.X && !bIgnoreInput)
#define NY (Pad.Newpress.Y && !bIgnoreInput)
#define NRT (Pad.Newpress.R && !bIgnoreInput)
#define NLT (Pad.Newpress.L && !bIgnoreInput)
/*#define RU Pad.Released.Up && !bIgnoreInput
#define RR Pad.Released.Right && !bIgnoreInput
#define RD Pad.Released.Down && !bIgnoreInput
#define RL Pad.Released.Left && !bIgnoreInput
#define RA Pad.Released.A && !bIgnoreInput
#define RB Pad.Released.B && !bIgnoreInput
#define RX Pad.Released.X && !bIgnoreInput
#define RY Pad.Released.Y && !bIgnoreInput
#define RRT Pad.Released.R && !bIgnoreInput
#define RLT Pad.Released.L && !bIgnoreInput*/

/**
 * Initialise a new Player and assign it to the gameManager that will manage it
 * 
 * @param	newgm	the gameManager to handle this Player
 */
Player::Player(gameManager *newgm)
	: massObject(dynamic_cast<spriteManager*>(newgm)), gm(newgm), lastCollision(CS_NONE), playerNum(-1), 
	bCrouching(false), bRolling(false), bUnstable(false), justJumped(false), bIgnoreInput(false), startx(0), 
	starty(0), startFlipped(false), touchingRope(NULL), wasTouchingRope(NULL), bClimbingRope(false), currWeapon(NULL),
	inputInterrupt(0), forceAnimUpdate(false), lastWeaponChangeTime(0), weaponJustCleared(false), nettedStrength(0), 
	bJustUnwarped(false)
{
	#ifdef __MDDEBUG
	className = "player";
	macros::debugMessage(className, "constructor");
	#endif
	setBounds(standingBounds[0], standingBounds[1], standingBounds[2], standingBounds[3]);
	setCollision(COL_SOLID);
	setCheckCollision(true);
	setLayer(2);
	
	//initialise the weapon timer
	lastWeaponChangeTime = Tick(gm->statTimerID);
}

/**
 * Upon deletion, disable the Player sprite's rotation set for when the sprite OAM id is reused and
 * destroy any Weapon they may be carrying
 */
Player::~Player()
{
	PA_SetSpriteRotDisable(gm->screen, spriteID);
	
	clearWeapon();
}

///the bounding box for a Player when they are standing upright
const s8 Player::standingBounds[4] = {-11, 5, 11, -5};
///the bounding box for a Player when they are in midair
const s8 Player::fallingBounds[4] = {-11, 5, 11, -5};
///the bounding box for a Player when they are crouching
const s8 Player::crouchingBounds[4] = {-2, 5, 11, -5};

/**
 * set the input system for this Player. 
 *
 * @param	playerNum	the type of controls for this Player:
 * 						- -1 = disabled (for pausing input)
 * 						- 0 = singleplayer (dpad and A button)
 * 						- 1 = hotseat, p1 (dpad and L)
 * 						- 2 = hotseat, p2 (ABXY and R)
 * 						- 3 TODO: will eventually be a remote player
 */
void Player::setControls(u8 nplayerNum)
{
	playerNum = nplayerNum;
	//if (nplayerNum==0 || nplayerNum==1)
	//	setSoundChannel(SLOT_PLAYER1);
	//else if (nplayerNum==2)
	//	setSoundChannel(SLOT_PLAYER2);
}

/**
 * Check the DS's buttons for input and work out this Player%'s movement
 */
void Player::checkPad()
{
	switch (playerNum)
	{
		case -1:		//disable controls
			//vx = 0;
		break;
		case 0:			//singleplayer controls
			basicInput(HR, HL, HA, HD, NR, NL, NA, ND, HB, NB);
		break;	
		case 1:			//P1 controls
			basicInput(HR, HL, HU, HD, NR, NL, NU, ND, HLT, NLT);
		break;
		case 2:			//P2 controls
			basicInput(HA, HY, HX, HB, NA, NY, NX, NB, HRT, NRT);
		break;
		case 3:			//wifi controls
			//TODO!
		break;
	}
	
	if (inputInterrupt > 0)
		inputInterrupt--;
	wasTouchingRope = touchingRope;
	touchingRope = NULL;	//will be recalculated in collision cycle
}

/**
 * Perform basic button input on this Player. This handles all logic on how the Player should move
 * and control.
 * 
 * @param	hr	whether or not the 'right' button is being held
 * @param	hl	whether or not the 'left' button is being held
 * @param	hu	whether or not the 'up' button is being held
 * @param	hd	whether or not the 'down' button is being held
 * @param	nr	whether or not the 'right' button is newly pressed
 * @param	nl	whether or not the 'left' button is newly pressed
 * @param	nu	whether or not the 'up' button is newly pressed
 * @param	nd	whether or not the 'down' button is newly pressed
 * @param	hf	whether or not the 'fire' button is being held
 * @param	nf	whether or not the 'fire' button is newly pressed
 */
void Player::basicInput(bool hr, bool hl, bool hu, bool hd, bool nr, bool nl, bool nu, bool nd, bool hf, bool nf)
{
	//weapon animation refreshes
	if (inputInterrupt == 0 && forceAnimUpdate)
	{
		if (isOnRope()) {
			if (vy > 0)
				playClimbingDown();
			else
				playClimbingUp();
		}
		else if (isOnGround())
			playRunning();
		else if (bUnstable)
			playPushedForward();
		else
			playFalling();
		
		if (hl || hr)
			setFlipped(hl, getFlippedv());
			
		forceAnimUpdate = false;
	}
	
	//netted physics
	if (nettedStrength > 0)
	{
		handleNetted(nu);
		return;
	}
	
	//bouncing off the walls
	if (getLeft() <= 0)
	{
		setFlipped(true);
		bounce();
	} else if (getRight() >= 255)
	{
		setFlipped(false);
		bounce();
	}
	
	//regular controls
	else if (inputInterrupt == 0) {
		if (bClimbingRope)
		{
			if (touchingRope == NULL)
			{
				vy = 0;
				vx = (hr - hl) * WALKSPEED;
				bClimbingRope = false;
				justfell();
			} else {
				vy = (hd - hu) * CLIMBSPEED;
				//don't allow to climb off rope via up/down keys
				if (touchingRope->childRopes[touchingRope->childRopes.size()-1]->getBottom() < getBottom() && vy > 0)
					vy = 0;
				else if (touchingRope->childRopes[0]->getTop() > getTop() && vy < 0)
					vy = 0;
				if (nd)
					playClimbingDown();
				else if (nu)
					playClimbingUp();
				if (vy == 0) setFrame(frame);
				if (hr || hl)	//jump off!
				{
					vx = (hr - hl) * WALKSPEED;
					vy = 0;
					bClimbingRope = false;
					setFlipped(hl);
					justfell();
				}
			}
		} else if (isOnGround())
		{			
			if (!wasOnGround())
				justlanded(hr, hl);	//reset collision bounds, etc
			
			bUnstable = false;
			
			if (touchingRope != NULL && (hu || nu || hd || nd) && !(hl || hr) && !bCrouching && !bRolling)
			{
				climbRope(hd);
			} else if (bRolling)
			{
				if (arbitraryAnim == 0)	//anim finished, so stop moving (PROBABLY A BAD IDEA)
				{
					bRolling = false;
					bCrouching = true;
					
					//recovered from a roll, so reset collision & warp usage state
					lastCollision = CS_NONE;
					bJustUnwarped = false;
				}
			} else if (bCrouching)
			{
				vx = 0;
				if (!hd)
					uncrouch(hr, hl);
			} else {
				vx = (hr - hl) * WALKSPEED;		//walking speed
				if (nr)
				{
					setFlipped(false);
					playRunning();
				}
				else if (nl)
				{
					setFlipped(true);
					playRunning();
				} else if (hd)	//crouch
					crouch();
				else if (!hr && !hl)
				{
					//standing still
					playIdle();
				}
				
				if (nu)
					jump();
			}
		} else {
			if (wasOnGround() && !justJumped)
				justfell();
			justJumped = false;
			if (usingChut())		//parachut midair controls
			{
				vx = (vx==0 && !hr && !hl ? 0 : (flippedh ? WALKSPEED*-1 : WALKSPEED));
				//vx = (flippedh ? WALKSPEED*-1 : WALKSPEED);
				//vx = (hr - hl) * WALKSPEED;
				if (nr)
					setFlipped(false);
				else if (nl)
					setFlipped(true);
			}
		}
	}
	
	//weapon stuffs
	if (currWeapon != NULL)
	{
		if (nf && !currWeapon->isFiring())
			currWeapon->fire();
		if (!hf && currWeapon->isFiring())
			currWeapon->stopFiring();
	}
}

//=================================================================

/**
 * Handle the Player%'s movement whilst they are stuck in a net (after being hit by a wp_net_proj)
 * 
 * @param	upPressed	true if the Player%'s controller just tapped the jump key
 */
void Player::handleNetted(bool upPressed)
{
	setFrame(67);
	setRotation(getx()*NETSPINSPEED);
	
	s32 testvel = vx >= 0 ? vx : vx * -1;
	if (testvel < NETROLLDECEL)
		vx -= (vx > 0 ? testvel : testvel * -1);
	else
		vx -= (vx > 0 ? NETROLLDECEL : NETROLLDECEL * -1);
	
	if (upPressed)
	{
		nettedStrength--;
		if (isOnGround())
			vy += NETJUMPIMPULSE;
	}
	if (nettedStrength == 0)
		stopNetted();
}

/**
 * called immediately after this Player is hit by a wp_net_proj to put them in a net.
 * Whilst in a net, the Player roll around and has no control over their sideways movement and can only
 * jump a little bit. Each jump weakens the net and they will eventually break free after
 * a few presses.
 */
void Player::startNetted()
{
	bClimbingRope = false;
	nettedStrength = PA_RandMinMax(6, 10);
	enableRotation();
	setBounds(crouchingBounds[0], crouchingBounds[1], crouchingBounds[2], crouchingBounds[3]);
}

/**
 * called when this Player frees themself from a net, to stop sprite rotation and refresh their bounds etc
 */
void Player::stopNetted()
{
	nettedStrength = 0;
	disableRotation();
	setFlipped(flippedh);	//have to refresh any flipping that might be applied
	setBounds(standingBounds[0], standingBounds[1], standingBounds[2], standingBounds[3]);
}

//=================================================================

/**
 * Perform high-level game logic when this Player collides with a variety of game-relevant objects.
 * Handles Player pushing, Pickup collisions, Rope climbing and deaths by lethal objects.
 * 
 * @param	other	a reference to a spriteObject that this Player is colliding with
 */
void Player::collidingWith(spriteObject *other)
{
	bool dontMove = false;	//true if should bounce straight up
	
	//=====================================================================================
	//PLAYER
	if (dynamic_cast<Player*>(other) != NULL)
	{
		Player *o = dynamic_cast<Player*>(other);
		
		if (currWeapon != NULL && currWeapon->isFiring())
			currWeapon->stopFiring();
		if (o->currWeapon != NULL && o->currWeapon->isFiring())
			o->currWeapon->stopFiring();
		
		//shield handling
		if (o->hasShield() && hasShield())
		{
			if (isFacing(o) && !o->isFacing(this))
			{
				o->collideNormally(this);
				o->lastCollision = CS_SHIELDPLAYER;
			}
			else if (o->isFacing(this) && !isFacing(o))
			{
				collideNormally(o);
				lastCollision = CS_SHIELDPLAYER;
			}
			else {
				o->collideNormally(this);
				collideNormally(o);
				o->lastCollision = CS_SHIELDPLAYER;
				lastCollision = CS_SHIELDPLAYER;
			}
			return;
		}
		else if (o->hasShield() && !hasShield() && o->isFacing(this))
		{
			collideNormally(o);
			lastCollision = CS_SHIELDPLAYER;
			return;
		}
		else if (hasShield() && !o->hasShield() && isFacing(o))
		{
			o->collideNormally(this);
			o->lastCollision = CS_SHIELDPLAYER;
			return;
		}
		
		//lightning handling
		if (o->has1000V() && !has1000V())	//we die
		{
			gm->playerLightningd(this);
			return;
		}	
		else if (has1000V() && !o->has1000V())	//they die
		{
			gm->playerLightningd(o);
			return;
		}
		else if (has1000V() && o->has1000V())	//nobody dies, weapons destroyed
		{
			clearWeapon();
			o->clearWeapon();
		}
		
		//regular contact
		if (!bCrouching || (bCrouching && (o->bRolling || !o->wasOnGround())))
		{
			//if other player gets under your feet
			if ((o->bCrouching || o->bRolling) && wasOnGround() && !bCrouching && !bRolling)
			{
				dontMove = (vx == 0 && o->bRolling);
				bounce(true);
				if (dontMove) vx = 0;
				
				ignoreUntilUntouched = other;
				
				vy = JUMPIMPULSE*2/3;
				
				if (o->usingInvis())
					lastCollision = CS_INVISPLAYER;
				else if (o->bJustUnwarped)
					lastCollision = CS_WARPEDPLAYER;
				else
					lastCollision = CS_BASICHIT;
			} 
			//if you get under other player's feet
			else if (bRolling && o->wasOnGround() && !o->bCrouching && !o->bRolling)
			{
				dontMove = (o->getvx() == 0);
				o->bounce(true);
				if (dontMove) o->setvx(0);
				
				ignoreUntilUntouched = other;
				
				o->vy = JUMPIMPULSE*2/3;
				
				if (usingInvis())
					o->lastCollision = CS_INVISPLAYER;
				else if (bJustUnwarped)
					o->lastCollision = CS_WARPEDPLAYER;
				else
					o->lastCollision = CS_BASICHIT;
			}
			//otherwise we must be talking a normal bump!
			else {
				o->collideNormally(this);
				collideNormally(o);
				
				if (o->usingInvis())
					lastCollision = CS_INVISPLAYER;
				else if (o->bJustUnwarped)
					lastCollision = CS_WARPEDPLAYER;
				else
					lastCollision = CS_BASICHIT;
				
				if (usingInvis())
					o->lastCollision = CS_INVISPLAYER;
				else if (bJustUnwarped)
					o->lastCollision = CS_WARPEDPLAYER;
				else
					o->lastCollision = CS_BASICHIT;
			}
		}
	}
	//=====================================================================================
	//ROPE
	else if (dynamic_cast<Rope*>(other) != NULL)
	{
		Rope *o = dynamic_cast<Rope*>(other);
		s16 diff = getx() - o->getx();
		if (diff < 0) diff *= -1;
		
		Rope * parent = o->getParent();	
		diff < Rope::ROPETOLERANCE ? touchingRope = parent : touchingRope = NULL;
	}
	//=====================================================================================
	
	else if (dynamic_cast<Pickup*>(other) != NULL)
	{
		Pickup *o = dynamic_cast<Pickup*>(other);
		o->playerTouchAction(this);
		if (o->getType() != Pickup::PT_TNT)
		{
			#ifdef __WITHSOUND
			playSound(&gm->gotPickup);
			#endif
			o->destroy();
		}
	}
	//=====================================================================================
	
	else if (dynamic_cast<wp_mine_proj*>(other) != NULL && wasOnGround())
	{
		gm->playerMined(this);
		other->destroy();
	}
	
	else if (dynamic_cast<wp_net_proj*>(other) != NULL)// && dynamic_cast<wp_net_proj*>(other)->owner != this)
	{
		if (hasShield() && isFacing(other))
		{
			other->turnAround();
		} else {
			startNetted();
			other->destroy();
		}
	}
	
	else if (dynamic_cast<wp_weasel_proj*>(other) != NULL)
	{
		if (hasShield() && isFacing(other))
		{
			other->turnAround();
		} else {
			gm->playerWeaseled(this);
		}
	}
	
	else if (dynamic_cast<wp_boomerang_proj*>(other) != NULL)
	{
		wp_boomerang_proj *o = dynamic_cast<wp_boomerang_proj*>(other);
		
		if (o->owner == this)
		{
			if (hasBoomerang() && o->weapon != NULL)
				o->weapon->boomerangReturned();
			#ifdef __WITHSOUND
			o->stopSound();	//doesnt seem to be working through the destructor as it should...
			#endif
			o->destroy();
		} else {
			if (hasShield() && isFacing(other))
			{
				other->turnAround();
			} else {
				bounce(isFacing(o));
				lastCollision = CS_BOOMERANG;
				o->returnToPlayer();
			}
		}
	}
	
	//=====================================================================================
	
	//only check for ground if not climbing a rope
	if (!bClimbingRope)
		massObject::collidingWith(other);
}

/**
 * Collision check with another spriteObject. The Player cannot be colliding with a Pickup of type
 * Pickup::PT_TNT, so in this case the collision always returns false.
 * 
 * @param other		the spriteObject to compare with
 * @param checkReverse	whether or not to compute collision for the complementary spriteObject
 * 
 * @return true if this spriteObject is colliding with other, false otherwise.
 */
bool Player::isColliding(const spriteObject *other, bool checkReverse)
{
	const Pickup *p = dynamic_cast<const Pickup*>(other);
	if (p != NULL && p->getType() == Pickup::PT_TNT)
		return false;
	
	return massObject::isColliding(other, checkReverse);
}

/**
 * Collide normally with another Player, flinging both Player%s away in equal and opposite directions.
 * 
 * @param	o	a reference to the Player who bumped into this one
 */
void Player::collideNormally(Player *o)
{
	bClimbingRope = false;
	if (getx() == o->getx())
		gety() < o->gety() ? bounce(flippedh) : bounce(!flippedh);
	else
		getx() < o->getx() ? bounce(flippedh) : bounce(!flippedh);
	vy = JUMPIMPULSE*2/3;
}

/**
 * Every tick this Player must call the weapon::weaponTick() event of their current Weapon (if present).
 * Additionally checks if the Player is outside the screen and dead, and in such a case stops them
 * from moving to prevent their sprite from wrapping the DS's screen.
 * If the player has fallen off the bottom of the screen, notify their gameManager that they sank.
 */
void Player::updateSprite()
{
	//calling this first gives the weapon a chance to override anims, etc
	if (currWeapon != NULL)
		currWeapon->weaponTick();
		
	//if player is outside the screen (not the bottom) from a death anim, freeze them in place to stop wrapping
	if (bStasis && (getx() < -32 || getx() > (s16)(SCREENW>>8) + 32 || gety() < -32))
	{
		setvx(0);
		setvy(0);
	}
	
	massObject::updateSprite();
	
	if (y >= (s32)SCREENH)
	{
		gm->playerSank(this);
		y = (s32)SCREENH - 2;
	}
}

//=============================================================

/**
 * The Player attempted to jump normally, by pressing their 'up' key.
 * 
 * @param	bootsJump	true if the Player has fired their boots, in which case they will jump much higher
 * 
 * @return	true if the Player was able to jump, false otherwise (not on the ground etc)
 */
bool Player::jump(bool bootsJump)
{
	if (!isOnGround())
		return false;
	//setBasePos(getBottom() - 1);	//move off the ground
	justJumped = true;	//stops routine 'just airborne' checks happening
	bCrouching = false;
	bRolling = false;
	vy = JUMPIMPULSE;
	if (bootsJump) vy = vy *3/2;
	if (vx == 0)
		playJumpedStanding();
	else
		playJumpedMoving();
	setBounds(fallingBounds[0], fallingBounds[1], fallingBounds[2], fallingBounds[3]);
	return true;
}

/**
 * The Player attempted to crouch normally, by pressing their 'down' key.
 * 
 * @return true if the Player was able to crouch, false otherwise (not on the ground etc)
 */
bool Player::crouch()
{
	if (!isOnGround())
		return false;
	bCrouching = true;
	if (vx == 0 && !bUnstable)
		playCrouching();
	else
		roll();
	setBounds(crouchingBounds[0], crouchingBounds[1], crouchingBounds[2], crouchingBounds[3]);
	return true;
}

/**
 * Force the Player to roll, for example when landing whilst unstable
 * 
 * @return true if the Player was able to roll, false otherwise (not on the ground etc)
 */
bool Player::roll(bool backwards)
{
	if (!isOnGround())
		return false;
	if (!backwards)
		playRolling();
	else
		playRollingBack();
	bRolling = true;
	bCrouching = false;
	bUnstable = false;
	setBounds(crouchingBounds[0], crouchingBounds[1], crouchingBounds[2], crouchingBounds[3]);
	return true;
}

/**
 * Force the Player to be pushed away from something (usually another Player) and become unstable.
 * 
 * @param	pushedForwards	true if the Player has 'tripped' and fallen forwards instead of backwards
 */
void Player::bounce(bool pushedForwards)
{
	if (currWeapon != NULL && currWeapon->isFiring())
		currWeapon->stopFiring();
	justJumped = true;
	bUnstable = true;
	bCrouching = false;
	bRolling = false;
	bClimbingRope = false;
	setBounds(fallingBounds[0], fallingBounds[1], fallingBounds[2], fallingBounds[3]);
	if (pushedForwards)
	{
		vx = (flippedh ? -WALKSPEED : WALKSPEED);
		playPushedForward();
	} else {
		vx = (flippedh ? WALKSPEED : -WALKSPEED);
		playPushedBackward();
	}
	//setBasePos(getBottom() - 1);
	if (isOnGround())
		vy = JUMPIMPULSE*2/3;
}

/**
 * Make this Player stand up after crouching. Resets bounds and animations and flips them based on the direction
 * keys being pressed.
 * 
 * @param	hr	whether or not the Player%'s right input key is being held at the time of uncrouching
 * @param	hl	whether or not the Player%'s left input key is being held at the time of uncrouching
 */
void Player::uncrouch(bool hr, bool hl)
{
	bCrouching = false;
	
	ignoreUntilUntouched = NULL;	//stop the whole tripping thing
	
	setBounds(standingBounds[0], standingBounds[1], standingBounds[2], standingBounds[3]);
	setFlipped((hr-hl==0 ? flippedh : hr-hl < 0), flippedv);
	playRunning();
}

/**
 * The Player has attempted to grab hold of a Rope. Make them climb it!
 * 
 * @param	hd	true if the Player%'s down input key is held. This determines the animation to play.
 */
void Player::climbRope(bool hd)
{
	if (touchingRope != NULL)
		setPos(touchingRope->getx(), gety());
	else if (wasTouchingRope != NULL)		//this is mainly for the hook's way of doing it
		setPos(wasTouchingRope->getx(), gety());
	else
		return;
	bClimbingRope = true;
	hd ? playClimbingDown() : playClimbingUp();
}

/**
 * A notification event that is fired when the Player lands on the ground. If the Player is unstable,
 * make them roll. Otherwise return them to a normal running state.
 * 
 * @param	hr	whether or not the Player%'s right input key is being held at the time of landing
 * @param	hl	whether or not the Player%'s left input key is being held at the time of landing
 */
void Player::justlanded(bool hr, bool hl)
{
	setBounds(standingBounds[0], standingBounds[1], standingBounds[2], standingBounds[3]);
	setFlipped((hr-hl==0 ? flippedh : hr-hl < 0), flippedv);
	if (bUnstable)
	{
		roll(vx > 0 && flippedh || vx < 0 && !flippedh);
	} else {
		vx = 0;
		playRunning();
		lastCollision = CS_NONE;	//back to normal!
		bJustUnwarped = false;		//reset the warp usage state.
	}
}

/**
 * The Player has just run off the edge of a baseable spriteObject. Make them lose control
 * of their x velocity, play a falling animation and set their bounds to Player::fallingBounds.
 */
void Player::justfell()
{
	//if (bRolling)
	//	bUnstable = true;
	bRolling = false;
	bCrouching = false;
	//if (vx == 0)
	//	vx = (flippedh ? -WALKSPEED : WALKSPEED);
	setBounds(fallingBounds[0], fallingBounds[1], fallingBounds[2], fallingBounds[3]);
	if (!bUnstable)
		playFalling();
	else
		vx < 0 && flippedh || vx > 0 && !flippedh ? playPushedForward() : playPushedBackward();
}

/**
 * Called when the ground under this Player is suddenly destroyed. Mainly used to determine their
 * cause of death if they should drown.
 * 
 * @param	newCollState	the collision state to set this Player to when they start falling
 */
void Player::groundDeleted(collisionState newCollState)
{
	justfell();
	//vy = FLOOREXPLOSIONIMPULSE;
	setPos(getx(), gety()-1);
	lastCollision = newCollState;
}

/**
 * Check if this Player is in a safe place and ready to do their victory dance animation
 * 
 * @return 	true if the Player is ready for a bit of a dance, false otherwise.
 */
bool Player::readyForVictory()
{
	return (inputInterrupt == 0 && !bStasis && !bUnstable && ((isOnGround() && !bCrouching && !bRolling) || bClimbingRope));		
}

//==============================================================================
//anims

///Play a victory dance animation. This is extremely important for the game to be awesome! EXTREMELY.
void Player::playVictory()
{
	if (!gm->menuBase->dieWhileDancing)
		freezePlayer();
	if (bClimbingRope)
	{
		u8 f[2] = {46, 47};
		vector<u8> temp(f, f+2);
		setArbitraryAnim(temp, false);
	} else {
		if (PA_RandMax(1))
		{
			u8 f[6] = {58, 59, 60, 61, 62, 63};
			vector<u8> temp(f, f+6);
			setArbitraryAnim(temp, false);
		} else {
			u8 f[2] = {44, 45};
			vector<u8> temp(f, f+2);
			setArbitraryAnim(temp, false);
		}
	}
}

///Play the crouch animation and freeze on the last frame
void Player::playCrouching()
{
	u8 f[2] = {6, 5};
	vector<u8> temp(f, f+2);
	setArbitraryAnim(temp, false);
}

///Play the animation for jumping straight up in the air
void Player::playJumpedStanding()
{
	u8 f[8] = {6, 12, 13, 14, 15, 16, 17, 21};
	vector<u8> temp(f, f+8);
	setArbitraryAnim(temp, false, TICKSPERFRAME);
}

///play the animation for jumping whilst running
void Player::playJumpedMoving()
{
	u8 f[6] = {17, 18, 19, 20, 20, 21};
	vector<u8> temp(f, f+6);
	setArbitraryAnim(temp, false, TICKSPERFRAME*2);
}

///play the rolling animation and stop on the last frame, which leaves us at crouching
void Player::playRolling()
{
	u8 f[5] = {7, 8, 9, 10, 5};
	vector<u8> temp(f, f+5);
	setArbitraryAnim(temp, false, TICKSPERFRAME);
}

///play the backwards roll animation and stop at a crouching position
void Player::playRollingBack()
{
	u8 f[6] = {6, 22, 23, 24, 25, 5};
	vector<u8> temp(f, f+6);
	setArbitraryAnim(temp, false, TICKSPERFRAME);
}

///play an animation cycle for climbing down a Rope. Must be arbitrary animation since it freezes mid-cycle
void Player::playClimbingDown()
{
	u8 f[4] = {41, 40, 39, 42};
	vector<u8> temp(f, f+4);
	setArbitraryAnim(temp, true, TICKSPERFRAME);
}

///play an animation cycle for climbing up a Rope. Must be arbitrary animation since it freezes mid-cycle
void Player::playClimbingUp()
{
	u8 f[4] = {39, 40, 41, 42};
	vector<u8> temp(f, f+4);
	setArbitraryAnim(temp, true, TICKSPERFRAME);
}

//===================================================================
/**
 * delete the Player%'s current Weapon
 */
void Player::clearWeapon()
{
	weaponChange();
	weaponJustCleared = true;	//stop weaponDestroyed() from calling weaponChange()
	if (currWeapon != NULL)
		delete currWeapon;
	currWeapon = NULL;
	weaponJustCleared = false;
}

/**
 * log the time spent holding the current Weapon when Weapon%s change
 */
void Player::weaponChange()
{
	u32 newTime = Tick(gm->statTimerID);
	u16 numSecs = (newTime-lastWeaponChangeTime) / 1000;
	u8 playerID = (this == gm->player1 ? gm->menuBase->player1id : gm->menuBase->player2id);
	
	u8 weaponID;
	if (currWeapon == NULL)
		weaponID = 18;
	else
		weaponID = currWeapon->getType();
		
	if (!gm->isGamePaused())
	{		
		//if we record this when the game is paused we'll get mucho random numbers
		gm->menuBase->scoreTime(playerID, weaponID, numSecs);
	}
	
	lastWeaponChangeTime = newTime;
}

//Weapon checks, for Weapons that modify touching behaviour
///quickly check if this Player is holding the 1000 Volts pickup
bool Player::has1000V()
{
	return dynamic_cast<wp_1000V *>(currWeapon) != NULL;
}

///quickly check if this Player is holding a shield
bool Player::hasShield()
{
	//for all intents and purposes, you can't have the shield when on a rope
	return dynamic_cast<wp_shield *>(currWeapon) != NULL && !bClimbingRope;
}

///quickly check if this Player is using the parachut to break their fall
bool Player::usingChut()
{
	return dynamic_cast<wp_chut *>(currWeapon) != NULL && currWeapon->isFiring();
}

///quickly check if this Player is holding a boomerang
bool Player::hasBoomerang()
{
	return dynamic_cast<wp_boomerang *>(currWeapon) != NULL;
}

///quickly check if this Player is using a magnet to pull Pickup%s in
bool Player::usingMagnet()
{
	return dynamic_cast<wp_magnet *>(currWeapon) != NULL && currWeapon->isFiring();
}

///quickly check if this Player is currently invisible via use of the invisibility Weapon 
bool Player::usingInvis()
{
	return dynamic_cast<wp_invis *>(currWeapon) != NULL && currWeapon->getAmmo() == 1;
}

//===================================================================
//sounds and effects

///Play the spawning sound effect and green-smokey singleFireSprite
void Player::playWarpEffect()
{
	u8 f[3] = {21, 22, 23};
	vector<u8> temp(f, f+3);
	gm->createSingleFireSprite(gm->FXSprite.palleteID, (this == gm->player1 ? gm->spawnGFX[0] : gm->spawnGFX[1]), temp, TICKSPERFRAME*2, getx(), gety(), OBJ_SIZE_32X32, 16, 16);
	#ifdef __WITHSOUND
	playSound(&gm->teleport);
	#endif
}

///Play a splash sound effect and marshmallow-spray singleFireSprite
void Player::playSplashEffect()
{
	u8 f[4] = {4, 5, 6, 7};
	vector<u8> temp(f, f+4);
	gm->createSingleFireSprite(gm->FXSprite.palleteID, (this == gm->player1 ? gm->spawnGFX[0] : gm->spawnGFX[1]), temp, spriteObject::TICKSPERFRAME, getx(), gameManager::MALLOWYPOS, OBJ_SIZE_32X32, 16, 16);
	#ifdef __WITHSOUND
	playSound(&gm->bigSplash);
	#endif
}

///Play the hit by skull sound and animation
void Player::playSkulled()
{
	u8 f[5] = {50, 51, 52, 53, 66};
	vector<u8> temp(f, f+5);
	setArbitraryAnim(temp, false, TICKSPERFRAME);
	#ifdef __WITHSOUND
	playSound(&gm->skullPickup);
	#endif
}

///play the animation for being obliterated
void Player::playDisintegrated()
{
	u8 f[5] = {54, 55, 56, 57, 66};
	vector<u8> temp(f, f+5);
	setArbitraryAnim(temp, false, TICKSPERFRAME);
}

///play a zap sound effect and electricity singleFireSprite, and fly dramatically offscreen
void Player::play1000VHitEffect()
{
	u8 f[2] = {8, 9};
	vector<u8> temp(f, f+2);
	gm->createSingleFireSprite(gm->FXSprite.palleteID, (this == gm->player1 ? gm->spawnGFX[0] : gm->spawnGFX[1]), temp, spriteObject::TICKSPERFRAME, getx(), gety(), OBJ_SIZE_32X32, 16, 16);
	
	setFrame(48);
	if (this == gm->player1)
		vx = (getx() < gm->player2->getx() ? POWERHITSPEED*-1 : POWERHITSPEED);
	else if (this == gm->player2)
		vx = (getx() < gm->player1->getx() ? POWERHITSPEED*-1 : POWERHITSPEED);
	vy = JUMPIMPULSE;
	#ifdef __WITHSOUND
	playSound(&gm->lightningHit);
	#endif
}

///explode upwards off the top of the screen
void Player::playMineHitEffect()
{
	setFrame(48);
	vy = POWERHITSPEED*-1;
}
