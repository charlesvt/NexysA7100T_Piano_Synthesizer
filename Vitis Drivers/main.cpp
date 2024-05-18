/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "xadc_core.h"
#include "sseg_core.h"
#include "spi_core.h"
#include "i2c_core.h"
#include "ps2_core.h"
#include "ddfs_core.h"
#include "adsr_core.h"
#include "vga_core.h"
#include "keyboard_map.h"
#include "note_map.h"

void test_start(GpoCore *led_p) {
   int i;

   for (i = 0; i < 20; i++) {
      led_p->write(0xff00);
      sleep_ms(50);
      led_p->write(0x0000);
      sleep_ms(50);
   }
}

/**
 * check bar generator core
 * @param bar_p pointer to Gpv instance
 */
void bar_check(GpvCore *bar_p) {
   bar_p->bypass(0);
   sleep_ms(3000);
}

/**
 * check color-to-grayscale core
 * @param gray_p pointer to Gpv instance
 */
void gray_check(GpvCore *gray_p) {
   gray_p->bypass(0);
   sleep_ms(3000);
   gray_p->bypass(1);
}

/**
 * check osd core
 * @param osd_p pointer to osd instance
 */
void osd_check(OsdCore *osd_p) {
   osd_p->set_color(0x0f0, 0x001); // dark gray/green
   osd_p->bypass(0);
   osd_p->clr_screen();
   for (int i = 0; i < 64; i++) {
      osd_p->wr_char(8 + i, 20, i);
      osd_p->wr_char(8 + i, 21, 64 + i, 1);
      sleep_ms(100);
   }
   sleep_ms(3000);
}

/**
 * test frame buffer core
 * @param frame_p pointer to frame buffer instance
 */
void frame_check(FrameCore *frame_p) {
   int x, y, color;

   frame_p->bypass(0);
   for (int i = 0; i < 10; i++) {
      frame_p->clr_screen(0x008);  // dark green
      for (int j = 0; j < 20; j++) {
         x = rand() % 640;
         y = rand() % 480;
         color = rand() % 512;
         frame_p->plot_line(400, 200, x, y, color);
      }
      sleep_ms(300);
   }
   sleep_ms(3000);
}

/**
 * test ghost sprite
 * @param ghost_p pointer to mouse sprite instance
 */
void mouse_check(SpriteCore *mouse_p) {
   int x, y;

   mouse_p->bypass(0);
   // clear top and bottom lines
   for (int i = 0; i < 32; i++) {
      mouse_p->wr_mem(i, 0);
      mouse_p->wr_mem(31 * 32 + i, 0);
   }

   // slowly move mouse pointer
   x = 0;
   y = 0;
   for (int i = 0; i < 80; i++) {
      mouse_p->move_xy(x, y);
      sleep_ms(50);
      x = x + 4;
      y = y + 3;
   }
   sleep_ms(3000);
   // load top and bottom rows
   for (int i = 0; i < 32; i++) {
      sleep_ms(20);
      mouse_p->wr_mem(i, 0x00f);
      mouse_p->wr_mem(31 * 32 + i, 0xf00);
   }
   sleep_ms(3000);
}

/**
 * test ghost sprite
 * @param ghost_p pointer to ghost sprite instance
 */
void ghost_check(SpriteCore *ghost_p) {
   int x, y;

   // slowly move mouse pointer
   ghost_p->bypass(0);
   ghost_p->wr_ctrl(0x1c);  //animation; blue ghost
   x = 0;
   y = 100;
   for (int i = 0; i < 156; i++) {
      ghost_p->move_xy(x, y);
      sleep_ms(100);
      x = x + 4;
      if (i == 80) {
         // change to red ghost half way
         ghost_p->wr_ctrl(0x04);
      }
   }
   sleep_ms(3000);
}

void ddfs_check(DdfsCore *ddfs_p, GpoCore *led_p) {
   int i, j;
   float env;

   //vol = (float)sw.read_pin()/(float)(1<<16),
   ddfs_p->set_env_source(0);  // select envelop source
   ddfs_p->set_env(0.0);   // set volume
   sleep_ms(500);
   ddfs_p->set_env(0.01);   // set volume
   ddfs_p->set_carrier_freq(262);
   sleep_ms(2000);
   ddfs_p->set_env(0.0);   // set volume
   sleep_ms(2000);
   // volume control (attenuation)
   ddfs_p->set_env(0.0);   // set volume
   env = 0.01;
   for (i = 0; i < 1000; i++) {
      ddfs_p->set_env(0.01);
      sleep_ms(10);
      env = env / 1.0109; //1.0109**1024=2**16
   }
   // frequency modulation 635-912 800 - 2000 siren sound
   ddfs_p->set_env(env);   // set volume
   ddfs_p->set_carrier_freq(635);
   for (i = 0; i < 5; i++) {               // 10 cycles
      for (j = 0; j < 30; j++) {           // sweep 30 steps
         ddfs_p->set_offset_freq(j * 10);  // 10 Hz increment
         sleep_ms(25);
      } // end j loop
   } // end i loop
   ddfs_p->set_offset_freq(0);
   ddfs_p->set_env(0.0);   // set volume
   sleep_ms(1000);
}

void twinkle_twinkle(DdfsCore *ddfs_p) {
	float env = 0.01;

	ddfs_p->set_env_source(0);  // select envelop source

	ddfs_p->set_carrier_freq(C4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(G4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(A4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(G4);
	ddfs_p->set_env(env);
	sleep_ms(1000);
	ddfs_p->set_env(0.00);
	sleep_ms(500);

	ddfs_p->set_carrier_freq(F4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(E4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(D4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(C4);
	ddfs_p->set_env(env);
	sleep_ms(1000);
	ddfs_p->set_env(0.00);
	sleep_ms(500);

	ddfs_p->set_carrier_freq(G4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(F4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(E4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(D4);
	ddfs_p->set_env(env);
	sleep_ms(1000);
	ddfs_p->set_env(0.00);
	sleep_ms(500);

	ddfs_p->set_carrier_freq(G4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(F4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(E4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(D4);
	ddfs_p->set_env(env);
	sleep_ms(1000);
	ddfs_p->set_env(0.00);
	sleep_ms(500);

	ddfs_p->set_carrier_freq(C4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(G4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(A4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(G4);
	ddfs_p->set_env(env);
	sleep_ms(1000);
	ddfs_p->set_env(0.00);
	sleep_ms(500);

	ddfs_p->set_carrier_freq(F4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(E4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(D4);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_env(env);
	sleep_ms(500);
	ddfs_p->set_env(0.00);
	sleep_ms(500);
	ddfs_p->set_carrier_freq(C4);
	ddfs_p->set_env(env);
	sleep_ms(1000);
	ddfs_p->set_env(0.00);
	sleep_ms(2500);
}

const int KEY_WIDTH = 91; // Adjust as needed
const int KEY_HEIGHT = 320; // Adjust as needed
const int NUM_WHITE_KEYS = 7; // One octave
const int BLACK_KEY_WIDTH = KEY_WIDTH * 0.55;
const int BLACK_KEY_HEIGHT = KEY_HEIGHT * 0.60;
const int HMAX = 640; // Screen width
const int VMAX = 480; // Screen height
const int outline_color = 0x888; // Gray color for outline
const int white_key_color = 0xFFF;   // White color for filling
const int black_key_color = 0x000; // Black color

void draw_white_key(int x, int y, int color, FrameCore* frame_p) {

    // Draw gray outline
    int outline_thickness = 2; // Adjust the thickness as needed
    for (int i = 0; i < KEY_WIDTH; ++i) {
        for (int j = 0; j < KEY_HEIGHT; ++j) {
            if (i < outline_thickness || i >= KEY_WIDTH - outline_thickness ||
                j < outline_thickness || j >= KEY_HEIGHT - outline_thickness) {
                frame_p->wr_pix(x + i, y + j, outline_color);
            } else {
                frame_p->wr_pix(x + i, y + j, color);
            }
        }
    }
}

void draw_black_key(int x_mid, int y, int color, FrameCore* frame_p ) {
    int x_start = x_mid - (BLACK_KEY_WIDTH / 2);
    int y_start = y;

    for (int i = 0; i < BLACK_KEY_WIDTH; ++i) {
        for (int j = 0; j < BLACK_KEY_HEIGHT; ++j) {
            frame_p->wr_pix(x_start + i, y_start + j, color);
        }
    }
}

void draw_piano_octave(int start_x, int start_y, FrameCore* frame_p) {
    // Draw white keys
    for (int i = 0; i < NUM_WHITE_KEYS; ++i) {
        draw_white_key(start_x + i * KEY_WIDTH, start_y, white_key_color, frame_p);
    }

    // Draw black keys
    draw_black_key(start_x + KEY_WIDTH, start_y, black_key_color, frame_p); // C#
    draw_black_key(start_x + 2 * KEY_WIDTH, start_y, black_key_color, frame_p); // D#
    draw_black_key(start_x + 4 * KEY_WIDTH, start_y, black_key_color, frame_p); // F#
    draw_black_key(start_x + 5 * KEY_WIDTH, start_y, black_key_color, frame_p); // G#
    draw_black_key(start_x + 6 * KEY_WIDTH, start_y, black_key_color, frame_p); // A#
}

void manual_play(Ps2Core *ps2_p, DdfsCore *ddfs_p, OsdCore *osd_p, FrameCore* frame_p)
{
    // Initialize variables
    char ch;
	ddfs_p->set_env_source(0);  // select envelop source
	osd_p->set_color(0xfff, 0x001);
	float env = 0.10; // Volume set
	if (ps2_p->get_kb_ch(&ch)) {
		// note detection
		if (ch == KEY_q || ch == KEY_Q) //Q
		{
			ddfs_p->set_carrier_freq(C4);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(5, 18, 'C', 1);
			//uart.disp("C4");
		}
		else if (ch == KEY_2) //2
		{
			ddfs_p->set_carrier_freq(C4s);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(10, 10, 'C');
			osd_p->wr_char(12, 10, '#');
			//uart.disp("C4s");
		}
		else if (ch == KEY_w || ch == KEY_W) //W
		{
			ddfs_p->set_carrier_freq(D4);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(16, 18, 'D', 1);
			//uart.disp("D4");
		}
		else if (ch == KEY_3) //3
		{
			ddfs_p->set_carrier_freq(D4s);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(21, 10, 'D');
			osd_p->wr_char(23, 10, '#');
			//uart.disp("D4s");
		}
		else if (ch == KEY_e || ch == KEY_E) //E
		{
			ddfs_p->set_carrier_freq(E4);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(28, 18, 'E', 1);
			//uart.disp("E4");
		}
		else if (ch == KEY_r || ch == KEY_R) //R
		{
			ddfs_p->set_carrier_freq(F4);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(39, 18, 'F', 1);
			//uart.disp("F4");
		}
		else if (ch == KEY_5) //5
		{
			ddfs_p->set_carrier_freq(F4s);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(44, 10, 'F');
			osd_p->wr_char(46, 10, '#');
			//uart.disp("F4s");
		}
		else if (ch == KEY_t || ch == KEY_T) //T
		{
			ddfs_p->set_carrier_freq(G4);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(51, 18, 'G', 1);
			//uart.disp("G4");
		}
		else if (ch == KEY_6) //6
		{
			ddfs_p->set_carrier_freq(G4s);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(55, 10, 'G');
			osd_p->wr_char(57, 10, '#');
			//uart.disp("G4s");
		}
		else if (ch == KEY_y || ch == KEY_Y) //Y
		{
			ddfs_p->set_carrier_freq(A4);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(62, 18, 'A', 1);
			//uart.disp("A4");
		}
		else if (ch == KEY_7) //7
		{
			ddfs_p->set_carrier_freq(A4s);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(67, 10, 'A');
			osd_p->wr_char(69, 10, '#');
			//uart.disp("A4s");
		}
		else if (ch == KEY_u || ch == KEY_U) //U
		{
			ddfs_p->set_carrier_freq(B4);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(74, 18, 'B', 1);
			//uart.disp("B4");
		}
		else if (ch == KEY_i || ch == KEY_I) //I
		{
			ddfs_p->set_carrier_freq(C5);
			ddfs_p->set_env(env);
			osd_p->bypass(0);
			osd_p->wr_char(5, 18, 'C', 1);
			//uart.disp("C5");
		}
    }
	else
	{
		sleep_ms(200);
		ddfs_p->set_env(0.00);
		osd_p->clr_screen();
		osd_p->bypass(1);
	}


}

// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
SpriteCore banana(get_sprite_addr(BRIDGE_BASE, V4_USER4), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
PwmCore pwm(get_slot_addr(BRIDGE_BASE, S6_PWM));
DebounceCore btn(get_slot_addr(BRIDGE_BASE, S7_BTN));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
SpiCore spi(get_slot_addr(BRIDGE_BASE, S9_SPI));
I2cCore adt7420(get_slot_addr(BRIDGE_BASE, S10_I2C));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
DdfsCore ddfs(get_slot_addr(BRIDGE_BASE, S12_DDFS));
AdsrCore adsr(get_slot_addr(BRIDGE_BASE, S13_ADSR), &ddfs);

int main() {
	int start_x = 0;
	int start_y = 0;
//	frame.bypass(1);
	bar.bypass(1);
	gray.bypass(1);
	ghost.bypass(1);
	osd.bypass(1);
	mouse.bypass(1);
	banana.bypass(0);
	draw_piano_octave(start_x, start_y, &frame);
   while (1) {
	   manual_play(&ps2, &ddfs, &osd, &frame);
   } // while
} //main
