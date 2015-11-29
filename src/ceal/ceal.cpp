// CryptoLeq Enhanced Assembly Language (CEAL)
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: ceal.cpp


/*
 *** input grammar ***
program := {statement}
statement := instruction | macrodef | macrouse
macrouse := [label:]+ .id {expression}
macrodef := .def id {id} [ : {id} ] eos {statement} .end eos
instruction := [ [~]. ] {l-items} eos
instruction := [~]{{ {l-items} }}
l-item  := ( [label:]+ item | id = expression )
item    := ( [~]expression | [[unumber]] | [[id]]
        | [[(const-term)]] | [~]"string" )
label   := id | tsnumber
expression := ( term | term+expression | term-expression )
term    := ( -term | (expression) | const | id | func )
const   := ( tsnumber | 'letter' | ? )
eos     := ';' | EOL
tsnumber  := t-number[.s-number] | ([r-number],m-number)
t,s,r,m-number are unumber

const-term := term which can be evaluated at parsing time
func := $id(args)
args := expression[,expression]+

TOKENS := . ~ ; [ ] ID NUMBER + - ( ) 'char' "STR" ? EOF EOL = { } : , $
COMMENTS := # <...> EOL

directives := .pragma .include
$functions := peekrnd, random, B2, beta, X, unit ...

 *** output grammar ***
instruction := [tsnumber(address):] ( LOLI(.) | LOLI({}) | LOLI(3) )
LOLI==list of l-items
l-item  := tsnumber(value)

String and char literal escapes: \a \b \f \n \r \t \v \\ \' \" \? \xhh
*/

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#define CRYPTOLEQVERSION "0.8"
#define SUBVERSION 1600

#include "utils.h"
#include "contxt.h"
#include "tokzer.h"
#include "emltor.h"
#include "getptr.h"
#include "errex.h"

using std::cout;
using std::string;

void tmain(int ac, const char * av[]);

int main(int ac, const char * av[])
try
{
    try { tmain(ac, av); }
    catch (string e) { throw; }
    catch (char const * e) { throw string(e); }
    catch (Err e) { throw e.str(); }
    catch (...) { throw string("unknown"); }
}
catch (string e)
{
    cout << "Error: " << e << '\n';
}

void usage()
{
    std::cout << "Cryptoleq Enhanced Assembler v"
              CRYPTOLEQVERSION "." << SUBVERSION <<
              ", Oleg Mazonka 2015\n";

    cout << "Compiled with: Cell=" << CellName << ", Memory=" << MemName
         << ", Bignum=" << BIGNUM_NAME << '\n';

    cout << "\nUsage: ceal [options] filename\n";
    cout << "\t-        \tio with stdin/stdout\n";
    cout << "\t-o file  \toutput file ('stdout' for console output)\n";
    cout << "\t-p pragma\tpragma string; overrides cryptoleq.pgm and input\n";
    cout << "\t-s       \tshow compiler parameters and exit\n";
    cout << "\t-t nnn   \tshow AST at different stages (0 to 8), e.g. 0237\n";
    cout << "\t-r seed  \trandom seed: either number or word 'time'\n";
    cout << "\t-E       \tpreprocess only, no parse or evaluate\n";

    cout << "\t-x       \texecute (default for .sce files)\n";
    cout << "\t-e       \ttranslate only (default for .sct files)\n";
    cout << "\t-a       \ttranslate and execute (default for .sca files)\n";
    cout << "\t-I       \tadd include path\n";
    cout << "\t-b ascii \tset space separator; default ' ' for TS, '\\n' for X\n";
    cout << "\t-c n f   \trun crypter; n={x|ts}{enc|dec[_r]|ord} f={file|@num}\n";
    cout << "\t-d file  \tcollect statistics, slower; file is input and output\n";
    cout << "\t-m expr  \trun mmcalc; expr optional, file name or expression \n";
}

void tmain(int ac, const char * av[])
{
    if ( ac < 2 ) { void usage(); usage(); return; }

    Context ctx;
    ctx.init_clo(ac, av);

    if (ctx.mmcalc) return;

    Pragma pgm("cryptoleq", CRYPTOLEQVERSION);

    pgm.load_file("cryptoleq.pgm");

    string data = "stdin";
    if (!ctx.input_file.empty())
        data = file2str(ctx.input_file);

    if (data.empty())
        throw "Cannot read file " + ctx.input_file;

    if ( ctx.do_translate )
    {
        {
            std::istringstream is(data);
            pgm.load_dot_input(is);
        }

        pgm.read(ctx.clo_pragma);

        Compiler comp;
        {
            const Pragma & g = pgm;

            Unumber r = make_unumber(ctx.clo_seed, 0);
            if ( r.iszero() ) r = g.R();

            comp.init_pqkru(g.N(), g.P(), g.Q(), g.K(), r, g.U());

            if (g.beta())
                comp.proc.setB2Beta(g.beta());

            for ( auto i : g.include_paths)
                Input_token_stream::include_paths.push_back(i);
        }

        Cell::setN(comp.proc.N);
        ctx.setcompiler(comp);

        if ( ctx.bshow )
        {
            ctx.show();
            comp.show();
            return;
        }

        if ( ctx.bcrypt )
        {
            ctx.crypt();
            return;
        }

        auto token_stream = ctx.tokenize(data);

        if ( ctx.preproc )
        {
            cout << ctx.write(token_stream, pgm);
            return;
        }

        auto itr = ctx.parse(token_stream, &comp);

        ctx.evaluate(itr);

        bool outputx = (pgm.cqtype == pgm.Cqtype::X);
        auto out_stream = ctx.compile(itr, outputx);

        data = ctx.write(out_stream, pgm);
    }

    if (!ctx.do_emulate)
    {
        ctx.save(data);
        ctx.stat.output_e();
        return;
    }

    // execute
    {
        Emulator emu;
        Pragma pgm("cryptoleq", CRYPTOLEQVERSION);

        {
            std::istringstream in(data);
            emu.current_input_stream = &in;
            emu.mkmem_stream(pgm, ctx.clo_pragma);
        }

        Cell::setN(pgm.N());
        ProcessorTS proc(pgm.N());

        emu.io_type = pgm.io;

        if ( !ctx.separator.empty() )
            emu.sep_ts = emu.sep_x = char(std::atoi(ctx.separator.c_str()));

        if (!pgm.entry.empty())
            emu.entry = str2ts(pgm.entry);

        Stat * stat = nullptr;

        if ( !ctx.stat.filename.empty() )
        {
            stat = &ctx.stat;
            stat->addrinit(); // in case no compilation
        }


        emu.run(proc, stat);

        if (stat)
            stat->output();
    }
}




