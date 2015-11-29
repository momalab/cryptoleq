// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: contxt.h

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <istream>

#include "../unumber/unumberg.h"
#include "../processor/compiler.h"

#include "nodes.h"
#include "utils.h"
#include "stat.h"

using std::string;

struct Input_token_stream;

class Pragma;

class Context
{
    public:

        bool stdio = false;
        bool bshow = false;
        bool preproc = false;
        bool mmcalc = false;
        string input_file, output_file;
        string clo_pragma;
        string clo_show;
        string clo_seed;
        string separator;

        bool bcrypt = false;
        string crypt_file;
        string crypt_fun;

        bool do_translate = false;
        bool do_emulate = false;

        Compiler * compiler;
        Stat stat;

        void show() const;
        void crypt() const;

        void init_clo(int ac, const char * av[]);
        void run_mmc(int ac, const char * av[]);

        Tokens tokenize(const string & data) const;
        Pnode parse(const Tokens & tokens, Compiler * comp);
        void evaluate(Pnode & itr);
        Tokens compile(Pnode & itr, bool outx);
        string write(const Tokens & tokens, Pragma & pgm);
        void save(const string & data) const;
        void setcompiler(Compiler & comp) { compiler = &comp; }

    private:

        void itrshow(int stage, Pnode & itr);
};


