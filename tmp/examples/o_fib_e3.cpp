#define main mainz
#include "../regman.cpp"
#undef main

typedef Number SecureInt;
#define asm(x)

using namespace std;

#define MAX_NUM 50
#define NUM 12

int main()
try
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
	cout << "fib( " << num.str() << " ) = " << result.str() << "\n";

    cout << "loads: " << cpu.ld_cntr << "  strores: " << cpu.st_cntr << '\n';
    cout << cpu.REGNUM << '\t' << cpu.ld_cntr << '\t' << cpu.st_cntr << '\n';

	//asm("l.debug");
	return 0;
}
catch(...)
{
	cout<<"Error ex\n";
	return 1;
}

