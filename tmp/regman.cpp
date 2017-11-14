#include <iostream>
#include <string>
#include <vector>

using std::string;

namespace cpu {
const int REGNUM = 3;
const int UNLNUM = REGNUM; // this number [1,REGNUM] defines search cyclic for unload
// 1 means unload the next register
// n means check cyclicly n and unload the oldest
// REGNUM mean check all regs
// > REGNUM will return the same as REGNUM
// cannot be 0

using Value = int;
Value regs[REGNUM];

enum class OP { eq, mul, ad1, ad2, inc, cp };

void instr(int o1, OP op, int i1, int i2);
///void instr(int o1, OP op, int i1);
///void instr(int io, OP op);
void instr_load(int r, const Value &v) { regs[r] = v; }
void instr_store(int r, Value &v) { v = regs[r]; }
} // reg

class Number;

class Table
{
	static unsigned int ts_cntr;
	const static unsigned MAX_TS_CNTR = 100; // do not set to MAX UNINT
	static int regpos;

	class Entry
	{
	public:
		unsigned int ts; // timestamp;
		bool lock; // locking for unloading
	private:
		bool sync; // sychronized value
		Number *pn;
	public:
		Entry() : ts(0), lock(false), sync(false), pn(nullptr) {}
		void set(unsigned t, bool sy, Number *p = nullptr) { ts = t; sync = sy; if (p) pn = p; }
		void unsync() { sync = false; }

		bool syn() const { return sync; }
		Number & number(){ return *pn; }
	};

	std::vector<int> rvac;
	Entry rocc[cpu::REGNUM];

	void renormalize_ts()
	{
		throw 99;
	}

public:

	// ! init ts with 1 (check when its zero)
	Table() { for (int i = 0; i < cpu::REGNUM; i++) rvac.push_back(i); }

	void unload();
	void free(int idx);
	int alloc(Number *, bool syn);

	void touch(int x1, int x2, int x3);
	void unsync(int i) { rocc[i].unsync(); }

	void lock(int x, bool lc);
	void lock(int x1, int x2, int x3, bool lc);
	void lock(int x1, int x2) { lock(x1, x2, -1, true); }
	void lock(int x1, int x2, int x3) { lock(x1, x2, x3, true); }
	void lock(int x1) { lock(x1, true); }
	void unlock(int x1, int x2, int x3) { lock(x1, x2, x3, false); }
	void unlock(int x1, int x2) { lock(x1, x2, -1, false); }
};

class Regman
{
	Table tbl;

	int bind(Number &n);
	int load(Number &n);

public:

	///void operator()(Number & o1, cpu::OP op, Number &i1, Number &i2);
	///void operator()(Number & o1, cpu::OP op, Number &i1);
	///void operator()(Number & o1, cpu::OP op);
	void o1i2(Number & o1, cpu::OP op, Number &i1, Number &i2);
	void o1i1(Number & o1, cpu::OP op, Number &i1);
	void io1(Number & io1, cpu::OP op);

	void instr(int o1, cpu::OP op, int i1, int i2);

	void free(int idx) { tbl.free(idx); }
	static void store(Number &n, int idx);
	//static void setval(Number &n, const cpu::Value &v) { n.val = v; }
};

extern Regman regman;

// this class cannot be const because its inteernal are modified by Regman
// for enforce constness, an adator is required that casts away const modifier
class Number
{
        int regidx;
		bool modified;
        cpu::Value val;

		friend class Regman;

    public:
        Number(int x=0): regidx(-1), modified(false), val(x) {}
		Number operator==(Number &n){ Number r; regman.o1i2(r, cpu::OP::eq, *this, n); return r; }
		Number operator*(Number &n){ Number r; regman.o1i2(r, cpu::OP::mul, *this, n); return r; }
		Number operator+(Number &n){ Number r; regman.o1i2(r, cpu::OP::ad2, *this, n); return r; }

		// inplace
		Number operator+=(Number &n) 
		{ 
			regman.o1i1(*this, cpu::OP::ad1, n); 
			return *this;  
		}

		Number operator++() { regman.io1(*this, cpu::OP::inc); return *this; }
		string str();
		~Number(){ regman.free(regidx); }

		// copy and move semantics
		Number(Number &n) { copy(*this, n); }
		Number operator=(Number &n) { copy(*this, n); return *this; }
		Number(Number &&);
		static void copy(Number &o, Number &i); // cannot invoke copy-c-tor or ass-o-tor
};

inline std::ostream & operator<<(std::ostream &os, Number &n) { return os << n.str(); }

// END OF HEADER

// START IMPL

Regman regman;
unsigned int Table::ts_cntr = 1;
int Table::regpos = 0;

// NUMBER

void Number::copy(Number &o, Number &i) 
{
	if (&o== &i) return;
	o.regidx = -1;
	if (i.regidx < 0)
		o.val = i.val;
	else
		regman.o1i1(o, cpu::OP::cp, i);
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

string Number::str()
{
	throw 3;
	return "";
}

// REGMAN TABLE

void Table::touch(int x1, int x2, int x3) 
{
	rocc[x1].ts = rocc[x1].ts = rocc[x1].ts = ++ts_cntr;
	if (ts_cntr >= MAX_TS_CNTR) renormalize_ts();
}

void Table::lock(int x, bool lc)
{
	if (x >= 0) rocc[x].lock = lc;
}

void Table::lock(int x1, int x2, int x3, bool lc) 
{
	lock(x1, lc);
	lock(x2, lc);
	lock(x3, lc);
}

void Table::free(int idx)
{
	if (idx < 0 || idx >= cpu::REGNUM) return;

	rvac.push_back(idx);
	///rocc[idx].number().
	rocc[idx] = Entry(); // this should not be necessary
}

void Table::unload()
{
	if (!rvac.empty()) return;

	int idx = -1;
	unsigned int t = MAX_TS_CNTR+1;
	for (int i = 0; i < cpu::UNLNUM || idx < 0; i++)
	{
		int & j = regpos;
		j = (++j) % cpu::REGNUM;
		Entry & e = rocc[j];
		if (e.lock) continue;
		if (e.ts < t) { idx = j; t = e.ts; }
	}

	Entry & e = rocc[idx];
	if (!e.syn()) Regman::store(e.number(),idx);

	free(idx);
}

int Table::alloc(Number * n, bool syn)
{
	unload();
	int idx = rvac.back();
	rvac.pop_back();

	rocc[idx].set(++ts_cntr, syn, n);
	return idx;
}

// CPU

void cpu::instr(int o1, OP op, int i1, int i2)
{
	switch (op)
	{
	case OP::eq: regs[o1] = (regs[i1] == regs[i2]); break;
	case OP::mul: regs[o1] = (regs[i1] * regs[i2]); break;
	case OP::ad1: regs[o1] += regs[i1]; break;
	default:
		throw 975;
	}
}

// REGMAN

void Regman::store(Number &n, int idx) 
{
	n.val = cpu::regs[idx]; 
	n.regidx = -1;
}

int Regman::bind(Number &n)
{
	int & i = n.regidx;
	if (i >= 0) return i;
	return (i = tbl.alloc(&n,false));
}

int Regman::load(Number &n)
{
	int & i = n.regidx;
	if (i >= 0) return i;
	i = tbl.alloc(&n,true);
	cpu::instr_load(i, n.val);
	return i;
}

void Regman::instr(int o1, cpu::OP op, int i1, int i2)
{
	cpu::instr(o1, op, i1, i2);
	tbl.touch(o1, i1, i2);
}

void Regman::o1i2(Number & o1, cpu::OP op, Number &i1, Number &i2)
{
	tbl.lock(o1.regidx, i1.regidx, i2.regidx);
	
	int x1 = load(i1);
	tbl.lock(x1);

	int x2 = load(i2);
	tbl.lock(x2);

	int y = bind(o1);

	tbl.unlock(x1,x2,y);
	cpu::instr(y, op, x1, x2);
	tbl.unsync(y);
}

void Regman::o1i1(Number & o1, cpu::OP op, Number &i1)
{
	tbl.lock(o1.regidx, i1.regidx);

	int x1 = load(i1);
	tbl.lock(x1);

	int y = bind(o1);

	tbl.unlock(x1, y);
	cpu::instr(y, op, x1, -1);
	tbl.unsync(y);
}

void Regman::io1(Number & io, cpu::OP op)
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
