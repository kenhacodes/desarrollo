#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "esat/window.h"
#include "esat/draw.h"
#include "esat/input.h"
#include "esat/sprite.h"
#include "esat/time.h"


const unsigned int kWindowWidth = 800;
const unsigned int kWindowHeight = 600;

const int kMaxPoints = 50;
const int kNPointsArrow = 4;
const int kNPointsSquare = 4;
const int kNPointsHexagon = 6;
esat::Vec3 g_arrow[kNPointsArrow];
esat::Vec3 g_square[kNPointsSquare];
esat::Vec3 g_hexagon[kNPointsHexagon];

void InitFigures() {
  //Square:
  float angle = 6.28f / (float) kNPointsSquare;
  for (int i = 0; i < kNPointsSquare; ++i) {
    g_square[i] = { cosf(angle * (float) i),
                    sinf(angle * (float) i), 1.0f };
  }

  //Hexagon:
  angle = 6.28f / (float) kNPointsHexagon;
  for (int i = 0; i < kNPointsHexagon; ++i) {
    g_hexagon[i] = { cosf(angle * (float) i),
                     sinf(angle * (float) i), 1.0f };
  }

  //Arrow:
  g_arrow[0] = { -1.0f,  0.0f,  1.0f };
  g_arrow[1] = {  0.0f,  0.5f,  1.0f };
  g_arrow[2] = {  2.0f,  0.0f,  1.0f };
  g_arrow[3] = {  0.0f, -0.5f,  1.0f };
}

esat::Mat3 BasicTransform(esat::Vec2 scale = { 50.0f, 50.0f }, float angle = 3.14f,
                          esat::Vec2 pos = { 400.0f, 300.0f }) {
  esat::Mat3 m = esat::Mat3Identity();
  m = esat::Mat3Multiply(esat::Mat3Scale(scale.x, scale.y), m);
  m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
  m = esat::Mat3Multiply(esat::Mat3Translate(pos.x, pos.y), m);
  return m;
}

esat::Mat3 TransformCorners(float loop = 0.0f) {
  esat::Mat3 m = esat::Mat3Identity();
  m = esat::Mat3Multiply(esat::Mat3Scale(60.0f, 60.0f), m);
  m = esat::Mat3Multiply(esat::Mat3Rotate(0.0f), m);
  m = esat::Mat3Multiply(esat::Mat3Translate(180.0f, 0.0), m);
  m = esat::Mat3Multiply(esat::Mat3Rotate(0.78f + 1.57 * loop), m);
  m = esat::Mat3Multiply(esat::Mat3Translate(400.0f, 300.0f), m);
  return m;
}

esat::Mat3 TransformLoops(esat::Vec2 offset = { 5.0f, 0.0f },
                          float orbit = 0.0f,
                          esat::Vec2 scale = { 20.0f, 20.0f },
                          float angle = 0.0f,
                          esat::Vec2 pos = { 400.0f, 300.0f }) {
  esat::Mat3 m = esat::Mat3Identity();
  m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
  m = esat::Mat3Multiply(esat::Mat3Translate(offset.x, offset.y), m);
  m = esat::Mat3Multiply(esat::Mat3Scale(scale.x, scale.y), m);
  m = esat::Mat3Multiply(esat::Mat3Rotate(orbit), m);
  m = esat::Mat3Multiply(esat::Mat3Translate(pos.x, pos.y), m);
  return m;
}

void DrawFigure(const int n_points, esat::Vec3* points, esat::Mat3 m) {
  esat::Vec2 dr_points[kMaxPoints];
  for (int i = 0; i < n_points; ++i) {
    esat::Vec3 tmp = esat::Mat3TransformVec3(m, points[i]);
    dr_points[i] = { tmp.x, tmp.y };
  }
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSetFillColor(255, 255, 255, 0);
  esat::DrawSolidPath(&dr_points[0].x, n_points);
}

int esat::main(int argc, char** argv) {
  srand(time(nullptr));
  double current_time = 0.0;
  double last_time = 0.0;
  double fps = 60.0;

  esat::WindowInit(kWindowWidth, kWindowHeight);
  esat::WindowSetMouseVisibility(true);
  InitFigures();

  while (!esat::IsSpecialKeyDown(esat::kSpecialKey_Escape) &&
         esat::WindowIsOpened()) {
    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(120, 120, 120);

    //Center:
    esat::Mat3 m = BasicTransform({ 100.0f, 100.0f }, 1.57f * 0.5);
    DrawFigure(kNPointsSquare, g_square, m);
    m = BasicTransform({ 100.0f * cosf(0.78f), 100.0f * cosf(0.78f) });
    DrawFigure(kNPointsHexagon, g_hexagon, m);

    //Loops:
    for (int j = 0; j < 4; ++j) {
      for (int i = 0; i < 5; ++i) {
        esat::Mat3 arrow_tr = TransformLoops({ 6.5f, i * 1.3f - 2.0f * 1.3f }, 6.28f / 4.0f * j,
                                             { 20.0f, 20.0f }, 3.14f);
        DrawFigure(kNPointsArrow, g_arrow, arrow_tr);
        esat::Mat3 square_tr = TransformLoops({ 9.0f, i * 2.5f - 2.0f * 2.5f }, 6.28f / 4.0f * j,
                                              { 20.0f, 10.0f }, 0.78f);
        DrawFigure(kNPointsSquare, g_square, square_tr);
      }
    }

    //Corners:
    for (int j = 0; j < 4; ++j) {
      esat::Mat3 corner_tr = TransformCorners((float) j);
      DrawFigure(kNPointsArrow, g_arrow, corner_tr);
    }

    esat::DrawEnd();
    esat::WindowFrame();

  	do {
      current_time = esat::Time();
    } while((current_time - last_time) <= 1000.0 / fps);
  }
  esat::WindowDestroy();
  return 0;
}
