/************************************************************
  Implementions of CSEBase functions
***********************************************************/

#include <CSE/Base/CSEBase.h>
#include <CSE/Base/DateTime.h>

#include <iostream>

using namespace std;

_CSE_BEGIN

// Debugger functions
CSEDebugger CSESysDebug(cout, LOG_TIME_STAMP, LOG_THRESAD_STAMP, SYS_LOG_LEVEL);

string CSEDebugger::gettime()
{
    return _TIME CSETime::CurrentTime().ToString("[{:02}:{:02}:{:02}]").ToStdString();
}

_CSE_END
