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

//base sprite class that is effected by gravity

#ifndef _MASSOBJECT_H
#define _MASSOBJECT_H

#include <PA9.h>
#include "macros.h"
#include "spriteObject.h"

/**
 * The base class for any spriteObject that should obey the effects of gravity.
 * 
 * massObject%s will continue falling at a constant rate of acceleration (massObject::GRAVITY)
 * until they reach terminal velocity (massObject::TERMINALVEL). If they are interrupted at any
 * point by a baseable spriteObject, they will stop falling and 'land' on that spriteObject.
 * 
 * @author pospi
 */
class massObject : public spriteObject
{
	public:
		massObject(spriteManager* newsm);
		virtual ~massObject();
		
		virtual void updateSprite();
		virtual void checkPenetration();
		virtual void collidingWith(spriteObject* other);
		
		/**
		 * Check whether this massObject is currently on the ground.
		 * @return true if on the ground, false otherwise
		 */
		bool isOnGround() const {return grounded;}
		/**
		 * Check whether this massObject was on the ground in the previous tick.
		 * Useful for detecting changes in object state, firing animation events etc.
		 * @return true if just previously on the ground, false otherwise
		 */
		bool wasOnGround() const {return wasGrounded;}
		
		/**
		 * Check whether or not the massObject should stop falling.
		 * Reasonably sure this isn't actually implemented at present.
		 * @return true if y-velocity is halted, false otherwise
		 */
		bool isFrozen() {return bStasis;}
		
	protected:
		///downwards acceleration per frame in fixed point format (1/8 pixel)
		static const u8 GRAVITY = 32;
		///maximum falling speed in fixed point format (4 pixels/frame)
		static const u16 TERMINALVEL = 1024;
		
		///whether or not the massObject is currently on the ground
		bool grounded;
		///grounded state in previous tick
		bool wasGrounded;
		///freeze this object's y-velocity if true
		bool bStasis;
};

#endif
