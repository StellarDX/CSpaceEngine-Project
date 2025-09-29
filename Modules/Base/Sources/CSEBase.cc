/************************************************************
  Implementions of CSEBase functions
***********************************************************/

#include <CSE/Base/CSEBase.h>
#include <CSE/Base/DateTime.h>

#include <iostream>
#include <cstring>

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

using namespace std;

#ifdef CSE_OS_LINUX
errno_t memcpy_s(void* restrict dest, rsize_t destsz,
    const void* restrict src, rsize_t count,
    const char* FileName, uint32_t LineNumber)
{
    // Use @小虾米ing's implementation.
    // https://blog.csdn.net/u011073673/article/details/81632871
    uint8_t ErrorCode = 0;
    if (count > destsz || src == nullptr || dest == nullptr)
    {
        if (count > destsz) {ErrorCode = 1;}
        else if (src == nullptr) {ErrorCode = 2;}
        else if (dest == nullptr) {ErrorCode = 3;}
        cse::CSESysDebug("Main", cse::CSEDebugger::WARNING,
            std::format("{}.{} failed to copy: {} [{}---{}]!\n",
            FileName, LineNumber, ErrorCode, count, destsz));
        //::fflush(stdout);
        return -1;
    }
    else {::memcpy(dest, src, count);}
    return 0;
}
#endif

_CSE_BEGIN

// Debugger functions
CSEDebugger CSESysDebug(cout, LOG_TIME_STAMP, LOG_THRESAD_STAMP, SYS_LOG_LEVEL);

string CSEDebugger::gettime()
{
    return _TIME CSETime::CurrentTime().ToString("[{:02}:{:02}:{:02}]").ToStdString();
}

_CSE_END
