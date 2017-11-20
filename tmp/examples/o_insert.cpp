#include "o_reg.inc"
using namespace std;
 
int mainz(void)
{
	int n = 10, i, j;
	SecureInt array[10];
	array[0] = SecureInt::encrypt(4);
	array[1] = SecureInt::encrypt(2);
	array[2] = SecureInt::encrypt(5);
	array[3] = SecureInt::encrypt(9);
	array[4] = SecureInt::encrypt(1);
	array[5] = SecureInt::encrypt(0);
	array[6] = SecureInt::encrypt(4);
	array[7] = SecureInt::encrypt(3);
	array[8] = SecureInt::encrypt(9);
	array[9] = SecureInt::encrypt(8);

	SecureInt zero = SecureInt::encrypt(0);

	asm("l.debug");

	for (i = 1 ; i < n; i++)
	{
		j = i;
		while (j != 0)
		{
			SecureInt x = array[j-1];
			SecureInt y = array[j];

			SecureInt xmy = x-y;
			SecureInt ymx = y-x;
			SecureInt cond = xmy <= zero;
			//cout << "1\n";
			SecureInt diff = (xmy <= zero) * xmy + (ymx <= zero) * ymx;
			//SecureInt diff = cond.ternary(xmy, ymx);
			SecureInt max = (xmy <= zero) * x + (ymx <= zero) * y;
			//SecureInt max = cond.ternary(x, y);
			max += (x == y) * x; // if x == y set max to x
			SecureInt min = max - diff;
			//cout << "2\n";
			array[j-1] = min;
			array[j] = max;
			j--;
		}
	}

	for (i = 0; i < n; i++)
	{
		cout << array[i].str() << " ";
	}
	cout << "\n";
	
	asm("l.debug");
	return 0;
}

