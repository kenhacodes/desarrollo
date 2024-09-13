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

struct Vec2 {
  float x, y;
};

struct Vec3 {
  float x, y, z;
};

struct Vec4 {
  float x, y, z, w;
};

struct Mat2 {
  /*
    m = 0 2
        1 3
  */
  float m[4];
};

struct Mat3 {
  /*
    m = 0 3 6
        1 4 7
        2 5 8
  */
  float m[9];
};

struct Mat4 {
  /*
    m = 0 4  8 12
        1 5  9 13
        2 6 10 14
        3 7 11 15
  */
  float m[16];
};

// - Shapes -

struct Triangle2{
  Vec2 p[3];
};

struct Square2{
  Vec2 p[4];
};

struct Square3{
  Vec3 p[4];
};

struct Box3{
  Vec3 p[8];
};

struct Circle2{
  Vec2 c;
  float radius;
};

struct Sphere3{
  Vec3 c;
  float radius;
};

// - Vectors -

Vec2 CoordGlobalToLocalVec2(Vec2 v, Vec2 GlobalBoundingBox_min, Vec2 GlobalBoundingBox_max){
	return (Vec2{  ( v.x - GlobalBoundingBox_min.x ) / ( GlobalBoundingBox_max.x - GlobalBoundingBox_min.x ),
                  ( v.y - GlobalBoundingBox_min.y ) / ( GlobalBoundingBox_max.y - GlobalBoundingBox_min.y )});
}
Vec2 CoordLocalToGlobalVec2(Vec2 v, Vec2 LocalBoundingBox_min, Vec2 LocalBoundingBox_max){
	return (Vec2{  LocalBoundingBox_min.x + v.x * ( LocalBoundingBox_max.x - LocalBoundingBox_min.x ), 
                  LocalBoundingBox_min.y + v.y * ( LocalBoundingBox_max.y - LocalBoundingBox_min.y ) });
}

Vec2 InitVec2(){
  return (Vec2 {0.0f , 0.0f});
}
Vec3 InitVec3(){
  return (Vec3 {0.0f , 0.0f , 0.0f});
}
Vec4 InitVec4(){
  return (Vec4 {0.0f , 0.0f , 0.0f, 0.0f});
}

Vec2 SumVec2(Vec2 v, Vec2 w){
  return (Vec2{ v.x + w.x , v.y + w.y});
};
Vec3 SumVec3(Vec3 v, Vec3 w){
  return (Vec3{ v.x + w.x, v.y + w.y, v.z + w.z});
};
Vec4 SumVec4(Vec4 v, Vec4 w){
  return (Vec4{ v.x + w.x, v.y + w.y, v.z + w.z, v.w + w.w});
};

Vec2 SubtractVec2(Vec2 v, Vec2 w){
  return (Vec2{ v.x - w.x , v.y - w.y});
}
Vec3 SubtractVec3(Vec3 v, Vec3 w){
  return (Vec3{ v.x - w.x, v.y - w.y, v.z - w.z});
};
Vec4 SubtractVec4(Vec4 v, Vec4 w){
  return (Vec4{ v.x - w.x, v.y - w.y, v.z - w.z, v.w - w.w});
};

Vec2 ScaleVec2(Vec2 vec, float s){
  return (Vec2{ vec.x * s , vec.y * s });
}
Vec3 ScaleVec3(Vec3 vec, float s){
  return (Vec3{ vec.x * s , vec.y * s , vec.z * s});
}
Vec4 ScaleVec4(Vec4 vec, float s){
  return (Vec4{ vec.x * s , vec.y * s , vec.z * s, vec.w * s});
};

Vec2 DivideByScaleVec2(Vec2 vec, float s){
  return (Vec2{ vec.x / s , vec.y / s });
}
Vec3 DivideByScaleVec3(Vec3 vec, float s){
  return (Vec3{ vec.x / s , vec.y / s , vec.z / s});
}
Vec4 DivideByScaleVec4(Vec4 vec, float s){
  return (Vec4{ vec.x / s , vec.y / s , vec.z / s, vec.w / s});
};
                                  
float MagnitudeVec2(Vec2 v){
  return sqrt(v.x * v.x + v.y * v.y);
}
float MagnitudeVec3(Vec3 v){
  return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}
float MagnitudeVec4(Vec4 v){
  return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
}

float DotVec2(Vec2 v, Vec2 w){
  return ((v.x * w.x) + (v.y * w.y));
}
float DotVec3(Vec3 v, Vec3 w){
  return ((v.x * w.x) + (v.y * w.y) + (v.z * w.z));
}
float DotVec4(Vec4 v, Vec4 w){
  return ((v.x * w.x) + (v.y * w.y) + (v.z * w.z) + (v.w * w.w));
}

int NrmlizeV2(Vec2 *v){
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
int NrmlizeV3(Vec3 *v){
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
int NrmlizeV4(Vec4 *v){
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

Vec2 NormalizeVec2(Vec2 v){
  float magn = ( sqrt((v.x * v.x) + (v.y * v.y)) );
  if (magn == 0.0f) return (Vec2{ 0 , 0 });
  return (Vec2{ v.x / magn , v.y / magn });
}
Vec3 NormalizeVec3(Vec3 v){
  float magn = ( sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)) );
  if (magn == 0.0f) return (Vec3{ 0 , 0 , 0});
  return (Vec3{ v.x / magn , v.y / magn , v.z / magn});
}
Vec4 NormalizeVec4(Vec4 v){
  float magn = ( sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w)) );
  if (magn == 0.0f) return (Vec4{ 0 , 0 , 0 , 0 });
  return (Vec4{ v.x / magn , v.y / magn , v.z / magn , v.w / magn});
}

bool IsNormalizedVec2(Vec2 v){
  float magn = sqrt((v.x * v.x) + (v.y * v.y));
  return (magn > 0.99f && magn < 1.01f);
}
bool IsNormalizedVec3(Vec3 v){
  float magn = ( sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)) );
  return (magn > 0.99f && magn < 1.01f);
};
bool IsNormalizedVec4(Vec4 v){
  float magn = ( sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w)) );
  return (magn > 0.99f && magn < 1.01f);
};

Vec2 LeftPerpendicularVec2(Vec2 v){
  return (Vec2 { -v.y , v.x });
}
Vec2 RightPerpendicularVec2(Vec2 v){
  return (Vec2 { v.y , -v.x });
}

Vec2 Vec2MultMat2Vec2(Mat2 mat, Vec2 vec) {
  return (Vec2{mat.m[0] * vec.x + mat.m[2] * vec.y,
                mat.m[1] * vec.x + mat.m[3] * vec.y});
};
Vec3 Vec3MultMat3Vec3(Mat3 mat, Vec3 vec) {
  return (Vec3{mat.m[0] * vec.x + mat.m[3] * vec.y + mat.m[6] * vec.z,
                mat.m[1] * vec.x + mat.m[4] * vec.y + mat.m[7] * vec.z,
                mat.m[2] * vec.x + mat.m[5] * vec.y + mat.m[8] * vec.z});
};
Vec4 Vec4MultMat4Vec4(Mat4 mat, Vec4 vec) {
  return (Vec4{mat.m[0] * vec.x + mat.m[3] * vec.y + mat.m[6] * vec.z + mat.m[ 9] * vec.w, 
                mat.m[1] * vec.x + mat.m[4] * vec.y + mat.m[7] * vec.z + mat.m[10] * vec.w,
                mat.m[2] * vec.x + mat.m[5] * vec.y + mat.m[8] * vec.z + mat.m[11] * vec.w});
};

float angleBetween(Vec2 v1, Vec2 v2, Vec2 reference) {
  
    Vec2 u = {v1.x - reference.x, v1.y - reference.y};
    Vec2 v = {v2.x - reference.x, v2.y - reference.y};
    
    float dot = u.x * v.x + u.y * v.y;
    float det = u.x * v.y - u.y * v.x;
    
    float angle = atan2(det, dot);
    if (angle < 0) angle += 2 * PI; 

    return angle;
}

void PrintVec2(Vec2 v) {
  printf("[X]: %.3f [Y]: %.3f\n", v.x, v.y);
}
void PrintVec3(Vec3 v) {
  printf("[X]: %.3f [Y]: %.3f [Z]: %.3f\n", v.x, v.y, v.z);
}
void PrintVec4(Vec4 v) {
  printf("[X]: %.3f [Y]: %.3f [Z]: %.3f [W]: %.3f\n", v.x, v.y, v.z, v.w);
}

// - Matrix -

Mat2 InitMat2(){
  return (Mat2{0.0f, 0.0f, 0.0f, 0.0f});
}
Mat3 InitMat3(){
  return (Mat3{0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f});
}
Mat4 InitMat4(){
  return (Mat4{0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,});
}

Mat2 MatIdentity2(){
  return (Mat2{1.0f, 0.0f,
                0.0f, 1.0f});
}
Mat3 MatIdentity3(){
  return (Mat3{1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f});
}
Mat4 MatIdentity4(){
  return (Mat4{1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,});
}

Mat2 SumMat2(Mat2 m1, Mat2 m2){
return (Mat2{  m1.m[0] + m2.m[0], m1.m[1] + m2.m[1],
                m1.m[2] + m2.m[2], m1.m[3] + m2.m[3] });
}
Mat3 SumMat3(Mat3 m1, Mat3 m2){
  return (Mat3{  m1.m[0] + m2.m[0], m1.m[1] + m2.m[1], m1.m[2] + m2.m[2],
                  m1.m[3] + m2.m[3], m1.m[4] + m2.m[4], m1.m[5] + m2.m[5],
                  m1.m[6] + m2.m[6], m1.m[7] + m2.m[7], m1.m[8] + m2.m[8] });
}
Mat4 SumMat4(Mat4 m1, Mat4 m2){
  return (Mat4{  m1.m[ 0] + m2.m[ 0], m1.m[ 1] + m2.m[ 1], m1.m[ 2] + m2.m[ 2], m1.m[ 3] + m2.m[ 3],
                  m1.m[ 4] + m2.m[ 4], m1.m[ 5] + m2.m[ 5], m1.m[ 6] + m2.m[ 6], m1.m[ 7] + m2.m[ 7],
                  m1.m[ 8] + m2.m[ 8], m1.m[ 9] + m2.m[ 9], m1.m[10] + m2.m[10], m1.m[11] + m2.m[11],
                  m1.m[12] + m2.m[12], m1.m[13] + m2.m[13], m1.m[14] + m2.m[14], m1.m[15] + m2.m[15]});
}

Mat2 SubtractMat2(Mat2 m1, Mat2 m2){
return (Mat2{  m1.m[0] - m2.m[0], m1.m[1] - m2.m[1],
                m1.m[2] - m2.m[2], m1.m[3] - m2.m[3] });
}
Mat3 SubtractMat3(Mat3 m1, Mat3 m2){
  return (Mat3{  m1.m[0] - m2.m[0], m1.m[1] - m2.m[1], m1.m[2] - m2.m[2],
                  m1.m[3] - m2.m[3], m1.m[4] - m2.m[4], m1.m[5] - m2.m[5],
                  m1.m[6] - m2.m[6], m1.m[7] - m2.m[7], m1.m[8] - m2.m[8] });
}
Mat4 SubtractMat4(Mat4 m1, Mat4 m2){
  return (Mat4{  m1.m[0] - m2.m[0], m1.m[1] - m2.m[1], m1.m[2] - m2.m[2], m1.m[3] - m2.m[3],
                  m1.m[4] - m2.m[4], m1.m[5] - m2.m[5], m1.m[6] - m2.m[6], m1.m[7] - m2.m[7],
                  m1.m[6] - m2.m[6], m1.m[7] - m2.m[7], m1.m[8] - m2.m[8], m1.m[9] - m2.m[9] });
}

Mat2 ScaleMat2(Mat2 mat, float s) {
  return (Mat2{mat.m[0] * s , mat.m[1] * s,
                mat.m[2] * s , mat.m[3] * s});
};
Mat3 ScaleMat3(Mat3 mat, float s) {
  return (Mat3{mat.m[0] * s , mat.m[1] * s , mat.m[2] * s,
                mat.m[3] * s , mat.m[4] * s , mat.m[5] * s,
                mat.m[6] * s , mat.m[7] * s , mat.m[8] * s});
};
Mat4 ScaleMat4(Mat4 mat, float s) {
  return (Mat4{mat.m[ 0] * s , mat.m[ 1] * s , mat.m[ 2] * s , mat.m[ 3] * s, 
                mat.m[ 4] * s , mat.m[ 5] * s , mat.m[ 6] * s , mat.m[ 7] * s,
                mat.m[ 8] * s , mat.m[ 9] * s , mat.m[10] * s , mat.m[11] * s,
                mat.m[12] * s , mat.m[13] * s , mat.m[14] * s , mat.m[15] * s});
};

Mat2 DivideByScaleMat2(Mat2 mat, float s) {
  return (Mat2{mat.m[0] / s , mat.m[1] / s,
               mat.m[2] / s , mat.m[3] / s});
};
Mat3 DivideByScaleMat3(Mat3 mat, float s) {
  return (Mat3{mat.m[0] / s , mat.m[1] / s , mat.m[2] / s,
                mat.m[3] / s , mat.m[4] / s , mat.m[5] / s,
                mat.m[6] / s , mat.m[7] / s , mat.m[8] / s});
};
Mat4 DivideByScaleMat4(Mat4 mat, float s) {
  return (Mat4{mat.m[ 0] / s , mat.m[ 1] / s , mat.m[ 2] / s , mat.m[ 3] / s, 
                mat.m[ 4] / s , mat.m[ 5] / s , mat.m[ 6] / s , mat.m[ 7] / s,
                mat.m[ 8] / s , mat.m[ 9] / s , mat.m[10] / s , mat.m[11] / s,
                mat.m[12] / s , mat.m[13] / s , mat.m[14] / s , mat.m[15] / s});
};

Mat3 Mat3Multiply(Mat3 m1, Mat3 m2){
  return (Mat3{m1.m[0] * m2.m[0] + m1.m[1] * m2.m[3] + m1.m[2] * m2.m[6],
               m1.m[0] * m2.m[1] + m1.m[1] * m2.m[4] + m1.m[2] * m2.m[7],
               m1.m[0] * m2.m[2] + m1.m[1] * m2.m[5] + m1.m[2] * m2.m[8],
               m1.m[3] * m2.m[0] + m1.m[4] * m2.m[3] + m1.m[5] * m2.m[6],
               m1.m[3] * m2.m[1] + m1.m[4] * m2.m[4] + m1.m[5] * m2.m[7],
               m1.m[3] * m2.m[2] + m1.m[4] * m2.m[5] + m1.m[5] * m2.m[8],
               m1.m[6] * m2.m[0] + m1.m[7] * m2.m[3] + m1.m[8] * m2.m[6],
               m1.m[6] * m2.m[1] + m1.m[7] * m2.m[4] + m1.m[8] * m2.m[7],
               m1.m[6] * m2.m[2] + m1.m[7] * m2.m[5] + m1.m[8] * m2.m[8]
              });
};

Mat3 Mat3Scale(float scale_x, float scale_y){
return (Mat3 {scale_x, 0.0f, 0.0f,
              0.0f, scale_y, 0.0f,
              0.0f, 0.0f, 1.0f});
};
Mat3 Mat3Rotate(float radians){
  return (Mat3 {cosf(radians), sinf(radians), 0.0f,
                -sinf(radians), cosf(radians), 0.0f,
                0.0f,0.0f,1.0f});
};
Mat3 Mat3Translate(Vec2 translate){

  return (Mat3 {1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                translate.x, translate.y, 1.0f});
};

Vec2 Mat2TransformVec2(Mat3 m, Vec3 v){
  return (Vec2 {m.m[0] * v.x + m.m[2] * v.y,
                m.m[1] * v.x + m.m[3] * v.y});
}
Vec3 Mat3TransformVec3(Mat3 m, Vec3 v){
  return (Vec3 {m.m[0] * v.x + m.m[3] * v.y + m.m[6] * v.z,
                m.m[1] * v.x + m.m[4] * v.y + m.m[7] * v.z,
                m.m[2] * v.x + m.m[5] * v.y + m.m[8] * v.z});
};

Mat2 TransposeMat2(Mat2 m){
  return (Mat2 { m.m[0], m.m[2],
                  m.m[1], m.m[3] });
}

Mat3 TransposeMat3(Mat3 m){
  return (Mat3 { m.m[0], m.m[3], m.m[6],
                  m.m[1], m.m[4], m.m[7],
                  m.m[2], m.m[5], m.m[8] });
}

Mat4 TransposeMat4(Mat4 m){
  return (Mat4 { m.m[0], m.m[4], m.m[8], m.m[12],
                  m.m[1], m.m[5], m.m[9], m.m[13],
                  m.m[2], m.m[6], m.m[10], m.m[14],
                  m.m[3], m.m[7], m.m[11], m.m[15] });
}

float DeterMat2 (Mat2 m){
  return ((m.m[0] * m.m[3]) - (m.m[1] * m.m[2]));
}

float GetValMat2(Mat2 m, int row, int col){
  return m.m[col * 2 + row];
}
float GetValMat3(Mat3 m, int row, int col){
  return m.m[col * 3 + row];
}
float GetValMat4(Mat4 m, int row, int col){
  return m.m[col * 4 + row];
}

void SetValMat2(Mat2 *m, int row, int col, float value){
  m->m[col * 2 + row] = value;
}
void SetValMat3(Mat3 *m, int row, int col, float value){
  m->m[col * 3 + row] = value;
}
void SetValMat4(Mat4 *m, int row, int col, float value){
  m->m[col * 4 + row] = value;
}

void PrintMat2(Mat2 m){
  printf("[%.3f] [%.3f]\n[%.3f] [%.3f]\n",
          m.m[0], m.m[2], 
          m.m[1], m.m[3]);
}
void PrintMat3(Mat3 m){
  printf("[%.3f] [%.3f] [%.3f]\n[%.3f] [%.3f] [%.3f]\n[%.3f] [%.3f] [%.3f]\n",
        m.m[0], m.m[3], m.m[6],
        m.m[1], m.m[4], m.m[7], 
        m.m[2], m.m[5], m.m[8]);
}
void PrintMat4(Mat4 m){
  printf("[%.3f] [%.3f] [%.3f] [%.3f]\n[%.3f] [%.3f] [%.3f] [%.3f]\n[%.3f] [%.3f] [%.3f] [%.3f]\n[%.3f] [%.3f] [%.3f] [%.3f]\n",
          m.m[0], m.m[4],  m.m[8], m.m[12], 
          m.m[1], m.m[5],  m.m[9], m.m[13], 
          m.m[2], m.m[6], m.m[10], m.m[14], 
          m.m[3], m.m[7], m.m[11], m.m[15]);
}

// - Shapes -

float AreaSquare2(Square2 s){
  return (s.p->x * s.p->y);
}
float AreaCircle2(Circle2 c){
  return (PI * c.radius * c.radius);
}


}