#ifndef __Transformation__
#define __Transformation__

#include "Vec3.hpp"
#include "Point3.hpp"
#include "Normal3.hpp"



enum class Axis {X, Y, Z};

const float IDENTITY4[16] = {1., 0., 0., 0.,
                             0., 1., 0., 0.,
                             0., 0., 1., 0.,
                             0., 0., 0., 1.};

inline void matrixMult(const float A[16], const float B[16], float result[16]) {
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            float sum = 0.;
            for (int k = 0; k < 4; k++) {
                sum += A[k + 4 * j] * B[i + 4 * k]; // nesting 3 times is kinda ugly
            }
            result[i + 4 * j] = sum; // since we use operator +=, we would need to make sure each element of result is 0
        }
    }
}



class Transformation {
public:
    float matrix[16] = {0.}, inverseMatrix[16] = {0.};
    
    Transformation() : Transformation(IDENTITY4, IDENTITY4) {}
    Transformation(const float mat[16], const float inv[16]) {
        for (int i = 0; i < 16; i++) {
            matrix[i] = mat[i];
            inverseMatrix[i] = inv[i];
        }
    }

    inline Transformation inverse() {
        return Transformation(inverseMatrix, matrix);
    }

    inline Vec3 operator*(const Vec3& v) const {
        Vec3 result;
        result.x = matrix[0] * v.x + matrix[1] * v.y + matrix[2] * v.z;
        result.y = matrix[4] * v.x + matrix[5] * v.y + matrix[6] * v.z;
        result.z = matrix[8] * v.x + matrix[9] * v.y + matrix[10] * v.z;
        return result;
    }

    inline Point3 operator*(const Point3& p) const {
        Point3 result;
        result.x = matrix[0] * p.x + matrix[1] * p.y + matrix[2] * p.z + matrix[3];
        result.y = matrix[4] * p.x + matrix[5] * p.y + matrix[6] * p.z + matrix[7];
        result.z = matrix[8] * p.x + matrix[9] * p.y + matrix[10] * p.z + matrix[11];
        return result;
    }

    inline Normal3 operator*(const Normal3& n) const { // to transform a normal we use the transposed matrix
        Normal3 result;
        result.x = inverseMatrix[0] * n.x + inverseMatrix[4] * n.y + inverseMatrix[8] * n.z;
        result.y = inverseMatrix[1] * n.x + inverseMatrix[5] * n.y + inverseMatrix[9] * n.z;
        result.z = inverseMatrix[2] * n.x + inverseMatrix[6] * n.y + inverseMatrix[10] * n.z;
        return result;
    }

    inline Transformation operator*(const Transformation& other) const {
        float mat[16], inv[16];
        matrixMult(matrix, other.matrix, mat);
        matrixMult(other.inverseMatrix, inverseMatrix, inv);
        return Transformation(mat, inv);
    }

    bool isConsistent() {
        float result[16];
        matrixMult(matrix, inverseMatrix, result);
        return areCloseMatrix(IDENTITY4, result);
    }

    bool isClose(const Transformation& other, float epsilon = 1e-5f) const {
        return areCloseMatrix(this->matrix, other.matrix, epsilon) &&
               areCloseMatrix(this->inverseMatrix, other.inverseMatrix, epsilon);
    }
    
};



Transformation scaling(float x, float y, float z) {
    float mat[16] = {0.}, inv[16] = {0.};
    mat[0] = x, mat[5] = y, mat[10] = z, mat[15] = 1.;
    inv[0] = 1. / x, inv[5] = 1. / y, inv[10] = 1. / z, inv[15] = 1.;
    return Transformation(mat, inv);
}
Transformation scaling(const Vec3& vec) {
    return scaling(vec.x, vec.y, vec.z);
}
Transformation scaling(float s, Axis axis) {
    if (axis == Axis::X) return scaling(s, 1., 1.);
    if (axis == Axis::Y) return scaling(1., s, 1.);
    return scaling(1., 1., s); // Z
}

Transformation translation(float x, float y, float z) {
    Transformation trasl(IDENTITY4, IDENTITY4);
    trasl.matrix[3] = x, trasl.matrix[7] = y, trasl.matrix[11] = z;
    trasl.inverseMatrix[3] = -x, trasl.inverseMatrix[7] = -y, trasl.inverseMatrix[11] = -z;
    return trasl;
}
Transformation translation(const Vec3& vec) {
    return translation(vec.x, vec.y, vec.z);
}

Transformation rotation(float angle, Axis axis) {
    float mat[16] = {0.}, inv[16] = {0.};
    mat[15] = 1., inv[15] = 1.; // element [3][3] is always 1

    float theta = degToRad(angle);
    float cos = std::cos(theta), sin = std::sin(theta);

    switch (axis) {
    case Axis::X:
        mat[0] = 1.;
        mat[5] = cos, mat[6] = -sin;
        mat[9] = sin, mat[10] = cos;

        mat[0] = 1.;
        mat[5] = cos, mat[6] = sin;
        mat[9] = -sin, mat[10] = cos;
        break;
    case Axis::Y:
        mat[0] = cos, mat[2] = sin;
        mat[5] = 1.;
        mat[8] = -sin, mat[10] = cos;

        mat[0] = cos, mat[2] = -sin;
        mat[5] = 1.;
        mat[8] = sin, mat[10] = cos;
        break;
    case Axis::Z:
        mat[0] = cos, mat[1] = -sin;
        mat[4] = sin, mat[5] = cos;
        mat[10] = 1.;

        inv[0] = cos, inv[1] = sin;
        inv[4] = -sin, inv[5] = cos;
        inv[10] = 1.;
        break;

        return Transformation(mat, inv);
    }

    return Transformation(mat, inv);
}

#endif