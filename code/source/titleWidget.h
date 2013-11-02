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

#ifndef _TITLEWIDGET_H
#define _TITLEWIDGET_H

#include <PA9.h>
#include "macros.h"
#include "spriteObject.h"

/**
 * titleWidget is a very simple sprite behaviour that simply vibrates around in a small area around
 * titleWidget::BASEX and titleWidget::BASEY. I don't really remember why I thought this needed its own
 * class, I mean really it could have just as easily been done from menuTitle. Perhaps it was going to
 * have some additional functionality that I just plain forgot about.
 * 
 * @author pospi
 */
class titleWidget : public spriteObject
{
	public:
		titleWidget(spriteManager *newsm);
		virtual ~titleWidget();
		
		virtual void updateSprite();
	private:
		///the screen x position, in pixels, that this graphic should vibrate around near
		static const s16 BASEX = 95;
		///the screen y position, in pixels, that this graphic should vibrate around near
		static const s16 BASEY = 95;
};

#endif
