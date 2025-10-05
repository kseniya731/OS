//#define VECTOR_EXPORTS
#include "Vector.h"
#include <cmath>
#include <sstream>
#include <stdexcept>

const double PI = 3.141592653589793;

extern Number ZERO;
extern Number ONE;

Vector::Vector() : x(ZERO), y(ZERO) {}

Vector::Vector(const Number& x_val, const Number& y_val) : x(x_val), y(y_val) {}

Vector::Vector(double x_val, double y_val) : x(createNumber(x_val)), y(createNumber(y_val)) {}

Vector ZERO_VECTOR(ZERO, ZERO);
Vector ONE_VECTOR(ONE, ONE);

Number Vector::getX() const {
    return x;
}

Number Vector::getY() const {
    return y;
}

void Vector::setX(const Number& x_val) {
    x = x_val;
}

void Vector::setY(const Number& y_val) {
    y = y_val;
}

Number Vector::getRadius() const {
    Number x_squared = x * x;
    Number y_squared = y * y;
    Number sum = x_squared + y_squared;
    return createNumber(sqrt(sum.getValue()));
}

Number Vector::getAngleInRadians() const {
    if (x == ZERO && y == ZERO) {
        return ZERO;
    }
    return createNumber(atan2(y.getValue(), x.getValue()));
}

Number Vector::getAngleInDegrees() const {
    Number angle_rad = getAngleInRadians();
    return angle_rad * createNumber(180.0 / PI);
}

Vector Vector::operator+(const Vector& other) const {
    return Vector(x + other.x, y + other.y);
}

std::string Vector::toString() const {
    std::stringstream ss;
    ss << "(" << x.getValue() << ", " << y.getValue() << ")";
    return ss.str();
}
