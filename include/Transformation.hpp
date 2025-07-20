#ifndef __Transformation__
#define __Transformation__

#include "Vec3.hpp"
#include "Point3.hpp"
#include "Normal3.hpp"

/**
 * @brief Enum to represent coordinate axes for rotations and scaling.
 */
enum class Axis {X, Y, Z};

inline constexpr float IDENTITY4[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, 1.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 1.0f};

// This explicit matrix multiplication is around 4.5 times faster than using for loops.
// There is no gain in speed in the rendering since matrix multiplication is only used
// in Transformation initialization. Still, there's no reason not to use this.
inline void matrixMult(const float A[16], const float B[16], float result[16]) {
    result[0] = A[0] * B[0] + A[1] * B[4] + A[2] * B[8] + A[3] * B[12];
    result[1] = A[0] * B[1] + A[1] * B[5] + A[2] * B[9] + A[3] * B[13];
    result[2] = A[0] * B[2] + A[1] * B[6] + A[2] * B[10] + A[3] * B[14];
    result[3] = A[0] * B[3] + A[1] * B[7] + A[2] * B[11] + A[3] * B[15];
    result[4] = A[4] * B[0] + A[5] * B[4] + A[6] * B[8] + A[7] * B[12];
    result[5] = A[4] * B[1] + A[5] * B[5] + A[6] * B[9] + A[7] * B[13];
    result[6] = A[4] * B[2] + A[5] * B[6] + A[6] * B[10] + A[7] * B[14];
    result[7] = A[4] * B[3] + A[5] * B[7] + A[6] * B[11] + A[7] * B[15];
    result[8] = A[8] * B[0] + A[9] * B[4] + A[10] * B[8] + A[11] * B[12];
    result[9] = A[8] * B[1] + A[9] * B[5] + A[10] * B[9] + A[11] * B[13];
    result[10] = A[8] * B[2] + A[9] * B[6] + A[10] * B[10] + A[11] * B[14];
    result[11] = A[8] * B[3] + A[9] * B[7] + A[10] * B[11] + A[11] * B[15];
    result[12] = A[12] * B[0] + A[13] * B[4] + A[14] * B[8] + A[15] * B[12];
    result[13] = A[12] * B[1] + A[13] * B[5] + A[14] * B[9] + A[15] * B[13];
    result[14] = A[12] * B[2] + A[13] * B[6] + A[14] * B[10] + A[15] * B[14];
    result[15] = A[12] * B[3] + A[13] * B[7] + A[14] * B[11] + A[15] * B[15];
}

/**
 * @brief Class representing a 3D affine transformation with forward and inverse matrices.
 * 
 * Supports application to vectors, points, normals, and composition of transformations.
 */
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

    inline Transformation inverse() const {
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

// scaling
inline Transformation scaling(float x, float y, float z) {
    float mat[16] = {0.0f}, inv[16] = {0.0f};
    mat[0] = x, mat[5] = y, mat[10] = z, mat[15] = 1.0f;
    inv[0] = 1.0f / x, inv[5] = 1.0f / y, inv[10] = 1.0f / z, inv[15] = 1.0f;
    return Transformation(mat, inv);
}

inline Transformation scaling(const Vec3& vec) {
    return scaling(vec.x, vec.y, vec.z);
}

inline Transformation scaling(float s, Axis axis) {
    if (axis == Axis::X) return scaling(s, 1.0f, 1.0f);
    if (axis == Axis::Y) return scaling(1.0f, s, 1.0f);
    return scaling(1.0f, 1.0f, s); // Z
}

inline Transformation scaling(float s) {
    return scaling(s, s, s);
}

// translations
inline Transformation translation(float x, float y, float z) {
    Transformation trasl(IDENTITY4, IDENTITY4);
    trasl.matrix[3] = x, trasl.matrix[7] = y, trasl.matrix[11] = z;
    trasl.inverseMatrix[3] = -x, trasl.inverseMatrix[7] = -y, trasl.inverseMatrix[11] = -z;
    return trasl;
}

inline Transformation translation(const Vec3& vec) {
    return translation(vec.x, vec.y, vec.z);
}

// rotation
inline Transformation rotation(float angle, Axis axis) {
    float mat[16] = {0.0f}, inv[16] = {0.0f};
    mat[15] = 1.0f, inv[15] = 1.0f; // element [3][3] is always 1

    float theta = degToRad(angle);
    float cos = std::cos(theta), sin = std::sin(theta);

    switch (axis) {
        case Axis::X:
            mat[0] = 1.0f;
            mat[5] = cos, mat[6] = -sin;
            mat[9] = sin, mat[10] = cos;
        
            inv[0] = 1.0f;
            inv[5] = cos, inv[6] = sin;
            inv[9] = -sin, inv[10] = cos;
            break;
        
        case Axis::Y:
            mat[0] = cos, mat[2] = sin;
            mat[5] = 1.0f;
            mat[8] = -sin, mat[10] = cos;
        
            inv[0] = cos, inv[2] = -sin;
            inv[5] = 1.0f;
            inv[8] = sin, inv[10] = cos;
            break;
        
        case Axis::Z:
            mat[0] = cos, mat[1] = -sin;
            mat[4] = sin, mat[5] = cos;
            mat[10] = 1.0f;
        
            inv[0] = cos, inv[1] = sin;
            inv[4] = -sin, inv[5] = cos;
            inv[10] = 1.0f;
            break;
        }
        
    return Transformation(mat, inv);
}

#endif