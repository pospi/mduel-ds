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

#ifndef _FLOORTILE_H
#define _FLOORTILE_H

#include <PA9.h>
#include <dswifi9.h>
#include <vector>
	using std::vector;
#include "macros.h"
#include "spriteObject.h"

/**
 * A very basic class to automate setting up the game's floor.
 * 
 * The only differences from that of spriteObject are automated setup of collision,
 * bounds and screen layer.
 * 
 * @author pospi
 */
class floorTile : public spriteObject
{
	public:
		floorTile(spriteManager *newsm);
		virtual ~floorTile();
};

#endif
