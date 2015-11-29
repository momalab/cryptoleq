
// Extended Modular Euclid

#include <vector>
#include <string>

using std::string;

template <class T>
class Meuclid
{
    public:

        struct Rec
        {
            T a, b, c, d, e, f, g;
        };

    private:

        T et, ft;
        typedef std::vector<Rec> Vr;
        Vr v;
        T gsmall;

        T mod;

        void fillAB(T x, T y);
        void fillEF();
        void fillG();

        void checkRec(const Rec & v0, const Rec & v,
                      const Rec & vb, T et, T ft);

        T mul(const T & a, const T & b) const { T t(a); t.mulMod(b, mod); return t; }
        T sub(const T & a, const T & b) const { T t(a); t.subMod(b, mod); return t; }
        T add(const T & a, const T & b) const { T t(a); t.addMod(b, mod); return t; }
        T neg(const T & a) const { T t(0); t.subMod(a, mod); return t; }
        T divq(const T & a, const T & b) const { T q; T::divABRQ(a, b, 0, &q); return q; }

    public:
        Meuclid(T x, T y, T m): et(0), ft(1), gsmall(0), mod(m) { fillAB(x, y); fillEFG(); }

        void fillEFG() { fillEF(); fillG(); }
        void setEF(T e, T f) { et = e; ft = f; }

        T gcd() const { return v[v.size() - 1].b; }

        string str() const;
        int size() const { return v.size(); }

        const Rec & operator[](int i) const { return v[i]; }

        T smallg() const { return gsmall; }

};


template <class T>
class Invertor
{
        Meuclid<T> e;
    public:
        Invertor(T x, T m): e(x, m, m) {}

        bool isOk() const { return e[e.size() - 1].b == T(1); }
        T get() const { return e[e.size() - 1].f; }
};

