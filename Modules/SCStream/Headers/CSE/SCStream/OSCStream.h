////////////////////////////////////////////////////////
//                  SC Output Module                  //
// -------------------------------------------------- //
// Output Celestial object data to .sc file           //
////////////////////////////////////////////////////////

#pragma once

#ifndef __CSE_OSTREAM__
#define __CSE_OSTREAM__

#include "CSE/SCStream/SCSBase.h"
#include <map>

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

_CSE_BEGIN
_SC_BEGIN

class OutputException : public std::runtime_error
{
public:
    explicit OutputException(const std::string& _Message) : runtime_error(_Message) {}
};

__interface __OSC_BASE
{
    using OutputType          = SCSTable;
    using CustomMatOutputType = std::map<ustring, int64>;
    enum _Fmtflags
    {
        // constants for formatting options
        _Fmtmask = 0b11111111111111111111111111111111,
        _Fmtzero = 0b00000000000000000000000000000000
    };

    // Custom matrix output option for keys,
    // the second value specifies max value
    // count in a row.
    CustomMatOutputType CustomMatOutputList;
    virtual int _BaseInit() = 0;
};

class __SC_Smart_Output_Base : public __OSC_BASE
{
public:
    using _TyBase  = __OSC_BASE;
    using fmtflags = int;
    using outstate = int; // Preparing, Writing, ...
    using encoder  = __StelCXX_Text_Codecvt_Base&;

    __SC_Smart_Output_Base(const __SC_Smart_Output_Base&) = delete;
    __SC_Smart_Output_Base& operator=(const __SC_Smart_Output_Base&) = delete;

private:
    __StelCXX_Text_Codecvt_65001 _DefEncod = __StelCXX_Text_Codecvt_65001();

    fmtflags        _Fmtfl;             // format flags
    encoder         _Encod = _DefEncod; // file encoder
    std::streamsize _Prec;              // field precision, 0 = Auto
    std::streamsize _KeyWide;           // Key wide, 0 = dynamic

protected:
    int _BaseInit()override {return 0;}
    __SC_Smart_Output_Base(){_Init();}
    void _Init();

public:
    #define OUTPUT_WATER_MARK(version, date, fmtfl, encod)   \
    (ustring)L"// File created by CSpaceEngine\n" +          \
    L"// A Open-sourced C++ static library by StellarDX\n" + \
    L"// Compiler version : " + version + L"\n" +            \
    L"// Date :             " + date + L"\n" +               \
    L"// Format Flags :     " + fmtfl + L"\n" +              \
    L"// Encoding :         " + encod + L"\n"                \

    SCSTable       _Buf;

    // FMT Flags
    _NODISCARD fmtflags flags() const;

    // set format flags to argument
    fmtflags flags(fmtflags _Newfmtflags);

    // merge in format flags argument under mask argument
    fmtflags setf(fmtflags _Newfmtflags, fmtflags _Mask = _TyBase::_Fmtmask);

    // clear format flags under mask argument
    void unsetf(fmtflags _Mask);

private:
    constexpr std::streamsize _Dynamic_Key_Wide(uint64 Arg)const;

public:
    _NODISCARD std::streamsize width() const;

    // set key width to argument
    std::streamsize width(std::streamsize _Newwidth);

    // Setting Precision
    _NODISCARD std::streamsize precision() const;
    std::streamsize precision(std::streamsize _Newprec);

    // Setting Encoding
    _NODISCARD encoder encod() const;
    void encod(encoder _NewEnc);

    ustring __Write(const SCSTable& Table, bool DisableWM = false, uint64 Indents = 0);
    virtual void Write() = 0;
};

_SC_END

class OSCStream : public _SC __SC_Smart_Output_Base
{
public:
    using _Mybase = _SC __SC_Smart_Output_Base;

    std::ostream& output;
    OSCStream(std::ostream& os) : output(os), _Mybase(){}
    OSCStream& operator=(const OSCStream&) = delete;

    int _BaseInit()override {return 0;}
    void Write()override;
};

class WOSCStream : public _SC __SC_Smart_Output_Base
{
public:
    using _Mybase = _SC __SC_Smart_Output_Base;

    std::wostream& output;
    WOSCStream(std::wostream& os) : output(os), _Mybase(){}
    WOSCStream& operator=(const WOSCStream&) = delete;

    int _BaseInit()override {return 0;}
    void Write()override;
};

_SC __SC_Smart_Output_Base& operator<<(_SC __SC_Smart_Output_Base& os, const _SC SCSTable& table);

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
