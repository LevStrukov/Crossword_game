#ifndef MENU_H
#define MENU_H

#include "graphics.h"

enum button_values { NONE = -1, GAME, ABOUT, EXIT, EASY, MIDDLE, HARD, N_BUTTONS };

extern int m;

struct Button
{
   int left;
   int top;
   int width;
   int height;
   IMAGE *image;
};

void load();
void complexity();
void start();
void menu();
void about();
void close();

void create_button(int, int, int, const char*);
int  select_button();

#endif