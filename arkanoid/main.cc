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
  kMenu = 0,
  kEditor,
  kGame,
  kLevelSelector
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

WindowState g_window_state = kMenu;

TButton *g_interface_list = nullptr;

TLevel *g_level_list = nullptr;
TLevel *g_current_level = nullptr;

bool g_mouse_is_down = false;
bool g_scrollbar_is_selected = false;

zoro::Vec2 g_mouse_down_pos;
TBrick *g_brick_selected = nullptr;
TBrick *g_brick_last_selected = nullptr;

zoro::Vec2 *g_square_points = nullptr;

TScrollbar *g_scrollbar_editor;
TScrollbar *g_scrollbar_level_selector;

float g_menu_cooldown_max_time = 300.0f;
double g_menu_cooldown_time = 0.0f;

TPlayer default_player;
TBrick default_brick;

float g_window_width = 1000.0f;
float g_window_height = 1000.0f;

TColor kDefaultBackground = {(char)10, (char)30, (char)40, (char)255};

FILE *g_file;

unsigned char fps = 60; // Frames per second
double current_time, last_time;

int CountNumberOfLevels()
{
  TLevel *p = g_level_list;
  int i = 0;
  while (p != nullptr)
  {
    i++;
    p = p->next;
  }

  return i;
}
int CountNumberOfBricks(TLevel *l)
{
  if (l == nullptr)
    return 0;

  TBrick *b = l->bricks;

  if (b == nullptr)
    return 0;

  int count = 0;

  while (b != nullptr)
  {
    count++;
    b = b->next;
  }

  return count;
}

bool CheckMouseClick(float minx, float maxx, float miny, float maxy, int mouseType)
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
bool CheckMouseClickOnLocalCoordinates(zoro::Vec2 TargetPos,
                                       zoro::Vec2 TargetSize, int mouseType,
                                       zoro::Vec2 LocalBoxPos, zoro::Vec2 LocalBoxSize)
{

  zoro::Vec2 min = zoro::CoordGlobalToLocalVec2(
      {TargetPos.x - (TargetSize.x / 2), TargetPos.y - (TargetSize.y / 2)},
      {0, 0}, {g_window_width, g_window_height});

  min = zoro::CoordLocalToGlobalVec2(min,
                                     {LocalBoxPos.x - (LocalBoxSize.x / 2), LocalBoxPos.y - (LocalBoxSize.y / 2)},
                                     {LocalBoxPos.x + (LocalBoxSize.x / 2), LocalBoxPos.y + (LocalBoxSize.y / 2)});

  zoro::Vec2 max = zoro::CoordGlobalToLocalVec2({TargetPos.x + (TargetSize.x / 2), TargetPos.y + (TargetSize.y / 2)},
                                                {0, 0}, {g_window_width, g_window_height});
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

// Returns true if a brick is active on the brick list
bool ActiveBricks(TBrick *bricks)
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

TLevel *NewEmptyLevel()
{
  TLevel *p = (TLevel *)malloc(sizeof(TLevel));
  p->difficulty = CountNumberOfLevels();
  p->bricks = nullptr;
  p->player = default_player;
  p->player.lives = 3;
  p->player.ball = nullptr;
  p->background = kDefaultBackground;
  p->next = g_level_list;
  g_level_list = p;
  g_scrollbar_editor->quantity = CountNumberOfLevels();
  printf("New level %d\n", g_scrollbar_editor->quantity);
  return p;
}
void DeleteBrick(TBrick *brick)
{
  TBrick *p = g_current_level->bricks;
  TBrick *temp;

  if (p == brick)
  {
    g_current_level->bricks = g_current_level->bricks->next;
    return;
  }

  while (p->next != brick)
    p = p->next;

  temp = p->next;
  p->next = p->next->next;
  free(temp);
}
void DeleteLevel(TLevel *level)
{
  TLevel *p = g_level_list;
  TLevel *temp;

  if (p == level)
  {
    g_level_list = g_level_list->next;
    return;
  }

  while (p->next != level)
    p = p->next;

  temp = p->next;
  p->next = p->next->next;
  printf("Deleting bricks.\n");
  while (temp->bricks != nullptr)
    DeleteBrick(temp->bricks);
  printf("Bricks deleted.");
  free(temp);
}

bool CheckHitBox(zoro::Vec2 box1pos, zoro::Vec2 box1size, zoro::Vec2 box2pos, zoro::Vec2 box2size)
{
  if (box1pos.x - (box1size.x / 2) < box2pos.x + (box2size.x / 2) &&
      box1pos.x + (box1size.x / 2) > box2pos.x - (box2size.x / 2) &&
      box1pos.y - (box1size.y / 2) < box2pos.y + (box2size.y / 2) &&
      box1pos.y + (box1size.y / 2) > box2pos.y - (box2size.y / 2))
    return true;

  return false;
}

bool BrickColisionSpawnCheck(zoro::Vec2 pos)
{
  TBrick *b = g_current_level->bricks;
  while (b != nullptr)
  {
    if (b != g_brick_last_selected)
    {
      if (CheckHitBox(b->pos, b->size, pos, g_brick_last_selected->size))
        return true;
    }
    b = b->next;
  }
  return false;
}

TButton *AddButtonToList(TButton b)
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
  newbutton->next = g_interface_list;
  g_interface_list = newbutton;
  return newbutton;
}
TBrick *AddBrick(TBrick **brickList, zoro::Vec2 pos)
{
  TBrick *nBrick = (TBrick *)malloc(sizeof(TBrick));

  nBrick->active = true;
  nBrick->color = default_brick.color;
  nBrick->lives = default_brick.lives;
  nBrick->pos = pos;
  nBrick->reward = default_brick.reward;
  nBrick->size = default_brick.size;

  nBrick->next = *brickList;
  *brickList = nBrick;

  printf("New brick\n");
  return nBrick;
}
TBrick *AddBrick(TBrick **brickList, TBrick *brick_reference)
{
  TBrick *nBrick = (TBrick *)malloc(sizeof(TBrick));

  nBrick->active = true;
  nBrick->color = brick_reference->color;
  nBrick->lives = brick_reference->lives;
  nBrick->pos = brick_reference->pos;
  nBrick->reward = brick_reference->reward;
  nBrick->size = brick_reference->size;

  nBrick->next = *brickList;
  *brickList = nBrick;

  return nBrick;
}
void InitBall(TBall **ball)
{
  TBall *nBall = (TBall *)malloc(sizeof(TBall));
  nBall->knPoints = 16;
  nBall->next = nullptr;
  nBall->onPlayer = true;
  nBall->pos = {g_window_width / 2, g_window_height / 2};
  nBall->size = 20.0f;
  nBall->speed = 10.0f;

  for (int i = 0; i < nBall->knPoints; i++)
    nBall->g_points[i] = {cosf((6.28f / nBall->knPoints) * i),
                          sinf(((6.28f / nBall->knPoints) * i)),
                          1.0f};

  nBall->next = *ball;
  *ball = nBall;
}

void GoToEditor()
{
  g_window_state = kEditor;
  g_scrollbar_editor->quantity = CountNumberOfLevels();
}

void SaveLevels()
{
  TLevel *level = g_level_list;

  if (fopen("./resources/save.zoro", "r+b") == NULL)
  {
    g_file = fopen("./resources/save.zoro", "w+b");
    fclose(g_file);
  }

  g_file = fopen("./resources/save.zoro", "w+b");

  int number_of_levels;
  TBrick *b;
  while (level != nullptr)
  {
    number_of_levels = CountNumberOfBricks(level);
    fwrite(&number_of_levels, sizeof(int), 1, g_file);
    b = level->bricks;
    for (int i = 0; i < number_of_levels; i++)
    {
      fwrite(b, sizeof(TBrick), 1, g_file);
      b = b->next;
    }
    fwrite(&level->player, sizeof(TPlayer), 1, g_file);
    fwrite(&level->background, sizeof(TColor), 1, g_file);
    fwrite(&level->difficulty, sizeof(int), 1, g_file);
    level = level->next;

    printf("Level saved\n");
  }
}

void LoadLevels()
{
  printf("Loading...\n");
  if (fopen("./resources/save.zoro", "r+b") == NULL)
    return;

  g_file = fopen("./resources/save.zoro", "r+b");

  TLevel *new_level = (TLevel *)malloc(sizeof(TLevel));
  TBrick *new_brick = (TBrick *)malloc(sizeof(TBrick));

  new_brick->active = true;
  int number_of_levels;

  while (fread(&number_of_levels, sizeof(int), 1, g_file) == 1)
  {
    TLevel *l = NewEmptyLevel();
    for (int i = 0; i < number_of_levels; i++)
    {
      fread(new_brick, sizeof(TBrick), 1, g_file);
      AddBrick(&l->bricks, new_brick);
    }
    fread(&l->player, sizeof(TPlayer), 1, g_file);
    fread(&l->background, sizeof(TColor), 1, g_file);
    fread(&l->difficulty, sizeof(int), 1, g_file);
    printf("Level loaded!\n");
  }
  free(new_level);
  free(new_brick);
}

zoro::Vec2 ColisionBallSquare(TBall *ball, zoro::Vec2 pos, zoro::Vec2 size)
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

  // Debug stuff
  // esat::DrawSetStrokeColor(255, 0, 0, 255);
  // esat::DrawLine(VDistance.x + pos.x, VDistance.y + pos.y, pos.x, pos.y);

  if (zoro::MagnitudeVec2(VDistance) + ball->size / 2 > zoro::MagnitudeVec2(zoro::SubtractVec2(ball->pos, pos)))
  {

    if (zoro::DotVec2(zoro::NormalizeVec2(zoro::ScaleVec2(VDistance, -1)), zoro::NormalizeVec2(ball->dir)) > 0.5f)
    {
      if (!CheckHitBox(zoro::SumVec2(zoro::SumVec2(pos, VDistance), zoro::NormalizeVec2(zoro::LeftPerpendicularVec2(VDistance))), {1, 1}, pos, size))
      {
        return zoro::NormalizeVec2(zoro::LeftPerpendicularVec2(VDistance));
      }
      else
      {
        return zoro::NormalizeVec2(zoro::RightPerpendicularVec2(VDistance));
      }
    }
    else
      return zoro::NormalizeVec2(VDistance);
  }
  return {99.0f, 99.0f};
}

void paintBall(TBall *ball)
{
  ball->M = zoro::MatIdentity3();
  ball->M = zoro::Mat3Multiply(zoro::Mat3Translate(ball->pos), ball->M);
  ball->M = zoro::Mat3Multiply(zoro::Mat3Scale(ball->size, ball->size), ball->M);

  for (int i = 0; i < ball->knPoints; i++)
  {
    zoro::Vec3 tmp = zoro::Mat3TransformVec3(ball->M, ball->g_points[i]);
    ball->dr_points[i] = {tmp.x, tmp.y};
  }

  esat::DrawSetStrokeColor(210, 210, 210, 50);
  esat::DrawSetFillColor(180, 180, 220, 255);
  esat::DrawSolidPath(&ball->dr_points[0].x, ball->knPoints);
}
void Ball()
{
  TBall *balls = g_current_level->player.ball;
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
      p->pos = {g_current_level->player.pos.x,
                g_current_level->player.pos.y - g_current_level->player.ball->size - (g_current_level->player.size.y / 2)};
      p->dir = zoro::NormalizeVec2(g_current_level->player.dir);
      if (esat::IsKeyDown('X'))
        p->onPlayer = false;
    }
    else
    {
      if (g_current_level->bricks != nullptr && !p->onPlayer)
      {
        zoro::Vec2 VDistance;
        zoro::Vec2 newDir;
        TBrick *br = g_current_level->bricks;
        while (br != nullptr)
        {
          if (br->active)
          {
            newDir = ColisionBallSquare(p, br->pos, br->size);
            if (zoro::MagnitudeVec2(newDir) < 2.0f)
            {
              p->dir = newDir;
              g_current_level->player.score += br->reward;
              br->active = false;
            }
          }

          br = br->next;
        }

        newDir = ColisionBallSquare(p,
                                    g_current_level->player.pos, g_current_level->player.size);

        if (zoro::MagnitudeVec2(newDir) < 2.0f && p->pos.y < g_current_level->player.pos.y)
        {
          p->dir.y *= -1;
          p->dir.x += g_current_level->player.acceleration * 0.6f;
          p->dir = zoro::NormalizeVec2(p->dir);
          p->pos.y = g_current_level->player.pos.y - (g_current_level->player.size.y / 2) - p->size - 5;

          if (p->dir.y <= 0.0f && p->dir.y > -0.2f)
            p->dir.y = -0.5f;
        }
      }

      p->pos = zoro::SumVec2(p->pos, zoro::ScaleVec2(p->dir, p->speed));

      if (p->pos.x + p->size > g_window_width)
      {
        p->dir.x *= -1;
        p->pos.x = g_window_width - p->size - 1;
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

      if (p->pos.y + p->size > g_window_height)
      {
        p->onPlayer = true;
        g_current_level->player.lives--;
      }
    }

    paintBall(p);

    p = p->next;
  }
}

void PaintSquareToBox(zoro::Vec2 pos, zoro::Vec2 size, TColor border, TColor background, zoro::Vec2 BoxPos, zoro::Vec2 BoxSize)
{

  zoro::Vec2 GlobalBoundingBox_min = {BoxPos.x - (BoxSize.x / 2), BoxPos.y - (BoxSize.y / 2)};
  zoro::Vec2 GlobalBoundingBox_max = {BoxPos.x + (BoxSize.x / 2), BoxPos.y + (BoxSize.y / 2)};

  *(g_square_points + 0) = pos;
  (*(g_square_points + 0)).x -= size.x / 2;
  (*(g_square_points + 0)).y -= size.y / 2;

  *(g_square_points + 0) = zoro::CoordGlobalToLocalVec2(*(g_square_points + 0), {0, 0}, {g_window_width, g_window_height});
  *(g_square_points + 0) = zoro::CoordLocalToGlobalVec2(*(g_square_points + 0), {GlobalBoundingBox_min}, GlobalBoundingBox_max);

  *(g_square_points + 1) = pos;
  (*(g_square_points + 1)).x += size.x / 2;
  (*(g_square_points + 1)).y -= size.y / 2;

  *(g_square_points + 1) = zoro::CoordGlobalToLocalVec2(*(g_square_points + 1), {0, 0}, {g_window_width, g_window_height});
  *(g_square_points + 1) = zoro::CoordLocalToGlobalVec2(*(g_square_points + 1), GlobalBoundingBox_min, GlobalBoundingBox_max);

  *(g_square_points + 2) = pos;
  (*(g_square_points + 2)).x += size.x / 2;
  (*(g_square_points + 2)).y += size.y / 2;

  *(g_square_points + 2) = zoro::CoordGlobalToLocalVec2(*(g_square_points + 2), {0, 0}, {g_window_width, g_window_height});
  *(g_square_points + 2) = zoro::CoordLocalToGlobalVec2(*(g_square_points + 2), GlobalBoundingBox_min, GlobalBoundingBox_max);

  *(g_square_points + 3) = pos;
  (*(g_square_points + 3)).x -= size.x / 2;
  (*(g_square_points + 3)).y += size.y / 2;

  *(g_square_points + 3) = zoro::CoordGlobalToLocalVec2(*(g_square_points + 3), {0, 0}, {g_window_width, g_window_height});
  *(g_square_points + 3) = zoro::CoordLocalToGlobalVec2(*(g_square_points + 3), GlobalBoundingBox_min, GlobalBoundingBox_max);

  esat::DrawSetFillColor(background.r, background.g, background.b, background.o);
  esat::DrawSetStrokeColor(border.r, border.g, border.b, border.o);
  esat::DrawSolidPath(&g_square_points[0].x, 4);
}
void PaintSquare(zoro::Vec2 pos, zoro::Vec2 size, TColor border, TColor background)
{
  *(g_square_points + 0) = pos;
  (*(g_square_points + 0)).x -= size.x / 2;
  (*(g_square_points + 0)).y -= size.y / 2;

  *(g_square_points + 1) = pos;
  (*(g_square_points + 1)).x += size.x / 2;
  (*(g_square_points + 1)).y -= size.y / 2;

  *(g_square_points + 2) = pos;
  (*(g_square_points + 2)).x += size.x / 2;
  (*(g_square_points + 2)).y += size.y / 2;

  *(g_square_points + 3) = pos;
  (*(g_square_points + 3)).x -= size.x / 2;
  (*(g_square_points + 3)).y += size.y / 2;

  esat::DrawSetFillColor(background.r, background.g, background.b, background.o);
  esat::DrawSetStrokeColor(border.r, border.g, border.b, border.o);
  esat::DrawSolidPath(&g_square_points[0].x, 4);
}
void PaintPlayer()
{

  g_current_level->player.acceleration = 0.0f;
  if (esat::IsKeyPressed('A') || esat::IsSpecialKeyPressed(esat::kSpecialKey_Left))
  {
    g_current_level->player.acceleration = -1.0f;
    g_current_level->player.dir.x = -1.0f;
  }

  if (esat::IsKeyPressed('D') || esat::IsSpecialKeyPressed(esat::kSpecialKey_Right))
  {
    g_current_level->player.dir.x = 1.0f;
    g_current_level->player.acceleration = +1.0f;
  }

  g_current_level->player.speed = g_current_level->player.acceleration + (g_current_level->player.speed * 0.93);
  g_current_level->player.pos.x += g_current_level->player.speed;

  if (g_current_level->player.pos.x + (g_current_level->player.size.x / 2) > g_window_width)
    g_current_level->player.pos.x = -2 + g_window_width - (g_current_level->player.size.x / 2);
  if (g_current_level->player.pos.x - (g_current_level->player.size.x / 2) < 0)
    g_current_level->player.pos.x = (g_current_level->player.size.x / 2) + 2;

  Ball();

  PaintSquare(g_current_level->player.pos, g_current_level->player.size, g_current_level->player.color, g_current_level->player.color);
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
      if (b == g_brick_selected)
      {
        PaintSquareToBox(b->pos, b->size, {(char)255, (char)0, (char)0, (char)255}, b->color, origin, size);
      }
      else if (b == g_brick_last_selected)
      {
        PaintSquareToBox(b->pos, b->size, {(char)0, (char)0, (char)255, (char)255}, b->color, origin, size);
      }
      else
      {
        if (g_window_state == kGame)
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

void CallButtonFunction(int id)
{
  switch (id)
  {
  case 0:
    // Go to LEVEL kEditor
    GoToEditor();

    break;
  case 1:
    // Go to LEVEL SELECTOR
    g_window_state = kLevelSelector;
    g_scrollbar_level_selector->quantity = CountNumberOfLevels();
    break;
  case 2:
    // Go to kGame
    g_window_state = kGame;
    break;
  case 3:
    // Go to kMenu
    g_window_state = kMenu;
    g_brick_selected = nullptr;
    g_brick_last_selected = nullptr;
    SaveLevels();
    break;
  case 4:
    // Add new empty level
    NewEmptyLevel();
    break;
  default:
    printf("ID button not found\n");
    break;
  }
}
void PaintButton(TButton *p)
{

  zoro::Vec2 finalTextPos;

  *(g_square_points + 0) = p->pos;
  (*(g_square_points + 0)).x -= p->dimensions.x / 2;
  (*(g_square_points + 0)).y -= p->dimensions.y / 2;

  *(g_square_points + 1) = p->pos;
  (*(g_square_points + 1)).x += p->dimensions.x / 2;
  (*(g_square_points + 1)).y -= p->dimensions.y / 2;

  *(g_square_points + 2) = p->pos;
  (*(g_square_points + 2)).x += p->dimensions.x / 2;
  (*(g_square_points + 2)).y += p->dimensions.y / 2;

  *(g_square_points + 3) = p->pos;
  (*(g_square_points + 3)).x -= p->dimensions.x / 2;
  (*(g_square_points + 3)).y += p->dimensions.y / 2;

  if (CheckMouseClick((*(g_square_points + 0)).x, (*(g_square_points + 1)).x, (*(g_square_points + 0)).y, (*(g_square_points + 2)).y, -1))
  {
    if (esat::MouseButtonUp(0))
    {
      esat::ResetBufferdKeyInput();
      if (esat::Time() - g_menu_cooldown_time > g_menu_cooldown_max_time)
      {
        CallButtonFunction(p->idFunction);
        g_menu_cooldown_time = esat::Time();
      }
    }

    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetStrokeColor(255, 255, 255, 255);
    esat::DrawSolidPath(&g_square_points[0].x, 4);

    esat::DrawSetFillColor(0, 0, 0, 255);
  }
  esat::DrawSetFillColor(255, 255, 255, 10);
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSolidPath(&g_square_points[0].x, 4);

  esat::DrawSetFillColor(255, 255, 255, 255);

  if (p->text != nullptr)
  {
    esat::DrawSetTextSize(p->fontSize);

    finalTextPos.y = p->pos.y + (p->dimensions.y / 2); // 1 fontsize = 0.75pixels
    finalTextPos.y -= (p->dimensions.y - p->fontSize * 0.65) / 2;

    finalTextPos.x = p->pos.x - (p->dimensions.x / 2);
    finalTextPos.x += (p->dimensions.x - strlen(p->text) * ((p->fontSize / 2))) / 2; // widthLetter = (fontsize / 2) + 4

    if (CheckMouseClick((*(g_square_points + 0)).x, (*(g_square_points + 1)).x, (*(g_square_points + 0)).y, (*(g_square_points + 2)).y, -1))
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
  TButton *p = g_interface_list;
  while (p != nullptr)
  {
    if (g_window_state == p->windowContext)
    {
      PaintButton(p);
    }
    p = p->next;
  }
}

void StartGame(int baseScore)
{
  if (g_current_level == nullptr)
  {
    printf("Level not found!\n");
    return;
  }

  if (g_current_level->bricks == nullptr)
  {
    printf("Level empty!\n");
    return;
  }
  g_brick_selected = nullptr;
  g_brick_last_selected = nullptr;
  TBrick *br = g_current_level->bricks;

  while (br != nullptr)
  {
    br->active = true;
    br = br->next;
  }

  g_window_state = kGame;
  g_current_level->player = default_player;
  g_current_level->player.score = baseScore;
  printf("INIT BALL\n");
  InitBall(&g_current_level->player.ball);
  printf("Game Started\n");
}
void PaintGameUI()
{
  char *TextBuffer = (char *)malloc(30 * sizeof(char));
  esat::DrawSetFillColor(255, 255, 255, 255);
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetTextSize(35);
  esat::DrawText(15, 35, "SCORE");
  itoa(g_current_level->player.score, TextBuffer, 10);
  esat::DrawText(130, 35, TextBuffer);

  esat::DrawText(860, 35, "LIVES");
  itoa(g_current_level->player.lives, TextBuffer, 10);
  esat::DrawText(965, 35, TextBuffer);
}
void ScrollbarController(TScrollbar *scrollbar)
{
  float scrollRange = scrollbar->max_height - scrollbar->min_height;
  float contentSize = ((scrollbar->quantity / scrollbar->columns) * (scrollbar->boxheight + scrollbar->margin));

  float scrollerHeight = scrollRange * (scrollRange / contentSize);

  if (esat::MousePositionX() < scrollbar->xpos + scrollbar->width && esat::MousePositionX() > scrollbar->xpos && esat::MousePositionY() > scrollbar->min_height && esat::MousePositionY() < scrollbar->max_height && esat::MouseButtonDown(0))
  {
    g_mouse_is_down = true;
    g_scrollbar_is_selected = true;
    g_mouse_down_pos = {(float)esat::MousePositionX(), (float)esat::MousePositionY()};
  }

  if (esat::MouseButtonUp(0))
  {
    if (g_mouse_is_down)
      g_scrollbar_is_selected = false;

    g_mouse_is_down = false;
  }

  if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Down))
  {
    scrollbar->scrollOffset -= 10 + (scrollbar->quantity * 2);
  }
  else if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Up))
  {
    scrollbar->scrollOffset += 10 + (scrollbar->quantity * 2);
  }

  if (g_mouse_is_down && g_scrollbar_is_selected)
  {
    scrollbar->scrollOffset += g_mouse_down_pos.y - esat::MousePositionY();
    g_mouse_down_pos.y = esat::MousePositionY();
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

  *(g_square_points + 0) = {scrollbar->xpos, scrollbar->min_height - scrollerOffsethandle};
  *(g_square_points + 1) = {scrollbar->xpos + scrollbar->width, scrollbar->min_height - scrollerOffsethandle};
  *(g_square_points + 2) = {scrollbar->xpos + scrollbar->width, scrollbar->min_height + scrollerHeight - scrollerOffsethandle};
  *(g_square_points + 3) = {scrollbar->xpos, scrollbar->min_height + scrollerHeight - scrollerOffsethandle};

  if (scrollRange < scrollbar->quantity * scrollbar->boxheight)
  {
    esat::DrawSetStrokeColor(250, 250, 250, 255);
    esat::DrawSetFillColor(250, 250, 250, 180);
    if (g_mouse_is_down && g_scrollbar_is_selected)
      esat::DrawSetFillColor(210, 210, 250, 180);
    esat::DrawSolidPath(&g_square_points[0].x, 4);
  }
}
void EditorWindow()
{

  // Scrollbar
  TLevel *p = g_level_list;
  bool emptyClick = false;
  for (int i = 0; i < g_scrollbar_editor->quantity; i++)
  {
    if (p == g_current_level)
    {
      PaintSquare({895, 93 + g_scrollbar_editor->boxheight * i + (g_scrollbar_editor->scrollOffset) + (i * g_scrollbar_editor->margin)}, {160, 160}, {(char)255, (char)255, (char)255, (char)255}, {(char)50, (char)50, (char)255, (char)100});
    }
    else
    {
      PaintSquare({895, 93 + g_scrollbar_editor->boxheight * i + (g_scrollbar_editor->scrollOffset) + (i * g_scrollbar_editor->margin)}, {160, 160}, {(char)255, (char)255, (char)255, (char)255}, {(char)255, (char)255, (char)255, (char)10});
    }

    PaintLevel(p, {895, 93 + g_scrollbar_editor->boxheight * i + (g_scrollbar_editor->scrollOffset) + (i * g_scrollbar_editor->margin)}, {160, 160});

    // SELECT LEVEL
    if (esat::MouseButtonDown(0))
    {
      if (CheckMouseClick(810, 970, (13 + g_scrollbar_editor->boxheight * i + (g_scrollbar_editor->scrollOffset)) + (i * g_scrollbar_editor->margin), (173 + g_scrollbar_editor->boxheight * i + (g_scrollbar_editor->scrollOffset)) + (i * g_scrollbar_editor->margin), 0))
      {
        // printf("\n%d---------------------------", p->difficulty);
        g_current_level = p;
        g_brick_last_selected = nullptr;
      }
    }
    // DELETE LEVEL
    if (esat::MouseButtonDown(1))
    {
      if (CheckMouseClick(810, 970, (13 + g_scrollbar_editor->boxheight * i + (g_scrollbar_editor->scrollOffset)) + (i * g_scrollbar_editor->margin), (173 + g_scrollbar_editor->boxheight * i + (g_scrollbar_editor->scrollOffset)) + (i * g_scrollbar_editor->margin), 1))
      {
        if (g_current_level == p)
          g_current_level = nullptr;

        DeleteLevel(p);
        g_scrollbar_editor->quantity = CountNumberOfLevels();
        g_brick_last_selected = nullptr;
      }
    }
    p = p->next;
  }

  PaintSquare({900, 5}, {200, 10}, kDefaultBackground, kDefaultBackground);
  PaintSquare({900, 985}, {200, 100}, kDefaultBackground, kDefaultBackground);

  PaintSquare({500, 475}, {990, 940}, {(char)255, (char)255, (char)255, (char)255}, {(char)255, (char)255, (char)255, (char)10});
  ScrollbarController(g_scrollbar_editor);

  PaintSquare({410, 410}, {800, 800}, {(char)255, (char)255, (char)255, (char)255}, kDefaultBackground);

  if (g_current_level != nullptr)
  {
    zoro::Vec2 GlobalBoundingBox_min = {10, 10};
    zoro::Vec2 GlobalBoundingBox_max = {810, 810};
    if (esat::MouseButtonUp(0))
    {
      g_mouse_is_down = false;
      g_brick_selected = nullptr;
    }
    if (g_brick_selected != nullptr)
    {

      zoro::Vec2 mouse = zoro::CoordGlobalToLocalVec2({(float)esat::MousePositionX(), (float)esat::MousePositionY()},
                                                      GlobalBoundingBox_min,
                                                      GlobalBoundingBox_max);

      mouse = zoro::CoordLocalToGlobalVec2(mouse,
                                           {0, 0}, {g_window_width, g_window_height});

      if (!BrickColisionSpawnCheck(mouse))
      {
        g_brick_selected->pos = mouse;
      }

      if (g_brick_selected->pos.x - (g_brick_selected->size.x / 2) < 0)
      {
        g_brick_selected->pos.x = (g_brick_selected->size.x / 2);
      }
      if (g_brick_selected->pos.x + (g_brick_selected->size.x / 2) > g_window_width)
      {
        g_brick_selected->pos.x = g_window_width - (g_brick_selected->size.x / 2);
      }

      if (g_brick_selected->pos.y - (g_brick_selected->size.y / 2) < 0)
      {
        g_brick_selected->pos.y = (g_brick_selected->size.y / 2);
      }
      if (g_brick_selected->pos.y + (g_brick_selected->size.y / 2) > g_window_height)
      {
        g_brick_selected->pos.y = g_window_height - (g_brick_selected->size.y / 2);
      }
    }

    if (g_brick_last_selected != nullptr)
    {
      if (esat::IsKeyDown('D') &&
          (g_brick_last_selected->pos.x + (g_brick_last_selected->size.x * 1.5) < g_window_width) &&
          !BrickColisionSpawnCheck({g_brick_last_selected->pos.x + g_brick_last_selected->size.x, g_brick_last_selected->pos.y}))
      {
        g_brick_last_selected = AddBrick(&g_current_level->bricks,
                                         {g_brick_last_selected->pos.x + g_brick_last_selected->size.x, g_brick_last_selected->pos.y});
      }
      else if (esat::IsKeyDown('A') &&
               (g_brick_last_selected->pos.x - (g_brick_last_selected->size.x * 1.5) > 0) &&
               !BrickColisionSpawnCheck({g_brick_last_selected->pos.x - g_brick_last_selected->size.x, g_brick_last_selected->pos.y}))
      {
        g_brick_last_selected = AddBrick(&g_current_level->bricks,
                                         {g_brick_last_selected->pos.x - g_brick_last_selected->size.x, g_brick_last_selected->pos.y});
      }
      if (esat::IsKeyDown('S') &&
          (g_brick_last_selected->pos.y + (g_brick_last_selected->size.y * 1.5) < g_window_height) &&
          !BrickColisionSpawnCheck({g_brick_last_selected->pos.x, g_brick_last_selected->pos.y + g_brick_last_selected->size.y}))
      {
        g_brick_last_selected = AddBrick(&g_current_level->bricks,
                                         {g_brick_last_selected->pos.x, g_brick_last_selected->pos.y + g_brick_last_selected->size.y});
      }
      else if (esat::IsKeyDown('W') &&
               (g_brick_last_selected->pos.y - (g_brick_last_selected->size.y * 1.5) > 0) &&
               !BrickColisionSpawnCheck({g_brick_last_selected->pos.x, g_brick_last_selected->pos.y - g_brick_last_selected->size.y}))
      {
        g_brick_last_selected = AddBrick(&g_current_level->bricks,
                                         {g_brick_last_selected->pos.x, g_brick_last_selected->pos.y - g_brick_last_selected->size.y});
      }
    }

    // Editor main level painting
    PaintLevel(g_current_level, {410, 410}, {800, 800});

    if (esat::MouseButtonDown(0))
    {
      emptyClick = true;
    }
    if (g_current_level->bricks != nullptr)
    {
      zoro::Vec2 brickPosGlobalPos;
      zoro::Vec2 brickPosGlobalSize;
      TBrick *p = g_current_level->bricks;

      while (p != nullptr)
      {

        if (CheckMouseClickOnLocalCoordinates(p->pos, p->size, 0, {410, 410}, {800, 800}))
        {
          emptyClick = false;
          printf("selected");
          g_brick_selected = p;
          g_brick_last_selected = p;
          g_mouse_is_down = true;
          g_mouse_down_pos = zoro::CoordGlobalToLocalVec2({(float)esat::MousePositionX(), (float)esat::MousePositionY()}, GlobalBoundingBox_min, GlobalBoundingBox_max);
        }
        if (CheckMouseClickOnLocalCoordinates(p->pos, p->size, 1, {410, 410}, {800, 800}))
        {
          emptyClick = false;
          DeleteBrick(p);
        }
        p = p->next;
      }
    }

    if (g_current_level->bricks != nullptr)
    {
      if (emptyClick && CheckMouseClick(GlobalBoundingBox_min.x + (g_current_level->bricks->size.x / 2), GlobalBoundingBox_max.x - (g_current_level->bricks->size.x / 2), GlobalBoundingBox_min.y + (g_current_level->bricks->size.y / 2), GlobalBoundingBox_max.y - (g_current_level->bricks->size.y / 2), -1))
      {

        zoro::Vec2 mousePosLocal = zoro::CoordGlobalToLocalVec2(zoro::CoordLocalToGlobalVec2({(float)esat::MousePositionX(), (float)esat::MousePositionY()}, {0, 0}, {g_window_width, g_window_height}), GlobalBoundingBox_min, GlobalBoundingBox_max);

        g_brick_last_selected = g_current_level->bricks;

        if (!BrickColisionSpawnCheck(mousePosLocal))
        {
          g_brick_last_selected = AddBrick(&g_current_level->bricks, mousePosLocal);
        }
        g_brick_last_selected = nullptr;
      }
    }
    else
    {
      if (CheckMouseClick(GlobalBoundingBox_min.x, GlobalBoundingBox_max.x, GlobalBoundingBox_min.y, GlobalBoundingBox_max.y, 0))
      {
        zoro::Vec2 mousePosLocal = zoro::CoordGlobalToLocalVec2(zoro::CoordLocalToGlobalVec2({(float)esat::MousePositionX(), (float)esat::MousePositionY()}, {0, 0}, {g_window_width, g_window_height}), GlobalBoundingBox_min, GlobalBoundingBox_max);
        g_brick_last_selected = AddBrick(&g_current_level->bricks, mousePosLocal);
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
void GameWindow()
{
  if (!ActiveBricks(g_current_level->bricks))
  {
    int baseScore = g_current_level->player.score;
    if (g_current_level->next != nullptr)
    {
      g_current_level = g_current_level->next;
    }
    else
    {
      if (g_current_level == g_level_list)
      {
        g_window_state = kMenu;
      }
      else
      {
        g_current_level = g_level_list;
      }
    }
    StartGame(baseScore);
    return;
  }

  if (g_current_level->player.lives < 0)
    g_window_state = kMenu;

  if (esat::IsKeyDown('B'))
    g_window_state = kMenu;

  PaintLevel(g_current_level, {500, 500}, {1000, 1000});

  PaintPlayer();
  PaintGameUI();
}
void LevelSelectorWindow()
{
  PaintSquare({500, 475}, {990, 940}, {(char)255, (char)255, (char)255, (char)255}, {(char)255, (char)255, (char)255, (char)10});

  TLevel *p = g_level_list;
  int column = 0;
  int row = 0;
  zoro::Vec2 boxPos, boxSize;

  for (int i = 0; i < g_scrollbar_level_selector->quantity; i++)
  {
    boxPos = {105 + (column * g_scrollbar_level_selector->boxheight) + (column * g_scrollbar_level_selector->margin), 105 + g_scrollbar_level_selector->boxheight * row + (g_scrollbar_level_selector->scrollOffset) + (row * g_scrollbar_level_selector->margin)};
    boxSize = {g_scrollbar_level_selector->boxheight, g_scrollbar_level_selector->boxheight};

    PaintSquare(boxPos, boxSize,
                {(char)255, (char)255, (char)255, (char)255},
                {(char)255, (char)255, (char)255, (char)10});

    PaintLevel(p, boxPos, boxSize);

    // SELECT LEVEL
    if (esat::MouseButtonDown(0))
    {
      if (CheckMouseClick(boxPos.x - (boxSize.x / 2),
                          boxPos.x + (boxSize.x / 2),
                          boxPos.y - (boxSize.y / 2),
                          boxPos.y + (boxSize.y / 2),
                          0))
      {
        g_current_level = p;
        // Start game
        // ...
        printf("start: %d", g_current_level->difficulty);
        StartGame(0);
      }
    }

    column++;
    if (column >= g_scrollbar_level_selector->columns)
    {
      column = 0;
      row++;
    }

    p = p->next;
  }
  ScrollbarController(g_scrollbar_level_selector);
  PaintSquare({500, 1000}, {1000, 110}, kDefaultBackground, kDefaultBackground);
}

void InitUI()
{
  // esat::DrawSetTextFont("./resources/fonts/Hyperspace.otf");
  esat::DrawSetTextFont("./resources/fonts/UbuntuMono.ttf");
  TButton new_button;

  new_button.fontSize = 40;

  // Buttons

  new_button.text = "LEVEL EDITOR";
  new_button.idFunction = 0;
  new_button.pos = {300, 500};
  new_button.windowContext = kMenu;
  new_button.dimensions = {350, 40};
  AddButtonToList(new_button);

  new_button.text = "START";
  new_button.idFunction = 1;
  new_button.pos = {700, 500};
  new_button.windowContext = kMenu;
  new_button.dimensions = {350, 40};
  AddButtonToList(new_button);

  /*
  new_button.text = "START LEVEL";
  new_button.fontSize = 25;
  new_button.idFunction = 2;
  new_button.pos = {300, 500};
  new_button.windowContext = kLevelSelector;
  new_button.dimensions = {200, 40};
  AddButtonToList(new_button);
  new_button.fontSize = 40;
  */

  new_button.fontSize = 25;

  new_button.text = "ADD LEVEL";
  new_button.idFunction = 4;
  new_button.pos = {130, 975};
  new_button.windowContext = kEditor;
  new_button.dimensions = {250, 40};
  AddButtonToList(new_button);

  new_button.text = "BACK TO kMenu";
  new_button.idFunction = 3;
  new_button.pos = {870, 975};
  new_button.windowContext = kLevelSelector;
  new_button.dimensions = {250, 40};
  AddButtonToList(new_button);
  new_button.windowContext = kEditor;
  AddButtonToList(new_button);

  // Scrollbar
  g_scrollbar_editor = (TScrollbar *)malloc(1 * sizeof(TScrollbar));

  g_scrollbar_editor->boxheight = 160;
  g_scrollbar_editor->max_height = 935;
  g_scrollbar_editor->min_height = 12;
  g_scrollbar_editor->quantity = 0;
  g_scrollbar_editor->scrollOffset = 0;
  g_scrollbar_editor->width = 13;
  g_scrollbar_editor->xpos = 975;
  g_scrollbar_editor->margin = 3;
  g_scrollbar_editor->columns = 1;

  g_scrollbar_level_selector = (TScrollbar *)malloc(1 * sizeof(TScrollbar));

  g_scrollbar_level_selector->boxheight = 180;
  g_scrollbar_level_selector->max_height = 935;
  g_scrollbar_level_selector->min_height = 12;
  g_scrollbar_level_selector->quantity = 0;
  g_scrollbar_level_selector->scrollOffset = 0;
  g_scrollbar_level_selector->width = 20;
  g_scrollbar_level_selector->xpos = 965;
  g_scrollbar_level_selector->margin = 10;
  g_scrollbar_level_selector->columns = 5;
}

void InitGame()
{
  g_square_points = (zoro::Vec2 *)malloc(4 * sizeof(zoro::Vec2));

  default_player.ball = nullptr;
  default_player.color = {(char)255, (char)255, (char)255, (char)255};
  default_player.lives = 5;
  default_player.pos = {500, 920};
  default_player.score = 0;
  default_player.size = {150, 25};
  default_player.dir = {1, -1};

  default_brick.active = true;
  default_brick.color = {(char)200, (char)200, (char)200, (char)230};
  default_brick.lives = 1;
  default_brick.next = nullptr;
  default_brick.size = {g_window_width / 10, 35};
  default_brick.reward = 10;

  InitUI();
  LoadLevels();
}

void FreeLevels()
{
  TLevel *l = g_level_list;
  while (l != nullptr)
    DeleteLevel(l);
}

int esat::main(int argc, char **argv)
{
  srand(time(NULL));

  esat::WindowInit(g_window_width, g_window_height);
  WindowSetMouseVisibility(true);

  InitGame();
  printf("\n:3");

  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {

    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(kDefaultBackground.r, kDefaultBackground.g, kDefaultBackground.b);

    // --------------

    switch (g_window_state)
    {
    case kMenu:
      MenuWindow();
      break;
    case kEditor:
      EditorWindow();
      break;
    case kGame:
      GameWindow();
      break;
    case kLevelSelector:
      LevelSelectorWindow();
      break;
    default:
      printf("Error with current window state. \n^w^*\n");
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