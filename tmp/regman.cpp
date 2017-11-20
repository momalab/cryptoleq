#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::string;


#ifndef REGS
#define REGS 16
#endif

#ifndef REG0
#define REG0 0
#endif


// has to deny direct access to regs
class Cpu
{
    public:
        const static int REGNUM = REGS;
        const static int UNLNUM = REGNUM; // this number [1,REGNUM] defines search cyclic for unload
        // 1 means unload the next register
        // n means check cyclicly n and unload the oldest
        // REGNUM mean check all regs
        // > REGNUM will return the same as REGNUM
        // cannot be 0

        using Value = int;
        enum class OP { eq, mu1, mu2, ad1, ad2, inc, cp, 
		su1, su2, leq, lsh, rsh, xo1, bor, le };

        void instr(int o1, OP op, int i1, int i2);
        void instr_load(int r, const Value & v)
        {
            regs[r] = v;
            ld_cntr++;
        }

        void instr_store(int r, Value & v)
        {
            v = regs[r];
            st_cntr++;
        }

        Value & dbg_access(int idx) { return regs[idx]; }

        int ld_cntr = 0, st_cntr = 0;

    private:
        Value regs[REGNUM];
};

class Number;

class Table
{
        static unsigned int ts_cntr;
        const static unsigned MAX_TS_CNTR = Cpu::REGNUM + 1000; // do not set to MAX UNINT, cannot be less then REGNUM
        static int regpos;

        class Entry
        {
                friend class Table;
                unsigned int ts; // timestamp;
                bool lock; // locking for unloading
                bool sync; // sychronized value
                const Number * pn;
            public:
                Entry() : ts(0), lock(false), sync(false), pn(nullptr) {}
                void set(unsigned t, bool sy, const Number * p = nullptr) { ts = t; sync = sy; if (p) pn = p; }
                void setsync(bool s) { sync = s; }

                bool syn() const { return sync; }
                const Number * number() { return pn; }
                void number(const Number * p) { pn = p; }
                bool issync() { return sync; }
        };

        std::vector<int> rvac;
        Entry rocc[Cpu::REGNUM];

        void renormalize_ts();

    public:

        // ! init ts with 1 (check when its zero)
        Table() { for (int i = 0; i < Cpu::REGNUM; i++) rvac.push_back(i); }

        void unload();
        void free(int idx);
        int alloc(const Number *, bool syn);

        void touch(int x1, int x2, int x3);
        void unsync(int i) { rocc[i].setsync(false); }

        void lock(int x, bool lc);
        void lock(int x1, int x2, int x3, bool lc);
        void lock(int x1, int x2) { lock(x1, x2, -1, true); }
        void lock(int x1, int x2, int x3) { lock(x1, x2, x3, true); }
        void lock(int x1) { lock(x1, true); }
        void unlock(int x1, int x2, int x3) { lock(x1, x2, x3, false); }
        void unlock(int x1, int x2) { lock(x1, x2, -1, false); }

        void owner(int idx, Number * pn) { rocc[idx].number(pn); }
        bool issync(int idx) { return rocc[idx].issync(); }
        void setsync(int idx) { rocc[idx].setsync(true); }
};

class Regman
{
        Table tbl;

        int bind(const Number & n);
        int load(const Number & n);

        static void store(const Number * n);

    public:

        void o1i2(Number & o1, Cpu::OP op, const Number & i1, const Number & i2);
        void o1i1(Number & o1, Cpu::OP op, const Number & i1);
        void io1i1(Number & io, Cpu::OP op, const Number & i1);
        void io1(Number & io, Cpu::OP op);

        void free(int idx) { tbl.free(idx); }
        void owner(int idx, Number * pn) { tbl.owner(idx, pn); }
        bool issync(int idx) { return tbl.issync(idx); }

        static void sync_detach(const Number * n, bool needstore);
        void sync_read(const Number * n);
};

extern Regman regman;

struct Nid
{
    static int ncntr;
    int id;
    Nid() : id(++ncntr)
    {
        //std::cout << " +" << id << std::flush;
    }
    ~Nid()
    {
        //std::cout << " -" << id << std::flush;
    }
};

class Number
{
        Nid nid;

        mutable int regidx;
        mutable Cpu::Value val;

        friend class Regman;

        void sync() const { regman.sync_read(this); }

    public:
        Number(int x = 0) : regidx(-1), val(x) {}
        Number operator==(const Number & n) const { Number r; regman.o1i2(r, Cpu::OP::eq, *this, n); return r; }
        Number operator<=(const Number & n) const { Number r; regman.o1i2(r, Cpu::OP::leq, *this, n); return r; }
        Number operator<(const Number & n) const { Number r; regman.o1i2(r, Cpu::OP::le, *this, n); return r; }

        Number operator<<(const Number & n) const { Number r; regman.o1i2(r, Cpu::OP::lsh, *this, n); return r; }
        Number operator>>(const Number & n) const { Number r; regman.o1i2(r, Cpu::OP::rsh, *this, n); return r; }
        Number operator|(const Number & n) const { Number r; regman.o1i2(r, Cpu::OP::bor, *this, n); return r; }

        Number operator*(const Number & n) const { Number r; regman.o1i2(r, Cpu::OP::mu2, *this, n); return r; }
        Number operator+(const Number & n) const { Number r; regman.o1i2(r, Cpu::OP::ad2, *this, n); return r; }
        Number operator-(const Number & n) const { Number r; regman.o1i2(r, Cpu::OP::su2, *this, n); return r; }

        // inplace
        Number operator+=(const Number & n)
        {
            regman.io1i1(*this, Cpu::OP::ad1, n);
            return *this;
        }
        Number operator-=(const Number & n)
        {
            regman.io1i1(*this, Cpu::OP::su1, n);
            return *this;
        }
        Number operator*=(const Number & n)
        {
            regman.io1i1(*this, Cpu::OP::mu1, n);
            return *this;
        }

        Number operator^=(const Number & n){ regman.io1i1(*this, Cpu::OP::xo1, n); return *this; }

        Number operator++() { regman.io1(*this, Cpu::OP::inc); return *this; }
        string str() const;
        string dbg();
        ~Number() { regman.free(regidx); }

        // copy and move semantics
        Number(Number & n): Number() { copy(*this, n); }
        Number operator=(const Number & n) { copy(*this, n); return *this; }
        Number(Number &&);
        static void copy(Number & o, const Number & i); // cannot invoke copy-c-tor or ass-o-tor

	// this is a ficticious function made to accommodate Edu's code
	// it has to be deleted ASAP
	static Number encrypt(Number x){ return x; }
};

// do not use this when debugging, because it may invoke modifications
inline std::ostream & operator<<(std::ostream & os, const Number & n) { return os << n.str(); }

// END OF HEADER

// START IMPL

Cpu cpu;
Regman regman;
unsigned int Table::ts_cntr = 1;
int Table::regpos = 0;
int Nid::ncntr = 20;

// NUMBER

void Number::copy(Number & o, const Number & i)
{
    if (&o == &i) return;

    if (i.regidx < 0)
    {
        o.val = i.val;
        if (o.regidx >= 0)
        {
            regman.free(o.regidx);
            o.regidx = -1;
        }
    }
    else
        regman.o1i1(o, Cpu::OP::cp, i);
}

Number::Number(Number && t)
{
    if (t.regidx >= 0)
    {
        regidx = t.regidx;
        t.regidx = -1;
        regman.owner(regidx, this);
        if ( regman.issync(regidx) ) val = t.val;
    }
    else
    {
        regidx = -1;
        val = t.val;
    }
}

string Number::str() const
{
    sync();
    std::ostringstream os;
    os << val;
    return os.str();
}

// this function directly access registers and should not be used in code, only debug purpose
string Number::dbg()
{
    Cpu::Value v;
    if (regidx < 0) v = val;
    else v = cpu.dbg_access(regidx);
    std::ostringstream os;
    os << v;
    return os.str();
}

// REGMAN TABLE

void Table::touch(int x1, int x2, int x3)
{
	if (x1 < 0 || x1 >= cpu.REGNUM || x2 < 0 || x2 >= cpu.REGNUM || x3 < 0 || x3 >= cpu.REGNUM)
		throw 258;

    rocc[x1].ts = rocc[x2].ts = rocc[x3].ts = ++ts_cntr;
    if (ts_cntr >= MAX_TS_CNTR) renormalize_ts();

    const bool UnloadALWAYS = REG0;
    if ( UnloadALWAYS )
    {
        if ( rocc[x1].pn ) regman.sync_detach(rocc[x1].pn, true);
        if ( rocc[x2].pn ) regman.sync_detach(rocc[x2].pn, true);
        if ( rocc[x3].pn ) regman.sync_detach(rocc[x3].pn, true);
    }
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
    if (idx < 0 || idx >= Cpu::REGNUM) return;

    rvac.push_back(idx);
    rocc[idx] = Entry(); // this should not be necessary
}

void Table::unload()
{
    if (!rvac.empty()) return;

    int idx = -1;
    unsigned int t = MAX_TS_CNTR + 1;
    for (int i = 0; i < Cpu::UNLNUM || idx < 0; i++)
    {
        int & j = regpos;
        j = (++j) % Cpu::REGNUM;
        Entry & e = rocc[j];
        if (e.lock) continue;
        if (e.ts < t) { idx = j; t = e.ts; }
    }

    Entry & e = rocc[idx];
    Regman::sync_detach(e.number(), !e.syn());

    free(idx);
}

int Table::alloc(const Number * n, bool syn)
{
    unload();
    int idx = rvac.back();
    rvac.pop_back();

    rocc[idx].set(ts_cntr, syn, n);
    return idx;
}

void Table::renormalize_ts()
{
    for (unsigned int i = 1; i <= Cpu::REGNUM; i++)
    {
        unsigned t = MAX_TS_CNTR + 1;
        int idx = 0;

        for (int j = 0; j < Cpu::REGNUM; j++)
        {
            if (rocc[j].ts < t && rocc[j].ts > i )
            {
                idx = j;
                t = rocc[j].ts;
            }
        }

        if (idx >= 0) rocc[idx].ts = i;
    }
    ts_cntr = Cpu::REGNUM + 1;
}

// CPU

void Cpu::instr(int o1, OP op, int i1, int i2)
{
    switch (op)
    {
        case OP::eq: regs[o1] = (regs[i1] == regs[i2]); break;
        case OP::leq: regs[o1] = (regs[i1] <= regs[i2]); break;
        case OP::le: regs[o1] = (regs[i1] < regs[i2]); break;

        case OP::lsh: regs[o1] = (regs[i1] << regs[i2]); break;
        case OP::rsh: regs[o1] = (regs[i1] >> regs[i2]); break;
        case OP::bor: regs[o1] = (regs[i1] | regs[i2]); break;

        case OP::xo1: regs[o1] ^= regs[i1]; break;

        case OP::mu1: regs[o1] *= regs[i1]; break;
        case OP::mu2: regs[o1] = (regs[i1] * regs[i2]); break;

        case OP::ad1: regs[o1] += regs[i1]; break;
        case OP::ad2: regs[o1] = (regs[i1] + regs[i2]); break;

        case OP::su1: regs[o1] -= regs[i1]; break;
        case OP::su2: regs[o1] = (regs[i1] - regs[i2]); break;

        case OP::cp: regs[o1] = regs[i1]; break;
        case OP::inc: ++regs[o1]; break;
        default:
            throw 279; // bad instruction
    }
}

// REGMAN

void Regman::store(const Number * n)
{
    int idx = n->regidx;
    if (idx < 0) return;

    cpu.instr_store(idx, n->val);
}

void Regman::sync_detach(const Number * n, bool needstore)
{
    int idx = n->regidx;
    if (idx < 0) return;

    if ( needstore ) store(n);
    n->regidx = -1;
}

void Regman::sync_read(const Number * n)
{
    int idx = n->regidx;
    if (idx < 0) return;

	if (tbl.issync(idx)) return;

	store(n);
    tbl.setsync(idx);
}

int Regman::bind(const Number & n)
{
    int & i = n.regidx;
    if (i >= 0) return i;
    return (i = tbl.alloc(&n, false));
}

int Regman::load(const Number & n)
{
    int & i = n.regidx;
    if (i >= 0) return i;
    i = tbl.alloc(&n, true);
    cpu.instr_load(i, n.val);
    return i;
}

void Regman::o1i2(Number & o1, Cpu::OP op, const Number & i1, const Number & i2)
{
    tbl.lock(o1.regidx, i1.regidx, i2.regidx);

    int x1 = load(i1);
    tbl.lock(x1);

    int x2 = load(i2);
    tbl.lock(x2);

    int y = bind(o1);

    tbl.unlock(x1, x2, y);
    cpu.instr(y, op, x1, x2);
    tbl.unsync(y);

    tbl.touch(x1, x2, y);
}

void Regman::o1i1(Number & o1, Cpu::OP op, const Number & i1)
{
    tbl.lock(o1.regidx, i1.regidx);

    int x1 = load(i1);
    tbl.lock(x1);

    int y = bind(o1);

    tbl.unlock(x1, y);
    cpu.instr(y, op, x1, -1);
    tbl.unsync(y);

    tbl.touch(x1, y, y);
}

void Regman::io1i1(Number & o1, Cpu::OP op, const Number & i1)
{
    tbl.lock(o1.regidx, i1.regidx);

    int x1 = load(i1);
    tbl.lock(x1);

    int y = load(o1);

    tbl.unlock(x1, y);
    cpu.instr(y, op, x1, -1);
    tbl.unsync(y);

    tbl.touch(x1, y, y);
}

void Regman::io1(Number & io, Cpu::OP op)
{
    int x = load(io);
    cpu.instr(x, op, -1, -1);
    tbl.unsync(x);
    tbl.touch(x, x, x);
}

// END OF IMPL

// START USER CODE

Number E(int x) { return Number(x); }

// static encryption
Number operator "" _E(unsigned long long int x)
{
    switch (x)
    {
        case 12: return E(12);
        case 7: return E(7);
        case 5: return E(5);
        case 1: return E(1);
        case 0: return E(0);
    }
    throw int(x);
}

const int MAX_NUM = 50;

using std::cout;

int main()
try
{
    Number num = 7_E;
    Number f1 = 1_E;
    Number f2 = 1_E;
    Number fi = 1_E;
    Number i = 1_E;
    Number res = 0_E;

    for ( int cntr = 0; cntr < MAX_NUM; cntr++ )
    {
        res += (i == num) * fi;
        fi = f1 + f2;
        f1 = f2;
        f2 = fi;
        ++i;
    }
    cout << "fib: " << res << "\n";
    cout << "loads: " << cpu.ld_cntr << "  strores: " << cpu.st_cntr << '\n';
    cout << cpu.REGNUM << '\t' << cpu.ld_cntr << '\t' << cpu.st_cntr << '\n';

    return 0;
}
catch (int x)
{
    cout << "error " << x << "\n";
    return 1;
}
catch (const char * e)
{
    cout << "error " << e << "\n";
    return 1;
}
catch (std::exception e)
{
	cout << "error e: "<<e.what()<<"\n";
	return 1;
}
catch (...)
{
	cout << "error ...\n";
	return 1;
}
