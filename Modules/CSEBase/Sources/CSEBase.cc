/************************************************************
  Implementions of CSEBase functions
***********************************************************/

#include <CSE/CSEBase/CSEBase.h>
#include <CSE/CSEBase/DateTime.h>

#include <iostream>

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
#else
#include <format>
#endif

using namespace std;

_CSE_BEGIN

// IEEE754
IEEE754_Dbl64 IEEE754_Dbl64::FromBytes(unsigned long long _By)
{
    IEEE754_Dbl64 d;
    d.Bytes = _By;
    return d;
}

// Debugger functions
CSEDebugger CSESysDebug(cout, LOG_TIME_STAMP, LOG_THRESAD_STAMP, SYS_LOG_LEVEL);

string CSEDebugger::gettime()
{
    return _TIME CSETime::CurrentTime().ToString("[{:02}:{:02}:{:02}]");
}

_CSE_END
