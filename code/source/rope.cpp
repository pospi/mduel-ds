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

#include "rope.h"

/**
 * Create a Rope and assign it a spriteManager. Creating a parent Rope will in turn
 * create several child Rope%s - these are essentially dumb graphics and perform not much
 * more than hittests with the Player%s.
 * 
 * @param	newsm	the spriteManager which should manage this spriteObject
 * @param	isCap	true if this Rope is the parent Rope of its descendants
 */
Rope::Rope(spriteManager *newsm, bool isCap) : spriteObject(newsm), childGFX(0)
{
	#ifdef __MDDEBUG
	className = "rope";
	macros::debugMessage(className, "constructor");
	#endif
	if (isCap)
	{
		setBounds(-8, 8, 8, -8);
		setFrame(4);
		setCollision(COL_NONE);
	} else {
		setBounds(-8, 0, 8, 0);
		setFrame(5);
		setCollision(COL_SOLID);
	}
	setLayer(2);
}

Rope::~Rope()
{
	//spritemanager always handles this garbage cleanup!
}

/**
 * When this Rope is made static (immobile and not calling collision checks), all of its Rope::childRopes
 * should follow suit.
 */
void Rope::makeStatic()
{
	for (u8 i=0; i<childRopes.size(); ++i)
		childRopes[i]->makeStatic();
	spriteObject::makeStatic();
}

/**
 * Set the length of this Rope to this many child sprites, which are each 16 pixels high.
 * 
 * @param	numRopes	this many tiles will be added to the bottom
 * @return	true, because I haven't implemented any checking yet.
 */
bool Rope::setLength(u8 numRopes)
{
	if (numRopes > childRopes.size())	//add ropes
	{
		for (u8 i=childRopes.size(); i<numRopes; ++i)
		{
			Rope * temp = new Rope(sm, false);
			temp->setPallete(palleteID);
			temp->giveGFX(childGFX, OBJ_SIZE_16X16, 8, 8);
			temp->setPos(getx(), gety() + centery*2*(i+1));
			temp->parentRope = this;
			childRopes.push_back(temp);
			//sm->gameSprites.push_back(temp);
		}
	} else if (numRopes < childRopes.size())	//delete ropes TODO: its old and crashy but im not using it
	{
		for (u8 i=childRopes.size()-1; i>=numRopes; i--)
			delete childRopes[i];
		childRopes.erase(childRopes.end() - numRopes, childRopes.end());
		//TODO! not sure if this is working, cos I don't really use it at all :S
	}
	return true;
}

/**
 * When setting this Rope%'s position, all childRopes should move along with it.
 */
void Rope::setPos(s16 nx, s16 ny)
{
	spriteObject::setPos(nx, ny);
	for (u8 i=0; i<childRopes.size(); ++i)
	{
		childRopes[i]->setPos(nx, ny + centery*2*(i+1));
	}
}
