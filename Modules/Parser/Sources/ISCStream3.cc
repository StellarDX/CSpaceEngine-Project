/*
    CSpaceEngine SC Parser.
    Copyright (C) StellarDX Astronomy.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <https://www.gnu.org/licenses/>.
*/

#include "CSE/Parser/SCSBase.h"
#include "CSE/Parser/ISCStream.h"

#include <iostream>
#include <fstream>

using namespace std;

_CSE_BEGIN
_SC_BEGIN

#if CAT_LOG_LEVEL == 3 || CAT_LOG_LEVEL == 4
CSEDebugger CSECatDebug(cout, LOG_TIME_STAMP, LOG_THRESAD_STAMP, CAT_LOG_LEVEL - 2);
#else
CSEDebugger CSECatDebug(cout, LOG_TIME_STAMP, LOG_THRESAD_STAMP, CAT_LOG_LEVEL);
#endif

_SC_END

_SC SharedTablePointer ISCStream::Parse()const
{
    ostringstream os;
    os << input.rdbuf();
    ustring str = ustring::Decode(os.str(), Decoder);
    _SC SkipComments(str);

    _SC __SE_Lexicality Lex;
    _SC TokenArrayType Tokens;
    Lex.AddVariable(VariableList);
    Lex.Run(str, &Tokens);

    _SC __SE_General_Parser Parser;
    auto Table = Parser.Run(Tokens);
    return Table;
}

_SC SharedTablePointer ParseFile(std::filesystem::path FileName)noexcept(false)
{
    ifstream is(FileName);
    if (!is) {throw _SC ParseException("File " + FileName.string() + " is not found.\n");}
    ISCStream sc(is);
    return sc.Parse();
}

_CSE_END
