// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: prog_sec.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include "secure_types.h"

using std::cout;

Compiler compiler;

typedef secure_int<compiler> sec_int;
typedef open_int<compiler> opn_int;
typedef secure_ptr<compiler> sec_ptr;

struct TableFile
{
    static const char * const name;

    opn_int size();
    void load(sec_ptr &);
};
const char * const TableFile::name = "db.sec";

void save_output(sec_int);

void test()
{
    opn_int o16 = opn_int::two * opn_int::two * opn_int::two * opn_int::two;

    cout << opn_int::two.show() << '\n';
    cout << o16.show() << '\n';

    sec_int s0 = sec_int::zero;
    sec_int s1 = sec_int::one;

    cout << s0.show() << ' ' << s1.show() << '\n';
    cout << "0+1= " << (s0 + s1).show() << '\n';
    sec_int s2 = s1 + s1;
    cout << "1+1=2= " << s2.show() << '\n';
    sec_int s3 = s2 + s1;
    sec_int s4 = s2 + s2;
    cout << "3+2= " << (s3 + s2).show() << '\n';
    cout << "0*1= " << (s0 * s1).show() << '\n';
    cout << "1*1= " << (s1 * s1).show() << '\n';
    cout << "1*2= " << (s1 * s2).show() << '\n';
    cout << "2*2= " << (s2 * s2).show() << '\n';
    cout << "0*3= " << (s0 * s3).show() << '\n';
    cout << "1*3= " << (s1 * s3).show() << '\n';
    cout << "3*2= " << (s3 * s2).show() << '\n';
    cout << "3*3= " << (s3 * s3).show() << '\n';
    cout << "3*4= " << (s3 * s4).show() << '\n';
    cout << "4*4= " << (s4 * s4).show() << '\n';
}

int main() try
{
    cout << "Compiled with: Cell=" << CellName << ", Bignum=" << BIGNUM_NAME << '\n';

    compiler.init_pqkru();
    compiler.show();

    Cell::setN(compiler.proc.N);

    open_int<compiler>::init_consts();
    secure_int<compiler>::init_consts();

    TableFile tablefile;

    sec_ptr tbl(tablefile.size()*opn_int::two, compiler.random());
    tablefile.load(tbl);

    sec_int load_input();
    sec_int input = load_input();

    if (0) {test(); return 0;}

    sec_int sum = sec_int::zero;

    opn_int tbl_sz = tablefile.size();

    cout << "entries : " << tbl_sz.show() << '\n';
    cout << "input   : " << input.show() << '\n';

    for ( opn_int i = opn_int::zero; i < tbl_sz; ++i )
    {
        sec_int key = tbl[i + i];
        sec_int val = tbl[i + i + opn_int::one];

        if (0)
        {
            sec_int ki = key.equal(input);
            sec_int mv = ki * val;
            sec_int s0 = sum;
            sum += mv;

            cout << "i:" << input.show()
                 << "  ki:" << ki.show()
                 << "  val:" << val.show()
                 << "  mv:" << mv.show()
                 << "  s0:" << s0.show()
                 << "  sum:" << sum.show()
                 << '\n';
        }
        else
            sum += key.equal(input) * val;

        cout << "key=" << key.show() << "\t\t" << "sum=" << sum.show() << '\n';
    }

    save_output(sum);
}
catch (const char * e) { cout << "Error: " << e << "\n"; }
catch (std::string e) { cout << "Error: " << e << "\n"; }
catch (std::exception & e) { cout << "Error: std::exception: " << e.what() << "\n"; }
catch (...) { cout << "Exception thrown\n"; }

void save_output(sec_int x)
{
    std::ofstream of("output.sec");
    of << x.str() << '\n';
}

sec_int load_input()
{
    const char * name = "input.sec";
    std::ifstream in(name);
    if ( !in ) throw string() + "Cannot open " + name;

    Unumber u;
    in >> u;
    return u;
}

void TableFile::load(sec_ptr & table)
{
    std::ifstream in(name);
    if ( !in ) throw string() + "Cannot open " + name;

    opn_int i = opn_int::zero;

    for ( Unumber x; in >> x; i++ )
        table[i] = x;
}

open_int<compiler> TableFile::size()
{
    std::ifstream in(name);
    if ( !in ) throw string() + "Cannot open " + name;

    open_int<compiler> i = open_int<compiler>::zero;

    for ( Unumber x; in >> x >> x; i++ ) {}

    return i;
}

