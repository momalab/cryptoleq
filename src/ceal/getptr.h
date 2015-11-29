// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: getptr.h

#pragma once

#include "nodes.h"
#include "errex.h"


using std::string;


#define LNFUN __LINE__,__FUNCTION__,__FILE__
#define NOTHR 0,0,0
template<class T>
inline T * get(int line, const char * fn, const char * fl, Pnode p)
{
    Node * n = p.get();
    if (n)
    {
        T * rp = dynamic_cast<T *>(n);
        if (rp)
            return rp;

        if (!fn) // no throw
            return nullptr;
    }

    string e = "Internal error - ";
    e += fl;
    e += ":";
    e += fn;
    e += ":" + bug::to_string(line);
    e += "";

    throw Err(e, n ? n->tok() : Token(-1, -1));
}
