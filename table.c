
#include <stdio.h>
#include <stdlib.h>

int main(void) {
 int x, y;
 int patx, paty;
 int sx, sy;
 int dst;
 for(y=0;y<64;y++)
  for(x=0;x<64;x++) {
   patx = (x / 4);
   paty = y / 8;

   sx = x % 4;
   sy = y % 8;

   dst = ((paty * 16)  + patx) * 32;
   dst += (sy * 4) + sx;
   printf("0x%x,\n", dst);
  }
}
