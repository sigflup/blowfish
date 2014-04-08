#include <libmega.h>

#include "stdlib.h"

#include "songs.h"

#include "vgm_drv.h"

#include "sprites.h"

#include "pal.h"

#include "../mega/q_pal.h"
#include "../mega/q_pat.h"
#include "../mega/q_scroll.h"
#include "../mega/q_scrollb.h"

#include "font.h"
#include "font2.h"
#include "font3.h"
#include "blowfish.h"
#include "one.h"
#include "bubble.h"
#include "small_bubble.h"
#include "small_one.h"

// #include "prog.h"

#define FISH_SPEED	fish_speed	
#define PSG_PORT	0xc00011
#define SPRITE_BASE	0xa000

#define OPCODE		0x1a00
/* Track 1 Tiido Beat
 * Track 2 Beyond The Stars
 * Track 3 Sea Breeze
 * Track 4 Happy */

u16 hscroll_buf[224*2];

int z80_mutex;

int pause;

int pal;

int lwvgm_stop;

int pending_draw;
int draw_to;

int vis_pending;
int vis_draw_to;

int b_track_change;

int fade_to, brightness;

#define NUM_TRACKS	5
typedef struct {
 char *name;
 void *track;
} track_t;

const track_t tracks[NUM_TRACKS] = {
 { "beyond the stars", track2_vgm },
 { "funk chunk",       track3_vgm },
 { "blip hop",         track4_vgm },
 { "sunset",           track5_vgm },
 { "tiido beat",       track1_vgm }
};

int current_track;

#define NUM_MESSAGES	6
const char messages[NUM_MESSAGES][25]= {
 "enjoy the music",
 "code sigflup",
 "snd code kulibus1 sigflup",
 "gfx zanagb sigflup",
 "music jredd",
 "promotion bonefish linde"
};

const char *scroll_msg = 
 "                 h e l l o   d a t a s t o r m   w e   a r e   b a c k     "
 "p l a n e t   e a r t h   c o m i n g   a t   y o u   w i t h   a   b r a n d   n e w   p r o d u c t i o n      "
 "o u r  f r i e n d   j r e d d   i s   b a n g i n g   o n   t h e   f m   c h i p   c r e a t i n g   a l l   s o r t s   o f   a w e s o m e   t r a c k s      "
 "w e   j u s t   h a d   t o   p r e s e n t   t o   y o u   s o m e   o f   h i s   w o r k        "
 "s p e c i a l   t h a n k s   g o e s   t o   b o n e f i s h   a n d   l i n d e  f o r   b e i n g   a w e s o m e   p e o p l e         "
 "w e   a r e   u s i n g   a   m o d i f i e d   s o u n d   c o r e   b y   k u b i l u s 1   w h i c h   i s   b a s e d   o n   m y   s o u n d   c o r e       f u n n y   h o w   t h i n g s   a r e            "
 "a n y w a y   t h e   a u d i o   y o u   h e a r   i s   e m b e d d e d   v g m   f i l e s         w h i c h   a r e   b i g       n e x t   t i m e   i   w r i t e   a   m u s i c d i s k   i   a m   g o i n g   w i t h    s o m e t h i n g   s m a l l e r         v g m   k i n d a   s u c k s   f o r   m u s i c d i s k s           i t   i s   g o o d   f o r   d e m o s   t h o u g h   b e c a u s e   y o u   o n l y  h a v e   o n e   t r a c k         "
"i   c h a l l e n g e   y o u   t o   e x t r a c t   a l l   t h e   v g m s   f r o m   t h i s   p r o d           h e r e   i s   a   h i n t             t h e y   s t a r t   w i t h   v g m   a n d   t h e y   a r e   c o n t i g u o u s             "
 "y o u   m i g h t   n o t i c e   a   s m a l l   j i t t e r   e s p e c i a l l y   w h e n   o n   t i i d o   b e a t   w h e n   o n   r e a l   h a r d w a r e         t h a t   i s   b e c a u s e   w h e n   t h e   s o u n d   p r o c e s s o r   a c c e s s e s   t h e   m e m o r y   o f   t h e   m a i n   p r o c e s s o r   t h e   m a i n   p r o c e s s o r   m i s s e s   c y c l e s            "
 "t h e   s o l u t i o n   t o   t h i s   i s   s m a l l e r   m u s i c   f i l e s       s i n c e   t h e y   a r e   o n   t h e   m a i n   p r o c e s s o r s   m e m o r y";

 int scroll_msg_len;

u8 ctrl, lctrl;

u8 wvgm_fm;
volatile u8 *wvgm_fm_addr;

u8 wvgm_psg;
volatile u8 *wvgm_psg_addr;

u8 wvgm_dac;
volatile u8 *wvgm_dac_addr;

u8 wvgm_stop;

int waggle, lwaggle, real_waggle;
unsigned int waggle_clk;

volatile u8 *wvgm_stop_addr;

int rad_clk;

int fish_speed;

const u8 fish_speed_const = 10;

volatile unsigned int *addr_z80;
volatile u8 *init_z80;
volatile u8 *play_z80;
volatile u8 *sfx_play;
volatile u16 *addr_sfx;
volatile u8 *bank_sfx;
volatile u16 *len_sfx;

typedef struct {
 int x, y;
} bubble_t;

bubble_t bubbles[3];

u8 fb[0x2000];
u8 *fb_p;
int fb_mutex;
int vs_x;
int vs_y;

int bank_clk;

int logo_x, logo_y;

int raster_clk;

int bank_fm, bank_psg, bank_dac;
int bubble_seed;

int line_count;

u16 sprite_ram_orig[80*8];

int sprite_mutex;

typedef struct {
 int x,y;
 int dx,dy;
} snake_head_t;

snake_head_t snake;

u16 rotate_colors_values[48];

u16 rnd(s32 *seed) {
 return ((*seed = *seed * 214013L + 2531011L)>>16)&0xffff;
}

void *memcpy(void *dst, void *src, int len) {
 int i;
 unsigned char *d,*s;
 d = dst;
 s = src;
 for(i=0;i<len;i++){
  d[i] = s[i];
 }
 return dst;
}

void h_bottom(void) {
 int d;
 d = line_count+(raster_clk>>2);
 if(brightness == 2) {
  if((line_count>0) && (line_count<20) ) {
   VDP_SET_REGISTER(BACKGROUND_COLOR, 48+((d)%16));
  } else {
   VDP_SET_REGISTER(BACKGROUND_COLOR,0);
  }
 }
}

void vgm_load_song(unsigned int in) {
 volatile u8 *pb;
 u16 i; 
 
 char *p = (char *)addr_z80;
 char *q = (char *)&in;

 YM2612_reset();

 pb = (u8 *)PSG_PORT;

 Z80_requestBus(1); 
 for(i=0;i<4;i++) {
  *pb = 0x80 | (i<<5) | 0x00;
  *pb = 0x00;
  *pb = 0x90 | (i<<5) | 0x0f;
 }
 Z80_releaseBus();


 Z80_requestBus(1); 
 p[0] = q[3];
 p[1] = q[2];
 p[2] = q[1];
 p[3] = q[0];
 Z80_releaseBus();

 addr_z80 = (unsigned int *)0xA00FFF;
 init_z80 = (u8 *)0xA01003;
 play_z80 = (u8 *)0xA01004;


 Z80_requestBus(1); 
 *init_z80 = 0x01;
 *play_z80 = 0x00;
 Z80_releaseBus();
}

void vgm_start(void) {
 Z80_requestBus(1);
 *play_z80 = 0x01;
 Z80_releaseBus();
}

void vgm_stop(void) {
 Z80_requestBus(1);
 *play_z80 = 0x00;
 Z80_releaseBus();
}

const u8 title[] = "blowfish";

int flip, lflip;

int show_palette;

void jredd_bottom(void) {
 int x,y,i,j,k;
 int d;
 int start;

 if(brightness!=fade_to) {
  push_pal(48,brightness);
 }

 if(sprite_mutex == 1) {
  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)&sprite_ram[(page^1)*80*4], SPRITE_BASE, 80*4);
  sprite_mutex = 0;
  page^=1;
 }


 if((bank_clk%2)==0)
  do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)hscroll_buf, 0xfc00, 0x3ff);


 if(vis_pending == 1) {
 vgm_stop(); 
 vgm_load_song(tracks[vis_draw_to].track);
 vgm_start();
 Z80_requestBus(1);
 *wvgm_stop_addr = 0;
 wvgm_stop =0;
 Z80_releaseBus();


  write_line(tracks[vis_draw_to].name);
  write_line(tracks[vis_draw_to].name);
  VRAM_WRITE( 0xc000 + ((32+(25*64))<<1), 0x8000| (0x36f+(vis_draw_to*2)));
  VRAM_WRITE( 0xc000 + ((33+(25*64))<<1), 0x8000| (0x36f+(vis_draw_to*2)+1));

  VRAM_WRITE( 0xc000 + ((32+(26*64))<<1), 0x8000| (0x383+(vis_draw_to*2)));
  VRAM_WRITE( 0xc000 + ((33+(26*64))<<1), 0x8000| (0x383+(vis_draw_to*2)+1));

  VRAM_WRITE( 0xc000 + ((32+(25*64))<<1), 0x8000| (0x36f+(vis_draw_to*2)));
  VRAM_WRITE( 0xc000 + ((33+(25*64))<<1), 0x8000| (0x36f+(vis_draw_to*2)+1));

  VRAM_WRITE( 0xc000 + ((32+(26*64))<<1), 0x8000| (0x383+(vis_draw_to*2)));
  VRAM_WRITE( 0xc000 + ((33+(26*64))<<1), 0x8000| (0x383+(vis_draw_to*2)+1));

  vis_pending = 0;
 }

 do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)hscroll_buf, 0xfc00, 0x1c0);

 CRAM_WRITE( (48+(((raster_clk>>2)+16)%16))<<1, 
            rotate_colors_values[(raster_clk>>2)%48]); 

 if((bank_clk%2)==0) {
  draw_banks(); 

  lflip = flip;
  flip = (raster_clk>>7)%NUM_MESSAGES;
  if(lflip!=flip)
   write_line2(messages[(raster_clk>>7)%NUM_MESSAGES]);

  }
  Z80_requestBus(1);
  lwvgm_stop = wvgm_stop;
  wvgm_stop= *wvgm_stop_addr;
  wvgm_fm =  *wvgm_fm_addr;
  if(wvgm_fm > 0) {
   if((wvgm_stop==0) || (current_track == (NUM_TRACKS-1)))
    bank_fm = 8;
   *wvgm_fm_addr = 0;
  }
  wvgm_psg = *wvgm_psg_addr;
  if(wvgm_psg > 0) {
   if((wvgm_stop == 0) || (current_track == (NUM_TRACKS-1)))
    bank_psg = 8;
   *wvgm_psg_addr = 0;
  }
  Z80_releaseBus();
 
 bank_clk++;
 vs_x-=(pal==1 ? 3 : 2);
 vs_y+=(pal==1 ? 6 : 5);
 
}

void write_line(char *in) {
 int i;
 int c;
 int buf[19*2];
 for(i=0;i<(19*2);i++) 
  buf[i] = 0xe4;
 for(i=0;i<(19*2);i++) {
  if(in[i] == 0) break;
  if((in[i] >= 'a') && (in[i]<='z')) {
   if(in[i] <= 'n') 
    c = in[i];
   else
    c = in[i]+1;

   buf[i] = 0x350+(c - 'a');
  }
 }

 for(i=0;i<19;i++) {
  VRAM_WRITE( 0xc000 + ((i+17+(21*64))<<1),  0x8000|buf[i]);
 }
 for(i=19;i<(19*2);i++) {
  VRAM_WRITE( 0xc000 + (((i-19)+17+(23*64))<<1), 0x8000| buf[i]);
 } 
}

void write_line2(char *in) {
 int i;
 int c;
 int buf[25];
 for(i=0;i<(25);i++) 
  buf[i] = 0xe4;
 for(i=0;i<(25);i++) {
  if(in[i] == 0) break;
  if((in[i] >= 'a') && (in[i]<='z')) {
   if(in[i] <= 'n') 
    c = in[i];
   else
    c = in[i]+1;
   buf[i] = 0x350+(c - 'a');
  } else {
   if(in[i] == '1')
    buf[i] = 0x89;
  }
 }

 for(i=0;i<25;i++) {
  VRAM_WRITE( 0xc000 + ((i+3+(25*64))<<1),  0x8000|buf[i]);
 }
}


int change_track(int in) {


/* 
 vgm_stop(); 
 vgm_load_song(tracks[in].track);
 vgm_start();
 Z80_requestBus(1);
 *wvgm_stop_addr = 0;
 wvgm_stop =0;
 Z80_releaseBus();
*/
 vis_pending = 1;
 vis_draw_to =in;
}

void draw_banks(void) {
 int i;
 for(i=0;i<8;i++) {
  if(i>=(bank_fm)) {
   VRAM_WRITE( 0xc000 + ((i+7+(21*64))<<1),0x8000| 0xdf);
  } else {
   VRAM_WRITE( 0xc000 + ((i+7+(21*64))<<1),0x8000| 0xeb);
  }
 }

 for(i=0;i<8;i++) {
  if(i>=(bank_psg)) {
   VRAM_WRITE( 0xc000 + ((i+7+(23*64))<<1),0x8000| 0xdf);
  } else {
   VRAM_WRITE( 0xc000 + ((i+7+(23*64))<<1),0x8000| 0xeb);
  }
 }
}


void logic(void) {
 int i,j,k,start;
 int x,y, size;

 if((brightness == 1) && (b_track_change == 0)) {
  change_track(0);
  b_track_change = 1;
 }

 if((raster_clk%2)==0) {
/*  Z80_requestBus(1);
  lwvgm_stop = wvgm_stop;
  wvgm_stop= *wvgm_stop_addr;
  wvgm_fm =  *wvgm_fm_addr;
  if(wvgm_fm > 0) {
   if((wvgm_stop==0) || (current_track == (NUM_TRACKS-1)))
    bank_fm = 8;
   *wvgm_fm_addr = 0;
  }
  wvgm_psg = *wvgm_psg_addr;
  if(wvgm_psg > 0) {
   if((wvgm_stop == 0) || (current_track == (NUM_TRACKS-1)))
    bank_psg = 8;
   *wvgm_psg_addr = 0;
  }
  Z80_releaseBus(); */
 }

 if(((raster_clk>>2)&1) == 0) {
  bank_fm--;
  if(bank_fm<0)
   bank_fm = 0;

  bank_psg--;
  if(bank_psg<0)
   bank_psg = 0;

  bank_dac--;
  if(bank_dac<0)
   bank_dac = 0;
 }

 if(sprite_mutex == 0) { 
  sprite_index = 0;

  if(((raster_clk<<3)&0x100)==0x100) {
   for(i=0;i<8;i++) {
    size =0xff-(0x22/2)-((cos_tab[(raster_clk<<3)&0xff] * float2fix((float)0x22))>>16);
    put_sprite(
      160+(((((i-4)*16)<<8)*abs(size))>>16), 
      20,
     1,1, 0,0, 2,1,0x398+ (title[i] - 'a')*4);
   }
  } else {
   for(i=7;i>-1;i--) {
    size =0xff-(0x22/2)-((cos_tab[(raster_clk<<3)&0xff] * float2fix((float)0x22))>>16);
    put_sprite(
      160+(((((i-4)*16)<<8)*abs(size))>>16), 
      20,
     1,1, 0,0, 2,1,0x398+ (title[i] - 'a')*4);
   }

  }

  start = -(vs_x/16);
  if(start<0)
   start = 0;
  for(i=start;i<start+(320/16)+3;i++) {
   x = (i*16)+vs_x;
   y = 100 + (((cos_tab[(x+vs_y)&0xff]*float2fix(10.0f))>>8)>>8);
   if((x>-16) && (x<(320+32))) {
     if(i<scroll_msg_len)
      if(scroll_msg[i] != ' ') {
       if(scroll_msg[i] == '1') 
        put_sprite(x,y,1,1,0,0,2,0,0x290);
       else
        put_sprite(x,y,1,1,0,0,2,0,0x398 + ((scroll_msg[i] - 'a')*4));
      }
    }
  }

  if(vs_x<-(scroll_msg_len*16))
   vs_x=500;


  for(i=0;i<3;i++) 
   if(bubbles[i].y != -100) {

    if(bubbles[i].y<20) 
     put_sprite(
      160+(bubbles[i].x+(((cos_tab[(bubbles[i].y*10)&0xff]*float2fix(5.0f))>>8)>>8))+4,
      (80-bubbles[i].y)+4, 0, 0, 0, 0, 2, 0, 0x88);
    else
     put_sprite(
      160+(bubbles[i].x+(((cos_tab[(bubbles[i].y*10)&0xff]*float2fix(5.0f))>>8)>>8)),
      80-bubbles[i].y, 1, 1, 0, 0, 2, 0, 0x249);

    bubbles[i].y++;
    if(bubbles[i].y == 80)
     bubbles[i].y = -100;
   }


  if(snake.dx > 0) {
   put_sprite(160+(snake.x>>FISH_SPEED)-32,80+(snake.y>>FISH_SPEED)-32,3,3,0,0,2,0,real_waggle);
   put_sprite(160+(snake.x>>FISH_SPEED)   ,80+(snake.y>>FISH_SPEED)-32,3,3,0,0,2,0,real_waggle+0x10);
   put_sprite(160+(snake.x>>FISH_SPEED)-32,80+(snake.y>>FISH_SPEED)   ,3,3,0,0,2,0,real_waggle+0x20);
   put_sprite(160+(snake.x>>FISH_SPEED)   ,80+(snake.y>>FISH_SPEED)   ,3,3,0,0,2,0,real_waggle+0x30);
  } else {
   put_sprite(160+(snake.x>>FISH_SPEED)-32,80+(snake.y>>FISH_SPEED)-32,3,3,0,1,2,0,real_waggle+0x10);
   put_sprite(160+(snake.x>>FISH_SPEED)   ,80+(snake.y>>FISH_SPEED)-32,3,3,0,1,2,0,real_waggle);
   put_sprite(160+(snake.x>>FISH_SPEED)-32,80+(snake.y>>FISH_SPEED)   ,3,3,0,1,2,0,real_waggle+0x30);
   put_sprite(160+(snake.x>>FISH_SPEED)   ,80+(snake.y>>FISH_SPEED)   ,3,3,0,1,2,0,real_waggle+0x20);
  }
  sprite_mutex = 1;
 }
 raster_clk++;
 
 snake.x+=snake.dx;
 snake.y+=snake.dy;

 if(snake.x > 0)
  snake.dx-=10;
 if(snake.x < 0)
  snake.dx+=10;

 if(snake.y > 0)
  snake.dy-=10;
 if(snake.y < 0)
  snake.dy+=10;

 if(brightness<fade_to)
  brightness++;

 //lwaggle = waggle;

 waggle = (raster_clk>>2)%2; 
 if(waggle == 0)
  real_waggle = 0x250; 
 else
  real_waggle = 8;
 
}

int main(void) {
 u16 *p;
 u8 *p2;
 int i,j,k, start;
 int x,y;
 int sx,sy;
 int px, py, dx, dy;
 int data;
 u8 vcount;
 volatile unsigned char *addr;

 unsigned char buf[20];

 v_hook = no_routine;
 h_hook = no_routine;

 addr = (u8 *)0xa10001;
 pal = (*address>>6)&1; 


 fish_speed = fish_speed_const;

 if(pal == 1)
  fish_speed = 9;

 Z80_init();
 Z80_upload(0,&vgm_drv_bin, (u16)vgm_drv_bin_len,1);
 
 VDP_SET_REGISTER(MODE_SET_1, 0x16);
 VDP_SET_REGISTER(NAME_TABLE_SCROLLA_BASE, SCROLL_A_ADDR(0xc000));
 VDP_SET_REGISTER(NAME_TABLE_WINDOW_BASE,  SCROLL_A_ADDR(0x10000));
 VDP_SET_REGISTER(NAME_TABLE_SCROLLB_BASE, SCROLL_B_ADDR(0xe000));
 VDP_SET_REGISTER(SPRITE_ATTRIBUTE_BASE,   SPRITE_ADDR(SPRITE_BASE));
 VDP_SET_REGISTER(0x06, 0x00);
 VDP_SET_REGISTER(BACKGROUND_COLOR, 0);
 VDP_SET_REGISTER(0x08, 0x01);
 VDP_SET_REGISTER(0x09, 0x01);
 VDP_SET_REGISTER(H_COUNTER,10);
 VDP_SET_REGISTER(MODE_SET_3, HORIZONTAL_LINE);
 VDP_SET_REGISTER(MODE_SET_4, CELLW_40 );
 VDP_SET_REGISTER(HSCROLL_BASE, HSCROLL_ADDR( 0xfc00));
 VDP_SET_REGISTER(0x0e, 0x00);
 VDP_SET_INC(0x02);
 VDP_SET_REGISTER(SCROLL_SIZE, SCROLL_WIDTH_64);
 VDP_SET_REGISTER(WINDOW_X, 0x00);
 VDP_SET_REGISTER(WINDOW_Y, 0x00);
 VDP_SET_REGISTER(19, 0x00);
 VDP_SET_REGISTER(20, 0x00);
 VDP_SET_REGISTER(21, 0x00);
 VDP_SET_REGISTER(22, 0x00);
 if(pal == 0) {
  VDP_SET_REGISTER(MODE_SET_2, 0x14); 
 } else { 
  VDP_SET_REGISTER(MODE_SET_2, 0x14 | 0x8);
 }

 current_pal = color;
 push_pal(64,1);


 addr_z80 = (unsigned int *)0xA00FFF;
 init_z80 = (u8 *)0xA01003;
 play_z80 = (u8 *)0xA01004;
 sfx_play = (u8 *)0xA01025;
 addr_sfx = (u16 *)0xA01027;
 bank_sfx = (u8 *)0xA01026;
 len_sfx = (u16 *)0xA01029;


 wvgm_fm_addr = (u8 *)Z80_RAM+OPCODE;
 wvgm_psg_addr = (u8 *)Z80_RAM+OPCODE+1;
 wvgm_dac_addr = (u8 *)Z80_RAM+OPCODE+2;
 wvgm_stop_addr= (u8 *)Z80_RAM+OPCODE+3;

 z80_mutex =0; 
 fb_mutex = 0;
 fb_p = fb;

 fade_to = 2;
 brightness = -7;

 sprite_mutex = 0;

 bubbles[0].y = -100;
 bubbles[1].y = -100;
 bubbles[2].y = -100;
 bubble_seed = 0x1337;
 
 logo_x = 0;
 logo_y = 0;

 pending_draw = 0;

 pause = 0;
 clear_sprites();
 i = 0;
 for(y=0;y<5;y++) 
  for(x=0;x<4;x++) {
   put_sprite(logo_x+(x*32),logo_y+(y*8),3,0,0,0,1,1,8+i);
   i+=4;
  }

 memcpy(sprite_ram_orig, sprite_ram, 80*8);
 memcpy(&first_sprite, &prev_sprite, sizeof(prev_sprite_t));

 snake.x = -100<<FISH_SPEED;
 snake.y = -5<<FISH_SPEED;
 snake.dx = 0;
 snake.dy = 0; 

 do_dma(DMA_68K, DMA_DST_VRAM, (u32 *)q_pat, 0x0000, 0x6a00); 
// vram_write(0,q_pat,0x6a00);
 vram_write(0x6a00, font, (8*4)*31);
 vram_write(0x6a00+((8*4)*31), font2, (8*4)*40);
 vram_write(0x7300, font3, (8*4)*(26*4));


 for(i=0;i<0x1000;i+=2) {
  p = &q_scroll[i];
  VRAM_WRITE(0xc000+i, 0x8000| *p);
 }
 for(i=0xc000+ (28*64*2);i<0xc000+(33*64*2);i++) {
  VRAM_WRITE(i, 0x8000 | 0xe4);
 }
  

 vram_write(0xe000, q_scrollb, 0x1000);

 vram_write(0x4a00, blowfish1, 32*16*4);
 vram_write(0x100, blowfish2, 32*16*4);

 vram_write(0x1100, small_bubble, 32);

 vram_write(0x5200, one, 32*4);
 vram_write(0x4920, bubble, 32*4);
 vram_write(0x1120, small_one, 32*4);

// change_track(0);

 b_track_change = 0;
 write_line2(messages[(raster_clk>>7)%NUM_MESSAGES]);

 scroll_msg_len = strlen(scroll_msg);
 vs_x = 500;

 write_line("");

 for(i=0;i<48;i++) {
  rotate_colors_values[i] = VDP_CRAM_COLOR(rotate_colors[i].r,
                                           rotate_colors[i].g,
					   rotate_colors[i].b);
 }

 VDP_SET_REGISTER(BACKGROUND_COLOR, 0);
 v_hook = jredd_bottom;
 h_hook = h_bottom;
 VDP_SET_REGISTER(MODE_SET_2, 0x74 | (pal==1 ? 0x8 : 0x00) );

 lctrl = 0;
 ctrl = 0;


 for(;;) {
  address2 = (u16 *)VDP_CONTROL;
  while (*address2 & (1<<3));
  while (!(*address2 & (1<<3)));

  rad_clk++;

  if((rad_clk%44)==0) {
   data = rnd(&bubble_seed);
   for(j=0;j<3;j++)
    if(bubbles[j].y==-100) {
     if(snake.dx < 0) 
      bubbles[j].x = (snake.x>>FISH_SPEED)-32;
     else
      bubbles[j].x = (snake.x>>FISH_SPEED)+22;
     bubbles[j].y = -snake.y>>FISH_SPEED;
     break;
    } 
  }

  /*
 for(i=0;i<224;i++)
  hscroll_buf[(i*2)+1] =
   ((cos_tab[((i*5)+raster_clk)&0xff]*float2fix(2.0f))>>8)>>8;
*/

 j = 0;
 for(i=1;i<224*2;i+=2)
  hscroll_buf[i] =
   ((cos_tab[(((j++)*5)+raster_clk)&0xff]*float2fix(2.0f))>>8)>>8;

 logic();

  lctrl = ctrl;
  ctrl = poll_ctrl1();
  if(lctrl!=ctrl) {
   if(((ctrl&LEFT)==LEFT)/* && (current_track != 0)*/) {
    current_track--;
    if(current_track<0)
     current_track = (NUM_TRACKS-1);
    change_track(current_track);
   }
   if(((ctrl&RIGHT)==RIGHT)/* && (current_track != (NUM_TRACKS-1))*/) {
    current_track++;
    if(current_track>(NUM_TRACKS-1))
     current_track = 0;
    change_track(current_track);
   }
  }



  if(lwvgm_stop != wvgm_stop) {
   if((wvgm_stop == 1 ) /*&& (current_track != (NUM_TRACKS-1))*/) {
    current_track++;
    if(current_track>(NUM_TRACKS-1))
     current_track =0; 
    change_track(current_track);   
   }
  }


 }

}
