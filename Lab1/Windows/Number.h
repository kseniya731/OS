#ifndef NUMBER_H
#define NUMBER_H

class Number {
private:
    double value;

public:
    Number();
    Number(double val);
    Number(const Number& other);

    Number& operator=(const Number& other);
    Number& operator=(double val);

    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;

    bool operator==(const Number& other) const;
    bool operator!=(const Number& other) const;

    double getValue() const;
    void setValue(double val);

    operator double() const;
};

extern Number ZERO;
extern Number ONE;

Number createNumber(double value);

#endif