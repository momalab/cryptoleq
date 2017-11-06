#include <iostream>
#include "e3extensions.h"
#define MAX_NUM 50

int main()
{
    SecureInt num = E(7);
    SecureInt f1 = E(1);
    SecureInt f2 = E(1);
    SecureInt fi = E(1);
    SecureInt i = E(1);
    SecureInt result = E(0);

    int counter = 0;
    do
    {
        result += (i == num) * fi;
        fi = f1 + f2;
        f1 = f2;
        f2 = fi;
        ++i;
    }
    while (++counter < MAX_NUM);
    std::cout << "fib: " << result << "\n";
}
