/*
    ⠀⠀⠀⠀⢀⣴⣠⡤⢲⣴⣀⣀⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⣼⡔⢳⠑⢰⣟⣡⣮⠁⣉⡒⡄⠀⠀⠀⠀⠀⠀⠀⠀
    ⣠⣰⣑⡮⠂⠏⠀⠀⠀⠙⠉⠛⢆⠜⢖⠀⠀⠀    +---------------------------------+
    ⢨⢤⢹⢀⡀⠀⢢⡀⠀⠀⢡⣀⣘⠟⠂⣣⠀⠀⠀          Zoro Math Lib in C  
    ⠸⡖⡎⣤⢾⣻⠞⠈⠉⠿⢿⠦⠘⢪⣧⣳⠀⠀        Author : Guillermo Boscá
    ⠀⢳⣷⠀⠀⠀⠠⣁⠀⠀⠠⠀⢠⣯⢻⣧⠀⠀⠀⠀⠀  < guillermobosca.bro@gmail.com >
    ⠀⠘⣿⡀⠀⡀⠤⠤⠤⠤⠒⠀⢸⣾⠟⠀⠀⠀⠀⠀       Last Update: 08/02/24
    ⠀⠀⠈⠳⣀⠀⠈⠉⠉⠁⢀⣰⡿⢹⡆⠀⠀⠀⠀⠀⠀+---------------------------------+⠀
    ⠀⠀⠀⠀⠈⣷⣶⣶⣶⣶⣿⣿⣲⣿⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⡇⠉⠛⢿⠿⣿⣿⠗⢀⣣⣤⡀⠀⠀⠀⠀⠀ ~ Don't get lost.
    ⠀⠀⠀⠀⠀⡇ ⠀⠀⠙⠉⢡⣶⣿⣿⣿⣿⣷⣶⣦⣀⠀
    ⠀⠀⠀⠀⢰⠀ ⠀⠀⣠⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧
    ⠀⠀⠀⣠⠇⠀⣀⠔⣡⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
    ⠀⢠⣾⠡⡤⢊⢤⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⣿⣿⣿⣿⣿⣀
*/
#include <stdio.h>
#include <math.h>

namespace zoro {

const float PI = 3.1416f;
const float longPI = 3.1415926f;

// - Numeric Types -

struct Vec2D {
  float x, y;
};

struct Vec3D {
  float x, y, z;
};

struct Vec4D {
  float x, y, z, w;
};

struct Mat2D {
  /*
    m = 0 2
        1 3
  */
  float m[4];
};

struct Mat3D {
  /*
    m = 0 3 6
        1 4 7
        2 5 8
  */
  float m[9];
};

struct Mat4D {
  /*
    m = 0 4  8 12
        1 5  9 13
        2 6 10 14
        3 7 11 15
  */
  float m[16];
};

// - Shapes -

struct Triangle2D{
  Vec2D p[3];
};

struct Square2D{
  Vec2D p[4];
};

struct Square3D{
  Vec3D p[4];
};

struct Box3D{
  Vec3D p[8];
};

struct Circle2D{
  Vec2D c;
  float radius;
};

struct Sphere3D{
  Vec3D c;
  float radius;
};

// - Vectors -

Vec2D CoordGlobalToLocalVec2D(Vec2D v, Vec2D GlobalBoundingBox_min, Vec2D GlobalBoundingBox_max){
	return (Vec2D{  ( v.x - GlobalBoundingBox_min.x ) / ( GlobalBoundingBox_max.x - GlobalBoundingBox_min.x ),
                  ( v.y - GlobalBoundingBox_min.y ) / ( GlobalBoundingBox_max.y - GlobalBoundingBox_min.y )});
}
Vec2D CoordLocalToGlobalVec2D(Vec2D v, Vec2D LocalBoundingBox_min, Vec2D LocalBoundingBox_max){
	return (Vec2D{  LocalBoundingBox_min.x + v.x * ( LocalBoundingBox_max.x - LocalBoundingBox_min.x ), 
                  LocalBoundingBox_min.y + v.y * ( LocalBoundingBox_max.y - LocalBoundingBox_min.y ) });
}

Vec2D InitVec2D(){
  return (Vec2D {0.0f , 0.0f});
}
Vec3D InitVec3D(){
  return (Vec3D {0.0f , 0.0f , 0.0f});
}
Vec4D InitVec4D(){
  return (Vec4D {0.0f , 0.0f , 0.0f, 0.0f});
}

Vec2D SumVec2D(Vec2D v, Vec2D w){
  return (Vec2D{ v.x + w.x , v.y + w.y});
};
Vec3D SumVec3D(Vec3D v, Vec3D w){
  return (Vec3D{ v.x + w.x, v.y + w.y, v.z + w.z});
};
Vec4D SumVec4D(Vec4D v, Vec4D w){
  return (Vec4D{ v.x + w.x, v.y + w.y, v.z + w.z, v.w + w.w});
};

Vec2D SubtractVec2D(Vec2D v, Vec2D w){
  return (Vec2D{ v.x - w.x , v.y - w.y});
}
Vec3D SubtractVec3D(Vec3D v, Vec3D w){
  return (Vec3D{ v.x - w.x, v.y - w.y, v.z - w.z});
};
Vec4D SubtractVec4D(Vec4D v, Vec4D w){
  return (Vec4D{ v.x - w.x, v.y - w.y, v.z - w.z, v.w - w.w});
};

Vec2D ScaleVec2D(Vec2D vec, float s){
  return (Vec2D{ vec.x * s , vec.y * s });
}
Vec3D ScaleVec3D(Vec3D vec, float s){
  return (Vec3D{ vec.x * s , vec.y * s , vec.z * s});
}
Vec4D ScaleVec4D(Vec4D vec, float s){
  return (Vec4D{ vec.x * s , vec.y * s , vec.z * s, vec.w * s});
};

Vec2D DivideByScaleVec2D(Vec2D vec, float s){
  return (Vec2D{ vec.x / s , vec.y / s });
}
Vec3D DivideByScaleVec3D(Vec3D vec, float s){
  return (Vec3D{ vec.x / s , vec.y / s , vec.z / s});
}
Vec4D DivideByScaleVec4D(Vec4D vec, float s){
  return (Vec4D{ vec.x / s , vec.y / s , vec.z / s, vec.w / s});
};
                                  
float MagnitudeVec2D(Vec2D v){
  return sqrt(v.x * v.x + v.y * v.y);
}
float MagnitudeVec3D(Vec3D v){
  return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}
float MagnitudeVec4D(Vec4D v){
  return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
}

float DotVec2D(Vec2D v, Vec2D w){
  return ((v.x * w.x) + (v.y * w.y));
}
float DotVec3D(Vec3D v, Vec3D w){
  return ((v.x * w.x) + (v.y * w.y) + (v.z * w.z));
}
float DotVec4D(Vec4D v, Vec4D w){
  return ((v.x * w.x) + (v.y * w.y) + (v.z * w.z) + (v.w * w.w));
}

int NrmlizeV2(Vec2D *v){
  float magn = ( sqrt((v->x * v->x) + (v->y * v->y)) );
  if (magn == 0.0f){
    *v = {0.0f , 0.0f};
    return 1;
  }else{
    v->x /= magn;
    v->y /= magn;
     return 0;
  }
}
int NrmlizeV3(Vec3D *v){
  float magn = ( sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z)) );
  if (magn == 0.0f){
    *v = {0.0f , 0.0f, 0.0f};
     return 1;
  }else{
    v->x /= magn;
    v->y /= magn;
    v->z /= magn;
     return 0;
  }
}
int NrmlizeV4(Vec4D *v){
  float magn = ( sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z) + (v->w * v->w)) );
  if (magn == 0.0f){
    *v = {0.0f , 0.0f, 0.0f, 0.0f};
     return 1;
  }else{
    v->x /= magn;
    v->y /= magn;
    v->z /= magn;
    v->w /= magn;
     return 0;
  }
}

Vec2D NormalizeVec2D(Vec2D v){
  float magn = ( sqrt((v.x * v.x) + (v.y * v.y)) );
  if (magn == 0.0f) return (Vec2D{ 0 , 0 });
  return (Vec2D{ v.x / magn , v.y / magn });
}
Vec3D NormalizeVec3D(Vec3D v){
  float magn = ( sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)) );
  if (magn == 0.0f) return (Vec3D{ 0 , 0 , 0});
  return (Vec3D{ v.x / magn , v.y / magn , v.z / magn});
}
Vec4D NormalizeVec4D(Vec4D v){
  float magn = ( sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w)) );
  if (magn == 0.0f) return (Vec4D{ 0 , 0 , 0 , 0 });
  return (Vec4D{ v.x / magn , v.y / magn , v.z / magn , v.w / magn});
}

bool IsNormalizedVec2D(Vec2D v){
  float magn = sqrt((v.x * v.x) + (v.y * v.y));
  return (magn > 0.99f && magn < 1.01f);
}
bool IsNormalizedVec3D(Vec3D v){
  float magn = ( sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)) );
  return (magn > 0.99f && magn < 1.01f);
};
bool IsNormalizedVec4D(Vec4D v){
  float magn = ( sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w)) );
  return (magn > 0.99f && magn < 1.01f);
};

Vec2D LeftPerpendicularVec2D(Vec2D v){
  return (Vec2D { -v.y , v.x });
}
Vec2D RightPerpendicularVec2D(Vec2D v){
  return (Vec2D { v.y , -v.x });
}

Vec2D Vec2DMultMat2DVec2D(Mat2D mat, Vec2D vec) {
  return (Vec2D{mat.m[0] * vec.x + mat.m[2] * vec.y,
                mat.m[1] * vec.x + mat.m[3] * vec.y});
};
Vec3D Vec3DMultMat3DVec3D(Mat3D mat, Vec3D vec) {
  return (Vec3D{mat.m[0] * vec.x + mat.m[3] * vec.y + mat.m[6] * vec.z,
                mat.m[1] * vec.x + mat.m[4] * vec.y + mat.m[7] * vec.z,
                mat.m[2] * vec.x + mat.m[5] * vec.y + mat.m[8] * vec.z});
};
Vec4D Vec4DMultMat4DVec4D(Mat4D mat, Vec4D vec) {
  return (Vec4D{mat.m[0] * vec.x + mat.m[3] * vec.y + mat.m[6] * vec.z + mat.m[ 9] * vec.w, 
                mat.m[1] * vec.x + mat.m[4] * vec.y + mat.m[7] * vec.z + mat.m[10] * vec.w,
                mat.m[2] * vec.x + mat.m[5] * vec.y + mat.m[8] * vec.z + mat.m[11] * vec.w});
};

void PrintVec2D(Vec2D v) {
  printf("[X]: %.3f [Y]: %.3f\n", v.x, v.y);
}
void PrintVec3D(Vec3D v) {
  printf("[X]: %.3f [Y]: %.3f [Z]: %.3f\n", v.x, v.y, v.z);
}
void PrintVec4D(Vec4D v) {
  printf("[X]: %.3f [Y]: %.3f [Z]: %.3f [W]: %.3f\n", v.x, v.y, v.z, v.w);
}

// - Matrix -

Mat2D InitMat2D(){
  return (Mat2D{0.0f, 0.0f, 0.0f, 0.0f});
}
Mat3D InitMat3D(){
  return (Mat3D{0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f});
}
Mat4D InitMat4D(){
  return (Mat4D{0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,});
}

Mat2D MatIdentity2D(){
  return (Mat2D{1.0f, 0.0f,
                0.0f, 1.0f});
}
Mat3D MatIdentity3D(){
  return (Mat3D{1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f});
}
Mat4D MatIdentity4D(){
  return (Mat4D{1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,});
}

Mat2D SumMat2D(Mat2D m1, Mat2D m2){
return (Mat2D{  m1.m[0] + m2.m[0], m1.m[1] + m2.m[1],
                m1.m[2] + m2.m[2], m1.m[3] + m2.m[3] });
}
Mat3D SumMat3D(Mat3D m1, Mat3D m2){
  return (Mat3D{  m1.m[0] + m2.m[0], m1.m[1] + m2.m[1], m1.m[2] + m2.m[2],
                  m1.m[3] + m2.m[3], m1.m[4] + m2.m[4], m1.m[5] + m2.m[5],
                  m1.m[6] + m2.m[6], m1.m[7] + m2.m[7], m1.m[8] + m2.m[8] });
}
Mat4D SumMat4D(Mat4D m1, Mat4D m2){
  return (Mat4D{  m1.m[ 0] + m2.m[ 0], m1.m[ 1] + m2.m[ 1], m1.m[ 2] + m2.m[ 2], m1.m[ 3] + m2.m[ 3],
                  m1.m[ 4] + m2.m[ 4], m1.m[ 5] + m2.m[ 5], m1.m[ 6] + m2.m[ 6], m1.m[ 7] + m2.m[ 7],
                  m1.m[ 8] + m2.m[ 8], m1.m[ 9] + m2.m[ 9], m1.m[10] + m2.m[10], m1.m[11] + m2.m[11],
                  m1.m[12] + m2.m[12], m1.m[13] + m2.m[13], m1.m[14] + m2.m[14], m1.m[15] + m2.m[15]});
}

Mat2D SubtractMat2D(Mat2D m1, Mat2D m2){
return (Mat2D{  m1.m[0] - m2.m[0], m1.m[1] - m2.m[1],
                m1.m[2] - m2.m[2], m1.m[3] - m2.m[3] });
}
Mat3D SubtractMat3D(Mat3D m1, Mat3D m2){
  return (Mat3D{  m1.m[0] - m2.m[0], m1.m[1] - m2.m[1], m1.m[2] - m2.m[2],
                  m1.m[3] - m2.m[3], m1.m[4] - m2.m[4], m1.m[5] - m2.m[5],
                  m1.m[6] - m2.m[6], m1.m[7] - m2.m[7], m1.m[8] - m2.m[8] });
}
Mat4D SubtractMat4D(Mat4D m1, Mat4D m2){
  return (Mat4D{  m1.m[0] - m2.m[0], m1.m[1] - m2.m[1], m1.m[2] - m2.m[2], m1.m[3] - m2.m[3],
                  m1.m[4] - m2.m[4], m1.m[5] - m2.m[5], m1.m[6] - m2.m[6], m1.m[7] - m2.m[7],
                  m1.m[6] - m2.m[6], m1.m[7] - m2.m[7], m1.m[8] - m2.m[8], m1.m[9] - m2.m[9] });
}

Mat2D ScaleMat2D(Mat2D mat, float s) {
  return (Mat2D{mat.m[0] * s , mat.m[1] * s,
                mat.m[2] * s , mat.m[3] * s});
};
Mat3D ScaleMat3D(Mat3D mat, float s) {
  return (Mat3D{mat.m[0] * s , mat.m[1] * s , mat.m[2] * s,
                mat.m[3] * s , mat.m[4] * s , mat.m[5] * s,
                mat.m[6] * s , mat.m[7] * s , mat.m[8] * s});
};
Mat4D ScaleMat4D(Mat4D mat, float s) {
  return (Mat4D{mat.m[ 0] * s , mat.m[ 1] * s , mat.m[ 2] * s , mat.m[ 3] * s, 
                mat.m[ 4] * s , mat.m[ 5] * s , mat.m[ 6] * s , mat.m[ 7] * s,
                mat.m[ 8] * s , mat.m[ 9] * s , mat.m[10] * s , mat.m[11] * s,
                mat.m[12] * s , mat.m[13] * s , mat.m[14] * s , mat.m[15] * s});
};

Mat2D DivideByScaleMat2D(Mat2D mat, float s) {
  return (Mat2D{mat.m[0] / s , mat.m[1] / s,
                mat.m[2] / s , mat.m[3] / s});
};
Mat3D DivideByScaleMat3D(Mat3D mat, float s) {
  return (Mat3D{mat.m[0] / s , mat.m[1] / s , mat.m[2] / s,
                mat.m[3] / s , mat.m[4] / s , mat.m[5] / s,
                mat.m[6] / s , mat.m[7] / s , mat.m[8] / s});
};
Mat4D DivideByScaleMat4D(Mat4D mat, float s) {
  return (Mat4D{mat.m[ 0] / s , mat.m[ 1] / s , mat.m[ 2] / s , mat.m[ 3] / s, 
                mat.m[ 4] / s , mat.m[ 5] / s , mat.m[ 6] / s , mat.m[ 7] / s,
                mat.m[ 8] / s , mat.m[ 9] / s , mat.m[10] / s , mat.m[11] / s,
                mat.m[12] / s , mat.m[13] / s , mat.m[14] / s , mat.m[15] / s});
};

// Review!!
Mat2D Mat2DMultMat2DVec2D(Mat2D mat, Vec2D vec) {
  return (Mat2D{mat.m[0] * vec.x , mat.m[1] * vec.y,
                mat.m[2] * vec.x , mat.m[3] * vec.y});
};
Mat3D Mat3DMultMat3DVec3D(Mat3D mat, Vec3D vec) {
  return (Mat3D{mat.m[0] * vec.x , mat.m[3] * vec.y , mat.m[6] * vec.z,
                mat.m[1] * vec.x , mat.m[4] * vec.y , mat.m[7] * vec.z,
                mat.m[2] * vec.x , mat.m[5] * vec.y , mat.m[8] * vec.z});
};
Mat4D Mat4DMultMat4DVec4D(Mat4D mat, Vec4D vec) {
  return (Mat4D{mat.m[0] * vec.x , mat.m[3] * vec.y , mat.m[6] * vec.z , mat.m[ 9] * vec.w, 
                mat.m[1] * vec.x , mat.m[4] * vec.y , mat.m[7] * vec.z , mat.m[10] * vec.w,
                mat.m[2] * vec.x , mat.m[5] * vec.y , mat.m[8] * vec.z , mat.m[11] * vec.w});
};

Mat2D TransposeMat2D(Mat2D m){
  return (Mat2D { m.m[0], m.m[2],
                  m.m[1], m.m[3] });
}
Mat3D TransposeMat3D(Mat3D m){
  return (Mat3D { m.m[0], m.m[3], m.m[6],
                  m.m[1], m.m[4], m.m[7],
                  m.m[2], m.m[5], m.m[8] });
}
Mat4D TransposeMat4D(Mat4D m){
  return (Mat4D { m.m[0], m.m[4], m.m[8], m.m[12],
                  m.m[1], m.m[5], m.m[9], m.m[13],
                  m.m[2], m.m[6], m.m[10], m.m[14],
                  m.m[3], m.m[7], m.m[11], m.m[15] });
}

float DeterMat2D (Mat2D m){
  return ((m.m[0] * m.m[3]) - (m.m[1] * m.m[2]));
}

float GetValMat2D(Mat2D m, int row, int col){
  return m.m[col * 2 + row];
}
float GetValMat3D(Mat3D m, int row, int col){
  return m.m[col * 3 + row];
}
float GetValMat4D(Mat4D m, int row, int col){
  return m.m[col * 4 + row];
}

void SetValMat2D(Mat2D *m, int row, int col, float value){
  m->m[col * 2 + row] = value;
}
void SetValMat3D(Mat3D *m, int row, int col, float value){
  m->m[col * 3 + row] = value;
}
void SetValMat4D(Mat4D *m, int row, int col, float value){
  m->m[col * 4 + row] = value;
}

void PrintMat2D(Mat2D m){
  printf("[%.3f] [%.3f]\n[%.3f] [%.3f]\n",
          m.m[0], m.m[2], 
          m.m[1], m.m[3]);
}
void PrintMat3D(Mat3D m){
  printf("[%.3f] [%.3f] [%.3f]\n[%.3f] [%.3f] [%.3f]\n[%.3f] [%.3f] [%.3f]\n",
        m.m[0], m.m[3], m.m[6],
        m.m[1], m.m[4], m.m[7], 
        m.m[2], m.m[5], m.m[8]);
}
void PrintMat4D(Mat4D m){
  printf("[%.3f] [%.3f] [%.3f] [%.3f]\n[%.3f] [%.3f] [%.3f] [%.3f]\n[%.3f] [%.3f] [%.3f] [%.3f]\n[%.3f] [%.3f] [%.3f] [%.3f]\n",
          m.m[0], m.m[4],  m.m[8], m.m[12], 
          m.m[1], m.m[5],  m.m[9], m.m[13], 
          m.m[2], m.m[6], m.m[10], m.m[14], 
          m.m[3], m.m[7], m.m[11], m.m[15]);
}

// - Shapes -

float AreaSquare2D(Square2D s){
  return (s.p->x * s.p->y);
}
float AreaCircle2D(Circle2D c){
  return (PI * c.radius * c.radius);
}


}

