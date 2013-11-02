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

#include "spriteObject.h"

/**
 * Builds a new spriteObject and assigns it to a spriteManager.
 * 
 * @param	newsm	the spriteManager which should be responsible for updating and deleting this sprite
 */
spriteObject::spriteObject(spriteManager *newsm)
	:  bNeedsGFXCleanup(false), attachment(NULL), isAttachment(false), staticSprite(false), sm(newsm), 
	parentSprite(NULL), childSprite(NULL), spriteID(-1), palleteID(-1), gfxID(-1),
		#ifdef __WITHSOUND
		soundChannel(-1),
		#endif
		x(0), y(0), vx(0), vy(0), oldx(0), oldy(0), flippedh(false), flippedv(false),
	  oldFliph(false), oldFlipv(false), layer(0), frame(0), oldFrame(0), arbitraryAnim(0),
	  arbitrarySpeed(0), arbitraryFrameCounter(0), arbitraryTimeCounter(0), centerx(0),
	  centery(0), collision(COL_NONE), checkCollision(false), ignoreUntilUntouched(NULL), rotSetID(-1), 
	  rotAngle(0), bRotating(false), UPDATEDlayer(false), UPDATEDflip(false), framesToLive(-1)
{
	sm->gameSprites.push_back(this);
	#ifdef __MDDEBUG
	className = "spriteObject";
	char buffer[255];
	sprintf(buffer, "%d: constructed at gameSprites %d", sm->screen, sm->gameSprites.size()-1);
	macros::debugMessage(className, buffer);
	#endif
}

/**
 * Halt any (looping) sounds that this spriteObject was playing and delete all sprite data.
 */
spriteObject::~spriteObject()
{
	#ifdef __WITHSOUND
	stopSound();
	#endif
	deleteSprite();
}

/**
 * Here is a copy constructor that probably isn't needed. I've been over my code and I'm pretty sure
 * there's actually no cases where I pass objects by value. So I'm probably pretty safe in just.. not
 * bothering.
 */
spriteObject::spriteObject(const spriteObject &s)
{
	sm = s.sm;
	parentSprite = s.parentSprite;
	childSprite = s.childSprite;		//perhaps childSprite and attachment should be freshly created?
	attachment = s.attachment;
	ignoreUntilUntouched = s.ignoreUntilUntouched;
	
	bNeedsGFXCleanup = s.bNeedsGFXCleanup;
	isAttachment = s.isAttachment;
	staticSprite = s.staticSprite;
	spriteID = s.spriteID;
	palleteID = s.palleteID;
	gfxID = s.gfxID;
	#ifdef __WITHSOUND
	soundChannel = s.soundChannel;
	#endif
	x = s.x; y = s.y; vx = s.vx; vy = s.vy; oldx = s.oldx; oldy = s.oldy;
	flippedh = s.flippedh; flippedv = s.flippedv; oldFliph = s.oldFliph; oldFlipv = s.oldFlipv;
	layer = s.layer; frame = s.frame; oldFrame = s.oldFrame;
	arbitraryAnim = s.arbitraryAnim; arbitrarySpeed = s.arbitrarySpeed;
	arbitraryFrameCounter = s.arbitraryFrameCounter; arbitraryTimeCounter = s.arbitraryTimeCounter;
	centerx = s.centerx; centery = s.centery; collision = s.collision;
	checkCollision = s.checkCollision;
	rotSetID = s.rotSetID; rotAngle = s.rotAngle; bRotating = s.bRotating;
	UPDATEDlayer = s.UPDATEDlayer; UPDATEDflip = s.UPDATEDflip;
	framesToLive = s.framesToLive;
}

/**
 * Give sprite data to this sprite. This will create new GFX for the sprite and load the sprite 
 * into one of the spriteManager%'s 128 OAM slots.
 * 
 * @param	spriteData	pointer to the sprite data
 * @param	sizex	memory constant to specify x size of the sprite
 * @param	sizey	memory constant to specify y size of the sprite
 * @param	cx	x offset from top-left corner to the center of the sprite
 * @param	cy	y offset from top-left corner to the center of the sprite
 * @param	minIndex	minimum sprite index in the spriteManager OAM array, which can be used as a depth hierachy
 * @param	nx	starting x screen position in pixels
 * @param	ny	starting y screen position in pixels
 * @param	colorMode	colour mode, using PALib constants. 1 = 8bit.
 * 
 * @return	sprite position in OAM array or -1 on failure
 */
s8 spriteObject::giveSprite(const unsigned char *spriteData, u8 sizex, u8 sizey, u8 cx, u8 cy, u8 minIndex,  s16 nx, s16 ny, u8 colorMode)
{
	if (palleteID == -1) {
		#ifdef __MDDEBUG
		macros::debugMessage(ERRTEXT, "giveSprite() but no pal!", ERRORPOS);
		#endif
		return -1;
	}
	
	//sprites are always created at OFFX,OFFY and then moved afterwards to prevent flickering.
	
	#ifdef __MDDEBUG
	char buffer[255];
	sprintf(buffer, "%d: attempting to get raw sprite", sm->screen);
	macros::debugMessage(className, buffer);
	#endif
	//lets try this a new way...
	gfxID = sm->loadGFX(spriteData, sizex, sizey, colorMode);
	spriteID = sm->loadSpriteFromGFX(gfxID, sizex, sizey, palleteID, minIndex, colorMode, OFFX, OFFY);
	bNeedsGFXCleanup = true;

	#ifdef __MDDEBUG
	memset(buffer, 0, 255);
	sprintf(buffer, "%d: raw sprite given (gfx #%d, sprite #%d)", sm->screen, gfxID, spriteID);
	macros::debugMessage(className, buffer);
	#endif
	
	//spriteID = sm->loadSprite(spriteData, sizex, sizey, minIndex, palleteID, OFFX, OFFY, colorMode);
	
	centerx = cx;
	centery = cy;
	setPos(nx, ny);
	return spriteID;
}

/**
 * Give raw pallete data to this sprite. Pallete will be loaded into one of the 16 available pallete slots.
 *
 * @param	palleteData	a pointer to raw pallete data
 * 
 * @return	ID of the created pallete in this spriteObject%'s spriteManager
 */
s8 spriteObject::givePallete(const unsigned short *palleteData)
{
	#ifdef __MDDEBUG
	char buffer[255];
	sprintf(buffer, "%d: attempting to get raw pallete", sm->screen);
	macros::debugMessage(className, buffer);
	#endif
	palleteID = sm->loadPallete((void*)palleteData);
	#ifdef __MDDEBUG
	memset(buffer, 0, 255);
	sprintf(buffer, "%d: raw pallete given (#%d)", sm->screen, palleteID);
	macros::debugMessage(className, buffer);
	#endif
	return palleteID;
}

/**
 * Update the pallete of this sprite with a new one. Only works for sprites created with giveGFX().
 * 
 * @param	newPalID ID of the new pallete in spriteManager palletes array
 * @param	sizex	memory constant to specify x size of the sprite
 * @param	sizey	memory constant to specify y size of the sprite
 * @param	colorMode	colour mode, using PALib constants. 1 = 8bit.
 * 
 * @return true if pallete was updated, false otherwise
 */
bool spriteObject::updatePallete(s8 newPalID, u8 sizex, u8 sizey, u8 colorMode)
{
	#ifdef __MDDEBUG
	char buffer[255];
	sprintf(buffer, "%d: attempting pallete update (#%d -> #%d)", sm->screen, palleteID, newPalID);
	macros::debugMessage(className, buffer);
	#endif
	if (gfxID != -1 && spriteID != -1 && newPalID != -1)
	{
		palleteID = newPalID;
		sm->updateSpritePallete(spriteID, gfxID, newPalID, sizex, sizey, colorMode, x>>8, y>>8);
		UPDATEDflip = true;		//giving a new pallete resets the GFX
		return true;
	}
	return false;
}

/**
 * Assign a pre-created GFX object to this sprite. This will create new GFX for the sprite and  
 * load the sprite into one of the spriteManager%'s 128 OAM slots.
 * 
 * @param	GFXid	index of sprite GFX in memory
 * @param	sizex	memory constant to specify x size of the sprite
 * @param	sizey	memory constant to specify y size of the sprite
 * @param	cx	x offset from top-left corner to the center of the sprite
 * @param	cy	y offset from top-left corner to the center of the sprite
 * @param	minIndex	minimum sprite index in the spriteManager OAM array, which can be used as a depth hierachy
 * @param	nx	starting x screen position in pixels
 * @param	ny	starting y screen position in pixels
 * @param	colorMode	colour mode, using PALib constants. 1 = 8bit.
 * 
 * @return	sprite position in OAM array or -1 on failure
 */
s8 spriteObject::giveGFX(u16 GFXid, u8 sizex, u8 sizey, u8 cx, u8 cy, u8 minIndex, s16 nx, s16 ny, u8 colorMode)
{
	#ifdef __MDDEBUG
	char buffer[255];
	sprintf(buffer, "%d: attempting to get preloaded GFX", sm->screen);
	macros::debugMessage(className, buffer);
	#endif
	gfxID = GFXid;
	spriteID = sm->loadSpriteFromGFX(GFXid, sizex, sizey, palleteID, minIndex, colorMode, OFFX, OFFY);
	#ifdef __MDDEBUG
	memset(buffer, 0, 255);
	sprintf(buffer, "%d: preloaded GFX given (gfx #%d, sprite #%d)", sm->screen, gfxID, spriteID);
	macros::debugMessage(className, buffer);
	#endif
	centerx = cx;
	centery = cy;
	setPos(nx, ny);
	return spriteID;
}

/**
 * Cleanup function to remove all low-level sprite junk from memory.
 * Should really be in spriteManager, but then you have to pass an object that's 
 * being destroyed to another function, and I imagine that isn't wise.
 * 
 * Cleanup of child sprites, GFX and the spriteManager%'s OAM array are only
 * performed if the spriteManager is not resetting, since otherwise this is a redundant
 * step.
 * 
 * @see spriteManager::clearOutSprites
 */
void spriteObject::deleteSprite()
{	
	#ifdef __MDDEBUG
	char buffer[255];
	sprintf(buffer, "%d: deleting (gfx #%d, sprite #%d, pallete #%d)", sm->screen, gfxID, spriteID, palleteID);
	macros::debugMessage(className, buffer);
	#endif
	
	PA_StopSpriteAnim(sm->screen, getSpriteID());
	PA_SetSpriteXY(sm->screen, getSpriteID(), OFFX, OFFY);
	
	//this stuff isnt necessary if the entire sprite system is resetting
	if (!sm->isResetting())
	{
		if (bNeedsGFXCleanup)
			PA_DeleteGfx(sm->screen, getGFXid());
		if (hasGraphics())
			sm->removeSprite(getSpriteID());
		
		//if (parentSprite != NULL)
		//	parentSprite->removeChild(this);
		
		#ifdef __MDDEBUG
		memset(buffer, 0, 255);
		sprintf(buffer, "%d: clearChildren", sm->screen);
		macros::debugMessage(className, buffer);
		#endif

		if (childSprite != NULL)
			childSprite->destroy();
		childSprite = NULL;
		
		//TODO: this method of deletion requires that children be created after parents
		/*if (!childSprites.empty())
		{
			vector<spriteObject*>::iterator it = childSprites.begin();
			while ( it != childSprites.end() )
			{
				if (*it != NULL)
				{
					(*it)->destroy();
				}
				#ifdef __MDDEBUG
				else
					macros::debugMessage(ERRTEXT, "NULL childSprite found", ERRORPOS);
				#endif
				++it;
			}
			childSprites.clear();
		}*/
	}
	
	#ifdef __MDDEBUG
	memset(buffer, 0, 255);
	sprintf(buffer, "%d: deleteSprite[end]", sm->screen);
	macros::debugMessage(className, buffer);
	#endif
}

/**
 * Work out whether or not this sprite has any visible graphics assigned.
 * 
 * @return whether or not this sprite has an assigned place in the spriteManager%'s OAM array
 */
bool spriteObject::hasGraphics() const
{
	return (sm->isSprite(spriteID));
}

/**
 * The main 'tick' loop for this sprite, executed every frame.
 * Basically updates visual properties only - further logic is left for child classes to implement.
 * 
 * @see spriteManager::gameTick
 */
void spriteObject::updateSprite()
{
	if (hasGraphics())
	{
		if (isAttachment && parentSprite != NULL)
			setFrame(parentSprite->getFrame());
		else if (arbitraryAnim != 0)
		{
			if (arbitraryTimeCounter == 0)
			{
				frame = arbFrames[arbitraryFrameCounter];
				++arbitraryFrameCounter;
			}
			++arbitraryTimeCounter;
			arbitraryTimeCounter %= arbitrarySpeed;
			if (arbitraryFrameCounter == arbFrames.size())
			{
				if (arbitraryAnim != 2)
					arbitraryAnim = 0;	//only played once, so stop animation and break out
				else
					arbitraryFrameCounter = 0;
			}
		}
		
		if (oldFrame != frame)
		{
			PA_SetSpriteAnim(sm->screen, spriteID, frame);
			oldFrame = frame;
		}
	}
	
	if (staticSprite)
		return;
	x += vx;	//take speed into account
	y += vy;
	
	if (hasGraphics())	//if this is an invisible sprite then we only really care about its position vars
	{
		if (parentSprite != NULL)
		{
			//TODO! this actually depends on the order sprites are created, which is BAD!
			//however, given that I never plan on making hierachies of more than 2, probably doesn't matter.
			setFlipped(parentSprite->flippedh, parentSprite->flippedv);
			s16 extrax = 0, extray = 0;
			for (spriteObject *i = this; i != NULL; i = i->parentSprite)
			{
				if (i->parentSprite == NULL)
				{
					extrax += i->getx();
					extray += i->gety();
				} else {
					extrax += (i->parentSprite->flippedh ? i->getx()*-1 : i->getx());
					extray += (i->parentSprite->flippedv ? i->gety()*-1 : i->gety());
				}
			}
			PA_SetSpriteXY(sm->screen, spriteID, extrax-centerx, extray-centery);
		}
		else if (x != oldx || y != oldy)
		{
			PA_SetSpriteXY(sm->screen, spriteID, (x>>8), (y>>8));	//use center as registration point
			oldx = x;
			oldy = y;
		}
		
		if (UPDATEDflip)
		{
			PA_SetSpriteHflip(sm->screen, spriteID, flippedh);
			PA_SetSpriteVflip(sm->screen, spriteID, flippedv);
			UPDATEDflip = false;
		}
		
		if (UPDATEDlayer)
		{
			PA_SetSpritePrio(sm->screen, spriteID, layer);
			UPDATEDlayer = false;
		}
		
		if (bRotating && rotSetID != -1)
			PA_SetRotsetNoZoom(sm->screen, rotSetID, rotAngle);
	}
	if (framesToLive > -1)
		framesToLive--;
}

/**
 * Check for collisions with other spriteObject%s. This is designed to be of O(2n) complexity,
 * so that an iteration of collision checks only requires 2 full loops through the sprite array.
 * Collision checks are of course bidirectional, so each check need only check from its position
 * in the spriteManager%'s OAM array onwards.
 * 
 * @see spriteObject::isColliding
 * 
 * @param	indexToCheckFrom	index in the spriteManager%'s OAM array to begin checking from.
 */
void spriteObject::checkCollisions(u8 indexToCheckFrom)
{
	if (!checkCollision)
		return;
	for (vector<spriteObject*>::iterator it = sm->gameSprites.begin()+indexToCheckFrom; it != sm->gameSprites.end(); ++it)
	{
		bool collides = isColliding(*it);
		
		if (collides)
			collidingWith(*it);
		else if (!collides && ignoreUntilUntouched == *it)
		{
			ignoreUntilUntouched = NULL;
		}
	}
}

/**
 * Set the transparency level of this spriteObject. Note that due to DS hardware limitations, there
 * is really only one transparency level at a time. Thus running this function will overwrite the
 * global transparency level as well as set this sprite to use it.
 * 
 * @see spriteManager::setAlpha()
 * @param translevel	the transparency level to set this sprite to. Levels range from 0 (invisible) to 15 (opaque)
 */
void spriteObject::setTransparency(u8 transLevel)
{
	transLevel %= 15;	//make sure it's in the proper range
	PA_SetSpriteMode(sm->screen, spriteID, 1);		//turn alphablending on for this one
	sm->setAlpha(transLevel);
}

/**
 * Flip this spriteObject to face the opposite way and reverse its x velocity. Will also shift it backwards 
 * an unnoticeable amount so that if its flip is the result of a collision, it won't loop.
 */
void spriteObject::turnAround()
{
	//shift slightly so that we move away from whatever made us turn around and stop it flipping again
	vx > 0 ? --x : ++x;
	
	vx *= -1;
	setFlipped(!flippedh, flippedv);
}

/**
 * Check whether this spriteObject is facing another one.
 * 
 * @param other	the spriteObject to check against
 * @return true if facing other, false otherwise.
 */
bool spriteObject::isFacing(const spriteObject *other) const
{
	return ((getx() > other->getx() && flippedh) || 
			(getx() < other->getx() && !flippedh));
}
/**
 * Check whether this spriteObject is overlapping another spriteObject on the y axis.
 * When combined with spriteObject::inVertPlaneOf(), this can be used to check for box collision.
 * 
 * @see spriteObject::isColliding
 * 
 * @param other	the spriteObject to check against
 * @return true if overlapping other on the y axis, false if not.
 */
bool spriteObject::inHorizPlaneOf(const spriteObject *other) const
{
	if (isOver(other)) return false;
    if (isUnder(other)) return false;
    return true;
}
/**
 * Check whether this spriteObject is overlapping another spriteObject on the x axis.
 * When combined with spriteObject::inHorizPlaneOf(), this can be used to check for box collision.
 * 
 * @see spriteObject::isColliding
 * 
 * @param other	the spriteObject to check against
 * @return true if overlapping other on the x axis, false otherwise.
 */
bool spriteObject::inVertPlaneOf(const spriteObject *other) const
{
	if (getRight() < other->getLeft()) return false;
    if (getLeft() > other->getRight()) return false;
    return true;
}
/**
 * Check whether this spriteObject%'s top is under the bottom of another spriteObject.
 * 
 * @param other	the spriteObject to check against
 * @return true if underneath other, false if not.
 */
bool spriteObject::isUnder(const spriteObject *other) const
{
    return (getTop() > other->getBottom());
}
/**
 * Check whether this spriteObject%'s base is above the top of another spriteObject.
 * 
 * @param other	the spriteObject to check against
 * @return true if over the top of other, false otherwise.
 */
bool spriteObject::isOver(const spriteObject *other) const
{
    return (getBottom() < other->getTop());
}
/**
 * Check whether this spriteObject is standing on another. Note that the other spriteObject
 * must have baseable collision set for this to ever occur.
 * 
 * @see spriteObject::isBaseable
 * 
 * @param other	the spriteObject to check against
 * @return true if standing on other, false otherwise.
 */
bool spriteObject::isStandingOn(const spriteObject *other) const
{
	if (other == this)
		return false;
	//can only be based if falling downwards or stationary
	if (other->isBaseable() && inVertPlaneOf(other) && getBottom() == other->getTop() && vy >= 0)
		return isColliding(other, true);
	return false;
}

/**
 * Check whether or not another spriteObject could possibly stand on this one.
 * 
 * @see spriteObject::isStandingOn
 * 
 * @return true if this spriteObject can be used as a base
 */
bool spriteObject::isBaseable() const
{
	return (collision & COL_BASEABLE);
}

/**
 * Collision check with another spriteObject. The check is run twice for each comparison - 
 * each spriteObject is only concerned with its own collision setting. If both are colliding,
 * then they are successfully touching.
 * 
 * @see spriteObject::checkCollision
 * 
 * @param other		the spriteObject to compare with
 * @param checkReverse	whether or not to compute collision for the complementary spriteObject
 * 
 * @return true if this spriteObject is colliding with other, false otherwise.
 */
bool spriteObject::isColliding(const spriteObject *other, bool checkReverse) const
{	
	if (other == this || other == ignoreUntilUntouched)
		return false;

	if (collision == COL_NONE)												//check for no collision
		return false;
		
	if (collision <= COL_DOWNBASEABLE && other->getBottom() != getTop())	//check for down-only collision
		return false;

	if (collision == COL_CENTERPOINT &&										//check for centerpoint collision
		 (getx() < other->getRight() && getx() > other->getLeft() && 
			gety() < other->getBottom() && gety() > other->getTop()))
		return (!checkReverse || other->isColliding(this, false));
	
	if (!inHorizPlaneOf(other)) return false;								//check for solid collision
	if (!inVertPlaneOf(other)) return false;

	if (collision != COL_PIXELPERFECT)
		return (!checkReverse || other->isColliding(this, false));
	
	/**
	 * These more intense pixel-perfect calculations work fairly well, however they
	 * tend to slow the game down a lot and create animation glitches etc as a side-effect.
	 * Since there are no COL_PIXELPERFECT sprites, it shouldn't matter.
	 */
	
	s32 over_bottom, over_top, over_left, over_right, over_width, over_height;
	s32 startSearch1, startSearch2, startSearchOffset1, startSearchOffset2;
    
    //still not ruled out, so compute area of overlap
    if (getBottom() > other->getBottom())
    	over_bottom = other->getBottom();
    else
    	over_bottom = getBottom();
    if (getTop() < other->getTop())
    	over_top = other->getTop();
    else
    	over_top = getTop();
    if (getRight() > other->getRight())
    	over_right = other->getRight();
    else
    	over_right = getRight();
    if (getLeft() < other->getLeft())
    	over_left = other->getLeft();
    else
    	over_left = getLeft();

    over_width = over_right - over_left;
    over_height = over_bottom - over_top;
    
    // Now compute starting offsets into both objects' bitmaps:
    startSearchOffset1 = frame * centerx*2 * centery*2;
    startSearchOffset2 = other->frame * other->centerx*2 * other->centery*2;
	
	//retrieve GFX data from registers
	//TODO: will possibly run outside of bounds when searching. How to check max length?
	u16* myGFX = PA_SpriteAnimP[sm->screen][getGFXid()];
	u16* otherGFX = PA_SpriteAnimP[other->sm->screen][other->getGFXid()];
    
    //the insaneo logic here is to not forget that the sprite may be flipped (the data isnt!)
    //and also that the bounds are smaller than the actual sprite (so borders must be added)
    //TODO: check that this actually works properly when sprites are flipped.
    startSearch1 = startSearchOffset1 + (over_top - (y>>8) * centerx*2) + (over_left - (x>>8));
    startSearch2 = startSearchOffset2 + (over_top - (other->y>>8) * other->centerx*2) + (over_left - (other->x>>8));	
    //still not ruled out, so scan through intersecting rectangle to find pixels that are the same.
    for (int i=0; i < over_height; ++i)
    {
        for (int j=0; j < over_width; ++j)
        {              
            if (myGFX[startSearch1] > 0 && otherGFX[startSearch2] > 0)
            	return (!checkReverse || other->isColliding(this, false));
            ++startSearch1;
            ++startSearch2;
        }
        startSearch1 += (centerx*2 - over_width);
        startSearch2 += (other->centerx*2 - over_width);
    }
    //searched through all pixels and no match. Bit of a waste of time, really.
    return false;
}

/**
 * Set this spriteObject to have its graphics flipped (or not) in both horizontal and vertical directions.
 * 
 * @param fh	whether or not to flip horizontally
 * @param fv	whether or not to flip vertically
 */
void spriteObject::setFlipped(bool fh, bool fv)
{
	if (bRotating && rotSetID != -1)	//cant be flipped if rotating at the same time
		return;
	
	flippedh = fh;
	flippedv = fv;
	if (oldFliph != flippedh)
	{
		swapHBounds();
		oldFliph = flippedh;
	}
	if (oldFlipv != flippedv)
	{
		swapVBounds();
		oldFlipv = flippedv;
	}
	UPDATEDflip = true;
}

#ifdef __WITHSOUND
/**
 * Tell this spriteObject to play a sound. The spriteManager is actually responsible for managing sound
 * channels, so this is more of a 'request' for a sound - if no channels are free, the sound is ignored.
 * 
 * @param sound		a pointer to the array of sound data
 * @param size		the length of the sound data array
 * @param repeat	whether or not to loop the sound. Looped sounds will stop when the spriteObject is destroyed.
 * @param vol		volume for the sound to play at
 */
void spriteObject::playSound(const u8 *sound, const u32 *size, bool repeat, u8 vol)
{
	stopSound();
	setSoundChannel(sm->playSound(sound, size, repeat, vol));
	#ifdef __MDDEBUG
	char buffer[255];
	sprintf(buffer, "%d: sound playing (channel %d, raw)", sm->screen, soundChannel);
	macros::debugMessage(className, buffer);
	#endif
}

/**
 * Tell this spriteObject to play a sound. The spriteManager is actually responsible for managing sound
 * channels, so this is more of a 'request' for a sound - if no channels are free, the sound is ignored.
 * 
 * @param sound		a pointer to a spriteManager soundData array, which includes the sound data and size value.
 * @param repeat	whether or not to loop the sound. Looped sounds will stop when the spriteObject is destroyed.
 * @param vol		volume for the sound to play at
 */
void spriteObject::playSound(spriteManager::soundData *sound, bool repeat, u8 vol)
{
	stopSound();
	setSoundChannel(sm->playSound(sound->data, sound->size, repeat, vol));
	#ifdef __MDDEBUG
	char buffer[255];
	sprintf(buffer, "%d: sound playing (channel %d, soundData)", sm->screen, soundChannel);
	macros::debugMessage(className, buffer);
	#endif
}
#endif

/**
 * Assign this spriteObject a parent sprite, which it will stick to as if they are one.
 * This is a fairly nasty bidirectional reference, since the parent sprite will also reference
 * the child in order to destroy it. TODO: make this better.
 * 
 * @see spriteObject::addChild
 * @see spriteObject::removeChild
 * 
 * @param	other	the spriteObject to parent to
 */
void spriteObject::setParent(spriteObject *other)
{
	parentSprite = other;
}

/**
 * Assign a child spriteObject to this sprite. Only used for cleanup and sprite deletion.
 * 
 * This was originally a vector of children so that a branching hierachy could be established,
 * but in the end I only ended up using '1 child <-> 1 parent' hierachies anyway. Thusly, the
 * more complex code has been commented out until this feature becomes useful.
 * 
 * @see spriteObject::setParent
 * @see spriteObject::removeChild
 * 
 * @param	other	the spriteObject to add as a child
 */
void spriteObject::addChild(spriteObject *other)
{
	if (other != NULL)
	{
		childSprite = other;
		//childSprites.push_back(other);
		other->setParent(this);
	}
	#ifdef __MDDEBUG
	else
		macros::debugMessage(ERRTEXT, "NULL childSprite added", ERRORPOS);
	#endif
}

/**
 * Remove reference to a child spriteObject.
 * 
 * @see spriteObject::addChild
 * @see spriteObject::setParent
 */
bool spriteObject::removeChild(/*spriteObject *other*/)
{
	/*u8 childID = 0;
	bool bFound = false;
	for (unsigned int i=0; i<childSprites.size(); ++i)
	{
		if (childSprites[i] == other)
		{
			childSprites[i]->parentSprite = NULL;
			childID = i;
			bFound = true;
			break;
		}
	}
	if (bFound)
		childSprites.erase(childSprites.begin()+childID, childSprites.begin()+childID+1);
	#ifdef __MDDEBUG
	else macros::debugMessage(ERRTEXT, "Removed invalid child sprite", ERRORPOS);
	#endif
	return bFound;*/
	childSprite = NULL;
	return true;
}
