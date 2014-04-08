#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

SDL_Surface *bmp;

const map[] = { 1,0,2 };

void get_pat(int x, int y) {
 unsigned char *pix;
 int sx,sy;
 int c1, c2;
 pix = (unsigned char *)bmp->pixels;
 for(sy=0;sy<8;sy++) 
  for(sx=0;sx<4;sx++) {
   c1 = pix[((sy+(y*8))*bmp->pitch) + (sx*2) + ( x*8)]+3;
   c2 = pix[((sy+(y*8))*bmp->pitch) + (sx*2) + ( x*8)+1]+3;
   if(c1 == 4) c1 = 0;
   if(c2 == 4) c2 = 0; 
   printf("0x%02X,\n", (c2 | (c1<<4)));
  }
}

void get_2x2(int x) {
 get_pat(0+(x*2),0);
 get_pat(0+(x*2),1);
 get_pat(1+(x*2),0);
 get_pat(1+(x*2),1);
}

void get_4x4(int x, int y) {
 get_pat(0+(x*4),0+(y*4));
 get_pat(0+(x*4),1+(y*4));
 get_pat(0+(x*4),2+(y*4));
 get_pat(0+(x*4),3+(y*4));

 get_pat(1+(x*4),0+(y*4));
 get_pat(1+(x*4),1+(y*4));
 get_pat(1+(x*4),2+(y*4));
 get_pat(1+(x*4),3+(y*4));

 get_pat(2+(x*4),0+(y*4));
 get_pat(2+(x*4),1+(y*4));
 get_pat(2+(x*4),2+(y*4));
 get_pat(2+(x*4),3+(y*4));

 get_pat(3+(x*4),0+(y*4));
 get_pat(3+(x*4),1+(y*4));
 get_pat(3+(x*4),2+(y*4));
 get_pat(3+(x*4),3+(y*4));

}

int main(int argc, char **argv) {
 int i;
 int r,g,b;
 if(argc>1) {
  if((bmp = SDL_LoadBMP(argv[1]))<=0) {
   printf("cannot read %s\n", argv[1]);
   exit(0);
  }
 } else {
  printf("usage\n\tfont2bin <file name>\n");
  exit(0);
 }

 /*
 printf("const color_t color[] = {\n"
       " {0,0,0},\n"
       " {0,0,0},\n"
       " {7,7,7},\n");
 for(i=0;i<13;i++) {
  r = bmp->format->palette->colors[i].r / (0xff/7);
  g = bmp->format->palette->colors[i].g / (0xff/7);
  b = bmp->format->palette->colors[i].b / (0xff/7);
  printf(" { %d,%d,%d },\n", r,g,b);
 }
 printf("};\n\n");
*/

/*
 for(i=0;i<20;i++) 
  get_pat(i,0);  

 for(i=0;i<20;i++) 
  get_pat(i,1);  
  */
/*
 for(i=0;i<37;i++)
  get_2x2(i);
  */

/* for(i=0;i<31;i++)
  get_pat(i,0);  */
printf("const u8 blowfish[] = {\n");
 get_4x4(0,0);
 get_4x4(1,0);
 get_4x4(0,1);
 get_4x4(1,1);
printf("};\n");
}
