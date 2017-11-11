// input
// P    Nsize   NameI   NameR   NameE   beta    consts
// R    Nsize       Name    NameE   beta    consts

// output
// N, pfi, g, mu, beta
// N, e, d, beta

// output
// e3ext.h
// e3ext.cpp

// no static encryption
//  - bad: user needs to use external tool to calculate ecrypted constants
//      and manually place them into the code;
//      need an external tool for generating keys
//  - good: simplicity
// static encryption with a custom syntax
//  - bad: user has to learn writing generator input file for non-default case
//  - good: simple to use with the default case;
//      flexible and least error prone for the user
// static encryption with C++ syntax (double stage compilation)
//  - bad: need for 2 C++ compilers; more complex for the user (error reporting)
//  - good: all c++


int main() {}
