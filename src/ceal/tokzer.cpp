// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: tokzer.cpp

#include <cstdlib>

#include "utils.h"
#include "getptr.h"
#include "errex.h"
#include "tokzer.h"

std::vector<string> Input_token_stream::include_files;
std::vector<string> Input_token_stream::include_paths;
std::set<string> Input_token_stream::include_once;


Input_token_stream::Input_token_stream(Tit & t, string f)
    : is(&fis), file(f), line(1), pos(1), tit(t)
{
    if (!isfile(f))
    {
        string ff;
        for (auto i : include_paths)
        {
            ff = i + "/" + f;
            if (isfile(ff))
            {
                f = ff;
                break;
            }
        }
    }

    fis.open(f.c_str());

    if (!fis)
    {
        string e;
        if (!include_paths.empty())
        {
            e += "Tried paths:\n\t.\n";
            for (auto i : include_paths)
                e += "\t" + i + "\n";
        }
        throw e + "Cannot open [" + file + "] cwd=(" + cwd() + ")";
    }
}

bool Input_token_stream::read_symb(Token & t, char symb, Token::Typk typ)
{
    char c = get_char();

    if ( c != symb )
    {
        put_back(c);
        return false;
    }

    t.typk = typ;

    return true;
}

bool Input_token_stream::read_tok_chr(Token & t)
{
    eat_spaces();

    char c = get_char();

    if (c != '\'')
    {
        put_back(c);
        return false;
    }

    c = get_esc_char();
    t.s = string() + c;
    c = get_char();

    if (c != '\'') throw Err("Bad char literal", line, pos - 1, cfi() );

    t.typk = Token::tChr;

    return true;
}

bool Input_token_stream::read_tok_str(Token & t)
{
    eat_spaces();

    char c = get_char();

    if (c != '\"')
    {
        put_back(c);
        return false;
    }

    string s;
    while (1)
    {
        c = get_esc_char();
        if (c == '\"') break;
        s += c;
    }

    t.s = s;
    t.typk = Token::tStr;

    if (s.empty()) throw Err("Empty string", line, pos - 1, cfi());

    return true;
}

bool Input_token_stream::read_tok_num(Token & t)
{
    eat_spaces();

    char c = get_char();

    if ( !std::isdigit(c) )
    {
        put_back(c);
        return false;
    }

    string s;
    while ( std::isdigit(c) )
    {
        s += c;
        c = get_char();
    }

    put_back(c);

    t.un = Unumber(s, Unumber::Decimal);
    t.typk = Token::tNum;

    return true;
}

void Input_token_stream::eat_spaces()
{
    const bool ES = false; // Eof is space

    char c = get_char();

    while ( c == ' ' || c == '\t' || c == '\r'
            || ( ES && c == '\n' ) || c == '#' )
    {
        // comment
        if ( c == '#' )
            while (c != '\n' && c) c = get_char();

        else // goto to the next byte
            c = get_char();
    }

    put_back(c);
}

char Input_token_stream::get_char()
{
    char c;

    if (USEBUF && !buf.empty() )
    {
        c = buf.back();
        buf.pop_back();
    }
    else
        is->get(c);

    if ( !*is ) c = '\0';
    ++pos;
    if ( c == '\n' ) { line++; pos = 1; }
    return c;
}

void Input_token_stream::put_back(char c)
{
    if (!*is) return;

    if (USEBUF)
        buf.push_back(c);
    else
        is->putback(c);

    --pos;
    if (c == '\n') { --line; pos = 1; }
}

char Input_token_stream::get_esc_char()
{
    char c;
    c = get_char(); if (!c) throw Err("End of file while reading string");

    if (c != '\\')
    {
        if (c >= 32 || c <= 126) return c;
        throw Err("Non printable char, use hex \\xXX", line, pos, cfi());
    }

    c = get_char(); if (!c) throw Err("End of file while reading string");

    const char esca[] = "xabfnrtv\\\'\"\?";
    const char escc[] = "x\a\b\f\n\r\t\v\\\'\"\?";

    static_assert(sizeof(esca) == sizeof(escc), "Escapce sequence");

    auto i = string(esca).find(c);
    if ( i == string::npos )
        throw Err("Unknown escape sequence (" + print_char(c) + ")", line, pos - 1, cfi());

    if (c != 'x')
        return escc[i];

    string h;
    h += get_char();
    h += get_char();
    char * e = nullptr;
    const char * ch = h.c_str();
    long x = std::strtol(ch, &e, 16);

    if (e != ch + 2 || x < 0 || x > 255)
        throw Err("Bad hex escape sequence", line, pos - 1, cfi());

    return char(x);
}

bool Input_token_stream::read_tok_id(Token & t)
{
    eat_spaces();

    char c = get_char();

    if ( !std::isalpha(c) && c != '_' )
    {
        put_back(c);
        return false;
    }

    string s;
    while ( std::isalnum(c) || c == '_' )
    {
        s += c;
        c = get_char();
    }

    put_back(c);

    t.s = s;
    t.typk = Token::tIdt;

    return true;
}

bool Input_token_stream::read_tok_dotid(Token & t)
{
    eat_spaces();

    char c = get_char();

    if ( c != '.' )
    {
        put_back(c);
        return false;
    }

    char c2 = get_char();
    put_back(c2);

    if ( !std::isalpha(c2) && c2 != '_' )
    {
        put_back(c);
        return false;
    }

    if ( !read_tok_id(t) )
    {
        put_back(c);
        return false;
    }

    return true;
}

bool Input_token_stream::read_directive(Token & t, Tokens & toks)
{
    if ( !read_tok_dotid(t) )
        return false;

    if (t.s == "include")
    {
        try
        {
            readon_include(toks);
            t.typk = Token::tIgn;
        }
        catch (string e)
        {
            if ( file.empty() )
                throw Err(e, t);

            throw Err(e + ", file '" + file + "'", t);
        }
    }

    else if (t.s == "pragma")
    {
        if (!toks.empty())
        {
            for ( auto i : toks )
                if (!i.is(Token::tEol))
                    throw Err("Pragma must be declared at the top", t);
        }

        t.s = readon_pragma();
        t.typk = Token::tIgn;
    }

    else
    {
        t.typk = Token::tMac;
    }

    // dotid
    return true;
}

string Input_token_stream::readon_pragma()
{
    char c = '\0';
    bool st = true;
    string r;
    while (is)
    {
        c = get_char();
        if (c == '\n') break;

        if (std::isblank(c) && st)
            continue;

        st = true;
        if ( c != '\r' ) r += c;
    }

    return r;
}

Tokens Input_token_stream::tokenize()
{
    Tokens r;

    while (!!*is)
    {
        eat_spaces();

        Token t(line, pos);

        if (0);

        else if (read_directive(t, r)) {}

        else if (read_tok_dotid(t)) {}
        else if (read_tok_eof(t)) {}
        else if (read_tok_num(t)) {}
        else if (read_tok_dot(t)) {}
        else if (read_tok_lbr(t)) {}
        else if (read_tok_rbr(t)) {}
        else if (read_tok_min(t)) {}
        else if (read_tok_eol(t)) {}
        else if (read_tok_str(t)) {}
        else if (read_tok_chr(t)) {}
        else if (read_tok_tld(t)) {}

        else if (read_tok_id(t)) {}
        else if (read_tok_smc(t)) {}
        else if (read_tok_lsq(t)) {}
        else if (read_tok_rsq(t)) {}
        else if (read_tok_pls(t)) {}
        else if (read_tok_qmk(t)) {}
        else if (read_tok_lcu(t)) {}
        else if (read_tok_rcu(t)) {}
        else if (read_tok_eql(t)) {}
        else if (read_tok_col(t)) {}
        else if (read_tok_dol(t)) {}
        else if (read_tok_cma(t)) {}

        if ( t.typk == Token::tNull )
        {
            string sc = print_char(get_char());
            throw Err(string() + "Bad token '" + sc + "'", line, pos, cfi());
        }

        if ( t.typk != Token::tIgn )
            r.push_back(t);
        else if (t.s == "once")
        {
            if (include_once.find(file) == include_once.end())
                include_once.insert(file);
            else
                return Tokens();
        }
    }

    if (r.empty())
        return r;

    Token t = r[r.size() - 1];

    if (!t.is(Token::tEof))
    {
        r.push_back(Token(t, Token::tEol));
        r.push_back(Token(t, Token::tEof));
    }

    return r;
}

void Input_token_stream::readon_include(Tokens & toks)
{
    string func = "asis";
    string file;

    Token t(line, pos);

    if (read_tok_str(t))
        file = t.s;

    else
    {
        if (!read_tok_id(t))
            throw Err("Expecting include method or \"filename\"", t);

        func = t.s;

        if (!read_tok_str(t))
            throw Err("Format: .include [method] \"filename\"", t);

        file = t.s;
    }

    // now func has include function type and file has filename

    if (depth > MAX_DEPTH)
        throw Err("Include directive reached max depth, file '" + file + "'", line, pos, cfi());

    Input_token_stream in(tit, file);
    in.depth = depth + 1;

    include_files.push_back(file);

    Tokens inc = in.tokenize();

    for (auto & j : inc)
        if (!j.file_idx)
            j.file_idx = include_files.size();

    if (inc.empty())
        return;

    if (inc.back().is(Token::tEof))
        inc.pop_back();

    if (inc.empty())
        return;

    auto & tif = tit(func);

    Tokens ts = tif.prefx(file, inc[0]);
    for (auto j : ts) toks.push_back(j);

    for (auto i : inc)
    {
        ts = tif(i);
        for ( auto j : ts)
            toks.push_back(j);
    }

    ts = tif.pstfx();

    for (auto j : ts)
        toks.push_back(j);
}

Tokens TokenTranslatorDatax::operator()(Token t)
{
    Tokens toks;

    t.line = start.line; // putting on the same line

    if (0) {}
    else if ( t.is(Token::tEol)) {}
    else if ( t.is(Token::tEof)) {}
    else if (t.is(Token::tNum))
    {
        Cell y(t.un, Cell::X);
        auto yts = y.ts();
        toks.push_back(Token(t, yts.t));
        if ( !yts.s.iszero() )
        {
            toks.push_back(Token(t, Token::tDot));
            toks.push_back(Token(t, yts.s));
        }
    }
    else
        throw Err("Unexpected token in file '" + file + "'", t);

    return toks;
}

