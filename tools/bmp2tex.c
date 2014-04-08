#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

SDL_Surface *bmp;

int main(int argc, char **argv) {
 int i, r,g,b;
 int x,y;
 int d;
 unsigned char *pix;
 if(argc>1) {
  if((bmp = SDL_LoadBMP(argv[1]))<=0) {
   printf("cannot read %s\n", argv[1]);
   exit(0);
  }
 } else {
  printf("usage:\n\tbmp2tex <file name>\n");
  exit(0);
 } 
 
 printf("const color_t color[] = {\n"
       " {0,0,0},\n");
 for(i=0;i<15;i++) {
  r = bmp->format->palette->colors[i].r / (0xff/7);
  g = bmp->format->palette->colors[i].g / (0xff/7);
  b = bmp->format->palette->colors[i].b / (0xff/7);
  printf(" { %d,%d,%d },\n", r,g,b);
 }
 printf("};\n\n");

 printf("const u8 tex[] = {\n");
 pix = (unsigned char *)bmp->pixels;
 i=0;
 for(y=0;y<bmp->h;y++)
  for(x=0;x<bmp->w;x++) {
   d = pix[x+(y*bmp->pitch)]+1;
   if(d == 4)
    printf(" 0x00,\n");
   else
    printf(" 0x%02X,\n", (d&0xf) | (d<<4) );
  } 
 printf("};\n");
}
