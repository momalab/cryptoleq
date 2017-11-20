#include "o_reg.inc"

using namespace std;

#define EDS 32

#define ROUNDS 22
#define KEY_LEN 4

SecureInt E0, E1, E2, E7, EEDS;

//#define ROR(x, r) ((x >> r) | (x << ((sizeof(uint16_t) * 8) - r)))
//#define ROL(x, r) ((x << r) | (x >> ((sizeof(uint16_t) * 8) - r)))
#define ROR(x, r) (SecureInt::ror(x,r))
#define ROL(x, r) ((x << r) | (x >> (EEDS - r)))
#define R(x, y, k) (x = ROR(x, E7), x += y, x ^= k, y = ROL(y, E2), y ^= x)

void speck_expand(SecureInt K[KEY_LEN], SecureInt S[ROUNDS])
{
	SecureInt b = K[0];
	SecureInt a[KEY_LEN - 1];

	for (int i = 0; i < KEY_LEN - 1 ; i++)
	{
		a[i] = K[i + 1];
	}

	S[0] = b;
	SecureInt ei = E0;
	for (int i = 0; i < ROUNDS - 1; i++, ei+=E1)
	{
		R(a[i % (KEY_LEN - 1)], b, ei);
		S[i + 1] = b;
	}
}

int mainz(void)
{
	E0 = SecureInt::encrypt(0);
	E1 = SecureInt::encrypt(1);
	E2 = SecureInt::encrypt(2);
	E7 = SecureInt::encrypt(7);
	EEDS = SecureInt::encrypt(EDS);

	SecureInt key[KEY_LEN], exp[ROUNDS];
	key[0] = SecureInt::encrypt(256);
	key[1] = SecureInt::encrypt(2312);
	key[2] = SecureInt::encrypt(4368);
	key[3] = SecureInt::encrypt(6424);

	asm("l.debug");

	speck_expand(key, exp);

	for (int i = 0; i < ROUNDS; ++i)
	{
		cout << exp[i].str() << "\n";
	}

	asm("l.debug");
	return EXIT_SUCCESS;
}

