#include "o_reg.inc"

using namespace std;

#define MAX_NUM 50

int mainz()
{
	SecureInt num = (12_E);
	SecureInt f1 = (0_E);
	SecureInt f2 = (1_E);
	SecureInt fi = (1_E);
	SecureInt i = (1_E);
	SecureInt result = (0_E);

	//asm("l.debug");

	int counter = 0;
	do {
		result += (i == num) * fi;
		fi = f1 + f2;
		f1 = f2;
		f2 = fi;
		++i;
	} while (++counter < MAX_NUM);
	cout << "fib( " << num.str() << " ) = " << result.str() << "\n";

	//asm("l.debug");
	return 0;
}

