//Gfx converted using Mollusk's PAGfx Converter

//This file contains all the .c, for easier inclusion in a project

/**
 * pospi says:
 *
 * Yeah. If you run PAGfx.exe to reconvert all these sprites, you'll lose all the #ifdef's
 * I had to put in them which will unfortunately make it not compile at all. That's why there's
 * an 'EditedBackup' folder with all the .c files in it, so that if you just want to change one
 * or two sprites you can copy everything back over and not have to worry.
 *
 * In addition, all the character sprites have a bunch of colours in the top of the first frame
 * that I needed to get all their palletes in the same order. Again, they must be edited to set all
 * those pallete indexes to 0.
 */

#ifndef __ALLGFXFILE
#define __ALLGFXFILE

#ifdef __cplusplus
extern "C" {
#endif


// Sprite files : 
#include "playerSprite1.c"
#include "playerSprite2.c"
#include "playerSprite3.c"
#include "envSprites.c"
#include "effectSprites.c"
#include "pickupSprites.c"
#include "dunceHatSprite.c"
#include "menuLogo.c"
#include "menuLogo2.c"
#include "bootsSprite.c"
#include "menuCursors.c"
#include "menuSprites.c"
#include "spawnerSpriteX.c"

// Background files : 
#include "menuBG.c"
#include "menuBGbottom.c"
#include "controlsBG.c"
#include "Font.c"
#include "spawnerBG.c"

// Palette files : 
#include "playerSprite1.pal.c"
#include "playerSprite2.pal.c"
#include "playerSprite3.pal.c"
#include "playerSprite4.pal.c"
#include "player1000V.pal.c"
#include "envSprites.pal.c"
#include "effectSprites.pal.c"
#include "pickupSprites.pal.c"
#include "dunceHatSprite.pal.c"
#include "menuLogo.pal.c"
#include "menuLogo2.pal.c"
#include "bootsSprite.pal.c"
#include "menuCursors.pal.c"
#include "menuSprites.pal.c"
#include "spawnerSpriteX.pal.c"
#include "menuBG.pal.c"
#include "menuBGbottom.pal.c"
#include "controlsBG.pal.c"
#include "Font.pal.c"
#include "spawnerBG.pal.c"


#ifdef __cplusplus
}
#endif

#endif
