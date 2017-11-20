#include "o_reg.inc"

using namespace std;

SecureInt E0, E3, E6, E10, E11, E15;

SecureInt jenkins_one_at_a_time_hash(const SecureInt* key, size_t length)
{
	size_t i = 0;
	SecureInt hash = E0;
	while (i != length)
	{
		hash += key[i++];
		hash += hash << E10;
		hash ^= hash >> E6;
	}
	hash += hash << E3;
	hash ^= hash >> E11;
	hash += hash << E15;
	return hash;
}

int mainz(void)
{
	E0 = SecureInt::encrypt(0);
	E3 = SecureInt::encrypt(3);
	E6 = SecureInt::encrypt(6);
	E10 = SecureInt::encrypt(10);
	E11 = SecureInt::encrypt(11);
	E15 = SecureInt::encrypt(15);

	const size_t length = 2;
	SecureInt array[length];
	array[0] = SecureInt::encrypt(1);
	array[1] = SecureInt::encrypt(2);

	asm("l.debug");

	cout << "hash : " << jenkins_one_at_a_time_hash(array, length).str() << "\n";

	asm("l.debug");
	return 0;
}
