#include <sstream>

#include "e3ext.h"


string Epod::str() const
{
    std::ostringstream os;
    os << x;
    return os.str();
}



