#include "Number.h"
#include <stdexcept>

Number::Number() : value(0.0) {}

Number::Number(double val) : value(val) {}

Number::Number(const Number& other) : value(other.value) {}

Number& Number::operator=(const Number& other) {
    if (this != &other) {
        value = other.value;
    }
    return *this;
}

Number& Number::operator=(double val) {
    value = val;
    return *this;
}

Number Number::operator+(const Number& other) const {
    return Number(value + other.value);
}

Number Number::operator-(const Number& other) const {
    return Number(value - other.value);
}

Number Number::operator*(const Number& other) const {
    return Number(value * other.value);
}

Number Number::operator/(const Number& other) const {
    if (other.value == 0.0) {
        throw std::runtime_error("Division by zero");
    }
    return Number(value / other.value);
}

bool Number::operator==(const Number& other) const {
    return value == other.value;
}

bool Number::operator!=(const Number& other) const {
    return value != other.value;
}

double Number::getValue() const {
    return value;
}

void Number::setValue(double val) {
    value = val;
}

Number::operator double() const {
    return value;
}

Number ZERO(0.0);
Number ONE(1.0);

Number createNumber(double value) {
    return Number(value);
}