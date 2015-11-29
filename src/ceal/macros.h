// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: macros.h

#pragma once

#include <map>
#include <set>
#include <string>

#include "nodes.h"

using std::string;

struct Autobits
{
    Root * root;
	string nm;
	Autobits(Root * r, Macuse & mu) : root(r), nm(gen_name(mu)) {}

    Unumber val(Macuse & mu);
    string getid(Pnode n);
    string gen_name(Unumber n, string b0, string b1);
	string name() const { return nm;  }
private:
    string gen_name(Macuse & mu);
};

struct Confinement
{
    static const int ORDMAX = 100;
    Root * root;
    Macuse & mu;
    Confinement(Root * r, Macuse & amu) : root(r), mu(amu) {}
    Nodes table();
    static Pnode tsnum(Token tok, Cell c);
    static Pnode item(Pnode ts);
};

class Macros
{
        Root * root;
        Macdef * macdef_instance = nullptr;
        static int unique_counter;
        string unique_name();

    public:
        Macros(Root * r): root(r) {}
        Nodes process_macuse(Macuse & m);
        void process_autobits(Macuse & mu, Autobits & ab);

    private:

        void build_id_table(Idn & d, Pnode n);
        void build_id_table(Instruction & i);
        void build_id_table(Macuse & m);
        void build_id_table(Labels & l);
        void build_id_table(Pnode n);
        void instance_process_args(const string & name, int idx);
        void instance_process_locs(const string & name);
        void instantiate(Macdef & md, Macuse & mu);
};
