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

#include "massObject.h"

/**
 * Builds a new massObject and assigns it to a spriteManager.
 * 
 * @param	newsm	the spriteManager which should be responsible for updating and deleting this sprite
 */
massObject::massObject(spriteManager* newsm)
	: spriteObject(newsm), grounded(false), wasGrounded(false), bStasis(false)
{
	#ifdef __MDDEBUG
	className = "massObject";
	macros::debugMessage(className, "constructor");
	#endif
	setCollision(COL_SOLID);
	setCheckCollision(true);
}

massObject::~massObject()
{
	
}

/**
 * Updates the sprite to apply gravitational effects to it.
 * The sprite will accellerate downwards unless it lands on another baseable spriteObject.
 */
void massObject::updateSprite()
{
	spriteObject::updateSprite();	//move sprite first
	
	wasGrounded = grounded;
	if (!isOnGround() && !bStasis)
	{
		vy += GRAVITY;
		if (vy > TERMINALVEL)
			vy = TERMINALVEL;
		checkPenetration();
	}
	
	grounded = false;		//is re-checked in collidingWith() later in the tick
}

/**
 * When standing on another SpriteObject, flag this massObject as grounded and set its y velocity to 0.
 * 
 * @param	other	the spriteObject we are touching to be checked for standing on.
 */
void massObject::collidingWith(spriteObject *other)
{
	if (isStandingOn(const_cast<spriteObject*>(other)))
	{
		grounded = true;
		vy = 0;
	}
}

/**
 * Update the massObject%'s position so that it lands on any baseable spriteObject%s that it may otherwise
 * pass through in the tick.
 */
void massObject::checkPenetration()
{
	for (vector<spriteObject*>::iterator it = sm->gameSprites.begin(); it != sm->gameSprites.end(); ++it)
	{
		if (*it == this || !(*it)->isBaseable())
			continue;
		if (inVertPlaneOf(*it) && isOver(*it))
		{
			y += vy;
			if (isUnder(*it) || inHorizPlaneOf(*it))
			{
				y -= vy;
				//round y off first
				s16 rounds = y % 256;
				y += rounds;
				//adjust vertical velocity accordingly.
				vy = (*it)->getTop() - getBottom();
				vy = vy<<8;
			} else {
				y -= vy;
			}
		}			
	}
}
