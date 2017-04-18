//
// Created by Pablo Conde on 23/3/17.
//

#ifndef HELLOARMULTITARGET_MULTITRACKER_NATIVE_UTILS_H
#define HELLOARMULTITARGET_MULTITRACKER_NATIVE_UTILS_H

// Includes:
#include <stdio.h>
#include "ar.hpp"

class utils {
public:
    /// Set the rotation components of this 4x4 matrix.
    static void setRotationMatrix(float angle, float x, float y, float z,
                                  float *nMatrix);

    /// Set the translation components of this 4x4 matrix.
    static void translatePoseMatrix(float x, float y, float z,
                                    float* nMatrix = NULL);

    /// Applies a rotation.
    static void rotatePoseMatrix(float angle, float x, float y, float z,
                                 float* nMatrix = NULL);

    /// Applies a scaling transformation.
    static void scalePoseMatrix(float x, float y, float z,
                                float* nMatrix = NULL);

    /// Multiplies the two matrices A and B and writes the result to C.
    static void multiplyMatrix(float *matrixA, float *matrixB,
                               float *matrixC);

    static EasyAR::Matrix44F Matrix44FTranspose(EasyAR::Matrix44F m);

    static float Matrix44FDeterminate(EasyAR::Matrix44F& m);

    static EasyAR::Matrix44F Matrix44FInverse(EasyAR::Matrix44F& m);

};


#endif //HELLOARMULTITARGET_MULTITRACKER_NATIVE_UTILS_H
