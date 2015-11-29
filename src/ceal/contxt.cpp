// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: contxt.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../mmcalc/mmcalc.h"

#include "contxt.h"
#include "tokzer.h"
#include "utils.h"
#include "parser.h"
#include "evltor.h"
#include "getptr.h"
#include "errex.h"
#include "pragma.h"

void Context::init_clo(int ac, const char * av[])
{
    for ( int i = 1; i < ac; i++ )
    {
        if ( av[i][0] != '-' ) { input_file = av[i]; continue; }

        string s = av[i];
        if ( s.size() == 1 ) stdio = true;
        else if ( s.size() == 2 )
        {
            if (0);
            else if ( s[1] == 's' ) bshow = true;
            else if ( s[1] == 'E' ) preproc = true;
            else if (s[1] == 'p' && ++i < ac) clo_pragma = av[i];
            else if (s[1] == 'o' && ++i < ac) output_file = av[i];
            else if (s[1] == 't' && ++i < ac) clo_show = av[i];
            else if (s[1] == 'r' && ++i < ac) clo_seed = av[i];
            else if ( s[1] == 'x' ) { do_emulate = true; do_translate = false; }
            else if ( s[1] == 'e' ) { do_emulate = false; do_translate = true; }
            else if ( s[1] == 'a' ) { do_emulate = do_translate = true; }
            else if (s[1] == 'b' && ++i < ac) separator = av[i];
            else if (s[1] == 'd' && ++i < ac) stat.init(av[i]);

            else if (s[1] == 'c' && i + 2 < ac)
            {
                bcrypt = true;
                crypt_fun = av[++i];
                crypt_file = av[++i];
            }

            else if (s[1] == 'I' && ++i < ac)
                Input_token_stream::include_paths.push_back(av[i]);

            else if ( s[1] == 'm' )
            {
                mmcalc = true;
                run_mmc(ac - i, &av[i]);
            }

            else
                throw "Unknown option [" + s + "]";
        }
        else
            throw "Unknown option [" + s + "]";
    }

    const string & in = input_file;

    string ext;
    if ( in.size() > 4 ) ext = in.substr( in.size() - 4 );

    if ( bcrypt && output_file.empty() )
    {
        output_file = crypt_file + ".out";
    }

    else if ( !in.empty() && output_file.empty() )
    {
        if (ext == ".sca" || ext == ".sct")
            output_file = in.substr(0, in.size() - 4) + ".sce";
        else
            output_file = in + ".sce";
    }

    if ( !do_emulate && !do_translate ) // not set - deduce
    {
        do_emulate = do_translate = true;

        if ( ext == ".sct" )
            do_emulate = false;

        if ( ext == ".sce" )
            do_translate = false;
    }
}

void Context::show() const
{
    std::cout << "Context: input=[" << input_file;
    std::cout << "] output=[" << output_file << "]\n";
}

Tokens Context::tokenize(const string & data) const
{
    std::istringstream fin(data);

    IncludeTranslator it(compiler->proc.N);

    Input_token_stream in(it);

    if ( data == "stdin" )
        in.is = &std::cin;
    else
    {
        if ( !fin ) throw "Internal error in Context::tokenize";
        in.is = &fin;
    }

    return in.tokenize();
}

void Context::save(const string & data) const
{
    std::ofstream fout;

    std::ostream * out;

    if (output_file.empty() || output_file == "stdout" || output_file == "-")
        out = &std::cout;
    else
    {
        fout.open(output_file.c_str());
        out = &fout;
        if (!fout) throw "Cannot open [" + output_file + "] for writing";
    }

    (*out) << data;
}

string Context::write(const Tokens & tokens, Pragma & pgm)
{
    std::ostringstream out;

    // first write pramga
    out << "#pragma N=" << compiler->proc.N.str();

    {
        string io = pgm.iostr();
        if ( !io.empty() )
            out << ' ' << io;
    }

    if ( !pgm.entry.empty() ) out << " entry=" << pgm.entry;
    if ( pgm.cqtype == pgm.Cqtype::X ) out << " cqtype=x";
    out << " id=" << pgm.id << " ver=" << pgm.ver;

    //out << '\n'; // new line is added by "tokens[0].line-1"

    if ( tokens.empty() ) return "";

    int line = tokens[0].line - 1;
    for ( auto t : tokens )
    {
        if ( line != t.line ) { out << '\n'; line = t.line; }
        else out << ' ';
        out << t.str();
    }

    out << '\n';

    return out.str();
}

Pnode Context::parse(const Tokens & tokens, Compiler * comp)
{
    return Parser(tokens).program(comp);
}

void Context::evaluate(Pnode & itr)
{
    // substitute macros
    // expand strings
    // move definitions
    // remove empty instructions and fill instruction3's
    // assign addresses
    // resolve names
    // evaluate expressions
    // check memory overlap
    // populate stat addresses

    Evaluator e(itr);
    int t = 0;

    itrshow(t++, itr);
    e.subst_macros();

    itrshow(t++, itr);
    e.move_defs();

    itrshow(t++, itr);
    e.expand_items();

    itrshow(t++, itr);
    e.fill3();

    itrshow(t++, itr);
    e.set_address();

    itrshow(t++, itr);
    e.resolve_names();

    itrshow(t++, itr);
    e.eval_expr();

    itrshow(t++, itr);
    e.overlap_check(do_emulate);

    itrshow(t++, itr);
    stat.populate_addr(*itr->root());
}


Tokens Context::compile(Pnode & itr, bool outx)
{
    Root * root = itr->root();

    const Unumber & N = root->comp->proc.N;
    if ( outx && N.iszero() )
        throw Err("N not defined: output request X format");

    Tokens r;

    Cell addr(0, 0);

    auto & proc = root->comp->proc;
    Nodes & instructions = root->children;

    for (auto i : instructions)
    {
        Nodes & lis = i->children;

        for (auto l : lis )
        {
            Litem * pl = get<Litem>(LNFUN, l);

            Token t(pl->tok());
            t.typk = Token::tStr;

            t.s = "";

            if (pl->address == addr) {}
            else
            {
                addr = pl->address;

                if (outx)
                    t.s = addr.x().str() + ":";
                else
                    t.s = addr.str() + ":";
            }

            if (outx)
                t.s += proc.str(Cell(pl->value.x(), Cell::TS));
            else
                t.s += proc.str(pl->value);

            r.push_back(t);
            ++addr;

        } // next litem
    } // next instruction

    return r;
}

void Context::itrshow(int stage, Pnode & itr)
{
    string sst = bug::to_string(stage);

    if ( clo_show.find(sst) == string::npos ) return;

    std::cout << "====AST" << sst << "=====\n";
    std::cout << itr->dump();
    std::cout << "= = root = =\n";
    std::cout << itr->root()->dump_names();
    std::cout << "============\n";
}

Unumber order(Unumber x, Unumber M)
{
    const unsigned ORCMAX = 10000;

    Unumber g = ma::gcd(M, x);

    if ( g != 1 )
        throw Err("Crypt order is not coprime to N ("
                  + x.str() + "|" + M.str() + ")");

    Unumber grn = x;
    Unumber gc = grn;

    unsigned cntr = ORCMAX;
    while (--cntr > 0)
    {
        gc = gc.mul(grn, M);

        if (gc == grn)
            break;
    }

    if (!cntr)
        throw Err("Crypt order is too high - increase constant ORCMAX ("
                  + bug::to_string(ORCMAX) + ")");

    return ORCMAX - cntr;
}

void Context::crypt() const
{
    string fun = crypt_fun;
    auto comp = compiler;

    auto f = [fun, comp](Unumber x)
    {
        if (0) {}

        else if (fun == "xenc")
        {
            Cell r = comp->encrypt(x);
            return r.x().str();
        }

        else if (fun == "xdec" || fun == "tsdec")
        {
            Cell a(x, Cell::X);
            return comp->decrypt(a, nullptr).str();
        }

        else if (fun == "xord" || fun == "tsord")
        {
            return order(x, comp->proc.N2).str();
        }

        else if (fun == "xdec_r" || fun == "tsdec_r")
        {
            Cell a(x, Cell::X);
            Unumber R;
            string d = comp->decrypt(a, &R).str();
            return d + " (" + R.str() + ")";
        }

        else if (fun == "tsenc")
        {
            Cell r = comp->encrypt(x);
            return r.str();
        }

        else
            throw Err("Invalid crypter function '" + fun + "'");
    };

    auto ts2x = [](string & x)
    {
        auto i = x.find('.');
        Unumber t, s(0);
        if ( i == string::npos )
            t = Unumber(x, Unumber::Decimal);
        else
        {
            t = Unumber(x.substr(0, i), Unumber::Decimal);
            s = Unumber(x.substr(i + 1), Unumber::Decimal);
        }

        x = Cell(t, s).x().str();
    };

    auto isTs = [](string x)
    {
        return !x.compare(0, 5, "tsdec") || !x.compare(0, 5, "tsord");
    };

    if ( !crypt_file.empty() && crypt_file[0] != '@' )
    {

        std::ifstream in(crypt_file.c_str());
        if (!in)
            throw Err("Cannot open " + crypt_file);

        string data;

        for (string s; in >> s;)
        {
            if ( isTs(fun) ) ts2x(s);
            Unumber x(s, Unumber::Decimal);
            string a = f(x);
            data += a + '\n';
        }

        save(data);
    }
    else
    {
        string s = crypt_file.substr(1);
        if ( isTs(fun) ) ts2x(s);
        Unumber x(s, Unumber::Decimal);
        std::cout << f(x) << '\n';
    }
}

void Context::run_mmc(int ac, const char * av[])
{
    if ( ac < 2 ) { mmc::gin = &std::cin; mmc::mmcf(); }
    else if ( ac == 2 )
    {
        std::ifstream in(av[1]);
        if ( !in ) throw "Cannot open " + string(av[1]);
        mmc::interactive = false;
        mmc::gin = &in; mmc::mmcf();
    }

    else
    {
        string s;
        for ( int i = 1; i < ac; i++ ) s += string() + ' ' + av[i];
        std::istringstream is(s);
        mmc::interactive = false;
        mmc::gin = &is; mmc::mmcf();
    }
}
