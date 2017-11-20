#include "o_reg.inc"

using namespace std;

/********************************************************
** Create a boolean array "prime[0..n]" and initialize
** all entries it as true. A value in prime[i] will
** finally be false if i is Not a prime, else true.
********************************************************/

SecureInt zero, one, pe;

void sieveOfEratosthenes(int n)
{
	SecureInt prime[101];
	for (int i = 0 ; i < n+1 ; i++) 
        	prime[i] = one;

	for (int p = 2; p*p < n+1 ; p++)
	{
		for (int i = 2*p; i < n+1 ; i += p)	// Update all multiples of p
			prime[i] = zero;
	}

	for (int p = 2; p < n+1; p++, pe+=one) // Print all prime numbers
	{
		cout << (pe * prime[p]).str() << " ";
	}
	cout<<'\n';
}

int mainz(void)
{
	zero = SecureInt::encrypt(0);
	one = SecureInt::encrypt(1);
	pe = SecureInt::encrypt(2);

	asm("l.debug");

	int n = 100;
	sieveOfEratosthenes(n);

	asm("l.debug");
	return 0;
}
