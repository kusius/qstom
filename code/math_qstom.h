#ifndef __MATH_QSTOM_H_
#define __MATH_QSTOM_H_

#include <math.h>

//TODO: MSCVC compiler same method with __forceinline
#define Q_INLINE static inline __attribute((always_inline))
#define PI32 3.1415926
#define DEG2RAD PI32/180

typedef float vec3[3];
typedef float mat4[4][4];

#define MAT4_IDENTITY \
    {1.0, 0.0, 0.0, 0.0,     \
     0.0, 1.0, 0.0, 0.0,     \
     0.0, 0.0, 1.0, 0.0,     \
     0.0, 0.0, 0.0, 1.0}

#define MAT4_ZERO \
    {0.0, 0.0, 0.0, 0.0,     \
     0.0, 0.0, 0.0, 0.0,     \
     0.0, 0.0, 0.0, 0.0,     \
     0.0, 0.0, 0.0, 0.0}

//TODO: Different copy if matrices are byte aligned
Q_INLINE void
CopyMatrix(mat4 mat, mat4 dest)
{
  dest[0][0] = mat[0][0];  dest[1][0] = mat[1][0];
  dest[0][1] = mat[0][1];  dest[1][1] = mat[1][1];
  dest[0][2] = mat[0][2];  dest[1][2] = mat[1][2];
  dest[0][3] = mat[0][3];  dest[1][3] = mat[1][3];

  dest[2][0] = mat[2][0];  dest[3][0] = mat[3][0];
  dest[2][1] = mat[2][1];  dest[3][1] = mat[3][1];
  dest[2][2] = mat[2][2];  dest[3][2] = mat[3][2];
  dest[2][3] = mat[2][3];  dest[3][3] = mat[3][3];
}

Q_INLINE void
PerspectiveMatrix(float FieldOfView,
                  float AspectRatio,
                  float NearClip,
                  float FarClip,
                  mat4 Out)
{
  mat4 t = MAT4_ZERO;
  CopyMatrix(t, Out);
 
  float f, fn;
  f = 1.0f / tanf(DEG2RAD * FieldOfView * 0.5f);
  fn = 1.0f / (NearClip - FarClip);
  Out[0][0] = f / AspectRatio;
  Out[1][1] = f;
  Out[2][2] = (NearClip + FarClip) * fn;
  Out[2][3] = -1.0f;
  Out[3][2] = 2.0f * NearClip * FarClip * fn;
}

//TODO: SIMD
Q_INLINE void
Multiply(mat4 m1, mat4 m2, mat4 Out)
{
    float a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3],
        a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3],
        a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3],
        a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3],

        b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2], b03 = m2[0][3],
        b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2], b13 = m2[1][3],
        b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2], b23 = m2[2][3],
        b30 = m2[3][0], b31 = m2[3][1], b32 = m2[3][2], b33 = m2[3][3];

  Out[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
  Out[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
  Out[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
  Out[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
  Out[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
  Out[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
  Out[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
  Out[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
  Out[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
  Out[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
  Out[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
  Out[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
  Out[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
  Out[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
  Out[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
  Out[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
}

//TODO: SIMD
Q_INLINE void
RotateTransform(mat4 InputT, float Angle, const vec3& Axis, mat4 DestT)
{
    float Cos = cosf(DEG2RAD * Angle);
    float One_Cos = 1 - Cos;
    float Sin   = sinf(DEG2RAD * Angle);
    float Rx = Axis[0], Ry = Axis[1], Rz = Axis[2];
    mat4 MulMatrix;

    MulMatrix[0][0] = Cos + Rx*Rx*One_Cos; MulMatrix[1][0] = Rx*Ry*One_Cos - Rz * Sin;
    MulMatrix[2][0] = Rx*Rz*One_Cos + Ry*Sin; MulMatrix[3][0] = 0;
    MulMatrix[0][1] = Ry*Rx*One_Cos + Rz*Sin; MulMatrix[1][1] = Cos + Ry*Ry*One_Cos;
    MulMatrix[2][1] = Ry*Rz*One_Cos - Rx*Sin; MulMatrix[3][1] = 0;
    MulMatrix[0][2] = Rz*Rx*One_Cos - Ry*Sin; MulMatrix[1][2] = Rz*Ry*One_Cos + Rx*Sin;
    MulMatrix[2][2] = Cos + Rz*Rz*One_Cos; MulMatrix[3][2] = 0;
    MulMatrix[0][3] = 0; MulMatrix[1][3] = 0;
    MulMatrix[2][3] = 0; MulMatrix[3][3] = 1;
    Multiply(MulMatrix, InputT, DestT);
}

//TODO: SIMD
Q_INLINE void
ScaleTransform(mat4 InputT, float ScaleX, float ScaleY,
               float ScaleZ, mat4 DestT)
{
    mat4 ScaleMatrix = MAT4_IDENTITY;
    ScaleMatrix[0][0] =  ScaleX ;
    ScaleMatrix[1][1] =  ScaleY ;
    ScaleMatrix[2][2] =  ScaleZ ;
    Multiply(ScaleMatrix, InputT, DestT);
}

//TODO: SIMD
Q_INLINE void
TranslateTransform(mat4 InputT, float TransX, float TransY,
                   float TransZ, mat4 DestT)
{
    mat4 TransMatrix = MAT4_IDENTITY;
    TransMatrix[3][0] = TransX;
    TransMatrix[3][1] = TransY;
    TransMatrix[3][2] = TransZ;
    TransMatrix[3][3] = 1;
    Multiply(TransMatrix, InputT, DestT);
}

#endif // __MATH_QSTOM_H_
