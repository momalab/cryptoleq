#include <ostream>
#include <string>

using std::string;

#define E(x) RsaE<x>()

struct Epod
{
	//uint64_t x;
	int x;
	string str() const;
};

struct RsaMule : Epod
{
};

struct RsaInt : RsaMule
{
};

inline std::ostream& operator<<(std::ostream & os, const RsaInt &ri)
{
	return os<<ri.str();
}


template<int n> RsaInt RsaE(){ return RsaInt(); }


