//
// Created by Pablo Conde on 23/3/17.
//

#include "utils.hpp"
#include <math.h>
#include <stdlib.h>



void
utils::translatePoseMatrix(float x, float y, float z, float* matrix)
{
    // Sanity check
    if (!matrix)
        return;

    // matrix * translate_matrix
    matrix[12] +=
            (matrix[0] * x + matrix[4] * y + matrix[8]  * z);

    matrix[13] +=
            (matrix[1] * x + matrix[5] * y + matrix[9]  * z);

    matrix[14] +=
            (matrix[2] * x + matrix[6] * y + matrix[10] * z);

    matrix[15] +=
            (matrix[3] * x + matrix[7] * y + matrix[11] * z);
}


void
utils::rotatePoseMatrix(float angle, float x, float y, float z,
                              float* matrix)
{
    // Sanity check
    if (!matrix)
        return;

    float rotate_matrix[16];
    utils::setRotationMatrix(angle, x, y, z, rotate_matrix);

    // matrix * scale_matrix
    utils::multiplyMatrix(matrix, rotate_matrix, matrix);
}


void
utils::scalePoseMatrix(float x, float y, float z, float* matrix)
{
    // Sanity check
    if (!matrix)
        return;

    // matrix * scale_matrix
    matrix[0]  *= x;
    matrix[1]  *= x;
    matrix[2]  *= x;
    matrix[3]  *= x;

    matrix[4]  *= y;
    matrix[5]  *= y;
    matrix[6]  *= y;
    matrix[7]  *= y;

    matrix[8]  *= z;
    matrix[9]  *= z;
    matrix[10] *= z;
    matrix[11] *= z;
}


void
utils::multiplyMatrix(float *matrixA, float *matrixB, float *matrixC)
{
    int i, j, k;
    float aTmp[16];

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            aTmp[j * 4 + i] = 0.0;

            for (k = 0; k < 4; k++)
                aTmp[j * 4 + i] += matrixA[k * 4 + i] * matrixB[j * 4 + k];
        }
    }

    for (i = 0; i < 16; i++)
        matrixC[i] = aTmp[i];
}


void
utils::setRotationMatrix(float angle, float x, float y, float z,
                               float *matrix)
{
    double radians, c, s, c1, u[3], length;
    int i, j;

    radians = (angle * M_PI) / 180.0;

    c = cos(radians);
    s = sin(radians);

    c1 = 1.0 - cos(radians);

    length = sqrt(x * x + y * y + z * z);

    u[0] = x / length;
    u[1] = y / length;
    u[2] = z / length;

    for (i = 0; i < 16; i++)
        matrix[i] = 0.0;

    matrix[15] = 1.0;

    for (i = 0; i < 3; i++)
    {
        matrix[i * 4 + (i + 1) % 3] = u[(i + 2) % 3] * s;
        matrix[i * 4 + (i + 2) % 3] = -u[(i + 1) % 3] * s;
    }

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
            matrix[i * 4 + j] += c1 * u[i] * u[j] + (i == j ? c : 0.0);
    }
}

EasyAR::Matrix44F
utils::Matrix44FTranspose(EasyAR::Matrix44F m)
{
    EasyAR::Matrix44F r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            r.data[i*4+j] = m.data[i+4*j];
    return r;
}

float
utils::Matrix44FDeterminate(EasyAR::Matrix44F& m)
{
    return  m.data[12] * m.data[9] * m.data[6] * m.data[3] - m.data[8] * m.data[13] * m.data[6] * m.data[3] -
            m.data[12] * m.data[5] * m.data[10] * m.data[3] + m.data[4] * m.data[13] * m.data[10] * m.data[3] +
            m.data[8] * m.data[5] * m.data[14] * m.data[3] - m.data[4] * m.data[9] * m.data[14] * m.data[3] -
            m.data[12] * m.data[9] * m.data[2] * m.data[7] + m.data[8] * m.data[13] * m.data[2] * m.data[7] +
            m.data[12] * m.data[1] * m.data[10] * m.data[7] - m.data[0] * m.data[13] * m.data[10] * m.data[7] -
            m.data[8] * m.data[1] * m.data[14] * m.data[7] + m.data[0] * m.data[9] * m.data[14] * m.data[7] +
            m.data[12] * m.data[5] * m.data[2] * m.data[11] - m.data[4] * m.data[13] * m.data[2] * m.data[11] -
            m.data[12] * m.data[1] * m.data[6] * m.data[11] + m.data[0] * m.data[13] * m.data[6] * m.data[11] +
            m.data[4] * m.data[1] * m.data[14] * m.data[11] - m.data[0] * m.data[5] * m.data[14] * m.data[11] -
            m.data[8] * m.data[5] * m.data[2] * m.data[15] + m.data[4] * m.data[9] * m.data[2] * m.data[15] +
            m.data[8] * m.data[1] * m.data[6] * m.data[15] - m.data[0] * m.data[9] * m.data[6] * m.data[15] -
            m.data[4] * m.data[1] * m.data[10] * m.data[15] + m.data[0] * m.data[5] * m.data[10] * m.data[15] ;
}


EasyAR::Matrix44F
utils::Matrix44FInverse(EasyAR::Matrix44F& m)
{
    EasyAR::Matrix44F r;

    float det = 1.0f / Matrix44FDeterminate(m);

    r.data[0]   = m.data[6]*m.data[11]*m.data[13] - m.data[7]*m.data[10]*m.data[13]
                  + m.data[7]*m.data[9]*m.data[14] - m.data[5]*m.data[11]*m.data[14]
                  - m.data[6]*m.data[9]*m.data[15] + m.data[5]*m.data[10]*m.data[15];

    r.data[4]   = m.data[3]*m.data[10]*m.data[13] - m.data[2]*m.data[11]*m.data[13]
                  - m.data[3]*m.data[9]*m.data[14] + m.data[1]*m.data[11]*m.data[14]
                  + m.data[2]*m.data[9]*m.data[15] - m.data[1]*m.data[10]*m.data[15];

    r.data[8]   = m.data[2]*m.data[7]*m.data[13] - m.data[3]*m.data[6]*m.data[13]
                  + m.data[3]*m.data[5]*m.data[14] - m.data[1]*m.data[7]*m.data[14]
                  - m.data[2]*m.data[5]*m.data[15] + m.data[1]*m.data[6]*m.data[15];

    r.data[12]  = m.data[3]*m.data[6]*m.data[9] - m.data[2]*m.data[7]*m.data[9]
                  - m.data[3]*m.data[5]*m.data[10] + m.data[1]*m.data[7]*m.data[10]
                  + m.data[2]*m.data[5]*m.data[11] - m.data[1]*m.data[6]*m.data[11];

    r.data[1]   = m.data[7]*m.data[10]*m.data[12] - m.data[6]*m.data[11]*m.data[12]
                  - m.data[7]*m.data[8]*m.data[14] + m.data[4]*m.data[11]*m.data[14]
                  + m.data[6]*m.data[8]*m.data[15] - m.data[4]*m.data[10]*m.data[15];

    r.data[5]   = m.data[2]*m.data[11]*m.data[12] - m.data[3]*m.data[10]*m.data[12]
                  + m.data[3]*m.data[8]*m.data[14] - m.data[0]*m.data[11]*m.data[14]
                  - m.data[2]*m.data[8]*m.data[15] + m.data[0]*m.data[10]*m.data[15];

    r.data[9]   = m.data[3]*m.data[6]*m.data[12] - m.data[2]*m.data[7]*m.data[12]
                  - m.data[3]*m.data[4]*m.data[14] + m.data[0]*m.data[7]*m.data[14]
                  + m.data[2]*m.data[4]*m.data[15] - m.data[0]*m.data[6]*m.data[15];

    r.data[13]  = m.data[2]*m.data[7]*m.data[8] - m.data[3]*m.data[6]*m.data[8]
                  + m.data[3]*m.data[4]*m.data[10] - m.data[0]*m.data[7]*m.data[10]
                  - m.data[2]*m.data[4]*m.data[11] + m.data[0]*m.data[6]*m.data[11];

    r.data[2]   = m.data[5]*m.data[11]*m.data[12] - m.data[7]*m.data[9]*m.data[12]
                  + m.data[7]*m.data[8]*m.data[13] - m.data[4]*m.data[11]*m.data[13]
                  - m.data[5]*m.data[8]*m.data[15] + m.data[4]*m.data[9]*m.data[15];

    r.data[6]   = m.data[3]*m.data[9]*m.data[12] - m.data[1]*m.data[11]*m.data[12]
                  - m.data[3]*m.data[8]*m.data[13] + m.data[0]*m.data[11]*m.data[13]
                  + m.data[1]*m.data[8]*m.data[15] - m.data[0]*m.data[9]*m.data[15];

    r.data[10]  = m.data[1]*m.data[7]*m.data[12] - m.data[3]*m.data[5]*m.data[12]
                  + m.data[3]*m.data[4]*m.data[13] - m.data[0]*m.data[7]*m.data[13]
                  - m.data[1]*m.data[4]*m.data[15] + m.data[0]*m.data[5]*m.data[15];

    r.data[14]  = m.data[3]*m.data[5]*m.data[8] - m.data[1]*m.data[7]*m.data[8]
                  - m.data[3]*m.data[4]*m.data[9] + m.data[0]*m.data[7]*m.data[9]
                  + m.data[1]*m.data[4]*m.data[11] - m.data[0]*m.data[5]*m.data[11];

    r.data[3]   = m.data[6]*m.data[9]*m.data[12] - m.data[5]*m.data[10]*m.data[12]
                  - m.data[6]*m.data[8]*m.data[13] + m.data[4]*m.data[10]*m.data[13]
                  + m.data[5]*m.data[8]*m.data[14] - m.data[4]*m.data[9]*m.data[14];

    r.data[7]  = m.data[1]*m.data[10]*m.data[12] - m.data[2]*m.data[9]*m.data[12]
                 + m.data[2]*m.data[8]*m.data[13] - m.data[0]*m.data[10]*m.data[13]
                 - m.data[1]*m.data[8]*m.data[14] + m.data[0]*m.data[9]*m.data[14];

    r.data[11]  = m.data[2]*m.data[5]*m.data[12] - m.data[1]*m.data[6]*m.data[12]
                  - m.data[2]*m.data[4]*m.data[13] + m.data[0]*m.data[6]*m.data[13]
                  + m.data[1]*m.data[4]*m.data[14] - m.data[0]*m.data[5]*m.data[14];

    r.data[15]  = m.data[1]*m.data[6]*m.data[8] - m.data[2]*m.data[5]*m.data[8]
                  + m.data[2]*m.data[4]*m.data[9] - m.data[0]*m.data[6]*m.data[9]
                  - m.data[1]*m.data[4]*m.data[10] + m.data[0]*m.data[5]*m.data[10];

    for (int i = 0; i < 16; i++)
        r.data[i] *= det;

    return r;
}
