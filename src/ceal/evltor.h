// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: evltor.h

#pragma once

#include <map>
#include <set>
#include <string>

#include "nodes.h"

using std::string;

class Evaluator
{
        static const int MAX_SUBST = 100;

        Root * root;

    public:
        Evaluator(Pnode i): root(i->root()) {}

        void subst_macros();
        void expand_items();
        void move_defs();
        void fill3();
        void set_address();
        void resolve_names();
        void eval_expr();
        void overlap_check(bool throwOnErr);

    private:
        static Plitem make_litem(char c, Pnode pn, Token tok, bool tilda, bool qmk);

        void assign_address(Litem * pl, Cell & addr);
        void next_address(Cell & addr);

        Cell encrypt(Cell x);
};
