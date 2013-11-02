//changed code in PALib/include/nds/arm9/PA_Sprite.h
void PA_ResetSpriteSysScreen(u8 screen);	//<-- add above PA_ResetSpriteSys(void)

//====================================================================================================
//changed code in PALib/lib/PA9/PA_Sprite.c to allow independent sprite system resetting

//REPLACE THIS:
//=========================================================================
void PA_ResetSpriteSys(void) {
u8 n;
u8 i;

   n_free_mem[0] = 1;
   free_mem[0][0].mem_block = 0; free_mem[0][0].free = 1024;
   n_free_mem[1] = 1;
   free_mem[1][0].mem_block = 0; free_mem[1][0].free = 1024;
	
   DMA_Copy((void*)Blank, (void*)used_mem, 256, DMA_32NOW);
   DMA_Copy((void*)Blank, (void*)obj_per_gfx, 256, DMA_32NOW);

   // Tous les sprites sont déplacés hors écran, et tout est initialisé...
for (i = 0; i < 2; i++) {
	for(n = 0; n < 128; n++) {
		PA_obj[i][n].atr0 = 192;
		PA_obj[i][n].atr1 = 256;
		PA_obj[i][n].atr2 = 0;
		PA_obj[i][n].atr3 = 0;
	}
	for(n = 0; n < 32; n++) {
		PA_SetRotset(i,n,0,256,256);  // Pas de zoom ou de rotation par défaut
	}
}
FirstGfx[0] = 0;
FirstGfx[1] = 0;

nspriteanims = 0; // No animations...
for (i = 0; i < 2; i++) {
	for(n = 0; n < 128; n++) {
		spriteanims[i][n].play = 0;
	}
}

PA_MoveSpriteType = 0; 

PA_InitSpriteExtPrio(0);// normal priority system by default

}

//WITH THIS:
//=========================================================================
void PA_ResetSpriteSysScreen(u8 screen) {
	u8 n;
	u8 i;
	
	n_free_mem[screen] = 1;
	free_mem[screen][0].mem_block = 0; free_mem[screen][0].free = 1024;
	
	DMA_Copy((void*)Blank, (void*)used_mem[screen], 256, DMA_32NOW);
	DMA_Copy((void*)Blank, (void*)obj_per_gfx[screen], 256, DMA_32NOW);
	
	for(n = 0; n < 128; n++) {
	PA_obj[screen][n].atr0 = 192;
	PA_obj[screen][n].atr1 = 256;
	PA_obj[screen][n].atr2 = 0;
	PA_obj[screen][n].atr3 = 0;
	}
	for(n = 0; n < 32; n++) {
	PA_SetRotset(screen,n,0,256,256); // Pas de zoom ou de rotation par défaut
	}
	
	FirstGfx[screen] = 0;
	
	nspriteanims = 0; // No animations...
	for(n = 0; n < 128; n++) {
	nspriteanims -= spriteanims[screen][n].play; // remove sprites from sprite to animate list
	spriteanims[screen][n].play = 0;
	}
	
	
	if (screen == 0) PA_MoveSpriteType = 0;
}


void PA_ResetSpriteSys(void) {

PA_ResetSpriteSysScreen(0);
PA_ResetSpriteSysScreen(1);

nspriteanims = 0; // No animations...
PA_InitSpriteExtPrio(0);// normal priority system by default
}

//====================================================================================================