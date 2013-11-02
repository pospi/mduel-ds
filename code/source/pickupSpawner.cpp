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

#include "pickupSpawner.h"
#include "pickup.h"
#include "gameManager.h"
#include "menuGBottom.h"

/**
 * Create a new pickupSpawner and assign it to a spriteManager.
 * pickupSpawner%s initialise themselves on screen layer 3 so that they are behind most of the other
 * game sprites.
 * 
 * @param	newgm	the gameManager to which this pickupSpawner belongs
 */
pickupSpawner::pickupSpawner(spriteManager *newgm) :
	spriteObject(newgm), pickupDir(SDIR_DOWN)
{
	#ifdef __MDDEBUG
	className = "pickupSpawner";
	macros::debugMessage(className, "constructor");
	#endif
	setLayer(3);
}

pickupSpawner::~pickupSpawner()
{
	
}

/**
 * Decrement spawn counters and generate Pickup%s at our location when one of the queued Pickup%s
 * timers reaches 0.
 */
void pickupSpawner::updateSprite()
{
	gameManager *gm = dynamic_cast<gameManager*>(sm);
	
	vector<pendingPickup>::iterator pk = pendingPickups.begin();
	while (pk != pendingPickups.end())
	{
		if ((*pk).framesUntilSpawn == 0)
		{
			u8 lifeTime = PA_RandMinMax(PICKUPLIFEMIN, PICKUPLIFEMAX);
			s16 tempx = PA_RandMinMax(PICKUPSPEEDMIN, PICKUPSPEEDMAX);
			s16 tempy = PA_RandMinMax(PICKUPSPEEDMIN, PICKUPSPEEDMAX);
			s8 xoffset = 0, yoffset = 0;
			s16 puffPosx = getx();
			s16 puffPosy = gety()-4;
			
			if (pickupDir == SDIR_DOWN)
			{
				tempx = PA_RandMax(PICKUPSPEEDMAX);
				if (PA_RandMax(1))
					tempx *= -1;
				yoffset = 8;
				puffPosy+=8;
			} else if (pickupDir == SDIR_LEFT)
			{
				tempy = PA_RandMax(PICKUPSPEEDMAX);
				if (PA_RandMax(1))
					tempy *= -1;
				tempx *= -1;
				xoffset = -8;
				puffPosx-=8;
			} else if (pickupDir == SDIR_RIGHT)
			{
				tempy = PA_RandMax(PICKUPSPEEDMAX);
				if (PA_RandMax(1))
					tempy *= -1;
				xoffset = 8;
				puffPosx+=8;
			}
			float speedx = (float)tempx / 256;
			float speedy = (float)tempy / 256;
			
			Pickup *temp = new Pickup(gm, this, lifeTime, getx()+xoffset, gety()+yoffset,
										(*pk).nextType, gm->getNextAvailablePickupGFXSet());
			
			if (temp != NULL)		//if it is, welll shit!
				temp->setSpeed(speedx, speedy);				
			
			//this one has spawned, clear it from the queue.
			pk = pendingPickups.erase(pk);
			continue;
		} 
		if ((*pk).framesUntilSpawn > 0) {
			(*pk).framesUntilSpawn--;
		}
		++pk;
	}
	spriteObject::updateSprite();
}

/**
 * Recieve notification from a Pickup that it has run out of lifetime and destroyed itself.
 * Reset the idle time of the relevant spawn counter in gameManager::gameBottom.
 */
void pickupSpawner::pickupDied()
{
	resetIdleTime();
}

/**
 * Reset the timer in gameManager::gameBottom that corresponds to this pickupSpawner, so that
 * a new Pickup will be queued to spawn in its place.
 */
void pickupSpawner::resetIdleTime()
{
	menuGBottom *mb = dynamic_cast<menuGBottom*>(dynamic_cast<gameManager*>(sm)->gameBottom);
	mb->resetIdleTime(pickupDir);
	mb->pickupsActive--;
}

/**
 * Called by gameManager::gameBottom to queue another Pickup to spawn via this pickupSpawner.
 * 
 * @param	secs	the time that the new Pickup should wait before being spawned
 * @param	type	the type (Pickup::pickupType) of the new Pickup to spawn
 */
void pickupSpawner::addSpawnTime(u8 secs, u8 type)
{
	pendingPickup p;
	p.framesUntilSpawn = secs*ANIMSPEED*TICKSPERFRAME;
	p.nextType = type;
	pendingPickups.push_back(p);
}
