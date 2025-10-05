#ifndef VECTOR_H
#define VECTOR_H

#ifdef VECTOR_EXPORTS
#define VECTOR_API __declspec(dllexport)
#else
#define VECTOR_API __declspec(dllimport)
#endif

#include <string>
#include "..\Number\Number.h"

class VECTOR_API Vector {
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

extern VECTOR_API Vector ZERO_VECTOR;
extern VECTOR_API Vector ONE_VECTOR;

#endif