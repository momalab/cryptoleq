
#include "cunumber.h"

// MS compiler non conformant to C99 inline ; also can be used as -Dinline=__inline
#ifdef _MSC_VER
#define inline __inline
#endif


static inline unsigned add_base(UBASE_TYPE * result, UBASE_TYPE a, UBASE_TYPE b, unsigned bit)
{
    *result = a + b;
    b = ~b;
    if ( !bit )
    {
        if ( a > b ) return 1;
        return 0;
    }

    ++*result;
    if ( a > b ) return 1;
    if ( *result ) return 0;
    return 1;
}

inline void ctozero(Cunumber * n)
{
    int i = 0;
    for (; i < UNUMBER_SIZE; i++) n->x[i] = 0;
    n->o = 0;
}

inline Cunumber cnewzero()
{
    Cunumber n;
    ctozero(&n);
    return n;
}

inline int cless(const Cunumber * n1, const Cunumber * n2)
{
    int o = n1->o, i;
    if ( n2->o != o ) return ( o < n2->o );
    for ( i = o; i >= 0; i-- )
    {
        if ( n1->x[i] < n2->x[i] ) return 1;
        if ( n1->x[i] > n2->x[i] ) return 0;
    }
    return 0;
}

inline int ciszero(const Cunumber * n)
{
    return ! ( n->o || n->x[n->o] );
}

inline void csetbit1(Cunumber * n, int b)
{
    const int FS =  (sizeof(UBASE_TYPE) << 3);
    int o = b / FS;
    int r = b % FS;
    if ( o >= UNUMBER_SIZE ) return;
    n->x[o] |= (((UBASE_TYPE)1) << r);
    if ( o > n->o ) n->o = o;
}

inline unsigned cgetbit(const Cunumber * n, int b)
{
    const int FS =  (sizeof(UBASE_TYPE) << 3);
    int o = b / FS;
    int r = b % FS;
    if ( o >= UNUMBER_SIZE ) return 0;
    if ( n->x[o] & (((UBASE_TYPE)1) << r) ) return 1;
    return 0;
}

inline void cinvert(Cunumber * n)
{
    int i;
    n->o = 0;
    for ( i = 0; i < UNUMBER_SIZE; i++ )
    {
        n->x[i] = ~n->x[i];
        if ( n->x[i] ) n->o = i;
    }
}

int cequal(const Cunumber * n1, const Cunumber * n2)
{
    int i, o = n1->o;
    if ( n2->o != o ) return 0;
    for ( i = 0; i <= o; i++ )
        if ( n1->x[i] != n2->x[i] ) return 0;

    return 1;
}

int cbitorder(const Cunumber * n)
{
    int x, i;
    UBASE_TYPE r;
    i = n->o;
    x = n->o * (sizeof(UBASE_TYPE) << 3);
    r = n->x[i];
    while (r) { r >>= 1; ++x; }
    return x;
}

Cunumber cadd(const Cunumber * n1, const Cunumber * n2)
{
    int i = 0;
    unsigned bit = 0;
    Cunumber result;
    ctozero(&result);

    if ( ciszero(n1) ) return *n2;
    if ( ciszero(n2) ) return *n1;

    for ( ; i < UNUMBER_SIZE; i++ )
    {
        if ( i > n1->o && i > n2->o && !bit ) break;
        bit = add_base(&result.x[i], n1->x[i], n2->x[i], bit);
        if ( result.x[i] ) result.o = i;
    }

    return result;
}


Cunumber cdiv_QnotND(const Cunumber * N, const Cunumber * D, Cunumber * Q)
{
    Cunumber R;
    int i = ( (sizeof(UBASE_TYPE) << 3) * UNUMBER_SIZE ) - 1;

    if (Q) ctozero(Q);
    ctozero(&R);

    if ( ciszero(N) || ciszero(D) ) return R;

    if ( cless(N, D) ) return *N;

    /* initial fast shift  */
    if (1)
    {
        int bon, bod;
        R = *N;
        bon = cbitorder(N);
        bod = cbitorder(D);
        i = bon - bod;
        cshr(&R, i + 1);
    }

    for ( ; i >= 0; i-- )
    {
        cshl(&R, 1);

        R.x[0] |= (UBASE_TYPE)( cgetbit(N, i) & 1u );

        if ( cless(&R, D) ) continue;

        csub(&R, D);
        if (Q) csetbit1(Q, i);
    }

    return R;
}

void cshl(Cunumber * n, int k)
{
    const int FS =  (sizeof(UBASE_TYPE) << 3);
    int o = k / FS;
    int r = k % FS;
    int i;

    if ( k < 0 ) { cshr(n, -k); return; }

    if ( o )
    {
        for ( i = UNUMBER_SIZE - 1; i >= o; i-- )
        {
            n->x[i] = n->x[i - o];
            if ( n->x[i] && n->o < i ) n->o = i;
        }
        for ( i = o - 1; i >= 0; i-- ) n->x[i] = 0;
    }

    if ( !r ) return;
    n->x[UNUMBER_SIZE - 1] <<= r;

    n->o = 0;

    for ( i = UNUMBER_SIZE - 1; i > o; i-- )
    {
        n->x[i] |= ( n->x[i - 1] >> ( (sizeof(UBASE_TYPE) << 3) - r ) );
        n->x[i - 1] <<= r;
        if ( n->x[i] && n->o < i ) n->o = i;
    }
    if ( n->x[o] && n->o < o ) n->o = o;
}

void cshr(Cunumber * n, int k)
{
    const int FS =  (sizeof(UBASE_TYPE) << 3);
    int o = k / FS;
    int r = k % FS;
    int i;

    if ( k < 0 ) { cshl(n, -k); return; }

    if ( o )
    {
        for ( i = 0; i < UNUMBER_SIZE - o; i++ ) n->x[i] = n->x[i + o];
        for ( i = UNUMBER_SIZE - o; i < UNUMBER_SIZE; i++ ) n->x[i] = 0;
        if ( o > n->o ) n->o = 0;
        else n->o -= o;
    }

    if ( !r ) return;
    n->x[0] >>= r;

    n->o = 0;
    for ( i = 1; i < UNUMBER_SIZE - o; i++ )
    {
        n->x[i - 1] |= ( n->x[i] << ( (sizeof(UBASE_TYPE) << 3) - r ) );
        n->x[i] >>= r;
        if ( n->x[i] ) n->o = i;
    }

}

void csub(Cunumber * S, const Cunumber * n)
{
    cinvert(S);
    *S = cadd(S, n);
    cinvert(S);
}

Cunumber cmul(const Cunumber * n1, const Cunumber * n2, const Cunumber * mod)
{
    const int HS =  (sizeof(UBASE_TYPE) << 2);
    const UBASE_TYPE SM =  ( (UBASE_TYPE)(1) << HS );
    Cunumber z;
    int B, i;

    ctozero(&z);
    if ( ciszero(n1) || ciszero(n2) ) return z;

    if ( n1->o == 0 && n2->o == 0 )
    {
        UBASE_TYPE h1, h2, l1, l2;
        unsigned bit;

        if ( n1->x[0] < SM && n2->x[0] < SM )
        {
            z.x[0] = n1->x[0] * n2->x[0];
            if ( mod ) z = cdiv_QnotND(&z, mod, 0);
            return z;
        }

        h1 = ( n1->x[0] >> HS );
        h2 = ( n2->x[0] >> HS );
        l1 = ( n1->x[0] & (SM - 1) );
        l2 = ( n2->x[0] & (SM - 1) );
        z.x[0] = h1 * h2;
        cshl(&z, HS);
        bit = add_base(&z.x[0], z.x[0], h1 * l2, 0);
        if (UNUMBER_SIZE > 1) if (bit) add_base(&z.x[1], z.x[1], 1, 0);
        bit = add_base(&z.x[0], z.x[0], h2 * l1, 0);
        if (UNUMBER_SIZE > 1) if (bit) add_base(&z.x[1], z.x[1], 1, 0);
        cshl(&z, HS);
        bit = add_base(&z.x[0], z.x[0], l1 * l2, 0);
        if (UNUMBER_SIZE > 1) if (bit) add_base(&z.x[1], z.x[1], 1, 0);

        if (UNUMBER_SIZE > 1) if ( z.x[1] ) z.o = 1;
        if ( mod ) z = cdiv_QnotND(&z, mod, 0);
        return z;
    }

    B = n1->o;
    if ( n2->o > B ) B = n2->o;
    ++B; // at least 2

    B >>= 1;

    // use Karatsuba
    {
        Cunumber a, b, c, d, apb, cpd, ac, bd;
        a = *n1;
        cshr(&a, B * (HS << 1));
        c = *n2;
        cshr(&c, B * (HS << 1));

        b = *n1;
        for ( i = B; i <= b.o; i++ ) b.x[i] = 0;
        if ( b.o >= B ) b.o = B - 1;

        d = *n2;
        for ( i = B; i <= d.o; i++ ) d.x[i] = 0;
        if ( d.o >= B ) d.o = B - 1;

        apb = cadd(&a, &b);
        cpd = cadd(&c, &d);

        apb = cmul(&apb, &cpd, mod);
        ac = cmul(&a, &c, mod);
        bd = cmul(&b, &d, mod);

        if ( mod && cless(&apb, &ac) ) apb = cadd(&apb, mod);
        csub(&apb, &ac);

        if ( mod && cless(&apb, &bd) ) apb = cadd(&apb, mod);
        csub(&apb, &bd);

        z = ac;
        cshl(&z, B * (HS << 1));
        z = cadd(&z, &apb);
        cshl(&z, B * (HS << 1));
        z = cadd(&z, &bd);
    }

    if ( mod ) z = cdiv_QnotND(&z, mod, 0);
    return z;
}


void cinc(Cunumber * n)
{
    int i;
    for ( i = 0; i < UNUMBER_SIZE; i++ )
    {
        if ( i > n->o ) n->o = i;
        if ( ++n->x[i] ) return;
    }
}

int cisvalid(const Cunumber * c)
{
    int i;

    if ( c->o >= UNUMBER_SIZE ) return 0;
    if ( c->o < 0 ) return 0;

    for ( i = 1 + c->o; i < UNUMBER_SIZE; i++ )
        if ( c->x[i] ) return 0;

    return 1;
}

Cunumber cpow(Cunumber * b, Cunumber * e, const Cunumber * mod)
{
    Cunumber result;
    ctozero(&result);
    cinc(&result);

    while ( !ciszero(e) )
    {
        if ( cgetbit(e, 0) )
            result = cmul(&result, b, mod);
        cshr(e, 1);
        *b = cmul(b, b, mod);
    }

    return result;
}

Cunumber cpow2(Cunumber * b, Cunumber * e, const Cunumber * mod, const Cunumber * res)
{
    Cunumber result;
    result = *res;

    while ( !ciszero(e) )
    {
        if ( cgetbit(e, 0) )
            result = cmul(&result, b, mod);
        cshr(e, 1);
        *b = cmul(b, b, mod);
    }

    return result;
}

