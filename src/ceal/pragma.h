// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: pragma.h

#include <map>
#include <vector>

#include "../unumber/unumberg.h"

#include "utils.h"

class Pragma
{
        Unumber n;
        string sp, sq, sk, sr, su;
        unsigned ubeta = 0;
        std::map<string, string> other_values;

        void init() { n = 0; }

    public:
        string id;
        string ver;
        enum class Io { Ts, Text, X } io = Io::Ts;
        string entry;
        enum class Cqtype { Ts, X } cqtype = Cqtype::Ts;
        std::vector<string> include_paths;

    public:
        Pragma(string i, string v): id(i), ver(v) { init(); }

        void read(std::istream & is);
        void read(const string & line);
        Unumber N() const { return n; }

        void load_file(const string & file);
        void load_dot_input(const string & file);
        void load_dot_input(std::istream & is);

        Unumber P() const { return make_unumber(sp, 0); }
        Unumber Q() const { return make_unumber(sq, 0); }
        Unumber K() const { return make_unumber(sk, 1); }
        Unumber R() const { return make_unumber(sr, 0); }
        unsigned U() const;
        unsigned beta() const { return ubeta; }

        string iostr() const;
        static Io str2io(const string & s);
};
