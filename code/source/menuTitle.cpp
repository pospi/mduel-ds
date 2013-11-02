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

#include "menuTitle.h"
#include "titleWidget.h"

/**
 * Initialise the top menu spriteManager and assign it a screen.
 * 
 * @param	newScreen	the screen this spriteManager should handle. 1 is the top screen, 0 is the bottom.
 */
menuTitle::menuTitle(u8 newScreen) : spriteManager(newScreen), bRotateBackground(false), backgroundAngle(0)
{
	
}
	
menuTitle::~menuTitle()
{
	
}

/**
 * Load the graphics for the frontend splash screen. Loads a rotating background image, and
 * vibrating title graphic.
 */
void menuTitle::loadRotatingBackground()
{
	PA_SetVideoMode(screen, 1);
	PA_Load8bitBgPal(screen, (void*)::menuBG_Pal);
	PA_LoadRotBg(screen, 3, ::menuBG_Tiles, ::menuBG_Map, BG_ROT_256X256, 0);
	bRotateBackground = true;
	//PA_Load8bitBgPal(screen, (void*)backGroundSprite.bg_Pal);
	//PA_LoadRotBg(screen, 3, backGroundSprite.bg_Tiles, backGroundSprite.bg_Map, BG_ROT_256X256, 0);
	
	titleWidget* t = new titleWidget(this);
	t->givePallete(::menuLogo_Pal);
	t->giveSprite(::menuLogo_Sprite, OBJ_SIZE_64X32, 32, 16, 0, 95, 95, 1);
	spriteObject* s = new spriteObject(this);
	s->givePallete(::menuLogo2_Pal);
	s->giveSprite(::menuLogo2_Sprite, OBJ_SIZE_64X32, 32, 16, 0, 64, 0, 1);
	t->addChild(s);
}

/**
 * Update the rotation of the background image and the title graphics.
 */
void menuTitle::gameTick()
{
	spriteManager::gameTick();
		
	if (bRotateBackground)
	{
		++backgroundAngle;
		PA_SetBgRot(screen, 3, 127, 95, 127, 95, backgroundAngle, 256);
	}
}
