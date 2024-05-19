#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>

float kwidth = 800.0f;
float kheight = 800.0f;

unsigned char fps = 60; // Control de frames por segundo
double current_time, last_time;

const int knMaxPoints = 50;
const int kNPoints = 5;

esat::Vec3 g_Points[kNPoints];

void initFigures()
{
  float points[10] = {
      0.0f,
      0.0f,
      0.0f,
      0.75f,
      0.125f,
      1.0f,
      0.25f,
      0.75f,
      0.25f,
      0.0f,
  };

  for (int i = 0; i < kNPoints * 2; i += 2)
  {
    g_Points[i / 2] = esat::Vec3{points[i], points[i + 1], 1.0f};
  }
}

esat::Mat3 basicTransform(esat::Vec2 scale, float rotation, esat::Vec2 pos)
{

  esat::Mat3 m = esat::Mat3Identity();

  m = esat::Mat3Multiply(esat::Mat3Scale(scale.x, scale.y), m);
  m = esat::Mat3Multiply(esat::Mat3Rotate(rotation), m);

  m = esat::Mat3Multiply(esat::Mat3Translate(pos.x, pos.y), m);
  return m;
}

void DrawFigure(const int n_points, esat::Vec3 *points, esat::Mat3 m)
{

  esat::Vec2 dr_points[knMaxPoints];

  for (int i = 0; i < n_points; i++)
  {
    esat::Vec3 tmp = esat::Mat3TransformVec3(m, points[i]);
    dr_points[i] = {tmp.x, tmp.y};
  }

  esat::DrawSetFillColor(255, 255, 255, 20);
  esat::DrawSetStrokeColor(255, 255, 255, 255);
  esat::DrawSolidPath(&dr_points[0].x, n_points);
}

int esat::main(int argc, char **argv)
{
  esat::WindowInit(kwidth, kheight);
  WindowSetMouseVisibility(true);

  initFigures();

  while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape))
  {

    last_time = esat::Time();
    esat::DrawBegin();
    esat::DrawClear(0, 0, 0);
    // --------------

    esat::Mat3 m = basicTransform({1, 1}, esat::Time() * 0.001f, {(float)esat::MousePositionX(), (float)esat::MousePositionY()});

    for (int i = 0; i < 4; i++)
    {
      esat::Mat3 m2 = esat::Mat3Identity();

      m2 = esat::Mat3Multiply(esat::Mat3Translate(-0.125, -1), m2); // Centro figura y la muevo en y para que la punta este en el raton
      m2 = esat::Mat3Multiply(esat::Mat3Scale(160, 160), m2); // Escalo
      m2 = esat::Mat3Multiply(esat::Mat3Translate(0, -80), m2);  // Muevo hacia abajo
      m2 = esat::Mat3Multiply(esat::Mat3Rotate((6.28f / 4) * i), m2); // Roto en base al punto de rotacion que es el raton
      m2 = esat::Mat3Multiply(m, m2);

      DrawFigure(kNPoints, g_Points, m2);
    }
    for (int i = 0; i < 4; i++)
    {
      esat::Mat3 m2 = esat::Mat3Identity();
      
      m2 = esat::Mat3Multiply(esat::Mat3Translate(-0.125, 0), m2);
      m2 = esat::Mat3Multiply(esat::Mat3Scale(160, 160), m2);
      m2 = esat::Mat3Multiply(esat::Mat3Translate(0, 80), m2); 
      m2 = esat::Mat3Multiply(esat::Mat3Rotate(((6.28f/4 )* i) + (3.14f/4)), m2);
      m2 = esat::Mat3Multiply(m, m2);

      DrawFigure(kNPoints, g_Points, m2);
    }

    esat::Mat3 m2 = esat::Mat3Identity();
      
      m2 = esat::Mat3Multiply(esat::Mat3Translate(-0.125, -0.5), m2);
      m2 = esat::Mat3Multiply(esat::Mat3Scale(160, 160), m2);
      m2 = esat::Mat3Multiply(m, m2);
      //DrawFigure(kNPoints, g_Points, m2);

    // --------------
    esat::DrawEnd();
    // Control fps
    do
    {
      current_time = esat::Time();
    } while ((current_time - last_time) <= 1000.0 / fps);
    esat::WindowFrame();
  }

  esat::WindowDestroy();
  return 0;
}
