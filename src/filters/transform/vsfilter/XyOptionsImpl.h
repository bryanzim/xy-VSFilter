#pragma once

#include "XyOptions.h"

class XyOptionsImpl : public IXyOptions
{
public:
    static const int MAX_OPTION_ID{ 127 };

    typedef int OptionType;
    static const int OPTION_TYPE_END_FLAG{ 0 };
    static const int OPTION_TYPE_BOOL{ 0x1 };
    static const int OPTION_TYPE_INT{ 0x2 };
    static const int OPTION_TYPE_SIZE{ 0x4 };
    static const int OPTION_TYPE_RECT{ 0x8 };
    static const int OPTION_TYPE_ULONGLONG{ 0x10 };
    static const int OPTION_TYPE_DOUBLE{ 0x20 };
    static const int OPTION_TYPE_STRING{ 0x40 };
    static const int OPTION_TYPE_BIN{ 0x80 };
    static const int OPTION_TYPE_BIN2{ 0x800 };

    typedef int OptionMode;
    static const int OPTION_MODE_READ{ 0x100 };
    static const int OPTION_MODE_WRITE{ 0x200 };
    static const int OPTION_MODE_RW{ 0x300 };

    struct Option
    {
        OptionType type;
        OptionMode rw_mode; // read/write
        unsigned option_id;
    };

public:
    explicit XyOptionsImpl(const Option* option_list);
    virtual ~XyOptionsImpl() {}

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetBool(unsigned field, bool* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetInt(unsigned field, int* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetSize(unsigned field, SIZE* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetRect(unsigned field, RECT* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetUlonglong(unsigned field, ULONGLONG* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetDouble(unsigned field, double* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetString(unsigned field, LPWSTR* value, int* chars);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetBin(unsigned field, LPVOID* value, int* size);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetBin2(unsigned field, void* value, int size);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetBool(unsigned field, bool value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetInt(unsigned field, int value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetSize(unsigned field, SIZE value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetRect(unsigned field, RECT value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetUlonglong(unsigned field, ULONGLONG value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetDouble(unsigned field, double value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetString(unsigned field, LPWSTR value, int chars);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetBin(unsigned field, LPVOID value, int size);

    // methods for array
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetBools(unsigned field, int id, bool* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetInts(unsigned field, int id, int* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetSizes(unsigned field, int id, SIZE* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetRects(unsigned field, int id, RECT* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetUlonglongs(unsigned field, int id, ULONGLONG* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetDoubles(unsigned field, int id, double* value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetStrings(unsigned field, int id, LPWSTR* value, int* chars);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetBins(unsigned field, int id, LPVOID* value, int* size);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetBins2(unsigned field, int id, void* value, int size);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetBools(unsigned field, int id, bool value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetInts(unsigned field, int id, int value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetSizes(unsigned field, int id, SIZE value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetRects(unsigned field, int id, RECT value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetUlonglongs(unsigned field, int id, ULONGLONG value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetDoubles(unsigned field, int id, double value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetStrings(unsigned field, int id, LPWSTR value, int chars);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetBins(unsigned field, int id, LPVOID value, int size);

    [[nodiscard]] inline bool TestOption(const unsigned field)

    {
        return (field < MAX_OPTION_ID && m_option_type[field] != OPTION_TYPE_END_FLAG);
    }
    [[nodiscard]] inline bool TestOption(const unsigned field, OptionType type, const OptionMode rw_mode = OPTION_MODE_RW)

    {
        type |= rw_mode;
        return (field < MAX_OPTION_ID && (m_option_type[field] & type) == type);
    }

protected:
    virtual HRESULT DoGetField(unsigned field, void* value);
    [[nodiscard]] virtual HRESULT DoSetField(unsigned field, void const* value);
    [[nodiscard]] virtual HRESULT OnOptionChanged(unsigned field);

protected:
    int m_option_type[MAX_OPTION_ID + 1];

    int m_xy_int_opt[MAX_OPTION_ID + 1];
    bool m_xy_bool_opt[MAX_OPTION_ID + 1];
    double m_xy_double_opt[MAX_OPTION_ID + 1];
    ULONGLONG m_xy_ulonglong_opt[MAX_OPTION_ID + 1];
    CSize m_xy_size_opt[MAX_OPTION_ID + 1];
    CRect m_xy_rect_opt[MAX_OPTION_ID + 1];
    CStringW m_xy_str_opt[MAX_OPTION_ID + 1];
};

#define DECLARE_IXYOPTIONS                                                                                                                                         \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetBool(unsigned field, bool* value) { return XyOptionsImpl::XyGetBool(field, value); }                                        \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetInt(unsigned field, int* value) { return XyOptionsImpl::XyGetInt(field, value); }                                           \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetSize(unsigned field, SIZE* value) { return XyOptionsImpl::XyGetSize(field, value); }                                        \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetRect(unsigned field, RECT* value) { return XyOptionsImpl::XyGetRect(field, value); }                                        \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetUlonglong(unsigned field, ULONGLONG* value) { return XyOptionsImpl::XyGetUlonglong(field, value); }                         \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetDouble(unsigned field, double* value) { return XyOptionsImpl::XyGetDouble(field, value); }                                  \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetString(unsigned field, LPWSTR* value, int* chars) { return XyOptionsImpl::XyGetString(field, value, chars); }               \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetBin(unsigned field, LPVOID* value, int* size) { return XyOptionsImpl::XyGetBin(field, value, size); }                       \
                                                                                                                                                                   \
    DECLSPEC_NOTHROW STDMETHODIMP XySetBool(unsigned field, bool value) { return XyOptionsImpl::XySetBool(field, value); }                                         \
    DECLSPEC_NOTHROW STDMETHODIMP XySetInt(unsigned field, int value) { return XyOptionsImpl::XySetInt(field, value); }                                            \
    DECLSPEC_NOTHROW STDMETHODIMP XySetSize(unsigned field, SIZE value) { return XyOptionsImpl::XySetSize(field, value); }                                         \
    DECLSPEC_NOTHROW STDMETHODIMP XySetRect(unsigned field, RECT value) { return XyOptionsImpl::XySetRect(field, value); }                                         \
    DECLSPEC_NOTHROW STDMETHODIMP XySetUlonglong(unsigned field, ULONGLONG value) { return XyOptionsImpl::XySetUlonglong(field, value); }                          \
    DECLSPEC_NOTHROW STDMETHODIMP XySetDouble(unsigned field, double value) { return XyOptionsImpl::XySetDouble(field, value); }                                   \
    DECLSPEC_NOTHROW STDMETHODIMP XySetString(unsigned field, LPWSTR value, int chars) { return XyOptionsImpl::XySetString(field, value, chars); }                 \
    DECLSPEC_NOTHROW STDMETHODIMP XySetBin(unsigned field, LPVOID value, int size) { return XyOptionsImpl::XySetBin(field, value, size); }                         \
                                                                                                                                                                   \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetBools(unsigned field, int id, bool* value) { return XyOptionsImpl::XyGetBools(field, id, value); }                          \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetInts(unsigned field, int id, int* value) { return XyOptionsImpl::XyGetInts(field, id, value); }                             \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetSizes(unsigned field, int id, SIZE* value) { return XyOptionsImpl::XyGetSizes(field, id, value); }                          \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetRects(unsigned field, int id, RECT* value) { return XyOptionsImpl::XyGetRects(field, id, value); }                          \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetUlonglongs(unsigned field, int id, ULONGLONG* value) { return XyOptionsImpl::XyGetUlonglongs(field, id, value); }           \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetDoubles(unsigned field, int id, double* value) { return XyOptionsImpl::XyGetDoubles(field, id, value); }                    \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetStrings(unsigned field, int id, LPWSTR* value, int* chars) { return XyOptionsImpl::XyGetStrings(field, id, value, chars); } \
    DECLSPEC_NOTHROW STDMETHODIMP XyGetBins(unsigned field, int id, LPVOID* value, int* size) { return XyOptionsImpl::XyGetBins(field, id, value, size); }         \
                                                                                                                                                                   \
    DECLSPEC_NOTHROW STDMETHODIMP XySetBools(unsigned field, int id, bool value) { return XyOptionsImpl::XySetBools(field, id, value); }                           \
    DECLSPEC_NOTHROW STDMETHODIMP XySetInts(unsigned field, int id, int value) { return XyOptionsImpl::XySetInts(field, id, value); }                              \
    DECLSPEC_NOTHROW STDMETHODIMP XySetSizes(unsigned field, int id, SIZE value) { return XyOptionsImpl::XySetSizes(field, id, value); }                           \
    DECLSPEC_NOTHROW STDMETHODIMP XySetRects(unsigned field, int id, RECT value) { return XyOptionsImpl::XySetRects(field, id, value); }                           \
    DECLSPEC_NOTHROW STDMETHODIMP XySetUlonglongs(unsigned field, int id, ULONGLONG value) { return XyOptionsImpl::XySetUlonglongs(field, id, value); }            \
    DECLSPEC_NOTHROW STDMETHODIMP XySetDoubles(unsigned field, int id, double value) { return XyOptionsImpl::XySetDoubles(field, id, value); }                     \
    DECLSPEC_NOTHROW STDMETHODIMP XySetStrings(unsigned field, int id, LPWSTR value, int chars) { return XyOptionsImpl::XySetStrings(field, id, value, chars); }   \
    DECLSPEC_NOTHROW STDMETHODIMP XySetBins(unsigned field, int id, LPVOID value, int size) { return XyOptionsImpl::XySetBins(field, id, value, size); }
