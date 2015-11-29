// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: emltor.h

#pragma once

#include <istream>

#include "../processor/compiler.h"
#include "utils.h"
#include "memory.h"
#include "pragma.h"
#include "stat.h"

struct Emulator
{
    Memory mem;
    Cell entry;
    Pragma::Io io_type = Pragma::Io::Ts;
    Pragma::Cqtype cq_type = Pragma::Cqtype::Ts;
    bool steps = false;
    bool trace = false;
    unsigned line_number = 1;
    std::istream * current_input_stream = nullptr;
    char sep_ts = ' ';
    char sep_x = '\n';

    void run(ProcessorTS &, Stat *);
    void get_ts(Cell & v, Unumber N);
    void get_number(Unumber & n, Unumber N);
    char get_char();
    void eat_spaces();
    void put_back(char c) { if (*current_input_stream) current_input_stream->putback(c); }

    bool read_placement_cell(Cell & place, Cell & val, Unumber N);
    void read_cell(Unumber N, Cell & addr);
    void mkmem_stream(Pragma & pgm, string pragmaClo);
};
