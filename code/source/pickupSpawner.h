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

#ifndef _PICKUPSPAWNER_H
#define _PICKUPSPAWNER_H

#include <PA9.h>
#include <dswifi9.h>
#include <vector>
	using std::vector;
#include "macros.h"
#include "spriteObject.h"

//forward declaration
class Pickup;

/**
 * A spriteObject that is additionally responsible for spawning Pickup%s into the level.
 * 
 * pickupSpawner%s work closely with a menuGBottom instance referenced through gameManager::gameBottom.
 * The menuGBottom handles the Pickup spawning and countdown whilst notifying the pickupSpawner when it
 * should fire a new Pickup.
 * 
 * Three pickupSpawner%s are spawned into the game by the gameManager - one at the top of the screen and
 * one at each side. pickupSpawner::spawnDir tells the spawner where it is positioned, and which direction
 * its Pickup%s should travel in once spawned.
 * 
 * Pickup%s are spawned moving in a random direction, at a random speed, and with a random lifetime.
 * Various pickupSpawner members dictate the range of these random initialisation values.
 * 
 * @author pospi
 */
class pickupSpawner : public spriteObject
{
	public:
		pickupSpawner(spriteManager *newgm);
		virtual ~pickupSpawner();
		
		virtual void updateSprite();
		
		void pickupDied();
		
		/**
		 * The direction that a Pickup will travel in after being spawned by a pickupSpawner.
		 * This is an approximate direction - the Pickup will begin moving somewhere in a 45 degree arc
		 * from the direction specified.
		 */
		enum spawnDir
		{
			SDIR_DOWN,
			SDIR_LEFT,
			SDIR_RIGHT
		};
		void setDirection(spawnDir dir) {pickupDir = dir;}
		spawnDir getDirection() {return pickupDir;}
		
		void addSpawnTime(u8 secs, u8 type);
		
		///the shortest time a pickup will stay alive for, in seconds
		static const u8 PICKUPLIFEMIN = 10;
		///the longest time a pickup will stay alive for, in seconds
		static const u8 PICKUPLIFEMAX = 25;
		///the minimum time this pickupSpawner will stay idle between spawns, in tenths of a second
		static const u8 IDLEMIN = 26;
		///the maximum time this pickupSpawner will stay idle between spawns, in tenths of a second
		static const u8 IDLEMAX = 50;
		
	private:
		///the minimum speed a pickup will be moving at when it spawns, in fixed point format
		static const u16 PICKUPSPEEDMIN = 64;
		///the maximum speed a pickup will be moving at when it spawns, in fixed point format
		static const u16 PICKUPSPEEDMAX = 144;
		
		void resetIdleTime();
		
		/**
		 * A Pickup that has not yet been spawned.
		 * Contains the time (number of frames) until this Pickup will spawn, and the type
		 * (corresponding to Pickup::pickupType) that the new Pickup will be.
		 */
		struct pendingPickup
		{
			u16	framesUntilSpawn;
			u8 nextType;
		};
		
		///a queue specifying how long each queued Pickup should wait before spawning, and which type each queued Pickup should be
		vector<pendingPickup> pendingPickups;
		///the direction this pickupSpawner%'s Pickup%s should move in when spawned
		spawnDir pickupDir;
};

#endif
