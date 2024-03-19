#include "CSE/Parser/SCSBase.h"
#include "CSE/Parser/ISCStream.h"
#include "CSE/Parser/StelCXXRes/LRParser.h"
#include <stack>
#include <map>

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

using namespace std;

_CSE_BEGIN
_SC_BEGIN

/*
 * ALGORITHM:
 *
 * The SpaceEngine sc grammar can be described as a combination
 * of keys, values and sub-tables. It can roughly written as
 * this syntax below:
 *
 *     <Key> [Values] [{<Sub-table>}]
 *
 * And this syntax can be repeated in a single file. So let this
 * grammar as G_sc(S), We can get these productions:
 *
 *     S -> KTS | Є
 *     T -> VT | V | N
 *
 * Where: K = Key, V = Value, N = Sub-tables, Є = Empty.
 *
 * Keys in this productions above are identifiers(i), then a
 * new production is get:
 *
 *     K -> i
 *
 * Then, value in this grammar can be a number, a string or a
 * boolean, which can all be viewed as expressions(E). It also
 * can be another complex types such as arrays and matrices,
 * for parsing these types we need extra productions.
 * value productions is showed below:
 *
 *     V -> E | (A) | {M}
 *     E -> n | s | b
 *     A -> E | ACE
 *     C -> , | Є
 *     M -> VCM | VC
 *
 * Contents of sub-tables are same as main table:
 *
 *     N -> {S}
 *
 * Now, all productions of G_sc(S) are found, Then we need to
 * recognize its type such as LL(1), LR(0), SLR(1) or LR(1).
 * Obviously, if we use LL(1), LR(0) or SLR(1) method, there
 * occurs conflicts. So this grammar is an LR(1) grammar at least.
 *
 * This program is originaly generated by GNU Bison and converted
 * to C++ format by StellarDX.
 *
 */

#define _LOG_LEVEL_KEY "LogLevel"

// This grammar table has been modified to expermental support ".se" scripts.
const __LR_Parser_Base<ucs2_t>::GrammaTableType __SE_Grammar_Production_Table =
{
    {STX, L"S"},
    {'S', L"iTS"},
    {'S', L"i{B}S"},
    {'S', L"iS"},
    {'S', L""},
    {'T', L"VT"},
    {'T', L"V"},
    {'T', L"N"},
    {'B', L"EoE"},
    {'V', L"E"},
    {'V', L"(A)"},
    {'V', L"{M}"},
    {'N', L"{S}"},
    {'E', L"n"},
    {'E', L"s"},
    {'E', L"v"},
    {'E', L"b"},
    {'A', L"E"},
    {'A', L"A,E"},
    {'A', L"AE"},
    {'M', L"V,M"},
    {'M', L"VM"},
    {'M', L"V,"},
    {'M', L"V"},
};

#if (SC_PARSER_LR == LALR1)
#include "Grammar/GSC_LALR1.tbl"
#elif (SC_PARSER_LR == LR1)
#include "Grammar/GSC_LR1.tbl"
#elif (SC_PARSER_LR == IELR1)
#include "Grammar/GSC_IELR1.tbl"
#else
#error Invalid option.
#endif

// Implementation of LR parser for SC

ustring __SE_General_Parser::TokenToString(TokenArrayType Tokens)
{
    ustring Res;
    for (auto i : Tokens)
    {
        if (i.Type == Punctuator) {Res.push_back(i.Value[0]);}
        else {Res.push_back(ucs2_t(i.Type));}
    }
    return Res;
}

SharedPointer<SCSTable> __SE_General_Parser::MakeTable(stack<SCSTable::SCKeyValue>& SubTableTempStack)
{
    SCSTable SubTable;
    while (!SubTableTempStack.empty())
    {
        SubTable._M_Elems.push_back(SubTableTempStack.top());
        SubTableTempStack.pop();
    }
    return make_shared<decltype(SubTable)>(SubTable);
}

void __SE_General_Parser::MakeSubMatrix(ValueType& ExpressionBuffer, ValueType SubMatrix)
{
    if (!ExpressionBuffer.SubMatrices)
    {
        ValueType::SubMatrixType Table;
        Table.insert({0, SubMatrix});
        ExpressionBuffer.SubMatrices = make_shared<decltype(Table)>(Table);
    }
    else {ExpressionBuffer.SubMatrices->insert({0, SubMatrix});}
}

void __SE_General_Parser::MoveSubMateix(ValueType& ExpressionBuffer)
{
    if (!ExpressionBuffer.SubMatrices) {return;}
    std::vector<size_t> Keys;
    std::vector<ValueType> Values;
    auto it = ExpressionBuffer.SubMatrices->begin();
    auto end = ExpressionBuffer.SubMatrices->end();
    while (it != end)
    {
        Keys.push_back(it->first);
        Values.push_back(it->second);
        ++it;
    }
    ExpressionBuffer.SubMatrices->clear();
    for (int i = 0; i < Keys.size(); ++i)
    {
        ExpressionBuffer.SubMatrices->insert({Keys[i] + 1, Values[i]});
    }
}

void __SE_General_Parser::ThrowError(size_t CurrentState, ivec2 Pos, std::string Msg)
{
    auto StateMsg = _Mybase::States[CurrentState].ErrorMessage;
    auto FullMsg = (StateMsg.empty() ? "" : (": " + StateMsg));
    if (Msg.empty())
    {
        Msg = vformat("Syntax error at ({}, {}){}",
            make_format_args(Pos.x, Pos.y, FullMsg));
    }
    else {Msg = vformat("Syntax error at ({}, {}): {}",
        make_format_args(Pos.x, Pos.y, Msg));}
    throw std::runtime_error(Msg);
}

SharedPointer<SCSTable> __SE_General_Parser::Run(TokenArrayType Tokens) noexcept(0)
{
    // Initialize storage and buffer
    std::stack<SCSTable::SCKeyValue> KTStack, SubTableTempStack;
    std::stack<ValueType> ValueStack;
    ValueType ExpressionBuffer = ValueType();

    // Initialize parsing stacks
    ustring SymbolString = TokenToString(Tokens);
    std::stack<size_t> StateStack;
    std::stack<ucs2_t> SymbolStack;

    SymbolString.push_back(ETX);
    auto CurrentPosition = SymbolString.begin();
    StateStack.push(0);
    SymbolStack.push(STX);

    TokenType PreviousWord;
    auto CurrentWordIter = Tokens.begin();

    // Log Level
    #if CAT_LOG_LEVEL == 1 || CAT_LOG_LEVEL == 2
    bool DetectedLogLevel = 0;
    #endif

    // Start Parsing.
    bool Accepted = 0;
    while(!Accepted)
    {
        size_t CurrentState = StateStack.top();
        ucs2_t CurrentSymbol;
        if (CurrentPosition != SymbolString.end()) {CurrentSymbol = *CurrentPosition;}
        else {CurrentSymbol = ETX;}
        typename _Mybase::Actions CurrentAction;
        size_t NewState = GetNewState(CurrentState, CurrentSymbol, &CurrentAction);

        TokenType CurrentWord;
        if (CurrentSymbol != ETX) {CurrentWord = *CurrentWordIter;}

        switch (CurrentAction)
        {
        case _Mybase::Accept:
            Accepted = 1;
            CSECatDebug("SCParser", CSECatDebug.INFO, "Accepted.");
            break;

        case _Mybase::Shift:
        case _Mybase::Goto:
        {
            StateStack.push(NewState);
            SymbolStack.push(CurrentSymbol);
            ++CurrentPosition;

            if (CurrentSymbol != ETX)
            {
                PreviousWord = CurrentWord;
                ++CurrentWordIter;
            }

            switch (CurrentSymbol)
            {
            case 'i': // If this symbol is identifier, push a new key-value to stack.
                #if CAT_LOG_LEVEL == 1 || CAT_LOG_LEVEL == 2
                if (PreviousWord.Value == _LOG_LEVEL_KEY)
                {
                    CSECatDebug("SCParser", CSECatDebug.INFO, "Detected custom log-level setting.");
                    DetectedLogLevel = 1;
                }
                #endif
                KTStack.push({.Key = PreviousWord.Value});
                break;

            case 'o': // If this symbol is operator, push it to.
                ExpressionBuffer.Value.push_back(PreviousWord.Value);
                break;

            default:
                break;
            }

            CSECatDebug("SCParser", CSECatDebug.INFO,
                format("Shifting, Current state: {}, Current symbol: {}, Next state is {}.",
                CurrentState, (char)CurrentSymbol, NewState));
        }
        break;

        case _Mybase::Reduce:
        {
            auto Production = _Mybase::Grammar[NewState];
            PopStack(StateStack, Production.second.size());
            PopStack(SymbolStack, Production.second.size());
            SymbolStack.push(Production.first);

            switch (NewState)
            {
                // Operations when reducing
            case 1:
            case 2:
            case 3:
                SubTableTempStack.push(KTStack.top());
                KTStack.pop();
                break;

            case 5:
            case 6:
                KTStack.top().Value.insert(KTStack.top().Value.begin(), ValueStack.top());
                ValueStack.pop();
                break;

            case 8:
                ExpressionBuffer.Type = decltype(ExpressionBuffer.Type)
                    (0b110000 | ExpressionBuffer.Boolean);
                KTStack.top().Value.insert(KTStack.top().Value.begin(), ExpressionBuffer);
                ExpressionBuffer = ValueType();
                break;

            case 9:
                ValueStack.push(ExpressionBuffer);
                ExpressionBuffer = ValueType();
                break;

            case 10:
                ExpressionBuffer.Type = decltype(ExpressionBuffer.Type)
                    (ExpressionBuffer.Type | ExpressionBuffer.Array);
                ValueStack.push(ExpressionBuffer);
                ExpressionBuffer = ValueType();
                break;

            case 11:
                ExpressionBuffer.Type = ExpressionBuffer.Matrix;
                ValueStack.push(ExpressionBuffer);
                ExpressionBuffer = ValueType();
                break;

            case 12:
                KTStack.top().SubTable = MakeTable(SubTableTempStack);
                break;

            case 13:
                #if CAT_LOG_LEVEL == 1 || CAT_LOG_LEVEL == 2
                if (DetectedLogLevel)
                {
                    int LogLvl = stoi(PreviousWord.Value);
                    CSECatDebug("SCParser", CSECatDebug.INFO,
                        vformat("Log level has been set to {}.", make_format_args(LogLvl)));
                    CSECatDebug.__LogLevel = std::min(CAT_LOG_LEVEL, LogLvl);
                    DetectedLogLevel = 0;
                }
                #endif
                if ((ExpressionBuffer.Type & 0b1111) != ExpressionBuffer.Others &&
                    (int(ExpressionBuffer.Type) & ExpressionBuffer.Mask) != ExpressionBuffer.Number)
                {
                    ThrowError(CurrentState, CurrentWord.Posiston,
                        format("Deduced conflicting types ('{}' vs '{}') for array element type",
                        int(ExpressionBuffer.Type), int(ExpressionBuffer.Number)));
                }
                ExpressionBuffer.Type = ExpressionBuffer.Number;
                ExpressionBuffer.Value.push_back(PreviousWord.Value);
                ExpressionBuffer.Base = PreviousWord.NumBase;
                break;

            case 14:
                if ((ExpressionBuffer.Type & 0b1111) != ExpressionBuffer.Others &&
                    (int(ExpressionBuffer.Type) & ExpressionBuffer.Mask) != ExpressionBuffer.String)
                {
                    ThrowError(CurrentState, CurrentWord.Posiston,
                        format("Deduced conflicting types ('{}' vs '{}') for array element type",
                        int(ExpressionBuffer.Type), int(ExpressionBuffer.String)));
                }
                ExpressionBuffer.Type = ExpressionBuffer.String;
                ExpressionBuffer.Value.push_back(PreviousWord.Value);
                break;

            case 15:
                ExpressionBuffer.Type = decltype(ExpressionBuffer.Type)(0b10000);
                ExpressionBuffer.Value.push_back(PreviousWord.Value);
                ExpressionBuffer.Pos = PreviousWord.Posiston;
                break;

            case 16:
                if ((ExpressionBuffer.Type & 0b1111) != ExpressionBuffer.Others &&
                    (int(ExpressionBuffer.Type) & ExpressionBuffer.Mask) != ExpressionBuffer.Boolean)
                {
                    ThrowError(CurrentState, CurrentWord.Posiston,
                        format("Deduced conflicting types ('{}' vs '{}') for array element type",
                        int(ExpressionBuffer.Type), int(ExpressionBuffer.Boolean)));
                }
                ExpressionBuffer.Type = ExpressionBuffer.Boolean;
                ExpressionBuffer.Value.push_back(PreviousWord.Value);
                break;

            case 20:
            case 21:
            case 22:
            case 23:
                MoveSubMateix(ExpressionBuffer);
                if (ValueStack.top().Type & ValueStack.top().Array)
                {
                    MakeSubMatrix(ExpressionBuffer, ValueStack.top());
                }
                else
                {
                    ExpressionBuffer.Value.insert(ExpressionBuffer.Value.begin(), ValueStack.top().Value.front());
                }
                ValueStack.pop();
                break;

            default:
                break;
            }

            CurrentState = StateStack.top();
            NewState = GetNewState(CurrentState, SymbolStack.top(), &CurrentAction);
            if (CurrentAction == _Mybase::Error)
            {
                ThrowError(CurrentState, CurrentWord.Posiston, "");
            }
            StateStack.push(NewState);

            CSECatDebug("SCParser", CSECatDebug.INFO,
                format("Reducing: {}->{}, Go to state {}.",
                char(Production.first), ustring(Production.second).ToStdString(), NewState));
        }
        break;

        default:
            ThrowError(CurrentState, CurrentWord.Posiston, "");
        }
    }

    return MakeTable(SubTableTempStack);
}

_SC_END
_CSE_END
