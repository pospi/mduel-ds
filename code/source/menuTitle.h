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

#ifndef _MENUTITLE_H
#define _MENUTITLE_H

#include <PA9.h>
#include "spriteManager.h"
#include "macros.h"
#ifndef _ALLGRAPHICS_
#define _ALLGRAPHICS_
#include "../data/all_gfx_ed.c"
#endif

/**
 * The top screen for the game frontend, which is not interactive at all and really only
 * serves to look pretty. It displays a rotating background image, and a vibrating title
 * graphic that looks pretty cool.
 * 
 * @author pospi
 */
class menuTitle : public spriteManager
{
	public:
		menuTitle(u8 newScreen = 0);
		virtual ~menuTitle();
		
		void loadRotatingBackground();
		
		virtual void gameTick();
		
		/**
		 * When activated, load the background image and all necessary sprites.
		 */
		virtual void activate() {spriteManager::activate(); loadRotatingBackground();}
		virtual void deactivate() {spriteManager::deactivate(); unloadBackground();}
	
	private:
		
		///if true, attempt to rotate the background image, which must be a BGRotMap
		bool bRotateBackground;
		///angle that the background image is at currently
		int backgroundAngle;
};

#endif
