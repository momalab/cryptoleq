#ifndef CUNUMBER_H__
#define CUNUMBER_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNUMBER_SIZE
#error UNUMBER_SIZE not defined
#endif

#ifndef UBASE_TYPE
#error UBASE_TYPE not defined
#endif

typedef struct
{
    UBASE_TYPE x[UNUMBER_SIZE];
    int o;
} Cunumber;

void cinc(Cunumber *);
void cdec(Cunumber *);
void cshr(Cunumber *, int);
void cshl(Cunumber *, int);

void csub(Cunumber * n1, const Cunumber * n2);
Cunumber cadd(const Cunumber * n1, const Cunumber * n2);
Cunumber cmul(const Cunumber * n1, const Cunumber * n2, const Cunumber * mod);
Cunumber cdiv_QnotND(const Cunumber * N, const Cunumber * D, Cunumber * Q);

void ctozero(Cunumber * n);
Cunumber cnewzero();
int cless(const Cunumber * n1, const Cunumber * n2);
int ciszero(const Cunumber * n);
void csetbit1(Cunumber * n, int b);
unsigned cgetbit(const Cunumber * n, int b);
void cinvert(Cunumber * n);
int cequal(const Cunumber * n1, const Cunumber * n2);
int cbitorder(const Cunumber * n);

int cisvalid(const Cunumber * c);
Cunumber cpow(Cunumber * b, Cunumber * e, const Cunumber * mod);
Cunumber cpow2(Cunumber * b, Cunumber * e, const Cunumber * mod, const Cunumber * res);

#ifdef __cplusplus
}
#endif

#endif
