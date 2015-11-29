// CryptoLeq Processor Module
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: memcell.h

#pragma once

#ifndef CELLDEF
#define CELLDEF 4
#endif

#include <tuple>

#include "../unumber/unumberg.h"

class CellCommon
{
    protected:
        static Unumber N, N2;

        static bool inv(const Unumber & x, const Unumber & n, Unumber * xm1);
        static Unumber toX(Unumber t, Unumber s);
        static Unumber fake();

    public:

        struct TsVal
        {
            Unumber t, s;
            TsVal(Unumber at, Unumber as) : t(at), s(as) {}
        };

        enum Vtype { X, TS };

        static void setN(Unumber n) { N = N2 = n; N2 *= N; }
        static Unumber getN() { return N; }
        static Unumber inv(const Unumber & x, const Unumber & n);
};

class CellTs : public CellCommon
{
    private:

        TsVal v;

        static CellTs fromX(Unumber x);

    public:

        static const Vtype based = TS;

        CellTs() : v { 0, 0 } {}
        CellTs(Unumber at, Unumber as) : v { at, as } {}
        CellTs(Unumber at, Vtype vt) : v { at, 0 } { if (vt == X) v = fromX(at).v;  }

        TsVal ts() const { return v; }
        Unumber x() const { return toX(v.t, v.s); }

        bool operator<(const CellTs & x) const
        { return std::tie(v.s, v.t) < std::tie(x.v.s, x.v.t); }

        bool operator==(const CellTs & x) const
        { return std::tie(v.t, v.s) == std::tie(x.v.t, x.v.s); }

        void operator++() { ++v.t; overflow();  }
        void operator--() { --v.t; underflow();  }

        void overflow()
        {
            if (N.iszero()) return;
            if (v.t < N) return;
            v.t -= N;
        }

        void underflow()
        {
            if (N.iszero()) return;
            if (v.t < N) return;
            v.t += N;
        }

        string str() const;

        CellTs invert() const
        { return CellTs(CellCommon::inv(x(), N2), X); }

        CellTs operator*(const CellTs & z)
        {
            Unumber r = x().mul(z.x(), N2);
            return CellTs(r, X);
        }
};

class CellX : public CellCommon
{
        Unumber z;

    public:

        static const Vtype based = X;

        CellX() : z { 1 } {}
        CellX(Unumber t, Unumber s): z {toX(t, s)} {}
        CellX(Unumber t, Vtype v): z {t} { if (v == TS) z = toX(t, 0);  }

        TsVal ts() const;
        Unumber x() const { return z; }

        void operator++()
        {
            if (N.iszero()) { ++z; return; }
            z += N;
            if (z < N2) return;
            z -= N2;
        }

        void operator--()
        {
            if (N.iszero()) { --z; return; }
            z -= N;
            if (z < N2) return;
            z += N2;
        }

        string str() const;

        bool operator==(const CellX & x) const
        { return z == x.z; }

        bool operator<(const CellX & x) const
        { return z < x.z; }

        CellX invert() const
        { return CellX(CellCommon::inv(x(), N2), X); }

        CellX operator*(const CellX & x)
        {
            Unumber r = z.mul(x.z, N2);
            return CellX(r, X);
        }
};

template <class T>
class CellInv : public CellCommon
{
        T a;
        T b;

    public:

        static const Vtype based = T::based;

        static void setN(Unumber n) { CellCommon::setN(n);  T::setN(n); }

        CellInv(const T & za, const T & zb) : a(za), b(zb) {}

        CellInv() : a(), b() {}
        CellInv(Unumber t, Unumber s) : a { t, s }, b(a.invert()) {}
        CellInv(Unumber t, Vtype v) : a { t, v }, b(a.invert()) {}

        CellInv invert() const
        {
            CellInv r;
            r.a = b;
            r.b = a;
            return r;
        }

        TsVal ts() const { return a.ts(); }
        Unumber x() const { return a.x(); }

        void operator++() { ++a; --b; }
        void operator--() { --a; ++b; }
        string str() const { return a.str(); }
        bool operator==(const CellInv & x) const { return a == x.a; }
        bool operator<(const CellInv & x) const { return a < x.a; }

        CellInv operator*(const CellInv & x)
        {
            return CellInv(a * x.a, b * x.b);
        }
};

using CellInvTs = CellInv<CellTs>;
using CellInvX = CellInv<CellX>;

struct DoublePlug : public CellCommon
{
    using A = CellTs;
    using B = CellInvX;
    using D = DoublePlug;

    A a;
    B b;

    static const Vtype based = A::based;

    static void setN(Unumber n) { CellCommon::setN(n);  A::setN(n); B::setN(n); }

    DoublePlug(A x)
    {
        auto ts = x.ts();
        a = A(ts.t, ts.s);
        b = B(ts.t, ts.s);
    }

    void chk(TsVal a, TsVal b) const
    {
        if (a.t != b.t)
            throw "Boom1";
        if (a.s != b.s)
            throw "Boom2";
    }

    void chk(Unumber a, Unumber b) const
    {
        if (a != b)
            throw "Boom3";
    }

    void chk(A a, B b) const
    {
        auto ats = a.ts();
        auto bts = b.ts();

        auto ax = a.x();
        auto bx = b.x();

        chk(ats, bts);
        chk(ax, bx);
    }

    DoublePlug() : a(), b() {}
    DoublePlug(Unumber t, Unumber s) : a { t, s }, b { t, s } {}
    DoublePlug(Unumber t, Vtype v) : a { t, v }, b { t, v } {}

    D invert() const
    {
        A x = a.invert();
        B y = b.invert();
        chk(a, b);
        chk(x, y);
        D r(x);
        return r;
    }

    TsVal ts() const { auto ats = a.ts(); auto bts = b.ts(); chk(ats, bts); return ats; }
    Unumber x() const { auto ax = a.x(); auto bx = b.x(); chk(ax, bx); return ax; }

    void operator++() { ++a; ++b; chk(a, b); }
    void operator--() { --a; --b; chk(a, b); }
    string str() const { chk(a, b); return a.str(); }
    bool operator==(const D & x) const { chk(a, b); return a == x.a; }
    bool operator<(const D & x) const { chk(a, b); return a < x.a; }

    D operator*(const D & x)
    {
        A ra = a * x.a;
        B rb = b * x.b;
        chk(ra, rb);
        return D(ra);
    }
};

#if CELLDEF==1
using Cell = CellTs;
const char CellName[] = "1:Ts";

#elif CELLDEF==2
using Cell = CellX;
const char CellName[] = "2:X";

#elif CELLDEF==3
using Cell = CellInvTs;
const char CellName[] = "3:InvTs";

#elif CELLDEF==4
using Cell = CellInvX;
const char CellName[] = "4:InvX";

#elif CELLDEF==5
using Cell = DoublePlug;
const char CellName[] = "5:DoublePlug";

#else
#error CELLDEF bad value
#endif

Cell minusone();
