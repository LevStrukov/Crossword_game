#define WIDTH  1200
#define HEIGHT 800

#include "menu.h"
#include "graphics.h"

int main()
{
   initwindow(WIDTH, HEIGHT);
   load();
   start();
   menu();
   closegraph();
   return 0;
}