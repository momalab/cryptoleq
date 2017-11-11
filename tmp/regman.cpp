#include <iostream>
#include <string>
///#include <list>

using std::string;

const int REGNUM = 3;

using Value = int;
Value regs[REGNUM];

class Number;

class Table
{
	struct Entry
	{
		unsigned int ts;
		const Number *pn;
		Entry(int i, const Number *p = nullptr) : ts(i), pn(p) {}
	};

	Entry rvac[REGNUM], rocc[REGNUM];

public:

	// ! init ts with 1 (check when its zero)
	Table() { for (int i = 0; i < REGNUM; i++) rvac.push_back(i); }

	void unload();
	void free(int idx);
	int alloc(const Number *);
};

class Regman
{
	Table tbl;

	int bind(const Number &n);
	int load(const Number &n);

public:

	enum class OP { eq, mul, ad1, ad2, inc };

	void instr(int o1, OP op, int i1, int i2);
	void instr(int o1, OP op, int i1);
	void instr_load(int r, const Value &v) { regs[r] = v; }
	void instr_store(int r, Value &v) { v = regs[r]; }

	void operator()(Number & o1, OP op, const Number &i1, const Number &i2);
	void operator()(Number & o1, OP op, const Number &i1);

	void free(int idx) { tbl.free(idx); }
};

extern Regman regman;

class Number
{
        mutable int regidx;
		mutable bool modified;
        Value val;

		friend class Regman;

    public:
        Number(int x=0): regidx(-1), modified(false), val(x) {}
		Number operator==(const Number &n) const { Number r; regman(r, Regman::OP::eq, *this, n); return r; }
		Number operator*(const Number &n) const { Number r; regman(r, Regman::OP::mul, *this, n); return r; }
		Number operator+(const Number &n) const { Number r; regman(r, Regman::OP::ad2, *this, n); return r; }

		// inplace
		Number operator+=(const Number &n) { regman(*this, Regman::OP::ad1, *this, n); return *this;  }
		Number operator++() { regman(*this, Regman::OP::inc, *this); return *this; }
		string str() const;
		~Number(){ regman.free(regidx); }

		// copy and move semantics
		Number(const Number &);
		Number operator=(const Number &);
		Number(Number &&);
};

inline std::ostream & operator<<(std::ostream &os, const Number &n) { return os << n.str(); }

// END OF HEADER

// START IMPL

Regman regman;

// NUMBER

Number::Number(const Number &) 
{
	throw 7;
}

Number Number::operator=(const Number &) 
{
	throw 5;
}

Number::Number(Number && t) 
{
	if (t.regidx >= 0)
	{
		regidx = t.regidx;
		t.regidx = -1;
	}
	else
	{
		regidx = -1;
		val = t.val;
	}
}

string Number::str() const
{
	throw 3;
	return "";
}

// REGMAN TABLE

void Table::free(int idx)
{
	if (idx < 0 || idx >= REGNUM) return;

	throw 8;
}

void Table::unload()
{
	if (!rvac.empty()) return;
	throw 18;
}

int Table::alloc(const Number * n)
{
	unload();
	Entry e = rvac.front(); rvac.pop_front();
	e.pn = n;
	rocc.push_front(e);
	return e.ridx;
}

// REGMAN

int Regman::bind(const Number &n)
{
	int & i = n.regidx;
	if (i >= 0) return i;
	return (i = tbl.alloc(&n));
}

int Regman::load(const Number &n)
{
	int & i = n.regidx;
	if (i >= 0) return i;
	i = tbl.alloc(&n);
	instr_load(i, n.val);
	return i;
}

void Regman::instr(int o1, OP op, int i1, int i2)
{
	switch (op)
	{
		case OP::eq:
			regs[o1] = (regs[i1] == regs[i2]); 
			tbl.touch(o1);
			break;
	default:
		throw 975;
	}
}

void Regman::operator()(Number & o1, OP op, const Number &i1, const Number &i2)
{
	int x1 = load(i1);
	int x2 = load(i2);
	int y = bind(o1);
	instr(y, op, x1, x2);
}

void Regman::operator()(Number & o1, OP op, const Number &i1)
{
	throw 2;
}


// END OF IMPL

// START USER CODE

Number E(int x) { return Number(x); }

const int MAX_NUM = 50;

int main()
try
{
    Number num = E(7);
    Number f1 = E(1);
    Number f2 = E(1);
    Number fi = E(1);
    Number i = E(1);
    Number result = E(0);

    int counter = 0;
    do
    {
        result += (i == num) * fi;
        fi = f1 + f2;
        f1 = f2;
        f2 = fi;
        ++i;
    }
    while (++counter < MAX_NUM);
    std::cout << "fib: " << result << "\n";
}
catch (...)
{
	std::cout << "error\n";
	return 1;
}
