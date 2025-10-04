#include <iostream>
#include <iomanip>
#include "Number.h"
#include "Vector.h"

using std::cout;
using std::endl;

static void demonstrateNumberLibrary() {
    cout << "ZERO = " << ZERO.getValue() << endl;
    cout << "ONE = " << ONE.getValue() << endl;

    Number a = createNumber(5.0);
    Number b = createNumber(3.0);
    Number c = createNumber(2.0);

    cout << "a = " << a.getValue() << endl;
    cout << "b = " << b.getValue() << endl;
    cout << "c = " << c.getValue() << endl;

    Number sum = a + b;
    Number diff = a - b;
    Number prod = a * b;
    Number quot = a / b;

    cout << "a + b = " << sum.getValue() << endl;
    cout << "a - b = " << diff.getValue() << endl;
    cout << "a * b = " << prod.getValue() << endl;
    cout << "a / b = " << quot.getValue() << endl;
}

static void demonstrateVectorLibrary() {
    cout << "ZERO_VECTOR = " << ZERO_VECTOR.toString() << endl;
    cout << "ONE_VECTOR = " << ONE_VECTOR.toString() << endl;

    Vector v1(createNumber(3.0), createNumber(4.0));
    Vector v2(createNumber(1.0), createNumber(2.0));
    Vector v3(createNumber(-2.0), createNumber(2.0));

    cout << "v1 = " << v1.toString() << endl;
    cout << "v2 = " << v2.toString() << endl;
    cout << "v3 = " << v3.toString() << endl;

    cout << "v1: radius = " << v1.getRadius().getValue()
        << ", angle = " << v1.getAngleInRadians().getValue() << " radians"
        << " (" << v1.getAngleInDegrees().getValue() << " degrees)" << endl;

    cout << "v2: radius = " << v2.getRadius().getValue()
        << ", angle = " << v2.getAngleInRadians().getValue() << " radians"
        << " (" << v2.getAngleInDegrees().getValue() << " degrees)" << endl;

    cout << "v3: radius = " << v3.getRadius().getValue()
        << ", angle = " << v3.getAngleInRadians().getValue() << " radians"
        << " (" << v3.getAngleInDegrees().getValue() << " degrees)" << endl;

    Vector sum1 = v1 + v2;
    Vector sum2 = v1 + v3;

    cout << "v1 + v2 = " << sum1.toString() << endl;
    cout << "v1 + v3 = " << sum2.toString() << endl;
}

int main() {
    try {
        demonstrateNumberLibrary();
        demonstrateVectorLibrary();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
