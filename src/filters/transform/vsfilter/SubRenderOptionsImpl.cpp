#include "StdAfx.h"
#include "SubRenderOptionsImpl.h"

int SubRenderOptionsImpl::GetIFiled( LPCSTR field, OptionType type )
{
    if (!_optionMap)
    {
        return -1;
    }
    const OptionMap * map{_optionMap};
    while( map->str_name && _stricmp(field, map->str_name) )
    {
        ++map;
    }
    return (map->str_name && map->type==type) ? map->int_name : -1;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::GetBool(const LPCSTR field, bool *value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_BOOL)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XyGetBool(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::GetInt(const LPCSTR field, int *value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_INT)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XyGetInt(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::GetSize(const LPCSTR field, SIZE *value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_SIZE)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XyGetSize(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::GetRect(const LPCSTR field, RECT *value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_RECT)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XyGetRect(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::GetUlonglong(const LPCSTR field, ULONGLONG *value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_ULONGLONG)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XyGetUlonglong(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::GetDouble(const LPCSTR field, double *value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_DOUBLE)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XyGetDouble(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::GetString(const LPCSTR field, LPWSTR *value, int *chars)
{
    int ifield{GetIFiled(field, OPTION_TYPE_STRING)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XyGetString(ifield, value, chars);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::GetBin(const LPCSTR field, LPVOID *value, int *size)
{
    int ifield{GetIFiled(field, OPTION_TYPE_BIN)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XyGetBin(ifield, value, size);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::SetBool(const LPCSTR field, const bool value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_BOOL)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XySetBool(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::SetInt(const LPCSTR field, const int value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_INT)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XySetInt(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::SetSize(const LPCSTR field, SIZE value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_SIZE)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XySetSize(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::SetRect(const LPCSTR field, RECT value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_RECT)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XySetRect(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::SetUlonglong(const LPCSTR field, const ULONGLONG value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_ULONGLONG)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XySetUlonglong(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::SetDouble(const LPCSTR field, const double value)
{
    int ifield{GetIFiled(field, OPTION_TYPE_DOUBLE)};
    return _impl->XySetDouble(ifield, value);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::SetString(const LPCSTR field, const LPWSTR value, const int chars)
{
    int ifield{GetIFiled(field, OPTION_TYPE_STRING)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XySetString(ifield, value, chars);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SubRenderOptionsImpl::SetBin(const LPCSTR field, const LPVOID value, const int size)
{
    int ifield{GetIFiled(field, OPTION_TYPE_BIN)};
    CheckPointer(_impl, E_POINTER);
    return _impl->XySetBin(ifield, value, size);
}
