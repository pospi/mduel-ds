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

#ifndef _WPPUCKPROJ_H
#define _WPPUCKPROJ_H

#include <PA9.h>
#include "macros.h"
#include "massObject.h"

//forward declaration
class Player;

class wp_puck_proj : public massObject
{
	public:
		wp_puck_proj(spriteManager* newsm, Player* p);
		virtual ~wp_puck_proj();
		
		virtual void updateSprite();
		virtual void collidingWith(spriteObject *other);
	
	private:
		Player* owner;
};

#endif
