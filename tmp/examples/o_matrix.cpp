#include "o_reg.inc"

using namespace std;

int mainz()
{
	SecureInt a[3][3], b[3][3], c[3][3];
	SecureInt zero = SecureInt::encrypt(0);

	a[0][0] = SecureInt::encrypt(1);
	a[0][1] = SecureInt::encrypt(2);
	a[0][2] = SecureInt::encrypt(3);
	a[1][0] = SecureInt::encrypt(2);
	a[1][1] = SecureInt::encrypt(3);
	a[1][2] = SecureInt::encrypt(4);
	a[2][0] = SecureInt::encrypt(3);
	a[2][1] = SecureInt::encrypt(4);
	a[2][2] = SecureInt::encrypt(5);

	b[0][0] = SecureInt::encrypt(1);
	b[0][1] = SecureInt::encrypt(2);
	b[0][2] = SecureInt::encrypt(3);
	b[1][0] = SecureInt::encrypt(2);
	b[1][1] = SecureInt::encrypt(3);
	b[1][2] = SecureInt::encrypt(4);
	b[2][0] = SecureInt::encrypt(3);
	b[2][1] = SecureInt::encrypt(4);
	b[2][2] = SecureInt::encrypt(5);

	asm("l.debug");

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			c[i][j] = zero;
			for (int k = 0; k < 3; k++)
			{
				c[i][j] += a[i][k] * b[k][j];
				//c[i][j].mac(a[i][k], b[k][j]);
			}
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++) cout << c[i][j].str() << " ";
	}
		cout << "\n";

	asm("l.debug");
	return 0;
}

