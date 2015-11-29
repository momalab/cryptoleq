// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: parser.h

#pragma once

#include <string>

#include "nodes.h"

using std::string;

class Parser
{
        const Tokens & ts;
        Tokens::size_type ip = 0;
        Root * root = nullptr;

    public:
        Parser(const Tokens & t) : ts(t) {}
        Pnode program(Compiler * comp);

    private:
        Pnode macro(Nodes & leftover_labels);
        Pnode macrodef();
        Pnode instruction(Nodes & leftover_labels);
        Pnode litem(Nodes & leftover_labels);
        Pnode id();
        Pnode expr();
        Pnode item();
        Pnode label();
        Pnode unumber();
        Pnode tsnumber();
        Pnode term();
        Pnode cnst();

        Nodes args();
};
