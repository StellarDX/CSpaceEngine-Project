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

#pragma once

#ifndef _SCS_BASE
#define _SCS_BASE

#include <array>
#include <vector>
#include <string>
#include <memory>
#include <regex>
#include <iomanip>
#include <map>

#include <CSE/Base/CSEBase.h>
#include <CSE/Base/DateTime.h>
#include <CSE/Base/GLTypes.h>
#include <CSE/Base/Algorithms.h>

#if defined _MSC_VER
#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
#if defined _STL_DISABLE_CLANG_WARNINGS
_STL_DISABLE_CLANG_WARNINGS
#endif
#pragma push_macro("new")
#undef new
#endif

#define _SC_BEGIN namespace scenario {
#define _SC_END }
#define _SC scenario::

_CSE_BEGIN
_SC_BEGIN

extern CSEDebugger CSECatDebug;

class ParseException : public std::runtime_error // output errors
{
public:
    using _Mybase = std::runtime_error;
    ParseException(const std::string& Error) : _Mybase{ Error } {}
    ParseException(const std::string& err, size_t line_number) : _Mybase{ err + " at line " + std::to_string(line_number) } {}
};

enum TerminalTypes
{
    Identifier = 'i', // Identifier
    Variable   = 'v', // Variable
    Operator   = 'o', // Opeartor
    Number     = 'n', // Number
    String     = 's', // String
    Boolean    = 'b', // Boolean
    Punctuator = 'p',
};

struct TokenType;
using TokenArrayType  = std::vector<TokenType>;
template<typename _Ty>
using SharedPointer   = std::shared_ptr<_Ty>;

struct TokenType
{
    TerminalTypes Type;
    ustring       Value;
    ivec2         Posiston;
    int           NumBase = 10;
};

inline bool stob(const ustring& __str)
{
    return __str == L"true" ? 1 : 0;
}

inline ustring __Matrix_To_String(struct ValueType Val);

struct ValueType
{
    using SubMatrixType = std::map<size_t, ValueType>;

    enum TypeID
    {
        Number  = 0b0001,
        String  = 0b0010,
        Boolean = 0b0011,
        Array   = 0b0100,
        Matrix  = 0b1100,
        Others  = 0b0000,

        Mask    = 0b0011
    }Type;

    ustringlist Value;
    SharedPointer<SubMatrixType> SubMatrices = nullptr;
    int Base = 10;
    ivec2 Pos; // Store variable or expression pos in source file.

    template<typename _Tp>
    static TypeID ToTypeID()
    {
        if (std::convertible_to<_Tp, float64>) {return Number;}
        else if (std::convertible_to<_Tp, ustring>) {return String;}
        else if (std::is_same_v<_Tp, bool>) {return Boolean;}
        else {return Others;}
    }

    void GetQualified(void* Dst, size_t Idx = 0)
    {
        switch (Type & Mask)
        {
        case Number:
            *((float64*)Dst) = std::stod(Value[Idx]);
            break;

        case String:
            *((ustring*)Dst) = Value[Idx];
            ((ustring*)Dst)->erase(((ustring*)Dst)->begin());
            ((ustring*)Dst)->pop_back();
            break;

        case Boolean:
            *((bool*)Dst) = stob(Value[Idx]);
            break;

        default:
            throw ParseException("Value is not convertable to single value.");
            break;
        }
    }

    TypeID GetTypeID() {return TypeID(Type & Mask);}

    void GetAsBasedInteger(void* Dst, size_t Idx = 0)
    {
        *((int64*)Dst) = std::stoll(Value.front(), nullptr, Base);
    }

    template<typename _Tp, std::size_t _Nm>
    void GetAsArray(gvec<_Tp, _Nm>* Dst)
    {
        if (ToTypeID<_Tp>() != GetTypeID())
        {
            throw ParseException("Type is not match.");
        }

        size_t Capacity = max(Value.size(), _Nm);
        for (int i = 0; i < Capacity; ++i)
        {
            GetQualified(&(*Dst)[i], i);
        }
    }

    template<typename _Tp>
    void GetAsDynamicArray(std::vector<_Tp>* Dst)
    {
        if (ToTypeID<_Tp>() != GetTypeID())
        {
            throw ParseException("Type is not match.");
        }

        if (Dst->size() < Value.size()) {Dst->resize(Value.size());}
        for (int i = 0; i < Value.size(); ++i)
        {
            GetQualified(&(*Dst)[i], i);
        }
    }

    void GetAsDateTime(CSEDateTime* Dst)noexcept(false)
    {
        if (GetTypeID() != String) {throw ParseException("Invalid value");}
        ustring Str;
        GetQualified(&Str);
        std::smatch Toks;
        std::string Data = Str.ToStdString();
        if (std::regex_match(Data, Toks, _TIME SEDateTimeStringRegex))
        {
            *Dst = CSEDateTime
            (
                {std::stoi(Toks[1]), std::stoi(Toks[2]), std::stoi(Toks[3])},
                {std::stoi(Toks[4]), std::stoi(Toks[5]), std::stod(Toks[6])}
            );
        }
        else if (std::regex_match(Data, Toks, _TIME SEDateStringRegex))
        {
            *Dst = CSEDateTime({std::stoi(Toks[1]), std::stoi(Toks[2]), std::stoi(Toks[3])});
        }
        else
        {
            *Dst = CSEDateTime({std::stoi(Str), 1, 1});
        }
    }

    ustring ToString()const
    {
        if ((Type & (~Mask)) == Array)
        {
            ustring Str;
            Str.push_back(L'(');
            for (int i = 0; i < Value.size(); ++i)
            {
                Str += Value[i];
                if (i < Value.size() - 1) {Str += L", ";}
            }
            Str.push_back(L')');
            return Str;
        }
        else if (Type & 0b100000)
        {
            ustring Str;
            Str.push_back(L'{');
            for (int i = 0; i < Value.size(); ++i)
            {
                Str += Value[i];
                if (i < Value.size() - 1) {Str += L" ";}
            }
            Str.push_back(L'}');
            return Str;
        }
        else if ((Type & (~Mask)) == Matrix)
        {
            return __Matrix_To_String(*this);
        }
        else {return Value.front();}
    }
};

inline ustring __Matrix_To_String(ValueType Val)
{
    ustring Str;
    Str += L"{ ";
    for (size_t i = 0; i <= Val.Value.size(); ++i)
    {
        if (Val.SubMatrices)
        {
            size_t j = i;
            while (Val.SubMatrices->contains(j))
            {
                Str += Val.SubMatrices->at(j).ToString();
                Str += L" ";
                ++j;
            }
        }
        if (i < Val.Value.size())
        {
            Str += Val.Value[i];
            Str += L" ";
        }
    }
    Str.push_back(L'}');
    return Str;
}

using ValueList = std::vector<ValueType>;

struct SCSTable
{
    using SubTablePointer = SharedPointer<SCSTable>;

    struct SCKeyValue
    {
        ustring            Key;
        ValueList          Value;
        SubTablePointer    SubTable = nullptr;
    };

    using CatalogType     = std::vector<SCKeyValue>;
    using ValueType       = SCKeyValue;
    using ReferType       = SCKeyValue&;
    using IterType        = CatalogType::iterator;
    using ConstIter       = CatalogType::const_iterator;

    CatalogType _M_Elems;

    CatalogType& Get() {return _M_Elems;}
};

using SharedTablePointer = SharedPointer<SCSTable>;

_SC_END

// Get Table Helpers

namespace __scstream_table_helpers {

inline auto __Str_Contain(ustring Left, ustring Right)
{
    return Left.find(Right) != ustring::npos;
}

inline ustringlist __Str_Split(ustring Str, ucs2_t Symb = L'/')
{
    ustringlist Toks;
    auto First = Str.find_first_not_of(Symb, 0);
    auto Current = Str.find_first_of(Symb, 0);
    for(; First != ustring::npos || Current != ustring::npos;)
    {
        Toks.emplace_back(Str.substr(First, Current - First));
        First = Str.find_first_not_of(Symb, Current);
        Current = Str.find_first_of(Symb, First);
    }
    return Toks;
}

inline auto __Find_Table_From_List(const _SC SharedTablePointer& Src, ustring Key)
{
    return find_if(Src->Get().begin(), Src->Get().end(), [Key](_SC SCSTable::ValueType Tbl)
    {
        return Tbl.Key == Key;
    });
}

inline auto __Find_Table_With_Unit(const _SC SharedTablePointer& Src, ustring Key)
{
    return find_if(Src->Get().begin(), Src->Get().end(), [Key](_SC SCSTable::ValueType Tbl)
    {
        return Tbl.Key.substr(0, Key.size()) == Key;
    });
}

inline auto __Find_Multi_Tables_From_List(const _SC SharedTablePointer& Src, ustring Key)
{
    std::vector<decltype(Src->Get().begin())> Result;
    auto it = Src->Get().begin();
    for (; it != Src->Get().end();)
    {
        it = find_if(it, Src->Get().end(), [Key](_SC SCSTable::ValueType Tbl)
        {
            return Tbl.Key == Key;
        });
        if (it != Src->Get().end())
        {
            Result.push_back(it);
            ++it;
        }
    }
    return Result;
}

inline auto __Find_Table_With_KeyWord(const _SC SharedTablePointer& Src, ustring Key)
{
    return find_if(Src->Get().begin(), Src->Get().end(), [Key](_SC SCSTable::ValueType Tbl)
    {
        return Tbl.Key.find(Key) != ustring::npos;
    });
}

template<typename VTy> requires
(
    std::is_same_v<VTy, float64> ||
    std::is_same_v<VTy, ustring> ||
    std::is_same_v<VTy, bool>
)
inline void __Get_Value_From_Table(VTy* Dst, const _SC SharedTablePointer& Src, ustring Key, VTy Alt)
{
    auto it = __Find_Table_From_List(Src, Key);
    if (it != Src->Get().end()) {it->Value.front().GetQualified(Dst);}
    else {*Dst = Alt;}
}

template<typename VTy> requires
(
    std::is_same_v<VTy, uint64> ||
    std::is_same_v<VTy, int64>
)
inline void __Get_Value_From_Table(VTy* Dst, const _SC SharedTablePointer& Src, ustring Key, VTy Alt)
{
    auto it = __Find_Table_From_List(Src, Key);
    if (it != Src->Get().end()) {it->Value.front().GetAsBasedInteger(Dst);}
    else {*Dst = Alt;}
}

template<typename _Tp, size_t _Nm>
inline void __Get_Value_From_Table(gvec<_Tp, _Nm>* Dst, const _SC SharedTablePointer& Src, ustring Key, gvec<_Tp, _Nm> Alt)
{
    auto it = __Find_Table_From_List(Src, Key);
    if (it != Src->Get().end()) {it->Value.front().GetAsArray(Dst);}
    else {*Dst = Alt;}
}

template<typename VTy> requires std::is_same_v<VTy, _TIME CSEDate>
inline void __Get_Value_From_Table(VTy* Dst, const _SC SharedTablePointer& Src, ustring Key, VTy Alt)
{
    auto it = __Find_Table_From_List(Src, Key);
    if (it != Src->Get().end())
    {
        CSEDateTime DateTime;
        it->Value.front().GetAsDateTime(&DateTime);
        *Dst = DateTime.date();
    }
    else {*Dst = Alt;}
}

inline void __Get_Value_With_Unit(float64* Dst, const _SC SharedTablePointer& Src, ustring Key, float64 Alt, float64 DefMultiply, std::map<ustring, float64> MultiplyOfUnits)
{
    if (Src->Get().front().Value.front().GetTypeID() != _SC ValueType::Number) {*Dst = Alt;}
    auto it = __Find_Table_With_Unit(Src, Key);
    if (it != Src->Get().end())
    {
        it->Value.front().GetQualified(Dst);
        if (it->Key != Key)
        {
            ustring Unit = ustring(it->Key.substr(Key.size(), it->Key.size() - Key.size()));
            if (MultiplyOfUnits.contains(Unit))
            {
                *Dst *= MultiplyOfUnits.at(Unit);
                return;
            }
        }
        *Dst *= DefMultiply;
    }
    else {*Dst = Alt;}
}

template<size_t _Nm>
inline void __Get_Value_With_Unit(gvec<float64, _Nm>* Dst, const _SC SharedTablePointer& Src, ustring Key, gvec<float64, _Nm> Alt, float64 DefMultiply, std::map<ustring, float64> MultiplyOfUnits)
{
    if (Src->Get().front().Value.front().GetTypeID() != _SC ValueType::Number) {*Dst = Alt;}
    auto it = __Find_Table_With_Unit(Src, Key);
    if (it != Src->Get().end())
    {
        it->Value.front().GetAsArray(Dst);
        if (it->Key != Key)
        {
            ustring Unit = ustring(it->Key.substr(Key.size(), it->Key.size() - Key.size()));
            if (MultiplyOfUnits.contains(Unit))
            {
                *Dst = *Dst * MultiplyOfUnits.at(Unit);
                return;
            }
        }
        *Dst = *Dst * DefMultiply;
    }
    else {*Dst = Alt;}
}

// Convert to Table Helpers
inline ustring __Str_List_To_String(ustringlist usl, ucs2_t pun = L'/')
{
    ustring ustr;
    for (int i = 0; i < usl.size(); ++i)
    {
        ustr += usl[i];
        if (i < usl.size() - 1) {ustr += pun;}
    }
    return ustr;
}

inline bool IsNoData(float64 Val)
{
    return IS_NO_DATA_DBL(Val);
}

inline bool IsNoData(uint64 Val)
{
    return IS_NO_DATA_INT(Val);
}

inline bool IsNoData(ustring Val)
{
    return IS_NO_DATA_STR(Val);
}

inline bool IsNoData(bool Val)
{
    return !Val;
}

template<typename Tp, size_t Nm>
inline bool IsNoData(gvec<Tp, Nm> Val)
{
    for (size_t i = 1; i < Val.size(); ++i)
    {
        if (IsNoData(Val[i])) {return 1;}
    }
    return 0;
}

inline void __Add_Empty_Tag(_SC SCSTable* Table)
{
    Table->Get().push_back(_SC SCSTable::SCKeyValue());
}

template<typename genType> requires
(
    std::is_same_v<genType, float64> ||
    std::is_same_v<genType, uint64> ||
    std::is_same_v<genType, int64> ||
    std::is_same_v<genType, ustring> ||
    std::is_same_v<genType, bool>
)
inline void __Add_Key_Value(_SC SCSTable* Table, ustring Key, genType Value, bool Fixed, std::streamsize Preci)
{
    if (!IsNoData(Value))
    {
        _SC SCSTable::SCKeyValue KeyValue;
        KeyValue.Key = Key;
        std::wostringstream ValueStr;
        if (Fixed) {ValueStr << std::fixed;}
        if (std::is_same_v<genType, bool>) {ValueStr << std::boolalpha;}
        if (std::is_same_v<genType, float64>) {ValueStr.precision(Preci);}
        if (std::is_same_v<genType, ustring>) {ValueStr << L'\"' << Value << L'\"';}
        else {ValueStr << Value;}
        KeyValue.Value.push_back({.Type = _SC ValueType::ToTypeID<decltype(Value)>(), .Value = {ustring(ValueStr.str())}});
        Table->Get().push_back(KeyValue);
    }
}

template<typename Tp, size_t Nm>
inline void __Add_Key_Value(_SC SCSTable* Table, ustring Key, gvec<Tp, Nm> Value, bool Fixed, std::streamsize Preci)
{
    if (!IsNoData(Value))
    {
        _SC SCSTable::SCKeyValue KeyValue;
        KeyValue.Key = Key;
        ustringlist VList;
        for (int i = 0; i < Value.size(); ++i)
        {
            std::wostringstream ValueStr;
            if (Fixed) {ValueStr << std::fixed;}
            ValueStr.precision(Preci);
            ValueStr << Value[i];
            VList.push_back(ustring(ValueStr.str()));
        }
        KeyValue.Value.push_back({.Type = _SC ValueType::TypeID(_SC ValueType::ToTypeID<decltype(Value[0])>() | _SC ValueType::Array), .Value = VList});
        Table->Get().push_back(KeyValue);
    }
}

inline void __Add_Key_Value(_SC SCSTable* Table, ustring Key, _TIME CSEDate Value, bool Fixed, std::streamsize Preci)
{
    if (Value.IsValid())
    {
        __Add_Key_Value(Table, Key,
        ustring(Value.ToString("{}-{:02}-{:02}")), Fixed, Preci);
    }
}

}

_CSE_END

#if defined _MSC_VER
#pragma pop_macro("new")
#if defined _STL_RESTORE_CLANG_WARNINGS
_STL_RESTORE_CLANG_WARNINGS
#endif
#pragma warning(pop)
#pragma pack(pop)
#endif

#endif
