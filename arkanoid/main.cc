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
  char *text;
  char *label;
  TButton *next;
  WindowState windowContext;
  int idFunction;
  int fontSize;
  zoro::Vec2 pos;
  zoro::Vec2 dimensions;
};

struct TBrick
{
  TBrick *next;
  zoro::Vec2 pos;
  zoro::Vec2 size;
  TColor color;
  int lives;
  int reward;
  bool active;
};

struct TBall
{
  int knPoints = 16;
  bool onPlayer;
  zoro::Vec2 pos;
  zoro::Vec2 dir;
  zoro::Mat3 M;
  float size;
  float speed;
  zoro::Vec3 g_points[16];
  zoro::Vec2 dr_points[16];
  TBall *next;
};

struct TPlayer
{
  int score;
  int lives;
  float acceleration;
  float speed;
  zoro::Vec2 pos;
  zoro::Vec2 size;
  zoro::Vec2 dir;
  TColor color;
  TBall *ball;
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
  int margin;
  int columns;
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
TLevel g_inGameLevel;

bool g_mouseIsDown = false;
bool g_scrollIsSelected = false;
zoro::Vec2 g_mouseDownPos;
TBrick *g_Brick_selected = nullptr;
TBrick *g_BrickLastSelected = nullptr;

zoro::Vec2 *g_Square_Points = nullptr;

TScrollbar *g_scrollbar_EDITOR;
TScrollbar *g_scrollbar_LEVELSELECTOR;

const float kMenuCooldownTimeRef = 300.0f;
double g_MenuCooldownTime = 0.0f;

TPlayer kDefault_Player;

const float kwidth = 1000.0f;
const float kheight = 1000.0f;

TColor kDefaultBackground = {(char)10, (char)30, (char)40, (char)255};

unsigned char fps = 60; // Frames per second
double current_time, last_time;

int countNumberOfLevels()
{
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
bool checkMouseClickOnLocalCoordinates(zoro::Vec2 TargetPos, zoro::Vec2 TargetSize, int mouseType, zoro::Vec2 LocalBoxPos, zoro::Vec2 LocalBoxSize)
{
  /*
    Target in local coordinates and the local box size
  */

  zoro::Vec2 min = zoro::CoordGlobalToLocalVec2({TargetPos.x - (TargetSize.x / 2), TargetPos.y - (TargetSize.y / 2)}, {0, 0}, {kwidth, kheight});
  min = zoro::CoordLocalToGlobalVec2(min,
                                     {LocalBoxPos.x - (LocalBoxSize.x / 2), LocalBoxPos.y - (LocalBoxSize.y / 2)},
                                     {LocalBoxPos.x + (LocalBoxSize.x / 2), LocalBoxPos.y + (LocalBoxSize.y / 2)});

  zoro::Vec2 max = zoro::CoordGlobalToLocalVec2({TargetPos.x + (TargetSize.x / 2), TargetPos.y + (TargetSize.y / 2)}, {0, 0}, {kwidth, kheight});
  max = zoro::CoordLocalToGlobalVec2(max,
                                     {LocalBoxPos.x - (LocalBoxSize.x / 2), LocalBoxPos.y - (LocalBoxSize.y / 2)},
                                     {LocalBoxPos.x + (LocalBoxSize.x / 2), LocalBoxPos.y + (LocalBoxSize.y / 2)});

  zoro::Vec2 mouse = {(float)esat::MousePositionX(), (float)esat::MousePositionY()};

  if (mouse.x > min.x && mouse.x < max.x && mouse.y > min.y && mouse.y < max.y)
  {
    if (mouseType == -1)
      return true;

    if (esat::MouseButtonDown(mouseType))
      return true;
  }

  return false;
}

void initBall(TBall **ball)
{
  TBall *nBall = (TBall *)malloc(sizeof(TBall));
  nBall->knPoints = 16;
  nBall->next = nullptr;
  nBall->onPlayer = true;
  nBall->pos = {500, 500};
  nBall->size = 20.0f;
  nBall->speed = 10.0f;

  for (int i = 0; i < nBall->knPoints; i++)
  {
    nBall->g_points[i] = {cosf((6.28f / nBall->knPoints) * i), sinf(((6.28f / nBall->knPoints) * i)), 1.0f};
  }

  nBall->next = *ball;
  *ball = nBall;
}

bool checkHitBox(zoro::Vec2 box1pos, zoro::Vec2 box1size, zoro::Vec2 box2pos, zoro::Vec2 box2size)
{
  if (box1pos.x - (box1size.x / 2) < box2pos.x + (box2size.x / 2) && box1pos.x + (box1size.x / 2) > box2pos.x - (box2size.x / 2) && box1pos.y - (box1size.y / 2) < box2pos.y + (box2size.y / 2) && box1pos.y + (box1size.y / 2) > box2pos.y - (box2size.y / 2))
    return true;

  return false;
}
void DeleteBrick(TBrick *brick)
{
  TBrick *p = g_Current_Level->bricks;
  TBrick *temp;

  if (p == brick)
  {
    g_Current_Level->bricks = g_Current_Level->bricks->next;
    return;
  }

  while (p->next != brick)
    p = p->next;

  temp = p->next;
  p->next = p->next->next;
  free(temp);
}
bool brickColisionSpawnCheck(zoro::Vec2 pos)
{
  TBrick *b = g_Current_Level->bricks;
  while (b != nullptr)
  {
    if (b != g_BrickLastSelected)
    {
      if (checkHitBox(b->pos, b->size, pos, g_BrickLastSelected->size))
        return true;
    }
    b = b->next;
  }
  return false;
}

zoro::Vec2 colisionBallSquare(TBall *ball, zoro::Vec2 pos, zoro::Vec2 size)
{
  if (ball == nullptr)
    return {99.0f, 99.0f};

  zoro::Vec2 VDistance = zoro::SubtractVec2(ball->pos, pos);

  if (VDistance.x > size.x / 2)
    VDistance.x = size.x / 2;
  if (VDistance.x < -size.x / 2)
    VDistance.x = -size.x / 2;
  if (VDistance.y > size.y / 2)
    VDistance.y = size.y / 2;
  if (VDistance.y < -size.y / 2)
    VDistance.y = -size.y / 2;

  esat::DrawSetStrokeColor(255, 0, 0, 255);
  esat::DrawLine(VDistance.x + pos.x, VDistance.y + pos.y, pos.x, pos.y);

  if (zoro::MagnitudeVec2(VDistance) + ball->size / 2 > zoro::MagnitudeVec2(zoro::SubtractVec2(ball->pos, pos)))
  {

    if (zoro::DotVec2(zoro::NormalizeVec2(zoro::ScaleVec2(VDistance, -1)), zoro::NormalizeVec2(ball->dir)) > 0.5f)
    {
      if (!checkHitBox(zoro::SumVec2(zoro::SumVec2(pos, VDistance), zoro::NormalizeVec2(zoro::LeftPerpendicularVec2(VDistance))), {1, 1}, pos, size))
      {
        return zoro::NormalizeVec2(zoro::LeftPerpendicularVec2(VDistance));
      }
      else
      {
        return zoro::NormalizeVec2(zoro::RightPerpendicularVec2(VDistance));
      }
    }
    else
    {
      return zoro::NormalizeVec2(VDistance);
    }
  }
  return {99.0f, 99.0f};
}

void Ball()
{
  TBall *balls = g_Current_Level->player.ball;
  TBall *p = balls;

  if (balls == nullptr)
  {
    printf("ball not painting cause stupid\n");
    return;
  }

  while (p != nullptr)
  {

    if (p->onPlayer)
    {
      p->pos = {g_Current_Level->player.pos.x, g_Current_Level->player.pos.y - g_Current_Level->player.ball->size - (g_Current_Level->player.size.y / 2)};
      p->dir = zoro::NormalizeVec2(g_Current_Level->player.dir);
      if (esat::IsKeyDown('X'))
        p->onPlayer = false;
    }
    else
    {
      if (g_Current_Level->bricks != nullptr && !p->onPlayer)
      {
        zoro::Vec2 VDistance;
        zoro::Vec2 newDir;
        TBrick *br = g_Current_Level->bricks;
        while (br != nullptr)
        {
          if (br->active)
          {
            newDir = colisionBallSquare(p, br->pos, br->size);
            if (zoro::MagnitudeVec2(newDir) < 2.0f)
            {
              p->dir = newDir;
              g_Current_Level->player.score += br->reward;
              br->active = false;
            }
          }

          br = br->next;
        }

        newDir = colisionBallSquare(p, g_Current_Level->player.pos, g_Current_Level->player.size);

        if (zoro::MagnitudeVec2(newDir) < 2.0f && p->pos.y < g_Current_Level->player.pos.y)
        {
          p->dir.y *= -1;
          p->dir.x += g_Current_Level->player.acceleration * 0.6f;
          p->dir = zoro::NormalizeVec2(p->dir);
          p->pos.y = g_Current_Level->player.pos.y - (g_Current_Level->player.size.y / 2) - p->size;
        }
      }

      p->pos = zoro::SumVec2(p->pos, zoro::ScaleVec2(p->dir, p->speed));

      if (p->pos.x + p->size > kwidth)
      {
        p->dir.x *= -1;
        p->pos.x = kwidth - p->size - 1;
      }

      if (p->pos.x - p->size < 0)
      {
        p->dir.x *= -1;
        p->pos.x = p->size + 1;
      }

      if (p->pos.y - p->size < 0)
      {
        p->dir.y *= -1;
        p->pos.y = p->size + 1;
      }

      if (p->pos.y + p->size > kheight)
      {
        p->onPlayer = true;
        g_Current_Level->player.lives--;
      }
    }

    p->M = zoro::MatIdentity3();
    p->M = zoro::Mat3Multiply(zoro::Mat3Translate(p->pos), p->M);
    p->M = zoro::Mat3Multiply(zoro::Mat3Scale(p->size, p->size), p->M);

    for (int i = 0; i < p->knPoints; i++)
    {
      zoro::Vec3 tmp = zoro::Mat3TransformVec3(p->M, p->g_points[i]);
      p->dr_points[i] = {tmp.x, tmp.y};
    }

    esat::DrawSetStrokeColor(210, 210, 210, 50);
    esat::DrawSetFillColor(180, 180, 220, 255);
    esat::DrawSolidPath(&p->dr_points[0].x, p->knPoints);

    p = p->next;
  }
}

void PaintSquareToBox(zoro::Vec2 pos, zoro::Vec2 size, TColor border, TColor background, zoro::Vec2 BoxPos, zoro::Vec2 BoxSize)
{

  zoro::Vec2 GlobalBoundingBox_min = {BoxPos.x - (BoxSize.x / 2), BoxPos.y - (BoxSize.y / 2)};
  zoro::Vec2 GlobalBoundingBox_max = {BoxPos.x + (BoxSize.x / 2), BoxPos.y + (BoxSize.y / 2)};

  *(g_Square_Points + 0) = pos;
  (*(g_Square_Points + 0)).x -= size.x / 2;
  (*(g_Square_Points + 0)).y -= size.y / 2;

  *(g_Square_Points + 0) = zoro::CoordGlobalToLocalVec2(*(g_Square_Points + 0), {0, 0}, {kwidth, kheight});
  *(g_Square_Points + 0) = zoro::CoordLocalToGlobalVec2(*(g_Square_Points + 0), {GlobalBoundingBox_min}, GlobalBoundingBox_max);

  *(g_Square_Points + 1) = pos;
  (*(g_Square_Points + 1)).x += size.x / 2;
  (*(g_Square_Points + 1)).y -= size.y / 2;

  *(g_Square_Points + 1) = zoro::CoordGlobalToLocalVec2(*(g_Square_Points + 1), {0, 0}, {kwidth, kheight});
  *(g_Square_Points + 1) = zoro::CoordLocalToGlobalVec2(*(g_Square_Points + 1), GlobalBoundingBox_min, GlobalBoundingBox_max);

  *(g_Square_Points + 2) = pos;
  (*(g_Square_Points + 2)).x += size.x / 2;
  (*(g_Square_Points + 2)).y += size.y / 2;

  *(g_Square_Points + 2) = zoro::CoordGlobalToLocalVec2(*(g_Square_Points + 2), {0, 0}, {kwidth, kheight});
  *(g_Square_Points + 2) = zoro::CoordLocalToGlobalVec2(*(g_Square_Points + 2), GlobalBoundingBox_min, GlobalBoundingBox_max);

  *(g_Square_Points + 3) = pos;
  (*(g_Square_Points + 3)).x -= size.x / 2;
  (*(g_Square_Points + 3)).y += size.y / 2;

  *(g_Square_Points + 3) = zoro::CoordGlobalToLocalVec2(*(g_Square_Points + 3), {0, 0}, {kwidth, kheight});
  *(g_Square_Points + 3) = zoro::CoordLocalToGlobalVec2(*(g_Square_Points + 3), GlobalBoundingBox_min, GlobalBoundingBox_max);

  esat::DrawSetFillColor(background.r, background.g, background.b, background.o);
  esat::DrawSetStrokeColor(border.r, border.g, border.b, border.o);
  esat::DrawSolidPath(&g_Square_Points[0].x, 4);
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

  g_Current_Level->player.acceleration = 0.0f;
  if (esat::IsKeyPressed('A') || esat::IsSpecialKeyPressed(esat::kSpecialKey_Left))
  {
    g_Current_Level->player.acceleration = -1.0f;
    g_Current_Level->player.dir.x = -1.0f;
  }

  if (esat::IsKeyPressed('D') || esat::IsSpecialKeyPressed(esat::kSpecialKey_Right))
  {
    g_Current_Level->player.dir.x = 1.0f;
    g_Current_Level->player.acceleration = +1.0f;
  }

  g_Current_Level->player.speed = g_Current_Level->player.acceleration + (g_Current_Level->player.speed * 0.93);
  g_Current_Level->player.pos.x += g_Current_Level->player.speed;

  if (g_Current_Level->player.pos.x + (g_Current_Level->player.size.x / 2) > kwidth)
    g_Current_Level->player.pos.x = -2 + kwidth - (g_Current_Level->player.size.x / 2);
  if (g_Current_Level->player.pos.x - (g_Current_Level->player.size.x / 2) < 0)
    g_Current_Level->player.pos.x = (g_Current_Level->player.size.x / 2) + 2;

  Ball();

  PaintSquare(g_Current_Level->player.pos, g_Current_Level->player.size, g_Current_Level->player.color, g_Current_Level->player.color);
}
TBrick *addBrick(TBrick **brickList, zoro::Vec2 pos)
{
  TBrick *nBrick = (TBrick *)malloc(sizeof(TBrick));

  nBrick->active = true;
  nBrick->color = {(char)200, (char)200, (char)200, (char)230};
  nBrick->lives = 1;
  nBrick->pos = pos;
  nBrick->reward = 10;
  nBrick->size = {100, 45};

  nBrick->next = *brickList;
  *brickList = nBrick;

  printf("New brick\n");
  return nBrick;
}

void deleteLevel(TLevel *level)
{
  TLevel *p = g_Level_List;
  TLevel *temp;

  if (p == level)
  {
    g_Level_List = g_Level_List->next;
    return;
  }

  while (p->next != level)
    p = p->next;

  temp = p->next;
  p->next = p->next->next;
  free(temp);
}

void newEmptyLevel()
{
  TLevel *p = (TLevel *)malloc(sizeof(TLevel));
  p->difficulty = countNumberOfLevels();
  p->bricks = nullptr;
  // addBrick(&p->bricks, {400, 400});
  p->player = kDefault_Player;
  p->player.lives = 3;
  p->player.ball = nullptr;
  p->background = kDefaultBackground;
  p->next = g_Level_List;
  g_Level_List = p;
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
    g_scrollbar_LEVELSELECTOR->quantity = countNumberOfLevels();
    break;
  case 2:
    // Go to GAME
    g_Window_State = GAME;
    break;
  case 3:
    // Go to MENU
    g_Window_State = MENU;
    g_Brick_selected = nullptr;
    g_BrickLastSelected = nullptr;
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
void PaintLevel(TLevel *level, zoro::Vec2 origin, zoro::Vec2 size)
{

  if (level == nullptr)
  {
    printf("Error level not found.");
    return;
  }

  if (level->bricks != nullptr)
  {
    TBrick *b = level->bricks;
    while (b != nullptr)
    {
      if (b == g_Brick_selected)
      {
        PaintSquareToBox(b->pos, b->size, {(char)255, (char)0, (char)0, (char)255}, b->color, origin, size);
      }
      else if (b == g_BrickLastSelected)
      {
        PaintSquareToBox(b->pos, b->size, {(char)0, (char)0, (char)255, (char)255}, b->color, origin, size);
      }
      else
      {
        if (g_Window_State == GAME)
        {
          if (b->active)
            PaintSquareToBox(b->pos, b->size, {(char)255, (char)255, (char)255, (char)255}, b->color, origin, size);
        }
        else
        {
          PaintSquareToBox(b->pos, b->size, {(char)255, (char)255, (char)255, (char)255}, b->color, origin, size);
        }
      }

      b = b->next;
    }
  }
}

void startGame(int baseScore)
{
  if (g_Current_Level == nullptr)
  {
    printf("Level not found!\n");
    return;
  }

  if (g_Current_Level->bricks == nullptr)
  {
    printf("Level empty!\n");
    return;
  }
  g_Brick_selected = nullptr;
  g_BrickLastSelected = nullptr;
  TBrick *br = g_Current_Level->bricks;

  while (br != nullptr)
  {
    br->active = true;
    br = br->next;
  }

  g_Window_State = GAME;
  g_Current_Level->player = kDefault_Player;
  g_Current_Level->player.score = baseScore;
  printf("INIT BALL\n");
  initBall(&g_Current_Level->player.ball);
  printf("Game Started\n");
}

void ScrollbarController(TScrollbar *scrollbar)
{
  float scrollRange = scrollbar->max_height - scrollbar->min_height;
  float contentSize = ((scrollbar->quantity / scrollbar->columns) * (scrollbar->boxheight + scrollbar->margin));

  float scrollerHeight = scrollRange * (scrollRange / contentSize);

  if (esat::MousePositionX() < scrollbar->xpos + scrollbar->width && esat::MousePositionX() > scrollbar->xpos && esat::MousePositionY() > scrollbar->min_height && esat::MousePositionY() < scrollbar->max_height && esat::MouseButtonDown(0))
  {
    g_mouseIsDown = true;
    g_scrollIsSelected = true;
    g_mouseDownPos = {(float)esat::MousePositionX(), (float)esat::MousePositionY()};
  }

  if (esat::MouseButtonUp(0))
  {
    if (g_mouseIsDown)
      g_scrollIsSelected = false;

    g_mouseIsDown = false;
  }

  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Down))
  {
    scrollbar->scrollOffset -= 10 + (scrollbar->quantity * 2);
  }
  else if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Up))
  {
    scrollbar->scrollOffset += 10 + (scrollbar->quantity * 2);
  }

  if (g_mouseIsDown && g_scrollIsSelected)
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

  if (scrollRange < scrollbar->quantity * scrollbar->boxheight)
  {
    esat::DrawSetStrokeColor(250, 250, 250, 255);
    esat::DrawSetFillColor(250, 250, 250, 180);
    if (g_mouseIsDown && g_scrollIsSelected)
      esat::DrawSetFillColor(210, 210, 250, 180);
    esat::DrawSolidPath(&g_Square_Points[0].x, 4);
  }
}
void EditorWindow()
{

  // Scrollbar
  TLevel *p = g_Level_List;
  bool emptyClick = false;
  for (int i = 0; i < g_scrollbar_EDITOR->quantity; i++)
  {
    if (p == g_Current_Level)
    {
      PaintSquare({895, 93 + g_scrollbar_EDITOR->boxheight * i + (g_scrollbar_EDITOR->scrollOffset) + (i * g_scrollbar_EDITOR->margin)}, {160, 160}, {(char)255, (char)255, (char)255, (char)255}, {(char)50, (char)50, (char)255, (char)100});
    }
    else
    {
      PaintSquare({895, 93 + g_scrollbar_EDITOR->boxheight * i + (g_scrollbar_EDITOR->scrollOffset) + (i * g_scrollbar_EDITOR->margin)}, {160, 160}, {(char)255, (char)255, (char)255, (char)255}, {(char)255, (char)255, (char)255, (char)10});
    }

    PaintLevel(p, {895, 93 + g_scrollbar_EDITOR->boxheight * i + (g_scrollbar_EDITOR->scrollOffset) + (i * g_scrollbar_EDITOR->margin)}, {160, 160});

    // SELECT LEVEL
    if (esat::MouseButtonDown(0))
    {
      if (checkMouseClick(810, 970, (13 + g_scrollbar_EDITOR->boxheight * i + (g_scrollbar_EDITOR->scrollOffset)) + (i * g_scrollbar_EDITOR->margin), (173 + g_scrollbar_EDITOR->boxheight * i + (g_scrollbar_EDITOR->scrollOffset)) + (i * g_scrollbar_EDITOR->margin), 0))
      {
        // printf("\n%d---------------------------", p->difficulty);
        g_Current_Level = p;
        g_BrickLastSelected = nullptr;
      }
    }
    // DELETE LEVEL
    if (esat::MouseButtonDown(1))
    {
      if (checkMouseClick(810, 970, (13 + g_scrollbar_EDITOR->boxheight * i + (g_scrollbar_EDITOR->scrollOffset)) + (i * g_scrollbar_EDITOR->margin), (173 + g_scrollbar_EDITOR->boxheight * i + (g_scrollbar_EDITOR->scrollOffset)) + (i * g_scrollbar_EDITOR->margin), 1))
      {
        if (g_Current_Level == p)
          g_Current_Level = nullptr;

        deleteLevel(p);
        g_scrollbar_EDITOR->quantity = countNumberOfLevels();
        g_BrickLastSelected = nullptr;
      }
    }
    p = p->next;
  }

  PaintSquare({900, 5}, {200, 10}, kDefaultBackground, kDefaultBackground);
  PaintSquare({900, 985}, {200, 100}, kDefaultBackground, kDefaultBackground);

  PaintSquare({500, 475}, {990, 940}, {(char)255, (char)255, (char)255, (char)255}, {(char)255, (char)255, (char)255, (char)10});
  ScrollbarController(g_scrollbar_EDITOR);

  PaintSquare({410, 410}, {800, 800}, {(char)255, (char)255, (char)255, (char)255}, kDefaultBackground);

  if (g_Current_Level != nullptr)
  {
    zoro::Vec2 GlobalBoundingBox_min = {10, 10};
    zoro::Vec2 GlobalBoundingBox_max = {810, 810};
    if (esat::MouseButtonUp(0))
    {
      g_mouseIsDown = false;
      g_Brick_selected = nullptr;
    }
    if (g_Brick_selected != nullptr)
    {

      zoro::Vec2 mouse = zoro::CoordGlobalToLocalVec2({(float)esat::MousePositionX(), (float)esat::MousePositionY()},
                                                      GlobalBoundingBox_min,
                                                      GlobalBoundingBox_max);

      mouse = zoro::CoordLocalToGlobalVec2(mouse,
                                           {0, 0}, {kwidth, kheight});

      if (!brickColisionSpawnCheck(mouse))
      {
        g_Brick_selected->pos = mouse;
      }

      if (g_Brick_selected->pos.x - (g_Brick_selected->size.x / 2) < 0)
      {
        g_Brick_selected->pos.x = (g_Brick_selected->size.x / 2);
      }
      if (g_Brick_selected->pos.x + (g_Brick_selected->size.x / 2) > kwidth)
      {
        g_Brick_selected->pos.x = kwidth - (g_Brick_selected->size.x / 2);
      }

      if (g_Brick_selected->pos.y - (g_Brick_selected->size.y / 2) < 0)
      {
        g_Brick_selected->pos.y = (g_Brick_selected->size.y / 2);
      }
      if (g_Brick_selected->pos.y + (g_Brick_selected->size.y / 2) > kheight)
      {
        g_Brick_selected->pos.y = kheight - (g_Brick_selected->size.y / 2);
      }
    }

    if (g_BrickLastSelected != nullptr)
    {
      if (esat::IsKeyDown('D') && (g_BrickLastSelected->pos.x + (g_BrickLastSelected->size.x * 1.5) < kwidth) && !brickColisionSpawnCheck({g_BrickLastSelected->pos.x + g_BrickLastSelected->size.x, g_BrickLastSelected->pos.y}))
      {
        g_BrickLastSelected = addBrick(&g_Current_Level->bricks, {g_BrickLastSelected->pos.x + g_BrickLastSelected->size.x, g_BrickLastSelected->pos.y});
      }
      if (esat::IsKeyDown('A') && (g_BrickLastSelected->pos.x - (g_BrickLastSelected->size.x * 1.5) > 0) && !brickColisionSpawnCheck({g_BrickLastSelected->pos.x - g_BrickLastSelected->size.x, g_BrickLastSelected->pos.y}))
      {
        g_BrickLastSelected = addBrick(&g_Current_Level->bricks, {g_BrickLastSelected->pos.x - g_BrickLastSelected->size.x, g_BrickLastSelected->pos.y});
      }
      if (esat::IsKeyDown('S') && (g_BrickLastSelected->pos.y + (g_BrickLastSelected->size.y * 1.5) < kheight) && !brickColisionSpawnCheck({g_BrickLastSelected->pos.x, g_BrickLastSelected->pos.y + g_BrickLastSelected->size.y}))
      {
        g_BrickLastSelected = addBrick(&g_Current_Level->bricks, {g_BrickLastSelected->pos.x, g_BrickLastSelected->pos.y + g_BrickLastSelected->size.y});
      }
      if (esat::IsKeyDown('W') && (g_BrickLastSelected->pos.y - (g_BrickLastSelected->size.y * 1.5) > 0) && !brickColisionSpawnCheck({g_BrickLastSelected->pos.x, g_BrickLastSelected->pos.y - g_BrickLastSelected->size.y}))
      {
        g_BrickLastSelected = addBrick(&g_Current_Level->bricks, {g_BrickLastSelected->pos.x, g_BrickLastSelected->pos.y - g_BrickLastSelected->size.y});
      }
    }

    PaintLevel(g_Current_Level, {410, 410}, {800, 800});
    if (esat::MouseButtonDown(0))
    {
      emptyClick = true;
    }
    if (g_Current_Level->bricks != nullptr)
    {
      zoro::Vec2 brickPosGlobalPos;
      zoro::Vec2 brickPosGlobalSize;
      TBrick *p = g_Current_Level->bricks;

      while (p != nullptr)
      {

        if (checkMouseClickOnLocalCoordinates(p->pos, p->size, 0, {410, 410}, {800, 800}))
        {
          emptyClick = false;
          printf("selected");
          g_Brick_selected = p;
          g_BrickLastSelected = p;
          g_mouseIsDown = true;
          g_mouseDownPos = zoro::CoordGlobalToLocalVec2({(float)esat::MousePositionX(), (float)esat::MousePositionY()}, GlobalBoundingBox_min, GlobalBoundingBox_max);
        }
        if (checkMouseClickOnLocalCoordinates(p->pos, p->size, 1, {410, 410}, {800, 800}))
        {
          emptyClick = false;
          DeleteBrick(p);
        }
        p = p->next;
      }
    }

    if (g_Current_Level->bricks != nullptr)
    {
      if (emptyClick && checkMouseClick(GlobalBoundingBox_min.x + (g_Current_Level->bricks->size.x / 2), GlobalBoundingBox_max.x - (g_Current_Level->bricks->size.x / 2), GlobalBoundingBox_min.y + (g_Current_Level->bricks->size.y / 2), GlobalBoundingBox_max.y - (g_Current_Level->bricks->size.y / 2), -1))
      {

        zoro::Vec2 mousePosLocal = zoro::CoordGlobalToLocalVec2(zoro::CoordLocalToGlobalVec2({(float)esat::MousePositionX(), (float)esat::MousePositionY()}, {0, 0}, {kwidth, kheight}), GlobalBoundingBox_min, GlobalBoundingBox_max);

        g_BrickLastSelected = g_Current_Level->bricks;

        if (!brickColisionSpawnCheck(mousePosLocal))
        {
          g_BrickLastSelected = addBrick(&g_Current_Level->bricks, mousePosLocal);
        }
        g_BrickLastSelected = nullptr;
      }
    }
    else
    {
      if (checkMouseClick(GlobalBoundingBox_min.x, GlobalBoundingBox_max.x, GlobalBoundingBox_min.y, GlobalBoundingBox_max.y, 0))
      {
        zoro::Vec2 mousePosLocal = zoro::CoordGlobalToLocalVec2(zoro::CoordLocalToGlobalVec2({(float)esat::MousePositionX(), (float)esat::MousePositionY()}, {0, 0}, {kwidth, kheight}), GlobalBoundingBox_min, GlobalBoundingBox_max);
        g_BrickLastSelected = addBrick(&g_Current_Level->bricks, mousePosLocal);
      }
    }
  }
}
void MenuWindow()
{
  esat::DrawSetFillColor(150, 150, 255, 100);
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetTextSize(143);
  esat::DrawText(215, 353, "BREAKOUT");
  esat::DrawSetFillColor(255, 255, 255, 255);
  esat::DrawSetTextSize(140);
  esat::DrawText(220, 350, "BREAKOUT");
  esat::DrawSetTextSize(30);
  esat::DrawText(380, 950, "Guillermo Bosca");
}

void PaintGameUI()
{
  char *TextBuffer = (char *)malloc(30 * sizeof(char));
  esat::DrawSetFillColor(255, 255, 255, 255);
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetTextSize(35);
  esat::DrawText(15, 35, "SCORE");
  itoa(g_Current_Level->player.score, TextBuffer, 10);
  esat::DrawText(130, 35, TextBuffer);

  esat::DrawText(860, 35, "LIVES");
  itoa(g_Current_Level->player.lives, TextBuffer, 10);
  esat::DrawText(965, 35, TextBuffer);
}

bool activeBricks(TBrick *bricks)
{
  TBrick *b = bricks;
  if (bricks == nullptr)
    return false;

  bool activeFound = false;

  while (b != nullptr && !activeFound)
  {
    if (b->active)
      activeFound = true;

    b = b->next;
  }
  return activeFound;
}

void GameWindow()
{
  if (!activeBricks(g_Current_Level->bricks))
  {
    int baseScore = g_Current_Level->player.score;
    if (g_Current_Level->next != nullptr)
    {
      g_Current_Level = g_Current_Level->next;
    }
    else
    {
      if (g_Current_Level == g_Level_List)
      {
        g_Window_State = MENU;
      }
      else
      {
        g_Current_Level = g_Level_List;
      }
    }
    startGame(baseScore);
    return;
  }

  if (g_Current_Level->player.lives < 0)
    g_Window_State = MENU;

  if (esat::IsKeyDown('B'))
    g_Window_State = MENU;

  PaintLevel(g_Current_Level, {500, 500}, {1000, 1000});

  PaintPlayer();
  PaintGameUI();
}
void LevelSelectorWindow()
{
  PaintSquare({500, 475}, {990, 940}, {(char)255, (char)255, (char)255, (char)255}, {(char)255, (char)255, (char)255, (char)10});

  TLevel *p = g_Level_List;
  int column = 0;
  int row = 0;
  zoro::Vec2 boxPos, boxSize;

  for (int i = 0; i < g_scrollbar_LEVELSELECTOR->quantity; i++)
  {
    boxPos = {105 + (column * g_scrollbar_LEVELSELECTOR->boxheight) + (column * g_scrollbar_LEVELSELECTOR->margin), 105 + g_scrollbar_LEVELSELECTOR->boxheight * row + (g_scrollbar_LEVELSELECTOR->scrollOffset) + (row * g_scrollbar_LEVELSELECTOR->margin)};
    boxSize = {g_scrollbar_LEVELSELECTOR->boxheight, g_scrollbar_LEVELSELECTOR->boxheight};

    PaintSquare(boxPos, boxSize,
                {(char)255, (char)255, (char)255, (char)255},
                {(char)255, (char)255, (char)255, (char)10});

    PaintLevel(p, boxPos, boxSize);

    // SELECT LEVEL
    if (esat::MouseButtonDown(0))
    {
      if (checkMouseClick(boxPos.x - (boxSize.x / 2),
                          boxPos.x + (boxSize.x / 2),
                          boxPos.y - (boxSize.y / 2),
                          boxPos.y + (boxSize.y / 2),
                          0))
      {
        g_Current_Level = p;
        // Start game
        // ...
        printf("start: %d", g_Current_Level->difficulty);
        startGame(0);
      }
    }

    column++;
    if (column >= g_scrollbar_LEVELSELECTOR->columns)
    {
      column = 0;
      row++;
    }

    p = p->next;
  }
  ScrollbarController(g_scrollbar_LEVELSELECTOR);
  PaintSquare({500, 1000}, {1000, 110}, kDefaultBackground, kDefaultBackground);
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
  new_button.pos = {130, 975};
  new_button.windowContext = EDITOR;
  new_button.dimensions = {250, 40};
  addButtonToList(new_button);

  new_button.text = "BACK TO MENU";
  new_button.idFunction = 3;
  new_button.pos = {870, 975};
  new_button.windowContext = LEVELSELECTOR;
  new_button.dimensions = {250, 40};
  addButtonToList(new_button);
  new_button.windowContext = EDITOR;
  addButtonToList(new_button);

  // Scrollbar
  g_scrollbar_EDITOR = (TScrollbar *)malloc(1 * sizeof(TScrollbar));

  g_scrollbar_EDITOR->boxheight = 160;
  g_scrollbar_EDITOR->max_height = 935;
  g_scrollbar_EDITOR->min_height = 12;
  g_scrollbar_EDITOR->quantity = 0;
  g_scrollbar_EDITOR->scrollOffset = 0;
  g_scrollbar_EDITOR->width = 13;
  g_scrollbar_EDITOR->xpos = 975;
  g_scrollbar_EDITOR->margin = 3;
  g_scrollbar_EDITOR->columns = 1;

  g_scrollbar_LEVELSELECTOR = (TScrollbar *)malloc(1 * sizeof(TScrollbar));

  g_scrollbar_LEVELSELECTOR->boxheight = 180;
  g_scrollbar_LEVELSELECTOR->max_height = 935;
  g_scrollbar_LEVELSELECTOR->min_height = 12;
  g_scrollbar_LEVELSELECTOR->quantity = 0;
  g_scrollbar_LEVELSELECTOR->scrollOffset = 0;
  g_scrollbar_LEVELSELECTOR->width = 20;
  g_scrollbar_LEVELSELECTOR->xpos = 965;
  g_scrollbar_LEVELSELECTOR->margin = 10;
  g_scrollbar_LEVELSELECTOR->columns = 5;
}

void init_Game()
{
  g_Square_Points = (zoro::Vec2 *)malloc(4 * sizeof(zoro::Vec2));

  kDefault_Player.ball = nullptr;
  kDefault_Player.color = {(char)255, (char)255, (char)255, (char)255};
  kDefault_Player.lives = 1;
  kDefault_Player.pos = {500, 920};
  kDefault_Player.score = 0;
  kDefault_Player.size = {150, 25};
  kDefault_Player.dir = {1, -1};

  init_UI();
}

int esat::main(int argc, char **argv)
{
  srand(time(NULL));

  esat::WindowInit(kwidth, kheight);
  WindowSetMouseVisibility(true);

  init_Game();
  printf("PROGRAM START :3");

  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {

    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(kDefaultBackground.r, kDefaultBackground.g, kDefaultBackground.b);

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