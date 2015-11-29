// CryptoLeq Processor Module
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: mmcalc.cpp

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "mmcalc.h"

using namespace mmc;
using std::string;
using std::cout;

const char * const modulus = "modulus"; // special variable

static bool go_on = true;
static int line = 1;
static int inside_expr = 0;
bool mmc::interactive = true;
std::istream * mmc::gin = nullptr;
static bool prn = false;

string tos(int x)
{
    return static_cast<std::ostringstream *>
           ( &(std::ostringstream() << x) )->str();
}

/*
Grammar
all := statements
statement := expr ; | ; | expr EOF
expr := var = expr
expr := esub | expr . esub | expr , esub | expr : esub
esub := term | esub + term | esub - term
term := powr | term * powr | term / powr | term % powr
powr := prim | prim ^ prim | prim ! | prim [ expr ]
prim := function arguments
prim := { statements expr }
prim := ( expr )
prim := -prim
prim := var - a123, abc
prim := number - 1234, 10101b, 0xabcdef
prim := string - "abc", "a\"\nb"

Syntax
Token := operator | name | number | string
operator: ^ * - + / % = ! += *= /= %= -= .
name: alpha alnum+
number: digit alnum+
string: "escape-char-sequence"

function:
    print expr
    delete var

*/

namespace mmc
{
struct Token
{
    enum Typ { Bad, Str, Num, Raw, Eof } typ;
    string str;
    Unumber num;
    Token(const string & s): typ(Raw), str(s), num(0) {}
    Token(char c): typ(Raw), str(string() + c), num(0) {}
    Token(Unumber n): typ(Num), num(n) {}

    Token(Token::Typ t, const string & s = ""): typ(t), str(s) {}

    bool is(const string & s) const { return typ == Raw && str == s; }
    string tos(unsigned base) const;

    Token opout(Token t, char op, Token * b) const;
    friend bool operator==(Token t1, Token t2);
};

bool operator==(Token t1, Token t2)
{
    if ( t1.typ != t2.typ ) return false;
    if ( t1.str != t2.str ) return false;
    return t1.num == t2.num;
}

} // mmc

typedef std::vector<mmc::Token> Vtoken;
static Vtoken g_token_buf;
void putback(Token t) { g_token_buf.push_back(t); }

struct Value
{
    std::vector<Token> x;

    Value() {}
    Value(Token t) { x.push_back(t); }

    string tos(unsigned base) const;

    Value operator+=(Value v) { opin(v, '+'); return *this; }
    Value operator-=(Value v) { opin(v, '-'); return *this; }
    Value operator*=(Value v) { opin(v, '*'); return *this; }
    Value operator/=(Value v) { opin(v, '/'); return *this; }
    Value operator%=(Value v) { opin(v, '%'); return *this; }
    Value operator&=(Value v) { opin(v, '^'); return *this; }

    void opin(Value v, char op);
    void factorial();

    friend bool operator==(Value v1, Value v2);
};

typedef std::map<string, Value> Vars;
Vars vars;
typedef std::map<string, Vtoken> Dictionary;
Dictionary dictionary;

void statement();
void mmc::mmcf()
{
    if ( interactive ) std::cout << "> ";

    while (go_on && !!*gin)
    {
        try
        {
            statement();
        }
        catch (string e)
        {
            inside_expr = 0;
            std::cout << "Error: (" << line << ") " << e << "\n";
        }
    }
}

void statement()
{
    Token input_token();
    Token t = input_token();
    if ( t.typ == Token::Eof ) { go_on = false; return; }
    if ( t.is(";")  ) return;

    putback(t);

    Value expr();
    inside_expr++;
    Value v = expr();
    inside_expr--;

    if (prn) std::cout << v.tos(10) << '\n';

    t = input_token();
    if ( t.typ == Token::Eof ) { go_on = false; return; }
    if ( t.is(";")  ) return;

    throw "Unexpected token [" + (t.typ == Token::Num ? t.num.str() : t.str) + "]:" + tos(t.typ);
}

Token input_token()
{
    Token input_token_nodic();
    Token t = input_token_nodic();

    Dictionary & d = dictionary;

    if ( t.typ != Token::Raw ) return t;

    Dictionary::iterator it = d.find(t.str);

    if ( it == d.end() ) return t;

    const Vtoken & vt = it->second;

    for ( size_t i = 0; i < vt.size(); i++ )
        putback(vt[vt.size() - i - 1]);

    return input_token();
}

Token input_token_nodic()
{
    if ( !g_token_buf.empty() )
    {
        Token t = g_token_buf.back();
        g_token_buf.pop_back();
        return t;
    }

    bool comm = false;

read:
    int ic = gin->get();
    if ( ic == std::char_traits<char>::eof() ) return Token(Token::Eof);
    char c = char(ic);

    if ( c == '#' ) comm = true;

    if ( c == ' ' || c == '\t' || c == '\r' ) goto read;
    if ( c == '\n' )
    {
        comm = false;

        if ( interactive )
            std::cout << (prn ? "prn" : "")
                      << (inside_expr ? "-" : "") << "> ";

        line++;
        goto read;
    }

    if ( comm ) goto read;

    Token input_number();
    Token input_word();
    Token input_strseq();

    if (0);
    else if ( std::isdigit(c) ) { gin->putback(c); return input_number(); }
    else if ( std::isalpha(c) || c == '_' ) { gin->putback(c); return input_word(); }
    else if ( c == '\"' ) { return input_strseq(); }

    gin->putback(c);
    Token symb();
    return symb();
}

Token symb()
{
    int ic = gin->get();
    if ( ic == std::char_traits<char>::eof() ) return Token(Token::Eof);
    char c = char(ic);

    if ( c == ';' || c == ',' || c == '(' || c == ')'
            || c == '[' || c == ']' || c == '!' || c == ':'
            || c == '{' || c == '}' || c == '.' ) return c;

    Token oper2(char c);
    if ( c == '+' || c == '-' || c == '/' || c == '*'
            || c == '%' || c == '^' || c == '=' ) return oper2(c);

    throw string() + "Unknown symbol [" + c + "]";
}

Token oper2(char c)
{
    int ic = gin->get();
    if ( ic == std::char_traits<char>::eof() ) return c;
    char c2 = char(ic);

    if ( c2 != '=' ) { gin->putback(c2); return c; }

    return Token(string() + c + c2);
}

Token input_word()
{
    string s;
    while (1)
    {
        int ic = gin->get();
        if ( ic == std::char_traits<char>::eof() ) break;
        char c = char(ic);
        if ( std::isalnum(c) || c == '_' ) s += c;
        else
        {
            gin->putback(c);
            break;
        }
    }

    return s;
}

Token input_number()
{
    Token t = input_word();
    void build_number(Token &);
    build_number(t);
    return t;
}

void build_number(Token & t)
{
    t.num = Unumber(t.str, Unumber::Decimal);
    t.typ = Token::Num;
    t.str = "";
}

Token input_strseq()
{
    string s;
    while (1)
    {
        int ic = gin->get();
        if ( ic == std::char_traits<char>::eof() ) break;
        char c = char(ic);
        if ( c == '\\' )
        {
            c = char(gin->get());
            if ( c == '\\' )c = '\\';
            if ( c == 'n' ) c = '\n';
            if ( c == 'r' ) c = '\r';
            if ( c == 't' ) c = '\t';
            if ( c == '"' ) c = '\"';
        }
        else if ( c == '\"' ) break;
        s += c;
    }

    return Token(Token::Str, s);
}

Value expr()
{
    Token t = input_token();
    if ( t.typ == Token::Raw )
    {
        Token t2 = input_token();

        if (0) {}
        else if ( t2.is("=") ) return ( vars[t.str] = expr() );
        else if ( t2.is("+=") ) return ( vars[t.str] += expr() );
        else if ( t2.is("-=") ) return ( vars[t.str] -= expr() );
        else if ( t2.is("*=") ) return ( vars[t.str] *= expr() );
        else if ( t2.is("/=") ) return ( vars[t.str] /= expr() );
        else if ( t2.is("%=") ) return ( vars[t.str] %= expr() );
        else if ( t2.is("^=") ) return ( vars[t.str] &= expr() );

        putback(t2);
    }
    putback(t);

    Value esub();
    Value es = esub();

    while (1)
    {
        t = input_token();
        if (0) {}
        else if ( t.is(",") ) es.opin(esub(), ',');
        else if ( t.is(":") ) es.opin(esub(), ':');
        else if ( t.is(".") ) es.opin(esub(), '.');
        else
        {
            putback(t);
            break;
        }
    }

    return es;
}

Value esub()
{
    Value term();
    Value tr = term();

    while (1)
    {
        Token t = input_token();
        if ( t.is("+") ) tr += term();
        else if ( t.is("-") ) tr -= term();
        else
        {
            putback(t);
            break;
        }
    }

    return tr;
}


Value term()
{
    Value powr();
    Value pw = powr();

    while (1)
    {
        Token t = input_token();
        if ( t.is("*") ) pw *= powr();
        else if ( t.is("/") ) pw /= powr();
        else if ( t.is("%") ) pw %= powr();
        else
        {
            putback(t);
            break;
        }
    }

    return pw;
}

Value powr()
{
    Value prim();
    Value pr = prim();

    while (1)
    {
        Token t = input_token();
        if ( t.is("^") ) pr &= prim();
        else if ( t.is("!") ) pr.factorial();
        else if ( t.is("[") )
        {
            Value v = expr();
            pr.opin(v, '[');
            Token t2 = input_token();
            if ( !t2.is("]") )
            {
                putback(t2);
                throw string() + "Expecting ']'";
            }
        }
        else
        {
            putback(t);
            break;
        }
    }

    return pr;
}


Value prim()
{
    Token t = input_token();

    if ( t.typ == Token::Str ) return t;
    if ( t.typ == Token::Num ) return t;

    if ( t.typ != Token::Raw ) throw "Unknown error at 'prim'";

    Vars::iterator it = vars.find(t.str);
    if ( it != vars.end() ) return it->second;

    if ( t.is("-") )
    {
        Value r(Unumber(0));
        r -= prim();
        return r;
    }

    if ( t.is("{") ) throw "Not implemented 744";

    if ( t.is("(") )
    {
        Value v = expr();
        Token t2 = input_token();
        if ( t2.is(")") ) return v;
        throw "Expecting ')' got [" + t.str + "]";
    }

    putback(t);

    Value func();
    return func();
}

Value func()
{
    Token t = input_token();

    Value print(unsigned base);
    Value modexpr();
    Value divide();
    Value deletevar();
    Value factors();
    Value factor1();
    Value assert();
    Value sizef();
    Value define();
    Value index();
    Value gcdf();
    Value lcmf();
    Value ordf();

    if (0) {}
    else if ( t.is("print") ) return print(10);
    else if ( t.is("printhex") ) return print(16);
    else if ( t.is("printbin") ) return print(2);
    else if ( t.is("delete") ) return deletevar();
    else if ( t.is("quit") || t.is("exit") || t.is("q") ) { go_on = false; return Value(); }
    else if ( t.is("div") ) return divide();
    else if ( t.is("mod") ) return modexpr();
    else if ( t.is("factors") ) return factors();
    else if ( t.is("factor") ) return factor1();
    else if ( t.is("assert") ) return assert();
    else if ( t.is("size") ) return sizef();
    else if ( t.is("define") ) return define();
    else if ( t.is("index") ) return index();
    else if ( t.is("gcd") ) return gcdf();
    else if ( t.is("lcm") ) return lcmf();
    else if ( t.is("ord") ) return ordf();
    else if ( t.is("prn") ) { prn = !prn; return Value(); }

    throw "Function [" + t.str + "] is not defined";
}

Value print(unsigned base)
{
    Value v = expr();
    std::cout << v.tos(base) << '\n';
    return v;
}

Value modexpr()
{
    Value savemod = vars[modulus];
    vars[modulus] = Value();

    Value v1 = expr();

    vars[modulus] = v1;

    Value v2 = expr();

    vars[modulus] = savemod;

    return v2;
}

Value divide()
{
    Value v1 = expr();
    Value v2 = expr();
    v1.opin(v2, 'd');
    return v1;
}

Value assert()
{
    Value v1 = expr();
    Value v2 = expr();
    if ( v1 == v2 ) return v1;
    throw "Assert failed";
}

Value gcdf()
{
    Value v1 = expr();
    Value v2 = expr();
    v1.opin(v2, 'g');
    return v1;
}

Value lcmf()
{
    Value v1 = expr();
    Value v2 = expr();
    v1.opin(v2, 'l');
    return v1;
}

Value index()
{
    Value v1 = expr();
    Value v2 = expr();
    v1.opin(v2, 'i');
    return v1;
}

Value deletevar()
{
    Token t = input_token();
    if ( t.typ != Token::Raw )
        throw "Bad token for delete function";

    if ( vars.find(t.str) == vars.end() ) return Value(0);
    vars.erase(t.str);
    return Value(1);
}

Value ordf()
{
    Value v = expr(), r;

    for ( size_t i = 0; i < v.x.size(); i++ )
        r.x.push_back( v.x[i].opout(v.x[i], 'o', 0) );

    return r;
}

string Value::tos(unsigned base) const
{
    string r;
    for ( size_t i = 0; i < x.size(); i++ )
        r += string(i ? "," : "") + x[i].tos(base);
    return r;
}

string Token::tos(unsigned base) const
{
    switch (typ)
    {
        case Str: return str;
        case Num: return num.str(base);
        case Bad: return "error(" + str + ")";
        case Raw: return "raw(" + str + ")";
        case Eof: return "EOF";
    }

    throw "Token::tos";
}

void Value::opin(Value v, char op)
{
    if ( op == ',' )
    {
        for ( size_t j = 0; j < v.x.size(); j++ )
            x.push_back( v.x[j] );

        return;
    }

    Value r;
    if ( op == '[' )
    {
        for ( size_t j = 0; j < v.x.size(); j++ )
        {
            Token t = v.x[j];
            if ( t.typ == Token::Str ) build_number(t);
            size_t index = size_t(t.num.to_ull());
            if ( index >= x.size() )
                throw "Index out of bounds: " + ::tos(index);
            else
                r.x.push_back( x[index] );
        }

        x.swap(r.x);
        return;
    }

    for ( size_t i = 0; i < x.size(); i++ )
        for ( size_t j = 0; j < v.x.size(); j++ )
        {
            if (0);
            else if ( op == 'i' )
            {
                if ( x[i] == v.x[j] )
                    r.x.push_back(Unumber(i));
            }
            else if ( op == ':' )
            {
                const Token & t1 = x[i];
                const Token & t2 = v.x[j];
                if ( t1.typ != Token::Num || t2.typ != Token::Num ) continue;
                for ( Unumber k = t1.num; !(t2.num < k); ++k )
                    r.x.push_back(k);
            }
            else if ( op == 'd' )
            {
                Token q(Unumber(0));
                Token a = x[i].opout(v.x[j], op, &q);
                r.x.push_back(q);
                r.x.push_back(a);
            }
            else
            {
                r.x.push_back( x[i].opout(v.x[j], op, 0) );
            }
        }

    x.swap(r.x);
}

void Value::factorial()
{
    Value r;

    for ( size_t i = 0; i < x.size(); i++ )
        r.x.push_back( x[i].opout(x[i], 'f', 0) );

    x.swap(r.x);
}

Token Token::opout(Token t, char op, Token * b) const
{
    Token m(*this);
    if ( typ == Bad || t.typ == Bad ) throw string() + "Cannot operate on Bad value";
    if ( typ == Raw || t.typ == Raw ) throw "Cannot operate on Raw value";

    if ( op == '.' )
    {
        if ( typ == Num ) m.str = m.num.str();
        if ( t.typ == Num ) t.str = t.num.str();
        return Token(Token::Str, m.str + t.str);
    }

    if ( typ == Str ) build_number(m);
    if ( t.typ == Str ) build_number(t);

    if ( t.typ != Num || m.typ != Num ) throw "Must operate on Num values";

    Unumber mod(0);
    Vars::iterator it = vars.find(modulus);
    if ( it != vars.end() && it->second.x.size() == 1 ) mod = it->second.x[0].num;

    if (0) {}
    else if ( op == '%' ) return m.num % t.num;
    else if ( op == 'd' ) return m.num.div(t.num, b->num);

    if ( !mod.iszero() )
    {
        if ( !(m.num < mod) ) m.num = m.num % mod;
        if ( !(t.num < mod) && (op != '^') ) t.num = t.num % mod;
    }

    if (0) {}

    else if ( op == '+' )
    {
        m.num += t.num;

        if ( !mod.iszero() )
            while ( !(m.num < mod) )
                m.num -= mod;

        return m.num;
    }

    else if ( op == '*' ) return (mod.iszero() ? m.num * t.num : m.num.mul(t.num, mod));

    else if ( op == '-' )
    {
        if ( m.num < t.num ) m.num += mod;
        m.num -= t.num;

        return m.num;
    }

    else if ( op == 'f' )
    {
        Unumber r(1);
        while ( !m.num.iszero() )
        {
            if ( mod.iszero() )
                r *= m.num;
            else
                r = m.num.mul(r, mod);
            --m.num;
        }
        return r;
    }

    else if ( op == 'o' )
    {
        if ( mod.iszero() ) return Unumber(0);
        Unumber r = m.num;
        for ( Unumber i = 1; i < mod; ++i )
        {
            if ( r == 1 ) return i;
            r = r.mul(m.num, mod);
        }
        return Unumber(0);
    }

    else if ( op == 'g' )
    {
        return ma::gcd(t.num, m.num);
    }

    else if ( op == 'l' )
    {
        Unumber g = ma::gcd(t.num, m.num);
        if ( g.iszero() ) return 0;
        return t.num / g * m.num;
    }

    else if ( op == '^' )
    {
        if ( mod.iszero() ) mod -= 1;
        m.num.pow(t.num, mod);
        return m.num;
    }

    else if ( op == '/' )
    {
        if ( mod.iszero() ) return m.num / t.num;
        Unumber xm1;
        bool k = ma::invert(t.num, mod, &xm1);
        if ( !k )
        {
            Unumber g = ma::gcd(t.num, m.num);
            if ( g == 1 ) { t.num = 0; return t; }
            t.num /= g;
            m.num /= g;
            bool k = ma::invert(t.num, mod, &xm1);
            if ( !k ) { t.num = 0; return t; }
        }
        return m.num.mul(xm1, mod);
    }

    else
        throw string() + "Operation [" + op + "] not supported";
}


Value factors()
{
    Value v = expr();
    Value r;

    for ( size_t i = 0; i < v.x.size(); i++ )
    {
        std::vector<string> factorize(const string & s);
        std::vector<string> vs = factorize(v.x[i].num.str());
        for ( size_t j = 0; j < vs.size(); j++ )
        {
            Token t(vs[j]);
            build_number(t);
            r.x.push_back(t);
        }
    }

    return r;
}

Value factor1()
{
    Value v = expr();
    Value r;

    for ( size_t i = 0; i < v.x.size(); i++ )
    {
        string factorone(const string & s);
        string vs = factorone(v.x[i].num.str());
        {
            Token t(vs);
            build_number(t);
            r.x.push_back(t);
        }
    }

    return r;
}

bool operator==(Value v1, Value v2)
{
    size_t sz = v1.x.size();
    if ( sz != v2.x.size() ) return false;

    for ( size_t i = 0; i < sz; i++ )
        if ( !(v1.x[i] == v2.x[i]) ) return false;

    return true;
}

Value sizef()
{
    Value v = expr();
    return Value(Token(Unumber(v.x.size())));
}

Value define()
{
    Token t = input_token();

    if ( t.is(";") ) throw "Bad define - no symbol";
    if ( t.typ != Token::Raw ) throw "Bad define - must be name";

    Vtoken v;

    while (1)
    {
        Token x = input_token();
        if ( x.is(";") ) { putback(x); break; }

        v.push_back(x);
    }

    dictionary[t.str] = v;

    return Value();
}
