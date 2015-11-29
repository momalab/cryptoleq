// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: tokzer.h

#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <set>

#include "nodes.h"
#include "utils.h"

using std::string;


struct TokenIncludeFunction
{
    virtual Tokens operator()(Token) = 0;
    virtual Tokens prefx(const string & file, Token start) { return Tokens(); }
    virtual Tokens pstfx() { return Tokens(); }
};
using Tif = TokenIncludeFunction;

struct TokenIncludeTranslator
{
    virtual Tif & operator()(const string & func) = 0;
};


using Tit = TokenIncludeTranslator;

struct Input_token_stream
{
        static std::set<string> include_once;
        static std::vector<string> include_paths;
        static std::vector<string> include_files;

        // current file index
        static int cfi() { return include_files.size(); }

        static const bool USEBUF = true;
        std::vector<char> buf;
        std::istream * is;
        std::ifstream fis;
        string file;
        int line, pos;
        Tit & tit;

        Input_token_stream(Tit & t) : is(nullptr), line(1), pos(1), tit(t) {}
        Input_token_stream(Tit & t, string file);

        Tokens tokenize();

    private:

        int depth = 0;
        static const int MAX_DEPTH = 20;

        bool read_tok_eof(Token & t) { return read_symb(t, '\0', Token::tEof); }
        bool read_tok_eol(Token & t) { return read_symb(t, '\n', Token::tEol); }
        bool read_tok_qmk(Token & t) { return read_symb(t, '\?', Token::tQmk); }
        bool read_tok_dot(Token & t) { return read_symb(t, '.', Token::tDot); }
        bool read_tok_lbr(Token & t) { return read_symb(t, '(', Token::tLbr); }
        bool read_tok_rbr(Token & t) { return read_symb(t, ')', Token::tRbr); }
        bool read_tok_min(Token & t) { return read_symb(t, '-', Token::tMin); }
        bool read_tok_pls(Token & t) { return read_symb(t, '+', Token::tPls); }
        bool read_tok_tld(Token & t) { return read_symb(t, '~', Token::tTld); }
        bool read_tok_smc(Token & t) { return read_symb(t, ';', Token::tSmc); }
        bool read_tok_lsq(Token & t) { return read_symb(t, '[', Token::tLsq); }
        bool read_tok_rsq(Token & t) { return read_symb(t, ']', Token::tRsq); }
        bool read_tok_eql(Token & t) { return read_symb(t, '=', Token::tEql); }
        bool read_tok_lcu(Token & t) { return read_symb(t, '{', Token::tLcu); }
        bool read_tok_rcu(Token & t) { return read_symb(t, '}', Token::tRcu); }
        bool read_tok_col(Token & t) { return read_symb(t, ':', Token::tCol); }
        bool read_tok_dol(Token & t) { return read_symb(t, '$', Token::tDol); }
        bool read_tok_cma(Token & t) { return read_symb(t, ',', Token::tCma); }

        bool read_tok_num(Token & t);
        bool read_tok_str(Token & t);
        bool read_tok_chr(Token & t);
        bool read_tok_id(Token & t);
        bool read_tok_dotid(Token & t);

        bool read_directive(Token & t, Tokens & toks);

        char get_char();
        void eat_spaces();

    private:

        bool read_symb(Token & t, char symb, Token::Typk typ);
        void put_back(char c);
        char get_esc_char();
        void readon_include(Tokens & toks);
        string readon_pragma();
};

struct TokenTranslatorAsis : Tif
{
    Tokens operator()(Token t) { return Tokens { t }; }
};

struct TokenTranslatorDatax : Tif
{
    Unumber N;
    Token start;
    string file;
    TokenTranslatorDatax(Unumber n): N(n), start(-1, -1) {}
    Tokens operator()(Token t);
    Tokens prefx(const string & f, Token t) { file = f;  return Tokens { Token((start = t), Token::tDot) }; }
};

struct IncludeTranslator : Tit
{
    TokenTranslatorAsis asis;
    TokenTranslatorDatax datax;
    Tif & operator()(const string & func)
    {
        if (func == "asis") return asis;
        if (func == "datax") return datax;
        throw "Include function '" + func + "' is not defined";
    }

    IncludeTranslator(Unumber n) : datax(n) {}
};


