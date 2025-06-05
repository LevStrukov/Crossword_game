#include "menu.h"
#include "game.h"
#include "graphics.h"

Button buttons[N_BUTTONS];
IMAGE *menu_image, *about_image, *start_image;
int m = 4;


void load()
{
   menu_image  = loadBMP("images/menu.bmp");
   about_image = loadBMP("images/about.bmp");
   start_image = loadBMP("images/start.bmp");


   create_button(GAME,  40, 150, "buttons/game.bmp");
   create_button(ABOUT, 40, 350, "buttons/about.bmp");
   create_button(EXIT,  40, 550, "buttons/exit.bmp");
}

void start()
{
   putimage(0, 0, start_image, COPY_PUT);
   getch();
}

void menu()
{
   int state;

   while (true)
   {
      putimage(0, 0, menu_image, COPY_PUT);
      for (int i = 0; i < N_BUTTONS - 3; i++)
      {
         putimage(buttons[i].left, buttons[i].top,
                  buttons[i].image, COPY_PUT);
      }

      state = NONE;
      while (state == NONE)
      {
         while (mousebuttons() != 1);
         state = select_button();
      }

      switch (state)
      {
         case GAME:  complexity();  break;
         case ABOUT: about(); break;
         case EXIT:  close(); return;
      }
   }
}

void complexity()
{
   putimage(0, 0, menu_image, COPY_PUT);

   Button easyBtn, middleBtn, hardBtn;
   
   setbkcolor(COLOR(0, 0, 0)); // Цвет фона
   setcolor(COLOR(255, 255, 255)); // Цвет текста
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
   outtextxy(40, 150, (char*)"Выбери сложность!");
   
   int x = 40;
   int y = 250;

   easyBtn.image = loadBMP("buttons/easy.bmp");
   middleBtn.image = loadBMP("buttons/middle.bmp");
   hardBtn.image = loadBMP("buttons/hard.bmp");

   easyBtn.left = x;
   easyBtn.top = y;
   easyBtn.width = imagewidth(easyBtn.image);
   easyBtn.height = imageheight(easyBtn.image);

   middleBtn.left = x;
   middleBtn.top = y+200;
   middleBtn.width = imagewidth(middleBtn.image);
   middleBtn.height = imageheight(middleBtn.image);

   hardBtn.left = x;
   hardBtn.top = y+400;
   hardBtn.width = imagewidth(hardBtn.image);
   hardBtn.height = imageheight(hardBtn.image);

   putimage(easyBtn.left, easyBtn.top, easyBtn.image, COPY_PUT);
   putimage(middleBtn.left, middleBtn.top, middleBtn.image, COPY_PUT);
   putimage(hardBtn.left, hardBtn.top, hardBtn.image, COPY_PUT);

   int x_click, y_click;
   
   
   while (true)
   {
      while (mousebuttons() != 1 && !kbhit());
      x_click = mousex();
      y_click = mousey();

      if (x_click > easyBtn.left && x_click < easyBtn.left + easyBtn.width &&
         y_click > easyBtn.top && y_click < easyBtn.top + easyBtn.height)
      {
         m = 3;
         game();
         break;
      }
      else if (x_click > middleBtn.left && x_click < middleBtn.left + middleBtn.width &&
               y_click > middleBtn.top && y_click < middleBtn.top + middleBtn.height)
      {
         m = 4;
         game();
         break;
      }
      else if (x_click > hardBtn.left && x_click < hardBtn.left + hardBtn.width &&
               y_click > hardBtn.top && y_click < hardBtn.top + hardBtn.height)
      {
         m=5;
         game();
         break;
      }
      
      if (kbhit())
      {
         int key = getch();
         if (key == KEY_ESC)
         {
            break;
         }
      }
   }
   freeimage(easyBtn.image);
   freeimage(middleBtn.image);
   freeimage(hardBtn.image);
}

void about()
{
   putimage(0, 0, about_image, COPY_PUT);
   getch();
}

void close()
{
   freeimage(menu_image);
   freeimage(about_image);
   freeimage(start_image);
   for (int i = 0; i < N_BUTTONS; i++)
   {
      freeimage(buttons[i].image);
   }
}

void create_button(int i, int left, int top,
                   const char *file_name)
{
   buttons[i].image  = loadBMP(file_name);
   buttons[i].left   = left;
   buttons[i].top    = top;
   buttons[i].width  = imagewidth(buttons[i].image);
   buttons[i].height = imageheight(buttons[i].image);
}

int select_button()
{
   int x, y;

   x = mousex();
   y = mousey();

   for (int i = 0; i < N_BUTTONS; i++)
   {
      if (x > buttons[i].left &&
          x < buttons[i].left + buttons[i].width &&
          y > buttons[i].top &&
          y < buttons[i].top + buttons[i].height)
      {
         return i;
      }
   }

   return NONE;
}