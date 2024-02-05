#include "CSE/CSEBase/Algorithms.h"
#include "CSE/SCStream/OSCStream.h"
#include "CSE/CSEBase/DateTime.h"
#include <iomanip>

_CSE_BEGIN
_SC_BEGIN

using namespace std;

// OSCStream 2.1 implementation

void __SC_Smart_Output_Base::_Init()
{
    _Fmtfl = _BaseInit();
    _Encod = _DefEncod;
    _Buf.Get().clear();
    _Prec = 10;
    _KeyWide = 0;
}

_NODISCARD __SC_Smart_Output_Base::fmtflags __SC_Smart_Output_Base::flags() const
{
    return _Fmtfl;
}

__SC_Smart_Output_Base::fmtflags __SC_Smart_Output_Base::flags(__SC_Smart_Output_Base::fmtflags _Newfmtflags)
{
    const fmtflags _Oldfmtflags = _Fmtfl;
    _Fmtfl = _Newfmtflags & _TyBase::_Fmtmask;
    return _Oldfmtflags;
}

__SC_Smart_Output_Base::fmtflags __SC_Smart_Output_Base::setf(fmtflags _Newfmtflags, fmtflags _Mask)
{
    const fmtflags _Oldfmtflags = _Fmtfl;
    _Fmtfl = _Oldfmtflags | (_Newfmtflags & _Mask & _TyBase::_Fmtmask);
    return _Oldfmtflags;
}

void __SC_Smart_Output_Base::unsetf(fmtflags _Mask)
{
    _Fmtfl &= ~_Mask;
}

constexpr streamsize __SC_Smart_Output_Base::_Dynamic_Key_Wide(uint64 Arg)const
{
    switch (Arg)
    {
    case 0:
        return 5;
        break;
    case 1:
    case 2:
        return 16;
        break;
    case 3:
        return 12;
        break;
    default:
        return 10;
        break;
    }
}

_NODISCARD streamsize __SC_Smart_Output_Base::width() const
{
    return _KeyWide;
}

streamsize __SC_Smart_Output_Base::width(streamsize _Newwidth)
{
    // set key width to argument
    const streamsize _Oldwidth = _KeyWide;
    _KeyWide = _Newwidth;
    return _Oldwidth;
}

_NODISCARD streamsize __SC_Smart_Output_Base::precision() const
{
    return _Prec;
}

streamsize __SC_Smart_Output_Base::precision(streamsize _Newprec)
{
    const streamsize _Oldprec = _Prec;
    _Prec = _Newprec;
    return _Oldprec;
}

_NODISCARD __SC_Smart_Output_Base::encoder __SC_Smart_Output_Base::encod() const
{
    return _Encod;
}

void __SC_Smart_Output_Base::encod(encoder _NewEnc)
{
    _Encod = _NewEnc;
}

ustring __SC_Smart_Output_Base::__Write(const SCSTable& Table, bool DisableWM, uint64 Indents)
{
    wostringstream _Os;

    if (!DisableWM && _Fmtfl < 0)
    {
        wostringstream FmtFlHex;
        FmtFlHex << hex << L"0x" << _Fmtfl;
        auto DateTime = CSEDateTime::CurrentDateTime();

        _Os << OUTPUT_WATER_MARK
            (__StelCXX_Text_Codecvt_65001().ToUnicode(COMPILER_VERSION),
            __StelCXX_Text_Codecvt_65001().ToUnicode(DateTime.ToString()),
            FmtFlHex.str(),
            to_wstring(encod().CodePage())) << L'\n';
    }

    bool HasSubTable = false;
    for (size_t i = 0; i < Table._M_Elems.size(); i++)
    {
        std::streamsize _KWide = _KeyWide;
        if (_KeyWide == 0) { _KWide = _Dynamic_Key_Wide(Indents); }
        else { _KWide = _KeyWide; }
        if (HasSubTable || (Table._M_Elems[i].SubTable && i != 0))
        {
            HasSubTable = false;
            _Os << L'\n';
        }
        if (uint64(_KWide) <= Table._M_Elems[i].Key.size()) { _KWide = Table._M_Elems[i].Key.size() + 1; }
        for (uint64 j = 0; j < Indents; ++j) { _Os << L"\t"; }

        // Custom matrix output
        if (CustomMatOutputList.contains(Table._M_Elems[i].Key))
        {
            _Os << Table._M_Elems[i].Key << L'\n';
            for (uint64 j = 0; j < Indents; ++j) { _Os << L"\t"; }
            _Os << L"{\n";
            size_t len = CustomMatOutputList.at(Table._M_Elems[i].Key);
            size_t count = Table._M_Elems[i].Value.front().Value.size();
            for (int j = 0; j < count; ++j)
            {
                for (uint64 k = 0; k < Indents + 1; ++k) { _Os << L"\t"; }
                _Os << Table._M_Elems[i].Value.front().Value[j];
                _Os << L", ";
                if (j % len == len - 1 || j == count - 1) {_Os << L"\n";}
            }
            for (uint64 j = 0; j < Indents; ++j) { _Os << L"\t"; }
            _Os << L"}\n";
        }

        else
        {
            auto DefFlags = _Os.flags();
            _Os << setw(_KWide) << setfill(L' ') << left << Table._M_Elems[i].Key;
            _Os.flags(DefFlags);
            for (int j = 0; j < Table._M_Elems[i].Value.size(); ++j)
            {
                _Os << Table._M_Elems[i].Value[j].ToString();
                if (j < Table._M_Elems[i].Value.size() - 1) {_Os << L' ';}
            }
            _Os << L'\n';
        }

        if (Table._M_Elems[i].SubTable)
        {
            HasSubTable = true;
            for (uint64 j = 0; j < Indents; ++j) { _Os << "\t"; }
            _Os << "{\n";
            _Os << __Write(*(Table._M_Elems[i].SubTable), true, Indents + 1);
            for (uint64 j = 0; j < Indents; ++j) { _Os << "\t"; }
            _Os << "}\n";
        }
    }

    return _Os.str();
}

_SC_END

void OSCStream::Write()
{
    output << _Mybase::encod().FromUnicode(__Write(_Mybase::_Buf));
    unsetf(_WaterMark);
    _Mybase::_Buf.Get().clear();
}

void WOSCStream::Write()
{
    output << __Write(_Mybase::_Buf);
    unsetf(_WaterMark);
    _Mybase::_Buf.Get().clear();
}

_SC __SC_Smart_Output_Base& operator<<(_SC __SC_Smart_Output_Base& os, const _SC SCSTable& table)
{
    if (!table._M_Elems.empty())
    {
        os._Buf.Get().insert(os._Buf.Get().end(), table._M_Elems.begin(), table._M_Elems.end());
    }
    return os;
}

_CSE_END
