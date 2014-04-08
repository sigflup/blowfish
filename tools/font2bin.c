#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

SDL_Surface *bmp;

const map[] = { 1,2,0xf };

void get_pat(int x, int y) {
 unsigned char *pix;
 int sx,sy;
 int c1, c2;
 pix = (unsigned char *)bmp->pixels;
 for(sy=0;sy<8;sy++) 
  for(sx=0;sx<4;sx++) {
   c1 = pix[((sy+(y*8))*bmp->pitch) + (sx*2) + ( x*8)];
   c2 = pix[((sy+(y*8))*bmp->pitch) + (sx*2) + ( x*8)+1];

   printf("0x%02X,\n", (map[c2] | (map[c1]<<4)));
  }
}

void get_2x2(int x) {
 get_pat(0+(x*2),0);
 get_pat(0+(x*2),1);
 get_pat(1+(x*2),0);
 get_pat(1+(x*2),1);
}

int main(int argc, char **argv) {
 int i;
 if(argc>1) {
  if((bmp = SDL_LoadBMP(argv[1]))<=0) {
   printf("cannot read %s\n", argv[1]);
   exit(0);
  }
 } else {
  printf("usage\n\tfont2bin <file name>\n");
  exit(0);
 }

 get_pat(0,0);

/*
 for(i=0;i<20;i++) 
  get_pat(i,0);  

 for(i=0;i<20;i++) 
  get_pat(i,1);  
  */

// for(i=0;i<37;i++)
//  get_2x2(0);

/* for(i=0;i<31;i++)
  get_pat(i,0);  */
}
