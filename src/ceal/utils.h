// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: utils.h

#pragma once

#include <string>
#include <vector>
#include <map>

#include "../unumber/unumberg.h"
#include "../processor/memcell.h"


using std::string;

string print_char(char c);
int hex_char_to_int(char c);
Unumber make_unumber(string v, Unumber def);
Cell str2ts(const string & s);
string file2str(const string & file);
bool isfile(const string & file);

string cwd();


