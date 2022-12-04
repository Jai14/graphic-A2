#include "vector.h"
#include <math.h>
#include <stdio.h>

// default constructor
Vector::Vector() {
    dx = dy = dz = 0.0f;
}

// copy constructor
Vector::Vector(float v_dx, float v_dy, float v_dz) {
    dx = v_dx;
    dy = v_dy;
    dz = v_dz;
}

// Copy another Vector & return self
Vector &Vector::operator=(Vector vector) {
    for (int i = 0; i < 3; i++)
        (*this)[i] = vector[i];
    return (*this);
}

// Scalar right-multiplication
Vector Vector::operator*(float scaleFactor) {
    Vector result;
    for (int i = 0; i < 3; i++)
        result[i] = (*this)[i] * scaleFactor;
    return result;
}

// Scalar right-division
Vector Vector::operator/(float scaleFactor) {
    Vector result;
    for (int i = 0; i < 3; i++)
        result[i] = (*this)[i] / scaleFactor;
    return result;
}

// scalar left-multiplication
Vector operator*(float scaleFactor,
                 Vector vector) {
    Vector result;
    for (int i = 0; i < 3; i++)
        result[i] = scaleFactor * vector[i];
    return result;
}

// Computes the length (sqrt of sum of squares)
float Vector::norm() {
    return static_cast<float>(sqrt(dot(*this)));
}

// Return the unit (normalized) vector
Vector Vector::normalised() {
    Vector result = (*this) / norm();
    return result;
}

// Add a vector to a vector to get another vector
// Does so in Cartesian space, but leaves w-coordinate the same
Vector Vector::operator+(Vector vector) {
    Vector result;
    for (int i = 0; i < 3; i++)
        result[i] = (*this)[i] + vector[i];
    return result;
}

// Add a point to a vector to get another point
// Does so in Cartesian space, but leaves w-coordinate the same
Point Vector::operator+(Point point) {
    Point result;
    for (int i = 0; i < 3; i++)
        result[i] = (*this)[i] * point[3] + point[i];
    result[3] = point[3];
    return result;
}

// Negation (unary minus)
Vector Vector::operator-() {
    return (*this) * -1;
}

// Subtraction (returns a vector)
Vector Vector::operator-(Vector vector) {
    Vector result;
    for (int i = 0; i < 3; i++)
        result[i] = (*this)[i] - vector[i];
    return result;
}

// Compute the cross-product (this x vector)
Vector Vector::cross(Vector vector) {
    Vector result;
    result.dx = dy * vector.dz - dz * vector.dy;
    result.dy = dz * vector.dx - dx * vector.dz;
    result.dz = dx * vector.dy - dy * vector.dx;
    return result;
}

// Dot product computes (this . vector)
float Vector::dot(Vector vector) {
    return (dx * vector.dx + dy * vector.dy + dz * vector.dz);
}

float &Vector::operator[](int i) {
    switch (i) {
    case 0:
        return dx;
    case 1:
        return dy;
    case 2:
        return dz;
    default:
        return dz;
    }
}
