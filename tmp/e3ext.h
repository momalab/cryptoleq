#include <ostream>
#include <string>

using std::string;

#define E(x) RsaE<x>()

struct Epod
{
    //uint64_t x;
    int x;
    string str() const;
    Epod(int x): x(x) {}
};

struct RsaInt : Epod
{
    RsaInt(int x = 0): Epod(x) {}
};

inline std::ostream & operator<<(std::ostream & os, const RsaInt & ri) { return os << ri.str(); }

template<int e> RsaInt RsaEf(const RsaInt & N, const RsaInt & x)
{
    if ( e == 0 || e == 1 ) return x;
    if ( e % 2 )
    {
        RsaInt r = RsaEf < e - 1 > (N, x); //*RsaEf<1>(N,x);
        return r;
    }

    RsaInt r = RsaEf < e / 2 > (N, x);
    //r = r*r;
    return r;
}


template<int n> RsaInt RsaE()
{
    return RsaEf<3>(91, n);
}


