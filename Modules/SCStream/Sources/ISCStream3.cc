#include "CSE/SCStream/SCSBase.h"
#include "CSE/SCStream/ISCStream.h"

#include <iostream>
#include <fstream>

using namespace std;

_CSE_BEGIN
_SC_BEGIN

CSEDebugger CSECatDebug(cout, LOG_TIME_STAMP, LOG_THRESAD_STAMP, CAT_LOG_LEVEL);

SharedTablePointer ISCStream::Parse()const
{
    ostringstream OStr;
    OStr << input.rdbuf();
    string Str = OStr.str();

    SCS::SkipComments(Str);

    auto Tokens = SCS::Tokenizer(Str);

    auto FinalTable = Parser(Tokens);
    return FinalTable;
}

WSharedTablePointer WISCStream::Parse()const
{
    wostringstream OStr;
    OStr << input.rdbuf();
    wstring Str = OStr.str();

    SCS::SkipCommentsW(Str);

    auto Tokens = SCS::TokenizerW(Str);

    auto FinalTable = ParserW(Tokens);
    return FinalTable;
}

_SC_END

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
