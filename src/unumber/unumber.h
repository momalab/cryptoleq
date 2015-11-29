#ifndef UNUMBER_H__
#define UNUMBER_H__

#include <sstream>
#include <string>
#include <ostream>
#include <istream>
#include <cctype>
#include <cstring>

#include "cunumber.h"

using std::string;

class Unumber
{
        Cunumber z;

    public:
        Unumber(unsigned long long x);
        Unumber() {}

        enum StringType { Binary, Decimal };
        Unumber(const string & s, StringType st);

        void init10(const char * s);
        void initBin(const string & s);

        void operator-=(const Unumber & n) { csub(&z, &n.z); }
        void operator+=(const Unumber & n) { z = cadd(&z, &n.z); }
        void operator*=(const Unumber & n) { z = cmul(&z, &n.z, 0); }
        void operator%=(const Unumber & n) { z = cdiv_QnotND(&z, &n.z, 0); }
        void operator/=(const Unumber & n) { Cunumber a; cdiv_QnotND(&z, &n.z, &a); z = a; }

        void setbit1(int b) { csetbit1(&z, b); }
        unsigned getbit(int b) const { return cgetbit(&z, b); }

        Unumber pow_recur(Unumber e, const Unumber & mod) const;
        void pow_r(Unumber e, const Unumber & mod) { *this = pow_recur(e, mod); }

        void pow(Unumber e, const Unumber & mod)
        {
            z = cpow(&z, &e.z, &mod.z);
        }

        void pow_c2(Unumber e, const Unumber & mod, const Unumber & res)
        {
            z = cpow2(&z, &e.z, &mod.z, &res.z);
        }

        Unumber div(const Unumber & d, Unumber & q) const
        {
            Unumber m;
            m.z = cdiv_QnotND(&z, &d.z, &q.z);
            return m;
        }

        Unumber mul(const Unumber & b, const Unumber & m) const
        {
            Unumber r;
            r.z = cmul(&z, &b.z, &m.z);
            return r;
        }

        void divABRQ(const Unumber & d, Unumber * r, Unumber * q) const
        {
            Unumber m;
            m.z = cdiv_QnotND(&z, &d.z, q ? &q->z : 0);
            if (r) *r = m;
        }

        bool iszero() const { return 0 != ciszero(&z); }

        Unumber & operator++() { cinc(&z); return *this; }
        Unumber & operator--() { *this -= 1; return *this; }

        void operator<<=(int b) { cshl(&z, b); }
        void operator>>=(int b) { cshr(&z, b); }

        friend Unumber operator-(const Unumber & n1, const Unumber & n2)
        { Unumber r(n1); r -= n2; return r; }

        friend Unumber operator*(const Unumber & n1, const Unumber & n2)
        { Unumber r(n1); r *= n2; return r; }

        friend Unumber operator+(const Unumber & n1, const Unumber & n2)
        { Unumber r(n1); r += n2; return r; }

        friend Unumber operator%(const Unumber & n1, const Unumber & n2)
        { Unumber r(n1); r %= n2; return r; }

        friend Unumber operator/(const Unumber & n1, const Unumber & n2)
        { Unumber r(n1); r /= n2; return r; }

        friend bool operator<(const Unumber & n1, const Unumber & n2)
        { return 0 != cless(&n1.z, &n2.z); }

        friend bool operator>(const Unumber & n1, const Unumber & n2) { return n2 < n1; }

        friend bool operator!=(const Unumber & n1, const Unumber & n2)
        { return !(n1 == n2); }

        friend bool operator==(const Unumber & n1, const Unumber & n2)
        { return 0 != cequal(&n1.z, &n2.z); }

        void swap(Unumber & n) { Cunumber y; y = z; z = n.z; n.z = y; }

        string str(unsigned base=10) const;
        unsigned long long to_ull() const;

        friend std::ostream & operator<<(std::ostream & os, const Unumber & n)
        { return (os << n.str()); }

        friend std::istream & operator>>(std::istream & os, Unumber & n);

        string raw(unsigned sz) const;
};

inline
Unumber::Unumber(const string & s, StringType st)
{
    if ( st == Decimal ) init10(s.c_str());
    if ( st == Binary ) initBin(s);
}

inline
void Unumber::init10(const char * s)
{
    ctozero(&z);
    Unumber ten(10);
    while (*s)
    {
        if ( std::isdigit(*s) )
        {
            unsigned x = (unsigned char)(*s) - (unsigned char)('0');
            Unumber c(x);
            z = cmul(&z, &ten.z, 0);
            z = cadd(&z, &c.z);
        }
        ++s;
    }
}

inline
void Unumber::initBin(const string & s)
{
    ctozero(&z);

    size_t szx = sizeof(z.x);
    size_t szs = s.size();
    if ( szx < szs ) return;
    std::memcpy( z.x, s.c_str(), szs ); // watch for byte order in Words
    // reset o
    cinvert(&z);
    cinvert(&z);
}

inline
string Unumber::raw(unsigned szs) const
{
    size_t szx = sizeof(z.x);
    if ( szx < szs ) return "";
    return string((const char *)z.x, szs); // same as above - watch for byte order
}

inline
string Unumber::str(unsigned base) const
{
    string r;
    Unumber ten(base);

    if ( !cisvalid(&z) ) return "NaN";

    Cunumber q(z), q1;
    while (1)
    {
        Cunumber m = cdiv_QnotND(&q, &ten.z, &q1);
        r = char( char('0') + char(m.x[0]) ) + r;
        if ( ciszero(&q1) ) break;
        q = q1;
    }

    return r;
}

inline
unsigned long long Unumber::to_ull() const
{
    std::istringstream is(str());
    unsigned long long x = 0;
    is >> x;
    return x;
}

inline
Unumber::Unumber(unsigned long long x)
{
    ctozero(&z);
    if ( sizeof(UBASE_TYPE) >= sizeof(unsigned long long) )
    {
        z.x[0] = (UBASE_TYPE)x;
        return;
    }

    const int sz = sizeof(unsigned long long) / sizeof(UBASE_TYPE);

    for ( int i = 0; i < sz; i++ )
        if ( i < UNUMBER_SIZE )
        {
            z.x[i] = (UBASE_TYPE)(x >> (i * (sizeof(UBASE_TYPE) << 3)));
            if ( z.x[i] ) z.o = i;
        }
}

inline
Unumber Unumber::pow_recur(Unumber e, const Unumber & mod) const
{
    if ( e == Unumber(0) ) return Unumber(1);
    if ( e == Unumber(1) ) return *this;

    unsigned odd = cgetbit(&e.z, 0);

    e >>= 1;

    Unumber r = pow_recur(e, mod);
    r.z = cmul(&r.z, &r.z, &mod.z);

    if ( odd )
        r.z = cmul(&r.z, &z, &mod.z);

    return r;
}

inline
std::istream & operator>>(std::istream & os, Unumber & n)
{
    std::string s; os >> s;
    Unumber u(s, Unumber::Decimal);
    n.swap(u);
    return os;
}

inline Unumber operator>>(Unumber a, unsigned b) { a >>= b; return a; }
inline Unumber operator<<(Unumber a, unsigned b) { a <<= b; return a; }

#endif
