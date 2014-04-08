#include <libmega.h>
#include "pal.h"

color_t *current_pal;

void push_pal(int count, int brightness) {
 int i;
 int nr,ng,nb;

 for(i=0;i<count;i++) {
  nr = current_pal[i].r + brightness;
  CLAMP(nr,0,7);
  ng = current_pal[i].g + brightness;
  CLAMP(ng,0,7);
  nb = current_pal[i].b + brightness;
  CLAMP(nb,0,7);

  VDP_SET_PAL2(i,nr,ng,nb);
 } 
}


const color_t rotate_colors[48]={
 {0, 7, 0},
 {1, 7, 0},
 {2, 7, 0},
 {3, 7, 0},
 {4, 7, 0},
 {5, 7, 0},
 {6, 7, 0},
 {7, 7, 0},
 {7, 7, 0},
 {7, 6, 0},
 {7, 5, 0},
 {7, 4, 0},
 {7, 3, 0},
 {7, 2, 0},
 {7, 1, 0},
 {7, 0, 0},
 {7, 0, 0},
 {7, 0, 1},
 {7, 0, 2},
 {7, 0, 3},
 {7, 0, 4},
 {7, 0, 5},
 {7, 0, 6},
 {7, 0, 7},
 {7, 0, 7},
 {6, 0, 7},
 {5, 0, 7},
 {4, 0, 7},
 {3, 0, 7},
 {2, 0, 7},
 {1, 0, 7},
 {0, 0, 7},
 {0, 0, 7},
 {0, 1, 7},
 {0, 2, 7},
 {0, 3, 7},
 {0, 4, 7},
 {0, 5, 7},
 {0, 6, 7},
 {0, 7, 7},
 {0, 7, 7},
 {0, 7, 6},
 {0, 7, 5},
 {0, 7, 4},
 {0, 7, 3},
 {0, 7, 2},
 {0, 7, 1},
 {0, 7, 0},
};

