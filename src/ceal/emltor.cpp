// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: emltor.cpp

#include <iostream>
#include <iomanip>

#include "emltor.h"

using std::cout;

void Emulator::run(ProcessorTS & proc, Stat * stat)
{
    const bool IOJMP = true;

    Cell ip = entry;

    Cell m1c = minusone();
    if (trace) cout << "minusone " << m1c.str() << "\n";


    int iter = 0;
    int iter2 = 0;

    try
    {
        while (++iter)
        {
            if (iter == 1000000) { iter2++; iter = 0; }

            if (trace) cout << "exec " << ip.str() << ":\t";

            if (stat) stat->ip(ip);

            Cell a = mem[ip];
            ++ip;
            Cell b = mem[ip];
            ++ip;

            if (trace) cout << proc.str(a) << ' ' << proc.str(b) << ' ' << proc.str(mem[ip]) << ' ';

            if (a == m1c && b == m1c)
            {
                if (stat) stat->io();
                current_input_stream = &std::cin;
                if (io_type == Pragma::Io::Text)
                {
                    char c;
                    std::cin.get(c);
                    if (std::cin) cout << c << std::flush;
                    if (trace) cout << " \tinout " << c << ' ';
                }
                else if (io_type == Pragma::Io::Ts)
                {
                    Cell x;
                    get_ts(x, proc.N);
                    if (std::cin) cout << x.str() << '\n';
                    if (trace) cout << " \tinout " << x.str() << ' ';
                }
                else if (io_type == Pragma::Io::X)
                {
                    Unumber x;
                    get_number(x, proc.N);
                    if (std::cin) cout << x.str() << '\n';
                    if (trace) cout << " \tinout " << x.str() << ' ';
                }
                if (IOJMP) ip = mem[ip]; else ++ip;
            }
            else if (a == m1c)
            {
                if (stat) stat->io();
                current_input_stream = &std::cin;
                Cell x;
                if (io_type == Pragma::Io::Text)
                {
                    char c;
                    std::cin.get(c);
                    x = Cell((unsigned char)c, 0);
                }
                else if (io_type == Pragma::Io::Ts) get_ts(x, proc.N);
                else if (io_type == Pragma::Io::X)
                {
                    Unumber y;
                    get_number(y, proc.N);
                    x = Cell(y, Cell::X);
                }

                mem[b] = x;
                if (IOJMP) ip = mem[ip]; else ++ip;
                if (trace) cout << " \tinput [b]=" << mem[b].str() << " ip=" << ip.str() << ' ';
            }
            else if (b == m1c)
            {
                if (stat) stat->io();
                if (io_type == Pragma::Io::Text)
                    std::cout << (unsigned char)(mem[a].ts().t.to_ull()) << std::flush;

                else if (io_type == Pragma::Io::Ts)
                    std::cout << proc.str(mem[a]) << sep_ts << std::flush;

                else if (io_type == Pragma::Io::X)
                    std::cout << mem[a].x().str() << sep_x << std::flush;

                if (IOJMP) ip = mem[ip]; else ++ip;
                if (trace) cout << " \toutput " << proc.str(mem[a]) << ' ';
            }

            else
            {
                const Cell & ma = mem[a];
                Cell & mb = mem[b];

                if (stat) stat->instr(ma, mb);

                mb = proc.BAm1(ma, mb);

                if (proc.leq(mb))
                    ip = mem[ip];
                else
                    ++ip;

                if (trace) cout << " \t[a]=" << proc.str(mem[a]) << " [b]=" << proc.str(mem[b]);
            }

            if (trace) cout << "\n";

            if (ip == m1c)
                break;

        }
    }
    catch (const char * s)
    {
        cout << s << "\nError\nip=" << ip.str() << '\n';
        cout << "mem[ip]=" << mem[ip].str() << '\n';
        cout << mem.dump() << '\n';
    }

    if (steps)
        cout << "\nsteps=" << iter2 << std::setfill('0') << std::setw(6) << iter << '\n';
}

void Emulator::get_ts(Cell & v, Unumber N)
{
    if (cq_type == Pragma::Cqtype::Ts)
    {
        Unumber t, s;
        get_number(t, N);
        char c = get_char();
        if (c == '.')
            get_number(s, N);
        else
        {
            put_back(c);
            s = 0;
        }
        v = Cell(t, s);
    }

    else if (cq_type == Pragma::Cqtype::X)
    {
        Unumber x;
        get_number(x, N);
        v = Cell(x, Cell::X);
    }

    else
        throw "Unknown cqtype";
}

void Emulator::get_number(Unumber & n, Unumber N)
{
    eat_spaces();

    char c = get_char();

    if (c == '(' || c == '-')
    {
        bool par = false;
        if (c == '(') par = true;

        if (par)
        {
            eat_spaces();
            c = get_char();
            if (c != '-')
                throw "Only unary minus is allowed in expressions";
        }

        get_number(n, N);

        if (N != 0 && n > 0)
        {
            n = n % N;
            n = N - n;
        }
        else
            n = Unumber(0) - n;

        if (par)
        {
            c = get_char();
            if (c != ')')
                throw "Expecting closing ')' in expression";
        }

        return;
    }

    if (!std::isdigit(c))
        throw string() + "Expecting number, got [" + c + "]";

    string s;
    while (std::isdigit(c))
    {
        s += c;
        c = get_char();
    }

    put_back(c);
    n = Unumber(s, Unumber::Decimal);
}

void Emulator::eat_spaces()
{
    char c = get_char();

    while (c == ' ' || c == '\t' || c == '\r' || c == '\n')
    {
        if (c == '\n') line_number++;
        c = get_char();
    }

    put_back(c);
}

char Emulator::get_char()
{
    char c;
    current_input_stream->get(c);
    if (!*current_input_stream) c = '\0';
    return c;
}

// reads [t[.s]:]t[.s] and sets 'is' to EOF if last
// returns true is placement present
bool Emulator::read_placement_cell(Cell & place, Cell & val, Unumber N)
{
    get_ts(val, N);
    eat_spaces();

    char c = get_char();
    if (c != ':')
    {
        put_back(c);
        return false;
    }

    place = val;
    get_ts(val, N);
    return true;
}

void Emulator::read_cell(Unumber N, Cell & addr)
{
    Cell place, value;
    bool placement = read_placement_cell(place, value, N);

    if (placement)
        addr = place;

    mem.put(addr, value);
}

void Emulator::mkmem_stream(Pragma & pgm, string pragmaClo)
{
    while (1)
    {
        char c = get_char();
        put_back(c);

        if (c != '#') break;

        string line;
        std::getline(*current_input_stream, line);

        if (line.size() > 8 && line.substr(0, 7) == "#pragma")
            pgm.read(line.substr(8));
    }

    if (!pragmaClo.empty())
        pgm.read(pragmaClo);

    // finished with pragma - set input type
    cq_type = pgm.cqtype;

    Cell addr(0, 0);
    const Unumber & N = pgm.N();
    Cell::setN(N);

    bool entry_set = false;

    while (*current_input_stream)
    {
        read_cell(N, addr);

        if (!entry_set)
        {
            entry = addr;
            entry_set = true;
        }

        eat_spaces();
        ++addr;

        if (!(addr.ts().t < N || N.iszero()))
            throw "Address reaches limit N while loading memory";
    }
}
