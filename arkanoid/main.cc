#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "zorolib.h"

enum WindowState
{
  MENU = 0,
  EDITOR,
  GAME,
  LEVELSELECTOR
};

struct TColor
{
  char r, g, b, o;
};

struct TButton
{
  int idFunction;
  int fontSize;
  zoro::Vec2 pos;
  zoro::Vec2 dimensions;
  WindowState windowContext;
  char *text;
  char *label;
  TButton *next;
};

struct TBrick
{
  TBrick *next;
  zoro::Vec2 pos;
  zoro::Vec2 size;
  TColor color;
  int lives;
  int reward;
};

struct TBall
{
  zoro::Vec2 pos;
  zoro::Vec2 dir;
  float size;
  float speed;
  TBall *next;
};

struct TPlayer
{
  zoro::Vec2 pos;
  zoro::Vec2 size;
  TColor color;
  TBall *ball;
  int score;
  int lives;
};

struct TLevel
{
  TBrick *bricks;
  TPlayer player;
  TColor background;
  int difficulty;
  TLevel *next;
};

struct TScrollbar
{
  int quantity;
  float boxheight;
  float min_height;
  float max_height;
  float width;
  float xpos;
  float scrollOffset;
};

WindowState g_Window_State = EDITOR;

TButton *g_InterfaceList = nullptr;

TLevel *g_Level_List = nullptr;
TLevel *g_Current_Level = nullptr;

bool g_mouseIsDown = false;
zoro::Vec2 g_mouseDownPos;

zoro::Vec2 *g_Square_Points = nullptr;

TScrollbar *g_scrollbar_EDITOR;
TScrollbar g_scrollbar_LEVELSELECTOR;

const float kMenuCooldownTimeRef = 300.0f;
double g_MenuCooldownTime = 0.0f;

float kwidth = 1000.0f;
float kheight = 1000.0f;

unsigned char fps = 60; // Frames per second
double current_time, last_time;

int countNumberOfLevels(){
  TLevel *p = g_Level_List;
  int i = 0;
  while (p != nullptr)
  {
    i++;
    p = p->next;
  }
  
  return i;
}
void goto_EDITOR()
{
  g_Window_State = EDITOR;
  g_scrollbar_EDITOR->quantity = countNumberOfLevels();
}
bool checkMouseClick(float minx, float maxx, float miny, float maxy, int mouseType)
{
  float mx, my;
  mx = esat::MousePositionX();
  my = esat::MousePositionY();
  if (mx > minx && mx < maxx && my > miny && my < maxy)
  {
    if (mouseType == -1)
      return true;

    if (esat::MouseButtonDown(mouseType))
      return true;
  }

  return false;
}
void PaintSquare(zoro::Vec2 pos, zoro::Vec2 size, TColor border, TColor background)
{
  *(g_Square_Points + 0) = pos;
  (*(g_Square_Points + 0)).x -= size.x / 2;
  (*(g_Square_Points + 0)).y -= size.y / 2;

  *(g_Square_Points + 1) = pos;
  (*(g_Square_Points + 1)).x += size.x / 2;
  (*(g_Square_Points + 1)).y -= size.y / 2;

  *(g_Square_Points + 2) = pos;
  (*(g_Square_Points + 2)).x += size.x / 2;
  (*(g_Square_Points + 2)).y += size.y / 2;

  *(g_Square_Points + 3) = pos;
  (*(g_Square_Points + 3)).x -= size.x / 2;
  (*(g_Square_Points + 3)).y += size.y / 2;

  esat::DrawSetFillColor(background.r, background.g, background.b, background.o);
  esat::DrawSetStrokeColor(border.r, border.g, border.b, border.o);
  esat::DrawSolidPath(&g_Square_Points[0].x, 4);
}
void PaintPlayer()
{
}
void newEmptyLevel()
{

  TLevel *p = g_Level_List;

  if (p == nullptr)
  {
    p = (TLevel *)malloc(1 * sizeof(TLevel));
  }
  else
  {
    while (p->next != nullptr)
    {
      p = p->next;
    }
    p->next = (TLevel *)malloc(1 * sizeof(TLevel));
    p = p->next;
  }
 
  p->next = nullptr;
  g_scrollbar_EDITOR->quantity = countNumberOfLevels();
  printf("New level %d\n", g_scrollbar_EDITOR->quantity);
}
void callButtonFunction(int id)
{
  switch (id)
  {
  case 0:
    // Go to LEVEL EDITOR
    goto_EDITOR();

    break;
  case 1:
    // Go to LEVEL SELECTOR
    g_Window_State = LEVELSELECTOR;
    break;
  case 2:
    // Go to GAME
    g_Window_State = GAME;
    break;
  case 3:
    // Go to MENU
    g_Window_State = MENU;
    break;
  case 4:
    // Add new empty level
    newEmptyLevel();
    break;
  default:
    printf("ID button not found\n");
    break;
  }
}
void PaintButton(TButton *p)
{

  zoro::Vec2 finalTextPos;

  *(g_Square_Points + 0) = p->pos;
  (*(g_Square_Points + 0)).x -= p->dimensions.x / 2;
  (*(g_Square_Points + 0)).y -= p->dimensions.y / 2;

  *(g_Square_Points + 1) = p->pos;
  (*(g_Square_Points + 1)).x += p->dimensions.x / 2;
  (*(g_Square_Points + 1)).y -= p->dimensions.y / 2;

  *(g_Square_Points + 2) = p->pos;
  (*(g_Square_Points + 2)).x += p->dimensions.x / 2;
  (*(g_Square_Points + 2)).y += p->dimensions.y / 2;

  *(g_Square_Points + 3) = p->pos;
  (*(g_Square_Points + 3)).x -= p->dimensions.x / 2;
  (*(g_Square_Points + 3)).y += p->dimensions.y / 2;

  if (checkMouseClick((*(g_Square_Points + 0)).x, (*(g_Square_Points + 1)).x, (*(g_Square_Points + 0)).y, (*(g_Square_Points + 2)).y, -1))
  {
    if (esat::MouseButtonUp(0))
    {
      esat::ResetBufferdKeyInput();
      if (esat::Time() - g_MenuCooldownTime > kMenuCooldownTimeRef)
      {
        callButtonFunction(p->idFunction);
        g_MenuCooldownTime = esat::Time();
      }
    }

    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetStrokeColor(255, 255, 255, 255);
    esat::DrawSolidPath(&g_Square_Points[0].x, 4);

    esat::DrawSetFillColor(0, 0, 0, 255);
  }
  esat::DrawSetFillColor(255, 255, 255, 10);
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSolidPath(&g_Square_Points[0].x, 4);

  esat::DrawSetFillColor(255, 255, 255, 255);

  if (p->text != nullptr)
  {
    esat::DrawSetTextSize(p->fontSize);

    finalTextPos.y = p->pos.y + (p->dimensions.y / 2); // 1 fontsize = 0.75pixels
    finalTextPos.y -= (p->dimensions.y - p->fontSize * 0.65) / 2;

    finalTextPos.x = p->pos.x - (p->dimensions.x / 2);
    finalTextPos.x += (p->dimensions.x - strlen(p->text) * ((p->fontSize / 2))) / 2; // widthLetter = (fontsize / 2) + 4

    if (checkMouseClick((*(g_Square_Points + 0)).x, (*(g_Square_Points + 1)).x, (*(g_Square_Points + 0)).y, (*(g_Square_Points + 2)).y, -1))
      esat::DrawSetFillColor(0, 0, 0, 255);

    esat::DrawText(finalTextPos.x, finalTextPos.y, p->text);
  }
  else
  {
    printf("Button has no text memory\n");
  }
}
void PaintInterface()
{
  TButton *p = g_InterfaceList;
  while (p != nullptr)
  {
    if (g_Window_State == p->windowContext)
    {
      PaintButton(p);
    }
    p = p->next;
  }
}

void ScrollbarController(TScrollbar *scrollbar)
{
  // i * 50 = altura total scorelist
  // 500 = altura max scroller
  float scrollRange = scrollbar->max_height - scrollbar->min_height;
  float contentSize = (scrollbar->quantity * scrollbar->boxheight);

  float scrollerHeight = scrollRange * (scrollRange / contentSize);

  if (esat::MousePositionX() < scrollbar->xpos + scrollbar->width && esat::MousePositionX() > scrollbar->xpos && esat::MousePositionY() > scrollbar->min_height && esat::MousePositionY() < scrollbar->max_height && esat::MouseButtonDown(0))
  {
    g_mouseIsDown = true;
    g_mouseDownPos = {(float)esat::MousePositionX(), (float)esat::MousePositionY()};
  }

  if (esat::MouseButtonUp(0))
    g_mouseIsDown = false;

  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Down))
  {
    scrollbar->scrollOffset -= 14;
  }
  else if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Up))
  {
    scrollbar->scrollOffset += 14;
  }

  if (g_mouseIsDown)
  {
    scrollbar->scrollOffset += g_mouseDownPos.y - esat::MousePositionY();
    g_mouseDownPos.y = esat::MousePositionY();
  }

  if (scrollbar->scrollOffset < -(contentSize - scrollRange))
  {
    scrollbar->scrollOffset = -(contentSize - scrollRange);
  }
  if (scrollbar->scrollOffset > 0)
  {
    scrollbar->scrollOffset = 0;
  }

  float scrollerOffsethandle = (scrollbar->scrollOffset * scrollRange) / contentSize;

  *(g_Square_Points + 0) = {scrollbar->xpos, scrollbar->min_height - scrollerOffsethandle};
  *(g_Square_Points + 1) = {scrollbar->xpos + scrollbar->width, scrollbar->min_height - scrollerOffsethandle};
  *(g_Square_Points + 2) = {scrollbar->xpos + scrollbar->width, scrollbar->min_height + scrollerHeight - scrollerOffsethandle};
  *(g_Square_Points + 3) = {scrollbar->xpos, scrollbar->min_height + scrollerHeight - scrollerOffsethandle};

  esat::DrawSetStrokeColor(250, 250, 250, 255);
  esat::DrawSetFillColor(250, 250, 250, 180);
  if (g_mouseIsDown)
    esat::DrawSetFillColor(210, 210, 250, 180);
  esat::DrawSolidPath(&g_Square_Points[0].x, 4);
}

void Editor_UI()
{
  for (int i = 0; i < g_scrollbar_EDITOR->quantity; i++)
  {
    PaintSquare({890, 93 + g_scrollbar_EDITOR->scrollOffset}, {160, 160}, {(char)255, (char)255, (char)255, (char)255}, {(char)255, (char)255, (char)255, (char)10});
  }

  PaintSquare({500, 460}, {990, 900}, {(char)255, (char)255, (char)255, (char)255}, {(char)255, (char)255, (char)255, (char)10});
  ScrollbarController(g_scrollbar_EDITOR);
}
void MenuWindow()
{
}
void EditorWindow()
{
  Editor_UI();
}
void GameWindow()
{
}
void LevelSelectorWindow()
{
}

TButton *addButtonToList(TButton b)
{
  // Creates new button
  TButton *newbutton = (TButton *)malloc(1 * sizeof(TButton));

  newbutton->pos = b.pos;
  newbutton->dimensions = b.dimensions;
  newbutton->idFunction = b.idFunction;
  newbutton->windowContext = b.windowContext;
  newbutton->fontSize = b.fontSize;
  newbutton->text = b.text;

  // Adds button to list.
  newbutton->next = g_InterfaceList;
  g_InterfaceList = newbutton;
  return newbutton;
}

void init_UI()
{
  // esat::DrawSetTextFont("./resources/fonts/Hyperspace.otf");
  esat::DrawSetTextFont("./resources/fonts/UbuntuMono.ttf");
  TButton new_button;

  new_button.fontSize = 40;

  // Buttons

  new_button.text = "LEVEL EDITOR";
  new_button.idFunction = 0;
  new_button.pos = {300, 500};
  new_button.windowContext = MENU;
  new_button.dimensions = {350, 40};
  addButtonToList(new_button);

  new_button.text = "START";
  new_button.idFunction = 1;
  new_button.pos = {700, 500};
  new_button.windowContext = MENU;
  new_button.dimensions = {350, 40};
  addButtonToList(new_button);

  /*
  new_button.text = "START LEVEL";
  new_button.fontSize = 25;
  new_button.idFunction = 2;
  new_button.pos = {300, 500};
  new_button.windowContext = LEVELSELECTOR;
  new_button.dimensions = {200, 40};
  addButtonToList(new_button);
  new_button.fontSize = 40;

  */
  new_button.fontSize = 25;

  new_button.text = "ADD LEVEL";
  new_button.idFunction = 4;
  new_button.pos = {140, 950};
  new_button.windowContext = EDITOR;
  new_button.dimensions = {250, 40};
  addButtonToList(new_button);

  new_button.text = "BACK TO MENU";
  new_button.idFunction = 3;
  new_button.pos = {860, 950};
  new_button.windowContext = LEVELSELECTOR;
  new_button.dimensions = {250, 40};
  addButtonToList(new_button);
  new_button.windowContext = EDITOR;
  addButtonToList(new_button);

  // Scrollbar
  g_scrollbar_EDITOR = (TScrollbar *)malloc(1 * sizeof(TScrollbar));

  g_scrollbar_EDITOR->boxheight = 160;
  g_scrollbar_EDITOR->max_height = 909;
  g_scrollbar_EDITOR->min_height = 12;
  g_scrollbar_EDITOR->quantity = 15;
  g_scrollbar_EDITOR->scrollOffset = 0;
  g_scrollbar_EDITOR->width = 20;
  g_scrollbar_EDITOR->xpos = 973;
}

void init_Game()
{
  g_Square_Points = (zoro::Vec2 *)malloc(4 * sizeof(zoro::Vec2));

  init_UI();
}

int esat::main(int argc, char **argv)
{
  srand(time(NULL));

  esat::WindowInit(kwidth, kheight);
  WindowSetMouseVisibility(true);

  init_Game();
  printf("PROGRAM START");
  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {

    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(10, 30, 40);

    // --------------

    switch (g_Window_State)
    {
    case MENU:
      MenuWindow();
      break;
    case EDITOR:
      EditorWindow();
      break;
    case GAME:
      GameWindow();
      break;
    case LEVELSELECTOR:
      LevelSelectorWindow();
      break;
    default:
      printf("Error with current window state. \n^w^\n");
      break;
    }

    PaintInterface();

    // --------------
    esat::DrawEnd();
    do
    {
      current_time = esat::Time();
    } while ((current_time - last_time) <= 1000.0 / fps);
    esat::WindowFrame();
  }

  esat::WindowDestroy();
  return 0;
}