/*
    Skeleton implementation of SLR(1)/LR(1) Parser in C++

    Copyright (C) 2023, 2024
    StellarDX Astronomy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C++ SLR(1)/LR(1) parser skeleton written by StellarDX. */

#pragma once

#ifndef __LR_PARSER__
#define __LR_PARSER__

#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include <stdexcept>

#ifndef _MSC_VER
#define __interface struct
#endif

#define STX 0x02
#define ETX 0x03
#define ESC 0x1B

// Base class of SC Parser
template<typename _CharT>
__interface __LR_Parser_Base
{
public:
    using ProductionType        = std::pair<_CharT, std::basic_string<_CharT>>;
    using GrammaTableType       = std::vector<ProductionType>;

    struct ItemCollectionType
    {
        using ActionShiftType   = std::map<_CharT, size_t>;
        using ActionReduceType  = std::map<_CharT, size_t>;
        using GotoType          = std::map<_CharT, size_t>;

        bool              IsAccept = 0;
        ActionShiftType   Shifts;
        ActionReduceType  Reduces;
        GotoType          Gotos;
        std::string       ErrorMessage;
    };

    using StatesType            = std::vector<ItemCollectionType>;

    const GrammaTableType       Grammar;
    const StatesType            States;

    static const size_t StateNPos = 0xFFFFFFFFFFFFFFFF;

    enum Actions {Accept, Shift, Reduce, Goto, Error};
};

template<typename _CharT>
class __StelCXX_LR_Parser : public __LR_Parser_Base<_CharT>
{
public:
    using _Mybase = __LR_Parser_Base<_CharT>;

    __StelCXX_LR_Parser
        (typename _Mybase::GrammaTableType Productions,
         typename _Mybase::StatesType States)
        : _Mybase{.Grammar = Productions, .States = States} {}

    __StelCXX_LR_Parser& operator=(__StelCXX_LR_Parser&) = delete;

    size_t GetNewState(size_t CurrentState, _CharT CurrentSymbol, _Mybase::Actions* Act)
    {
        auto State = _Mybase::States[CurrentState];

        if (State.IsAccept)
        {
            *Act = _Mybase::Accept;
            return _Mybase::StateNPos;
        }

        auto it1 = State.Shifts.find(CurrentSymbol);
        auto it2 = State.Reduces.find(CurrentSymbol);
        auto it3 = State.Gotos.find(CurrentSymbol);

        if (it1 != State.Shifts.end())
        {
            *Act = _Mybase::Shift;
            return it1->second;
        }
        else if (it2 != State.Reduces.end())
        {
            *Act = _Mybase::Reduce;
            return it2->second;
        }
        else if (it3 != State.Gotos.end())
        {
            *Act = _Mybase::Goto;
            return it3->second;
        }
        else
        {
            auto it4 = State.Reduces.find(0x00);
            if (it4 != State.Reduces.end())
            {
                *Act = _Mybase::Reduce;
                return it4->second;
            }
            *Act = _Mybase::Error;
            return _Mybase::StateNPos;
        }
    }

    void ThrowError(size_t CurrentState, _CharT CurrentSymbol)
    {
        auto StateMsg = _Mybase::States[CurrentState].ErrorMessage;
        std::string Msg = std::string("Syntax error") +
            (StateMsg.empty() ? "" : (": " + StateMsg));
        throw std::runtime_error(Msg);
    }

    template<typename _Tp>
    void PopStack(std::stack<_Tp>& Stack, size_t N)
    {
        for (size_t i = 0; i < N; ++i)
        {
            Stack.pop();
        }
    };

    int Run(std::basic_string<_CharT> SymbolString) noexcept(0)
    {
        std::stack<size_t> StateStack;
        std::stack<_CharT> SymbolStack;

        SymbolString.push_back(_CharT(ETX));
        auto CurrentPosition = SymbolString.begin();
        StateStack.push(0);
        SymbolStack.push(_CharT(STX));

        bool Accepted = 0;
        while(!Accepted)
        {
            size_t CurrentState = StateStack.top();
            _CharT CurrentSymbol;
            if (CurrentPosition != SymbolString.end()) {CurrentSymbol = *CurrentPosition;}
            typename _Mybase::Actions CurrentAction;
            size_t NewState = GetNewState(CurrentState, CurrentSymbol, &CurrentAction);

            switch (CurrentAction)
            {
            case _Mybase::Accept:
                Accepted = 1;
                printf("Accepted.\n");
                break;

            case _Mybase::Shift:
            case _Mybase::Goto:
                StateStack.push(NewState);
                SymbolStack.push(CurrentSymbol);
                ++CurrentPosition;
                printf("Shifting, Current state: %llu, Current symbol: %c, Next state is %llu.\n",
                       CurrentState, CurrentSymbol, NewState);
                break;

            case _Mybase::Reduce:
            {
                auto Production = _Mybase::Grammar[NewState];
                PopStack(StateStack, Production.second.size());
                PopStack(SymbolStack, Production.second.size());
                SymbolStack.push(Production.first);

                switch (NewState)
                {
                    // Operations when reducing...
                }

                CurrentState = StateStack.top();
                NewState = GetNewState(CurrentState, SymbolStack.top(), &CurrentAction);
                if (CurrentAction == _Mybase::Error)
                {
                    ThrowError(CurrentState, CurrentSymbol);
                }
                StateStack.push(NewState);

                printf("Reducing: %c->%s, Go to state %llu.\n",
                       Production.first, Production.second.c_str(), NewState);
            }
                break;

            default:
                ThrowError(CurrentState, CurrentSymbol);
            }
        }

        return 0;
    }
};

using LRParser = __StelCXX_LR_Parser<char>;
using WLRParser = __StelCXX_LR_Parser<wchar_t>;

#endif
