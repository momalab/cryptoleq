#include <iostream>
#include <istream>
#include "unumberz.h"

const bool NP_DBG = false;

Unumber Unumber::MOD_VALUE = 0;

inline void Unumber::init_mod_value()
{
    if ( MOD_VALUE == 0 )
    {
        Unumber a(1);
        a <<= MOD_POWER;
        MOD_VALUE = a;
    }
}

void Unumber::neg()
{
    if (z >= 0)
        return;

    if (NP_DBG) std::cout << "CCC " << ("mpz negative [" + str() + "] -> ");
    init_mod_value();

    while (z < 0)
        *this += MOD_VALUE;

    if (NP_DBG) std::cout << ("[" + str() + "]") << '\n';
}

void Unumber::pos()
{
    init_mod_value();

    if (z < MOD_VALUE.z)
        return;

    if (NP_DBG) std::cout << "CCC " << ("mpz positive [" + str() + "] -> ");

    while (z >= MOD_VALUE.z)
        *this -= MOD_VALUE;

    if (NP_DBG) std::cout << ("[" + str() + "]") << '\n';
}

Unumber::Unumber(unsigned long long x): z((unsigned long)(x)) {}
Unumber::Unumber(): z(0) {}

void Unumber::operator*=(const Unumber & n) { z *= n.z; }
void Unumber::operator+=(const Unumber & n) { z += n.z; pos(); }
void Unumber::operator-=(const Unumber & n) { z -= n.z; neg(); }
void Unumber::operator%=(const Unumber & n) { z %= n.z; }
void Unumber::operator/=(const Unumber & n) { z /= n.z; }

Unumber & Unumber::operator++() { z += 1; return *this; }
Unumber & Unumber::operator--() { z -= 1; return *this; }

bool Unumber::iszero() const { return z == 0; }

string Unumber::str(unsigned base) const { return z.get_str(base); }

unsigned long long Unumber::to_ull() const { return z.get_ui(); }

void Unumber::operator<<=(int b) { mpz_mul_2exp (z.get_mpz_t(), z.get_mpz_t(), b); }
void Unumber::operator>>=(int b) { mpz_div_2exp (z.get_mpz_t(), z.get_mpz_t(), b); }

bool operator<(const Unumber & n1, const Unumber & n2) { return n1.z < n2.z; }

Unumber::Unumber(const string & s, Unumber::StringType st)
{
    if ( st == Decimal ) z = s;
    if ( st == Binary ) throw "GMP binary initialization not implemented";
}

Unumber Unumber::div(const Unumber & d, Unumber & q) const
{
    Unumber r;
    mpz_fdiv_qr (q.z.get_mpz_t(), r.z.get_mpz_t(), z.get_mpz_t(), d.z.get_mpz_t());
    return r;
}

void Unumber::divABRQ(const Unumber & d, Unumber * r, Unumber * q) const
{
    Unumber m;

    if (q)
        mpz_fdiv_qr (q->z.get_mpz_t(), m.z.get_mpz_t(), z.get_mpz_t(), d.z.get_mpz_t());
    else
        mpz_fdiv_r (m.z.get_mpz_t(), z.get_mpz_t(), d.z.get_mpz_t());

    if (r) *r = m;
}

Unumber Unumber::mul(const Unumber & b, const Unumber & m) const
{
    Unumber r;
    mpz_mul(r.z.get_mpz_t(), z.get_mpz_t(), b.z.get_mpz_t());
    mpz_mod(r.z.get_mpz_t(), r.z.get_mpz_t(), m.z.get_mpz_t());
    return r;
}

unsigned Unumber::Unumber::getbit(int b) const { return mpz_tstbit(z.get_mpz_t(), b); }

void Unumber::setbit1(int b)
{
    if (b)
        mpz_setbit(z.get_mpz_t(), b);
    else
        mpz_clrbit(z.get_mpz_t(), b);
}


std::istream & operator>>(std::istream & is, Unumber & n)
{
    std::string s; is >> s;

    if(!s.empty() && !!is ) 
    {
        Unumber u(s, Unumber::Decimal);
        n.swap(u);
    }

    return is;
}

void Unumber::pow(Unumber e, const Unumber & mod)
{
    mpz_powm(z.get_mpz_t(), z.get_mpz_t(), e.z.get_mpz_t(), mod.z.get_mpz_t());
}
