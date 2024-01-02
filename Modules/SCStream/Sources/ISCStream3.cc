#include "CSE/SCStream/SCSBase.h"
#include "CSE/SCStream/ISCStream.h"

#include <iostream>
#include <fstream>

using namespace std;

_CSE_BEGIN

CSEDebugger CSECatDebug(cout, LOG_TIME_STAMP, LOG_THRESAD_STAMP, CAT_LOG_LEVEL);

_SC SharedTablePointer ParseFile(filesystem::path FileName)noexcept(false)
{
    ifstream is(FileName);
    if (!is) {throw _SC ParseException("File " + FileName.string() + " is not found.\n");}
    _SC ISCStream sc(is);
    return sc.Parse();
}

_SC WSharedTablePointer ParseFileW(filesystem::path FileName)noexcept(false)
{
    wifstream is(FileName);
    if (!is) {throw _SC ParseException("File " + FileName.string() + " is not found.\n");}
    _SC WISCStream sc(is);
    return sc.Parse();
}

_CSE_END
