#ifndef VECTOR_H
#define VECTOR_H

#include <string>
//#include "..\Number\Number.h"
#include "Number.h"

class Vector {
private:
    Number x;
    Number y;

public:
    Vector();
    Vector(const Number& x_val, const Number& y_val);
    Vector(double x_val, double y_val);

    Number getX() const;
    Number getY() const;
    void setX(const Number& x_val);
    void setY(const Number& y_val);

    Number getRadius() const;
    Number getAngleInRadians() const;
    Number getAngleInDegrees() const;

    Vector operator+(const Vector& other) const;

    std::string toString() const;
};

extern Vector ZERO_VECTOR;
extern Vector ONE_VECTOR;

#endif
