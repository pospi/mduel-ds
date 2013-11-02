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

#include "pickup.h"
#include "floorTile.h"
#include "gameManager.h"
#include "player.h"
#include "weaponIncludes.h"

/**
 * Create this pickup and assign it a spriteManager. The spriteManager assigned must also
 * be a gameManager. A lifetime, starting position and type are also specified.
 */
Pickup::Pickup(spriteManager *newgm, pickupSpawner *mySpawner, u8 lifeSeconds, s16 nx, s16 ny, u8 type, gameManager::pickupGraphics* graphicSet) :
	spriteObject(newgm), gm(dynamic_cast<gameManager*>(newgm)), spawner(mySpawner), exploding(false), 
	myGFXset(graphicSet), myIcon(NULL)
{
	#ifdef __MDDEBUG
	className = "pickup";
	macros::debugMessage(className, "constructor");
	#endif
	
	//important setup routines
	type %= NUMPICKUPVARIATIONS;
	gm->addPickup(this);
	setLifetime(lifeSeconds);
	if (myGFXset != NULL)
		myGFXset->bInUse = true;

	//create spawn puff
	u8 f[3] = {18, 19, 20};
	vector<u8> vect(f, f+3);
	if (myGFXset != NULL)
		gm->createSingleFireSprite(gm->FXSprite.palleteID, myGFXset->spawnGFX, vect, TICKSPERFRAME*2, nx, ny, OBJ_SIZE_32X32, 16, 16);
	else
		gm->createSingleFireSprite(gm->FXSprite.palleteID, gm->FXSprite.spriteData, vect, TICKSPERFRAME*2, nx, ny, OBJ_SIZE_32X32, 16, 16);
	
	//create icon sprite to match me	
	myIcon = new spriteObject(sm);
	myIcon->setPallete(gm->pickupSprite.palleteID);
	if (myGFXset != NULL)
		myIcon->giveGFX(myGFXset->iconGFX, OBJ_SIZE_16X16, 8, 8);
	else
		myIcon->giveSprite(gm->pickupSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
	myIcon->setLayer(1);
	myIcon->setFrame(0);
	addChild(myIcon);
	
	//create graphics
	setPallete(gm->pickupSprite.palleteID);
	if (myGFXset != NULL)
		giveGFX(myGFXset->blobGFX, OBJ_SIZE_16X16, 8, 8);
	else
		giveSprite(gm->pickupSprite.spriteData, OBJ_SIZE_16X16, 8, 8);
	
	setBounds(-6, 6, 6, -6);
	setCollision(COL_SOLID);
	setLayer(1);
	
	//set other properties
	setType(type);
	setPos(nx, ny);
}

/**
 * When destroyed, automatically play a singleFireSprite dieing effect and sound.
 * Also inform the pickupSpawner who created us that they are free to create a pickup to replace
 * this one.
 */
Pickup::~Pickup()
{
	if (gm->isResetting())
	{
		/*
		So, WHAT FOR BOWDGE!?, you might say. From what I can gather, it looks like I exceeded no$gba's
		ability to remove things from OAM. Take the wait out, and compile a non-debug build. Go on, I'll
		wait. Do it pussy! What's the matter? never-
		So that's done. Now open the game in no$gba, wait for a pickup to spawn, and jump in the drink.
		See? What the shit! Now compile in debug mode. Works fine, right? I imagine it's because of the
		execution slowdown with all the logging going on. So yeah, solution is to have a wait here. You
		don't even notice it. Don't look at me like that.
		*/
		PA_WaitForVBL();
		return;
	}

	gm->removePickup(this);	//remove from pickups array

	//myIcon->destroy();		<-- already done for us via spriteObject::deleteSprite

	if (spawner != NULL)
		spawner->pickupDied();

	vector<u8> temp;
	if (!exploding)
	{
		u8 dieAnim[2] = {16, 17};
		temp = vector<u8>(dieAnim, dieAnim+2);
	} else {
		u8 dieAnim[4] = {0, 1, 2, 3};
		temp = vector<u8>(dieAnim, dieAnim+4);
		#ifdef __WITHSOUND
		playSound(&gm->platformExplode);
		#endif
	}
	if (myGFXset != NULL)
		gm->createSingleFireSprite(gm->FXSprite.palleteID, myGFXset->dieGFX, temp, TICKSPERFRAME*2, getx(), gety(), OBJ_SIZE_32X32, 16, 16);
	else
		gm->createSingleFireSprite(gm->FXSprite.palleteID, gm->FXSprite.spriteData, temp, TICKSPERFRAME*2, getx(), gety(), OBJ_SIZE_32X32, 16, 16);
	
	if (myGFXset != NULL)
		myGFXset->bInUse = false;
}

/**
 * The mutation that will be performed on a Player when this Pickup comes in contact with them.
 * A different action is performed, depending on Pickup::pickupType.
 * 
 * @param	other	a reference to the Player who touched this Pickup 
 */
void Pickup::playerTouchAction(Player* other)
{
	switch (myType)
	{
		case PT_SKULL:
			gm->playerSkulled(other);
		break;
		case PT_1000V:
			other->setWeapon(new wp_1000V(other));
		break;
		case PT_INVIS:
			other->setWeapon(new wp_invis(other));
		break;
		case PT_MINE:
			other->setWeapon(new wp_mine(other));
		break;
		case PT_GUN:
			other->setWeapon(new wp_gun(other));
		break;
		case PT_TNT:
		break;
		case PT_BOOT:
			other->setWeapon(new wp_boot(other));
		break;
		case PT_GRENADE:
			other->setWeapon(new wp_gren(other));
		break;
		case PT_PUCK:
			other->setWeapon(new wp_puck(other));
		break;
		case PT_CHUT:
			other->setWeapon(new wp_chut(other));
		break;
		case PT_HOOK:
			other->setWeapon(new wp_hook(other));
		break;
		case PT_WARP:
			other->setWeapon(new wp_warp(other));
		break;
		case PT_MAGNET:
			other->setWeapon(new wp_magnet(other));
		break;
		case PT_NET:
			other->setWeapon(new wp_net(other));
		break;
		case PT_SHIELD:
			other->setWeapon(new wp_shield(other));
		break;
		case PT_DUNCE:
			other->setWeapon(new wp_dunce(other));
		break;
		case PT_WEASEL:
			other->setWeapon(new wp_weasel(other));
		break;
		case PT_BOOMERANG:
			other->setWeapon(new wp_boomerang(other));
		break;
		default:
			other->clearWeapon();
		break;
	}
}

/**
 * Set the type of this Pickup and make the according icon. Also initialises the animation
 * cycle for this Pickup and any sounds it may need to play.
 * 
 * @param	newType	an integer corresponding to one of Pickup::pickupType
 */
void Pickup::setType(u8 newType)
{	
	myType = (pickupType)(newType%NUMPICKUPVARIATIONS);
	
	#ifdef __WITHSOUND
	stopSound();
	#endif
	setCheckCollision(false);
	
	switch (myType)
	{
		case PT_SKULL:
			#ifdef __WITHSOUND
			playSound(&gm->skullLoop, true, 64);
			#endif
			makeIcon(3);
		break;
		case PT_1000V:
			makeIcon(4);
		break;
		case PT_INVIS:
			makeIcon(5);
		break;
		case PT_MINE:
			makeIcon(6);
		break;
		case PT_GUN:
			makeIcon(7);
		break;
		case PT_TNT:
			setCollision(COL_CENTERPOINT);
			setCheckCollision(true);	//doesnt interact with players
			makeIcon(8);
		break;
		case PT_BOOT:
			makeIcon(9);
		break;
		case PT_GRENADE:
			makeIcon(10);
		break;
		case PT_PUCK:
			makeIcon(11);
		break;
		case PT_CHUT:
			makeIcon(12);
		break;
		case PT_HOOK:
			makeIcon(13);
		break;
		case PT_WARP:
			makeIcon(14);
		break;
		case PT_MAGNET:
			makeIcon(15);
		break;
		case PT_NET:
			makeIcon(16);
		break;
		case PT_SHIELD:
			makeIcon(17);
		break;
		case PT_DUNCE:
			makeIcon(18);
		break;
		case PT_WEASEL:
			makeIcon(19);
		break;
		case PT_BOOMERANG:
			makeIcon(20);
		break;
	}
	
	//reset the anim so that not all Pickup%s are looping at the same time
	setAnim(0, 2, ANIMSPEED);
}

/**
 * set the Pickup::myIcon sprite's frame to the frame specified
 * 
 * @param	frame	the frame the icon should use
 */
void Pickup::makeIcon(u8 frame)
{
	myIcon->setFrame(frame);
}

/**
 * Mutations to happen when this Pickup is colliding with another spriteObject.
 * Pickup::PT_TNT being the only kind of Pickup which uses active collision, this function is sufficient
 * to check for collisions with floorTile%s only.
 * 
 * @param	other	the spriteObject this Pickup is touching
 */
void Pickup::collidingWith(spriteObject *other)
{
	if (dynamic_cast<floorTile*>(other) != NULL)
	{
		if (gm->player1->isStandingOn(const_cast<spriteObject*>(other)))
			gm->player1->groundDeleted(Player::CS_TNTFALL);
		if (gm->player2->isStandingOn(const_cast<spriteObject*>(other)))
			gm->player2->groundDeleted(Player::CS_TNTFALL);
		
		other->destroy();
		
		for (vector<spriteObject*>::iterator it = sm->gameSprites.begin(); it != sm->gameSprites.end(); ++it)
		{
			if (dynamic_cast<floorTile*>(*it) != NULL &&
				((*it)->pointCollision(getLeft(), gety()) || 
				(*it)->pointCollision(getRight(), gety())))
			{
				(*it)->destroy();
			}
		}
		
		destroy();
		exploding = true;	//play smoke FX
		return;
	} 
	spriteObject::collidingWith(other);
}

/**
 * Check whether or not this Pickup is colliding with another spriteObject.
 * 
 * @param	other	a reference to the spriteObject to test collision against.
 * 					The collision can only succeed if:
 * 					- other is a floorTile and this Pickup%'s type is Pickup::PT_TNT, or
 * 					- other is a Player and this Pickup%'s type is not Pickup::PT_TNT.
 * @param	checkReverse	whether or not to compute collision for the complementary spriteObject
 * 
 * @return true if this Pickup is colliding with other, false otherwise
 */
bool Pickup::isColliding(spriteObject *other, bool checkReverse) const
{
	if ((dynamic_cast<floorTile*>(other) == NULL && getType() == PT_TNT) ||
		(dynamic_cast<Player*>(other) == NULL && getType() != PT_TNT))
		return false;
	
	return spriteObject::isColliding(other, checkReverse);
}

/**
 * Call any update functions on this Pickup, then make it bounce off the edges of the screen if necessary.
 */
void Pickup::updateSprite()
{
	spriteObject::updateSprite();
	
	//bounce off any walls
	if (getx() <= MINX || getx() >= MAXX)
		vx *= -1;
	if (gety() <= MINY || gety() >= MAXY)
		vy *= -1;
}
