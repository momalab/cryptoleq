// / CryptoLeq / C++ prototype
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: secure_ptr.h

#include <map>

#include "base_int.h"
#include "open_int.h"
#include "secure_int.h"

template <Compiler & M>
class secure_ptr : public base_int<M>
{
        secure_int<M> ptr;
        std::map< open_int<M>, secure_int<M> > data;

    public:
        secure_ptr(const open_int<M> &, secure_int<M> p) : ptr(p) {}
        ~secure_ptr() {}

        secure_int<M> & operator[](const open_int<M> &);

    private:
        secure_ptr(const secure_ptr &);
        void operator=(const secure_ptr &);
};
