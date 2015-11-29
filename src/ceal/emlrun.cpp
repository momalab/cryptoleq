// CryptoLeq, Emulator
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: emlrun.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>

#include "utils.h"
#include "emltor.h"

using std::cout;
using std::string;

// list of numbers
// number is [t[.s]:]t[.s]

bool iosetbyclo = false;

unsigned g_line_number = 1;

string pragmaClo;

void tmain(int ac, const char * av[]);

int main(int ac, const char * av[])
try
{
    try { tmain(ac, av); }
    catch (string e) { throw; }
    catch (char const * e) { throw string(e); }
    catch (...) { throw string("unknown"); }
}
catch (string e)
{
    cout << "Error (sqcrun) " << "(" << g_line_number << ") : " << e << '\n';
}

void usage()
{
    cout << "Usage: sqcrun [options] input.sqc\n";
    cout << "\t-\tinput from stdin (no input file)\n";
    cout << "\t-i type\tinput/output type\n";
    cout << "\t\tnum\tts-numerical (default)\n";
    cout << "\t\tx\tX-numerical\n";
    cout << "\t\ttext\tASCII text\n";
    cout << "\t-s\tshow processor parameters and exit\n";
    cout << "\t-p <pragma>\tpragma string (overrides file pragma)\n";
    cout << "\t-t\ttracer\n";
    cout << "\t-n\tcount number of instructions\n";
}

void tmain(int ac, const char * av[])
{
    if ( ac < 2 ) { void usage(); usage(); return; }

    string file;
    bool bcin = false;
    bool show_proc = false;

    Emulator emu;

    try
    {

        for (int i = 1; i < ac; i++)
        {
            if (av[i][0] != '-') { file = av[i]; continue; }

            string s = av[i];
            if (s.size() == 1) bcin = true;
            else if (s.size() == 2)
            {
                if (s[1] == 's') show_proc = true;
                if (s[1] == 'n') emu.steps = true;
                if (s[1] == 't') emu.trace = true;
                if (s[1] == 'p' && ++i < ac) pragmaClo = av[i];
                if (s[1] == 'i' && ++i < ac)
                {
                    emu.io_type = Pragma::str2io(av[i]);
                    iosetbyclo = true;
                }
            }
            else
                throw "Unknown option [" + s + "]";
        }

        Pragma pgm("cryptoleq", "0.6");

        if (bcin)
        {
            emu.current_input_stream = &std::cin;
            emu.mkmem_stream(pgm, pragmaClo);
        }
        else
        {
            std::ifstream in(file.c_str());
            if (!in) throw "Cannot open '" + file + "' cwd=(" + cwd() + ")";
            emu.current_input_stream = &in;
            emu.mkmem_stream(pgm, pragmaClo);
        }

        ProcessorTS proc(pgm.N());
        Cell::setN(proc.N);

        if (show_proc)
        {
            proc.show();
            return;
        }

        if (!iosetbyclo) emu.io_type = pgm.io;
        if (!pgm.entry.empty()) emu.entry = str2ts(pgm.entry);

        emu.run(proc, nullptr);
    }
    catch (...)
    {
        g_line_number = emu.line_number;
        throw;
    }
}

