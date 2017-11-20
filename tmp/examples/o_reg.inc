#define main maindummy
#include "../regman.cpp"
#undef main

typedef Number SecureInt;
#define asm(x)

int mainz();

int main()
try
{
    mainz();
    cout << "\nloads: " << cpu.ld_cntr << "  strores: " << cpu.st_cntr << '\n';
    cout << cpu.REGNUM << '\t' << cpu.ld_cntr << '\t' << cpu.st_cntr << '\n';
}
catch (int x)
{
    cout << "error " << x << "\n";
    return 1;
}
catch (const char * e)
{
    cout << "error " << e << "\n";
    return 1;
}
catch (...)
{
    cout << "error ...\n";
    return 1;
}


