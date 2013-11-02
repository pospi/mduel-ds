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

#ifndef _SPRITEOBJECT_H
#define _SPRITEOBJECT_H

#include <PA9.h>
#include <vector>
#include <typeinfo>
#include <string>
	using namespace std;
#include "macros.h"
#include "spriteManager.h"

/**
 * The basic class for a self-managing sprite. 
 * 
 * Internally handles low-level logic such as collision,
 * graphics memory allocation & deallocation, sounds and display. spriteObject%s are linked to spriteManager 
 * instances which are responsibe for updating all sprites and deleting them when they are no longer needed.
 * 
 * Each spriteObject is assigned one of the 128 Object Address Memory (OAM) sprite slots that the DS
 * hardware is capable of. They are also assigned one of the 16 sprite palletes that fit into VRAM at once.
 * Whilst the spriteManager is responsible for creating and assigning palletes, the spriteObject is
 * responsible for its own OAM addressing - the spriteManager merely keeps track of which indexes are being
 * used.
 * 
 * When a spriteObject is assigned visible graphics (referred to as GFX), it is referencing an array in
 * VRAM which holds the sprite data. In this way, sprites which share the same GFX will be displayed and
 * animated as one, even though they may be positioned at different points on the screen. 'Dirty' sprites
 * which copy GFX for their own display are responsible for deleting that GFX when they are destroyed,
 * whilst sprites which are assigned GFX by the spriteManager must leave it for sharing sprites to use.
 * 
 * All position / velocity calculations are done internally in fixed point format to make things easier on
 * the ARM processor's lack of floating point calculation speed. Values are stored as 32 bit integers where
 * the last 8 bits denote 256 fractions of a pixel.
 * 
 * @author pospi
 */
class spriteObject
{
	public:
		spriteObject(spriteManager* newsm);
		virtual ~spriteObject();

		spriteObject(const spriteObject &s);
		
		/* 
			upon initialising, ensure you set the following:
				pallete, sprite, center, bounds, position, collision
			optionally set the following (order is important, do the above first!):
				layer, flipped, animation / frame, speed, sound channel
			
			movement vars (x, y, vx, vy, accel etc) are in fixed point format (256 = 1 pixel).
			all others (bounds, center etc) are in pixel values.
			All inputs to this class are pixel values, the conversion should be maintained internally.
		*/
		
		/**
		 * Set the pallete of this sprite by passing a pallete ID from a spriteManager
		 * @param	palleteID	ID of the pallete that the spriteManager has preloaded
		 */
		virtual void setPallete(u8 id)
		{
			palleteID = id;
			#ifdef __MDDEBUG
			char buffer[255];
			sprintf(buffer, "pallete set (#%d)", palleteID);
			macros::debugMessage(className, buffer);
			#endif
		}
		
		//only works for GFX created sprites becase we need a GFX id
		bool updatePallete(s8 newPalID, u8 sizex, u8 sizey, u8 colorMode = 1);
		
		s8 getPalleteID() const {return palleteID;}
		s8 getSpriteID() const {return spriteID;}
		u8 getFrame() const {return frame;}
		
		/**
		 * Set the collision bounds of this spriteObject to those provided.
		 * Values passed are the pixel distance from the center of the sprite to each of its edges.
		 * 
		 * @param	t	distance to top edge
		 * @param	r	distance to right edge
		 * @param	b	distance to bottom edge
		 * @param	l	distance to left edge
		 */
		void setBounds(s8 t, s8 r, s8 b, s8 l) {bounds.t = t; bounds.r = r; bounds.b = b; bounds.l = l;}
		/**
		 * Set this spriteObject's bounds by passing in an array of values.
		 * @see spriteObject::setBounds(s8,s8,s8,s8)
		 */
		void setBounds(s8 arr[4]) {bounds.t = arr[0]; bounds.r = arr[1]; bounds.b = arr[2]; bounds.l = arr[3];}
		void setCenter(u8 x, u8 y) {centerx = x; centery = y;}
		/**
		 * Set the layer of this sprite to one of the 4 sprite layers the DS provides. 
		 * Layers can be used to quickly prioritize groups of sprites.
		 * 
		 * @param newLayer	the new layer, from 0 to 3
		 */
		void setLayer(u8 newLayer) {UPDATEDlayer = true; layer = newLayer;}
		
		//'give' makes a new sprite from some data
		//@pre: palleteID has been set
		s8 giveSprite(const unsigned char* spriteData, u8 sizex, u8 sizey, u8 cx = 16, u8 cy = 16, u8 minIndex = 0, s16 nx = 0, s16 ny = 0, u8 colorMode = 1);
		s8 givePallete(const unsigned short* palleteData);
		s8 giveGFX(u16 GFXid, u8 sizex, u8 sizey, u8 cx = 16, u8 cy = 16, u8 minIndex = 0, s16 nx = 0, s16 ny = 0, u8 colorMode = 1);
		
		//hide an object's visuals without actually deleting it
		void deleteSprite();
		
		//work out whether this object has gfx associated with it
		bool hasGraphics() const;
		
		//basic animation
		virtual void setPos(s16 nx, s16 ny) {x = (nx-centerx)<<8; y = (ny-centery)<<8;}
		/**
		 * Sets the position of this spriteObject based on where its base will sit
		 * 
		 * @see spriteObject::setPos(s16, s16)
		 * @param ny	new y position for the spriteObject%'s base to sit at
		 */
		void setBasePos(s16 ny) {y = (ny-centery-bounds.b)<<8;}
		
		void setSpeed(float nvx, float nvy) {vx = (s32)(nvx*256); vy = (s32)(nvy*256);}
		void setSpeed(s32 nvx, s32 nvy) {vx = (nvx<<8); vy = (nvy<<8);}
		void setvx(s32 nvx) {vx = (nvx<<8);}
		void setvy(s32 nvy) {vy = (nvy<<8);}
		/**
		 * A speed setting function that can be used when necessary to set the spriteObject%'s
		 * speed directly in fixed point format.
		 * 
		 * @see spriteObject::setSpeed(float, float)
		 * @see spriteObject::setSpeed(s32, s32)
		 * @see spriteObject::setvx()
		 * @see spriteObject::setvy()
		 * @param	nvx	new x velocity
		 * @param	nvy	new y velocity
		 */
		void setRawSpeed(s32 nvx, s32 nvy) {vx = nvx; vy = nvy;}
		void setFlipped(bool fh, bool fv = false);
		
		void setTransparency(u8 transLevel = 15);	//0=0%, 15=100%
		
		virtual void turnAround();
		
		/**
		 * Set the animation frame of this spriteObject
		 * @param	fr	the frame index to set. Be careful not to set outside the sprite GFX's length!
		 */
		void setFrame(u8 fr)
		{
			frame = fr;
			arbitraryAnim = 0;
			PA_SetSpriteAnim(sm->screen, spriteID, frame);
		}
		/**
		 * Set an animation cycle for this spriteObject. All animations actually use
		 * spriteObject::setArbitraryAnim() because the normal sprite looping doesn't
		 * have as high an update priority and it tends to skip frames.
		 * 
		 * @see	spriteObject::setArbitraryAnim()
		 * @param	f1	frame to start at
		 * @param	f2	frame to end at
		 * @param	s	time between frames of the animation, in 100ths of a second
		 * @param	type	type of animation - ANIM_ONESHOT, ANIM_LOOP or ANIM_UPDOWN
		 * @param	times	LEGACY. NOT USED - number of times to play the animation
		 */
		void setAnim(u8 f1, u8 f2, u8 s, u8 type = ANIM_LOOP, u8 times = -1)
		{
			//regular PAlib animation isn't fast or responsive enough for me, so I'm using arbitraryAnims for everything
			vector<u8> frames;
			for (int i=0; i <= f2-f1; ++i)
				frames.push_back(f1+i);
			
			setArbitraryAnim(frames, (type == ANIM_LOOP), 60/s);	//TICKSPERFRAME = 60/ANIMSPEED
		}
		/**
		 * Set an animation cycle for this spriteObject which does not consist of a sequential
		 * list of frames.
		 * 
		 * @param	frames	a vector of unsigned chars that correspond to animation frames to play
		 * @param	loop	whether or not to loop the animation
		 * @param	ticksPerFrame	animation speed counter. The game runs at 60fps so the standard spriteObject::TICKSPERFRAME results in a 10fps animation speed.
		 */
		void setArbitraryAnim(vector<u8> frames, bool loop, u8 ticksPerFrame = TICKSPERFRAME)
		{
			arbitraryAnim = 1+loop;
			arbitrarySpeed = ticksPerFrame;
			arbitraryTimeCounter = 0;
			arbitraryFrameCounter = 0;
			arbFrames = frames;
			PA_SetSpriteAnim(sm->screen, spriteID, frames[0]);	//makes the first frame update happen straight away
		}
		/**
		 * Check if this spriteObject is running an animation
		 * @return true if animating, false otherwise
		 */
		bool isInAnim() const {return (arbitraryAnim != 0);}
		
		//totally bitching collision detection
		virtual bool isColliding(const spriteObject* other, bool checkReverse = true) const;
		bool inHorizPlaneOf(const spriteObject* other) const;
		bool inVertPlaneOf(const spriteObject* other) const;
		bool isUnder(const spriteObject* other) const;
		bool isOver(const spriteObject* other) const;
		bool isStandingOn(const spriteObject* other) const;
		bool isFacing(const spriteObject* other) const;
		
		/**
		 * The collision type of a spriteObject. No collision will skip all collision calculations,
		 * making it extremely fast if used correctly.
		 * 
		 * - If a collision has the first bit set, it is baseable - meaning that it can be stood upon.
		 * - Collisions from COL_DOWNBASEABLE (3) and lower (but not COL_NONE) only have collision on the
		 * top, much like the ground in a traditional 2d platformer that can be jumped through while moving
		 * upwards. 
		 * - The solid collision modes provide box collision.
		 * - COL_CENTERPOINT will only collide if the center of the spriteObject is touching another sprite.
		 * - COL_PIXELPERFECT is precise, pixel-accurate collision. It seems to work 'fairly well', but
		 * the processing overhead makes it a bit too much for the DS, so I don't use it.
		 */
		enum collisionInfo
		{
			COL_NONE = 0,
			COL_BASEABLE = 1,		//COL_BASEABLE
			COL_DOWNONLY = 2,
			COL_DOWNBASEABLE = 3,	//COL_DOWNONLY & COL_BASEABLE
			COL_SOLID = 4,
			COL_SOLIDBASEABLE = 5,	//COL_SOLID & COL_BASEABLE
			COL_CENTERPOINT = 8,		//collides if middle of sprite is within other sprite's bounds
			COL_PIXELPERFECT = 16		//exhaustive pixel-precise collision, only for really important stuff that doesnt animate too much (pickups)
		};
		void setCollision(collisionInfo c) {collision = c;}
		/**
		 * Set whether or not for this spriteObject to check collision AT ALL. Basically a very nice
		 * speed optimisation.
		 * @param	ch	whether or not to run collision checks whatsoever
		 */
		void setCheckCollision(bool ch) {checkCollision = ch;}
		bool isBaseable() const;
		
		/**
		 * Find the spriteObject (if any) that this one is currently ignoring collision with for the
		 * purposes of travelling through another spriteObject.
		 * 
		 * @return	a reference to the spriteObject this is ignoring or NULL if not ignoring any
		 */
		spriteObject* getIgnored() {return ignoreUntilUntouched;}
		/**
		 * Tell this spriteObject to ignore another spriteObject until the two are no longer colliding.
		 * 
		 * @param o	spriteObject to ignore
		 */
		void setIgnored(spriteObject* o) {ignoreUntilUntouched = o;}
		
		//update actual sprite object to which this object relates
		//@pre: palleteID and spriteID are set
		virtual void updateSprite();
		
		//check for collision. Check will start from sm->gameSprites[indexToCheckFrom],
		//since checks are bidirectional and must only be done once for each object pair.
		virtual void checkCollisions(u8 indexToCheckFrom = 0);
		
		/**
		 * Actions to take to mutate this spriteObject and another sprite object that is colliding with this one.
		 * spriteObject%s should not be moved in this function unless you know what you are doing, as this
		 * changes collision they may have with other spriteObject%s in the same tick.
		 * 
		 * @param	other	spriteObject to perform collision actions with
		 */
		virtual void collidingWith(spriteObject* other) {}
		
		/**
		 * Check if this spriteObject is colliding with a point on the screen.
		 * 
		 * @param	x	x pixel coordinate of the designated point
		 * @param	y	y pixel coordinate of the designated point
		 */
		bool pointCollision(s16 x, s16 y) {return (x < getRight() && x > getLeft() && y < getBottom() && y > getTop());}
		
		//getters
		s16 getx() const {return (x>>8)+centerx;}
		s16 gety() const {return (y>>8)+centery;}
		s32 getvx() const {return (vx>>8);}
		s32 getvy() const {return (vy>>8);}
		s32 getRawvx() const {return vx;}
		s32 getRawvy() const {return vy;}
		s16 getTop() const {return (y>>8)+centery+bounds.t;}
		s16 getRight() const {return (x>>8)+centerx+bounds.r;}
		s16 getBottom() const {return (y>>8)+centery+bounds.b;}
		s16 getLeft() const {return (x>>8)+centerx+bounds.l;}
		
		bool getFlippedh() const {return flippedh;}
		bool getFlippedv() const {return flippedv;}
		
		#ifdef __WITHSOUND
		//sound stuff
		void setSoundChannel(s8 channel) {soundChannel = channel;}
		void playSound(const u8* sound, const u32* size, bool repeat = false, u8 vol = 127);
		void playSound(spriteManager::soundData* sound, bool repeat = false, u8 vol = 127);
		/**
		 * Stop any LOOPING sounds this spriteObject has running. Sounds that are played only
		 * once need not be stopped as they will end soon enough and free up a sound channel.
		 */
		void stopSound()
		{
			if (hasSoundLooping()) sm->stopSound(soundChannel);
		}
		bool hasSoundLooping() {return sm->soundLooping(soundChannel);}
		#endif
		
		///width of the DS screen in fixed point
		static const u32 SCREENW = 65536;
		///height of the DS screen in fixed point
		static const u32 SCREENH = 49152;
		///10fps animation speed in ticks per frame (for arbitrary framerate dependent animation)
		static const u8 TICKSPERFRAME = 6;
		///10fps animation speed in fps (for regular timed animation)
		static const u8 ANIMSPEED = 10;
		
		//heriachies
		void setParent(spriteObject* other);	//only ever called by parent's addChild()
		void addChild(spriteObject* other);		//add another spriteObject to me
		bool removeChild(/*spriteObject* other*/);	//unparent child spriteObject
		
		/**
		 * Sets this spriteObject to be completely immobile and inanimate.
		 * Any more updates to this sprite from now on have no effect at all.
		 * Basically, after this is run, spriteObject::updateSprite() won't do anything.
		 */
		virtual void makeStatic() {updateSprite(); staticSprite = true;}
		
		/**
		 * Get this spriteObject%'s attachment sprite, if present.
		 * 
		 * @see spriteObject::setAttachment()
		 * @return a reference to the attachment sprite, or NULL if none is attached
		 */
		spriteObject* getAttachment() const {return attachment;}
		/**
		 * Allows for another spriteObject to mimic or conform to the animation of this one. 
		 * Attachment sprites are sprites that inherit the animation of the parent one. 
		 * An attachment sprite should usually be a child sprite, but could work with sprites 
		 * that are unparented as well.
		 * 
		 * @param	o	the sprite to set as an attachment
		 */
		void setAttachment(spriteObject* o) {attachment = o; o->isAttachment = true;}
		
		/**
		 * Another limitation of DS hardware - rotsets. The console is limited to 16 of these, which are
		 * then assigned sprites which rotate to whichever value is set on that rotset. After a rotset is
		 * given to a sprite, it must have its rotation enabled before rotation can be applied.
		 * 
		 * @see spriteObject::enableRotation()
		 * @see spriteObject::disableRotation()
		 * @see spriteObject::setRotation()
		 */
		void setRotSet(u8 rs) {rotSetID = rs;}
		/**
		 * @see spriteObject::setRotSet()
		 */
		bool enableRotation() {if (rotSetID != -1) {PA_SetSpriteRotEnable(sm->screen, spriteID, rotSetID); bRotating=true;} return (rotSetID != -1);}
		/**
		 * @see spriteObject::setRotSet()
		 */
		void disableRotation() {bRotating=false; PA_SetSpriteRotDisable(sm->screen, spriteID);}
		/**
		 * @see spriteObject::setRotSet()
		 */
		bool setRotation(u16 newRot) {if (rotSetID == -1 || !bRotating) return false; rotAngle = newRot; rotAngle &= 511; return true;}
		
		///if true, spriteManager should delete this sprite's GFX when cleaning it up
		bool bNeedsGFXCleanup;
		/**
		 * Find out the id of the GFX assigned to this spriteObject, so that it can be freed up later.
		 * 
		 * @return the id of this sprite's GFX
		 */
		s32 getGFXid() const {return gfxID;}
		
		/**
		 * Set the lifetime of this spriteObject. Lifetime is decremented every tick, and when it expires
		 * the spriteObject will be destroyed.
		 * 
		 * @see spriteObject::getLifetime()
		 * @see spriteObject::disableLifetime()
		 * @param	numSecs	the number of seconds for this spriteObject to live for
		 */
		void setLifetime(u16 numSecs) {framesToLive = numSecs*ANIMSPEED*TICKSPERFRAME;}
		s16 getLifetime() {return framesToLive;}
		void disableLifetime() {framesToLive = -1;}
		
		/**
		 * Delayed deletion - spriteManager deletes this spriteObject on next of tick if flagged for cleanup.
		 * 
		 * @return whether or not this spriteObject should be deleted in the next tick
		 */
		bool markedForDeletion() {return framesToLive == 0;}
		
		/**
		 * Flag this spriteObject for deletion on the next tick.
		 */
		void destroy()
		{
			framesToLive = 0;
			#ifdef __MDDEBUG
			char buffer[255];
			sprintf(buffer, "flagged for deletion (gfx #%d, sprite #%d, pallete #%d)", gfxID, spriteID, palleteID);
			macros::debugMessage(className, buffer);
			#endif
		}
	protected:
		void swapHBounds() {u8 btemp = bounds.r; bounds.r = bounds.l*-1; bounds.l = btemp*-1;}
		void swapVBounds() {u8 btemp = bounds.t; bounds.t = bounds.b*-1; bounds.b = btemp*-1;}
			//this could fuck stuff up if I happen to make a btemp variable near this function call :S
		
		///an attachment is another sprite which will be animated in the exact same way as this one
		spriteObject* attachment;
		bool isAttachment;			//TODO! if unattached, this should be set back to false..
		
		///if true, sprite only needs to be updated when first created
		bool staticSprite;
		
		///global sprite manager for this screen
		spriteManager* sm;
		
		//very simple sprite hierachy
		spriteObject* parentSprite;
		//vector<spriteObject*> childSprites;
		spriteObject* childSprite;
		
		///OAM id of this spriteObject%'s sprite
		s8 spriteID;
		///id of this spriteObject%'s pallete
		s8 palleteID;
		///id of this spriteObject%'s GFX
		s32 gfxID;

		#ifdef __WITHSOUND
		//channel to play all my sounds in (1 at a time per sprite!)
		s8 soundChannel;
		#endif
		
		//position and velocity variables, in fixed point format
		s32 x, y, vx, vy;
		s32 oldx, oldy;
		bool flippedh, flippedv, oldFliph, oldFlipv;
		u8 layer;
		u8 frame, oldFrame;
		
		///0 = none, 1 = play, 2 = loop
		u8 arbitraryAnim;
		///animation speed in ticks per frame
		u8 arbitrarySpeed;
		u8 arbitraryFrameCounter;
		u8 arbitraryTimeCounter;
		///animation frame IDs to play sequentially
		vector<u8> arbFrames;
		
		///bounding box, in pixel offsets from sprite registration point
		struct boundingbox
		{
			s8 t, r, b, l;
		};
		boundingbox bounds;
		u8 centerx, centery;	//offsets to sprite center (half width)
		
		collisionInfo collision;
		bool checkCollision;		//only active objects (players, pickups etc) should check collision
		///this sprite's collision is ignored until it stops colliding
		spriteObject* ignoreUntilUntouched;
		
		//rotation variables
		s8 rotSetID;
		u16 rotAngle;
		bool bRotating;
		
		#ifdef __MDDEBUG
		string className;
		#endif
		
		//if any of the following are set, the relevant PAlib() updating functions are called
		bool UPDATEDlayer;
		bool UPDATEDflip;
		
		//if this reaches 0, destroy self
		s16 framesToLive;
};

#endif
