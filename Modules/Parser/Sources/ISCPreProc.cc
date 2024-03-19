#include "CSE/Parser/SCSBase.h"
#include "CSE/Parser/ISCStream.h"

using namespace std;

_CSE_BEGIN
_SC_BEGIN

enum __Skip_Comments_States
{
    NoComment,
    SingleLine,
    MultiLine,
    InString
};

void __Skip_Comments_WCHAR(ustring& Input)
{
    __Skip_Comments_States State = NoComment;
    auto begin = Input.begin();
    auto it = begin;
    auto end = Input.end();
    uint64 LineNum = 1;
    uint64 CoSStartPos = 0;

    while (it != end)
    {
        bool IsContinue = false;
        switch (*it) // State machine
        {
        case L'\n':
            ++LineNum;
            if (State == SingleLine) {State = NoComment;}
            break;

        case L'\"':
            if (State == NoComment)
            {
                State = InString;
                CoSStartPos = LineNum;
            }
            else if (State == InString) {State = NoComment;}
            break;

        case L'/':
            if (it < (end - 1) && State == NoComment)
            {
                auto Index = it - begin;
                if (L"//" == Input.substr(Index, 2))
                {
                    State = SingleLine;
                    Input.erase(it, it + 2);
                    end = Input.end();
                    IsContinue = true;
                }
                else if (L"/*" == Input.substr(Index, 2))
                {
                    State = MultiLine;
                    CoSStartPos = LineNum;
                    *it = L' ';
                    *(++it) = L' ';
                    //end = Input.end();
                    IsContinue = true;
                }
            }
            break;

        case L'*':
            if (it < (end - 1) && State == MultiLine)
            {
                auto Index = it - begin;
                if (L"*/" == Input.substr(Index, 2))
                {
                    State = NoComment;
                    *it = L' ';
                    *(++it) = L' ';
                    //end = Input.end();
                    IsContinue = true;
                }
            }
            break;
        }

        if (IsContinue) {continue;}

        if ((State == SingleLine) && L'\n' != *it)
        {
            Input.erase(it);
            end = Input.end();
        }
        else if ((State == MultiLine) && L'\n' != *it)
        {
            *it = L' ';
            ++it;
            //end = Input.end();
        }
        else {++it;}
    }

    switch (State)
    {
    case NoComment:
    case SingleLine:
        return;
    case MultiLine:
        throw ParseException("Unterminated /* comment", CoSStartPos);
    case InString:
        throw ParseException("Warning: Missing terminating '\"' character", CoSStartPos);
    }
}

void SkipComments(ustring& Input) {_SC __Skip_Comments_WCHAR(Input);}

_SC_END
_CSE_END
