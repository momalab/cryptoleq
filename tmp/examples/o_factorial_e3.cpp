#include "o_reg.inc"

using namespace std;

#define MAX_NUM 50
#define NUM 5

int mainz()
{
	SecureInt num = SecureInt::encrypt(5_E);
	SecureInt fact = SecureInt::encrypt(1_E);
	SecureInt i = SecureInt::encrypt(1_E);
	SecureInt result = SecureInt::encrypt(0_E);
	
	asm("l.debug");

	int counter = 0;
	do {
		fact *= i;
		result += (i == num) * fact;
		++i;
	} while (++counter < MAX_NUM);
	cout << "fact( " << num.str() << " ) = " << result.str() << "\n";

	asm("l.debug");
	return 0;
}

