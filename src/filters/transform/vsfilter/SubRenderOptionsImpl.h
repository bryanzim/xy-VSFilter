#pragma once

#include "SubRenderIntf.h"

interface IXyOptions;

class SubRenderOptionsImpl : public ISubRenderOptions
{
public:
    enum OptionType
    {
        OPTION_TYPE_BOOL,
        OPTION_TYPE_INT,
        OPTION_TYPE_SIZE,
        OPTION_TYPE_RECT,
        OPTION_TYPE_ULONGLONG,
        OPTION_TYPE_DOUBLE,
        OPTION_TYPE_STRING,
        OPTION_TYPE_BIN
    };

    struct OptionMap
    {
        LPCSTR str_name;
        OptionType type;
        unsigned int_name;
    };

public:
    SubRenderOptionsImpl(const OptionMap *optionMap, IXyOptions *impl) : _impl(impl), _optionMap(optionMap){}
    virtual ~SubRenderOptionsImpl(void) {}

    // ISubRenderOptions
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetBool     (LPCSTR field, bool      *value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetInt      (LPCSTR field, int       *value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetSize     (LPCSTR field, SIZE      *value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetRect     (LPCSTR field, RECT      *value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetUlonglong(LPCSTR field, ULONGLONG *value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetDouble   (LPCSTR field, double    *value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetString   (LPCSTR field, LPWSTR    *value, int *chars);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetBin      (LPCSTR field, LPVOID    *value, int *size );

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetBool     (LPCSTR field, bool      value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetInt      (LPCSTR field, int       value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetSize     (LPCSTR field, SIZE      value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetRect     (LPCSTR field, RECT      value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetUlonglong(LPCSTR field, ULONGLONG value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetDouble   (LPCSTR field, double    value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetString   (LPCSTR field, LPWSTR    value, int chars);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetBin      (LPCSTR field, LPVOID    value, int size );
private:
    int GetIFiled(LPCSTR field, OptionType type);

    IXyOptions *_impl;
    const OptionMap *_optionMap;
};

#define DECLARE_ISUBRENDEROPTIONS                                                                                                         \
    DECLSPEC_NOTHROW STDMETHODIMP GetBool     (LPCSTR field, bool      *value) { return SubRenderOptionsImpl::GetBool(field, value); }                      \
    DECLSPEC_NOTHROW STDMETHODIMP GetInt      (LPCSTR field, int       *value) { return SubRenderOptionsImpl::GetInt(field, value); }                       \
    DECLSPEC_NOTHROW STDMETHODIMP GetSize     (LPCSTR field, SIZE      *value) { return SubRenderOptionsImpl::GetSize(field, value); }                      \
    DECLSPEC_NOTHROW STDMETHODIMP GetRect     (LPCSTR field, RECT      *value) { return SubRenderOptionsImpl::GetRect(field, value); }                      \
    DECLSPEC_NOTHROW STDMETHODIMP GetUlonglong(LPCSTR field, ULONGLONG *value) { return SubRenderOptionsImpl::GetUlonglong(field, value); }                 \
    DECLSPEC_NOTHROW STDMETHODIMP GetDouble   (LPCSTR field, double    *value) { return SubRenderOptionsImpl::GetDouble(field, value); }                    \
    DECLSPEC_NOTHROW STDMETHODIMP GetString   (LPCSTR field, LPWSTR    *value, int *chars) { return SubRenderOptionsImpl::GetString(field, value, chars); } \
    DECLSPEC_NOTHROW STDMETHODIMP GetBin      (LPCSTR field, LPVOID    *value, int *size ) { return SubRenderOptionsImpl::GetBin(field, value, size); }     \
    \
    DECLSPEC_NOTHROW STDMETHODIMP SetBool     (LPCSTR field, bool      value) { return SubRenderOptionsImpl::SetBool(field, value); }                       \
    DECLSPEC_NOTHROW STDMETHODIMP SetInt      (LPCSTR field, int       value) { return SubRenderOptionsImpl::SetInt(field, value); }                        \
    DECLSPEC_NOTHROW STDMETHODIMP SetSize     (LPCSTR field, SIZE      value) { return SubRenderOptionsImpl::SetSize(field, value); }                       \
    DECLSPEC_NOTHROW STDMETHODIMP SetRect     (LPCSTR field, RECT      value) { return SubRenderOptionsImpl::SetRect(field, value); }                       \
    DECLSPEC_NOTHROW STDMETHODIMP SetUlonglong(LPCSTR field, ULONGLONG value) { return SubRenderOptionsImpl::SetUlonglong(field, value); }                  \
    DECLSPEC_NOTHROW STDMETHODIMP SetDouble   (LPCSTR field, double    value) { return SubRenderOptionsImpl::SetDouble(field, value); }                     \
    DECLSPEC_NOTHROW STDMETHODIMP SetString   (LPCSTR field, LPWSTR    value, int chars) { return SubRenderOptionsImpl::SetString(field, value, chars); }   \
    DECLSPEC_NOTHROW STDMETHODIMP SetBin      (LPCSTR field, LPVOID    value, int size ) { return SubRenderOptionsImpl::SetBin(field, value, size); }
