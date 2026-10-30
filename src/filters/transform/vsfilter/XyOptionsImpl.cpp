#include "StdAfx.h"
#include "XyOptionsImpl.h"


XyOptionsImpl::XyOptionsImpl(const Option* option_list)
{
    for (int i{ 0 }; i <= MAX_OPTION_ID; ++i)
    {
        m_option_type[i] = OPTION_TYPE_END_FLAG;

        m_xy_int_opt[i] = 0;
        m_xy_bool_opt[i] = false;
        m_xy_double_opt[i] = 0;
    }
    for (const Option* op{ option_list }; op && op->type != OPTION_TYPE_END_FLAG; ++op)
    {
        ASSERT(op->option_id < MAX_OPTION_ID);
        m_option_type[op->option_id] = op->type | op->rw_mode;
    }
}

#define XY_GET_TEMPLATE(OPTION_TYPE_x, m_xy_opt_x)           \
    if (!TestOption(field, OPTION_TYPE_x, OPTION_MODE_READ)) \
        return E_INVALIDARG;                                 \
    if (!value)                                              \
        return S_FALSE;                                      \
    return DoGetField(field, value);

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetBool(const unsigned field, bool* value)
{
    XY_GET_TEMPLATE(OPTION_TYPE_BOOL, m_xy_bool_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetInt(const unsigned field, int* value)
{
    XY_GET_TEMPLATE(OPTION_TYPE_INT, m_xy_int_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetSize(const unsigned field, SIZE* value)
{
    XY_GET_TEMPLATE(OPTION_TYPE_SIZE, m_xy_size_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetRect(const unsigned field, RECT* value)
{
    XY_GET_TEMPLATE(OPTION_TYPE_RECT, m_xy_rect_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetUlonglong(const unsigned field, ULONGLONG* value)
{
    XY_GET_TEMPLATE(OPTION_TYPE_ULONGLONG, m_xy_ulonglong_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetDouble(const unsigned field, double* value)
{
    XY_GET_TEMPLATE(OPTION_TYPE_DOUBLE, m_xy_double_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetString(const unsigned field, LPWSTR* value, int* chars)
{
    if (!TestOption(field, OPTION_TYPE_STRING, OPTION_MODE_READ))
    {
        return E_INVALIDARG;
    }
    if (!value && !chars)
    {
        return S_FALSE;
    }
    CStringW tmp;
    HRESULT hr{ DoGetField(field, &tmp) };
    if (SUCCEEDED(hr))
    {
        if (value)
        {
            *value = static_cast<LPWSTR>(LocalAlloc(LPTR, sizeof(WCHAR) * (tmp.GetLength() + 1)));
            ASSERT(*value);
            std::memcpy(*value, tmp.GetString(), (tmp.GetLength() + 1) * sizeof(WCHAR));
        }
        if (chars)
        {
            *chars = tmp.GetLength();
        }
    }
    return hr;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetBin(const unsigned field, LPVOID* value, int* size)
{
    if (!TestOption(field, OPTION_TYPE_BIN, OPTION_MODE_READ))
    {
        return E_INVALIDARG;
    }
    if (!value && !size)
    {
        return S_FALSE;
    }
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetBin2(const unsigned field, void* value, const int size)
{
    if (!TestOption(field, OPTION_TYPE_BIN, OPTION_MODE_READ))
    {
        return E_INVALIDARG;
    }
    if (!value)
    {
        return S_FALSE;
    }
    return E_NOTIMPL;
}

#define XY_SET_TEMPLATE(OPTION_TYPE_x, m_xy_opt_x)            \
    if (!TestOption(field, OPTION_TYPE_x, OPTION_MODE_WRITE)) \
        return E_INVALIDARG;                                  \
    return DoSetField(field, &value);

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetBool(const unsigned field, const bool value)
{
    XY_SET_TEMPLATE(OPTION_TYPE_BOOL, m_xy_bool_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetInt(const unsigned field, const int value)
{
    XY_SET_TEMPLATE(OPTION_TYPE_INT, m_xy_int_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetSize(const unsigned field, SIZE value)
{
    XY_SET_TEMPLATE(OPTION_TYPE_SIZE, m_xy_size_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetRect(const unsigned field, RECT value)
{
    XY_SET_TEMPLATE(OPTION_TYPE_RECT, m_xy_rect_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetUlonglong(const unsigned field, const ULONGLONG value)
{
    XY_SET_TEMPLATE(OPTION_TYPE_ULONGLONG, m_xy_ulonglong_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetDouble(const unsigned field, const double value)
{
    XY_SET_TEMPLATE(OPTION_TYPE_DOUBLE, m_xy_double_opt);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetString(const unsigned field, const LPWSTR value, const int chars)
{
    if (!TestOption(field, OPTION_TYPE_STRING, OPTION_MODE_WRITE))
    {
        return E_INVALIDARG;
    }
    CStringW tmp{ value, chars };
    return DoSetField(field, &tmp);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetBin(const unsigned field, const LPVOID value, const int size)
{
    if (!TestOption(field, OPTION_TYPE_BIN, OPTION_MODE_WRITE))
    {
        return E_INVALIDARG;
    }
    return E_NOTIMPL;
}

// methods for array not implement

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetBools(const unsigned field, const int id, bool* value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetInts(const unsigned field, const int id, int* value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetSizes(const unsigned field, const int id, SIZE* value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetRects(const unsigned field, const int id, RECT* value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetUlonglongs(const unsigned field, const int id, ULONGLONG* value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetDoubles(const unsigned field, const int id, double* value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetStrings(const unsigned field, const int id, LPWSTR* value, int* chars)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetBins(const unsigned field, const int id, LPVOID* value, int* size)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XyGetBins2(const unsigned field, const int id, void* value, const int size)

{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetBools(const unsigned field, const int id, const bool value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetInts(const unsigned field, const int id, const int value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetSizes(const unsigned field, const int id, SIZE value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetRects(const unsigned field, const int id, RECT value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetUlonglongs(const unsigned field, const int id, const ULONGLONG value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetDoubles(const unsigned field, const int id, const double value)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetStrings(const unsigned field, const int id, const LPWSTR value, const int chars)
{
    return E_NOTIMPL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyOptionsImpl::XySetBins(const unsigned field, const int id, const LPVOID value, const int size)
{
    return E_NOTIMPL;
}

[[nodiscard]] HRESULT XyOptionsImpl::DoGetField(const unsigned field, void* value)
{
    switch (m_option_type[field] & ~OPTION_MODE_RW)
    {
    case OPTION_TYPE_BOOL:
        *(bool*)value = m_xy_bool_opt[field];
        break;
    case OPTION_TYPE_INT:
        *(int*)value = m_xy_int_opt[field];
        break;
    case OPTION_TYPE_SIZE:
        *(SIZE*)value = m_xy_size_opt[field];
        break;
    case OPTION_TYPE_RECT:
        *(RECT*)value = m_xy_rect_opt[field];
        break;
    case OPTION_TYPE_ULONGLONG:
        *(ULONGLONG*)value = m_xy_ulonglong_opt[field];
        break;
    case OPTION_TYPE_DOUBLE:
        *(double*)value = m_xy_double_opt[field];
        break;
    case OPTION_TYPE_STRING:
        *(CStringW*)value = m_xy_str_opt[field];
        break;
    case OPTION_TYPE_BIN:
    case OPTION_TYPE_BIN2:
        return E_NOTIMPL;
    }
    return S_OK;
}

[[nodiscard]] HRESULT XyOptionsImpl::DoSetField(const unsigned field, void const* value)
{
    HRESULT hr{ S_FALSE };
    switch (m_option_type[field] & ~OPTION_MODE_RW)
    {
    case OPTION_TYPE_BOOL:
        if (m_xy_bool_opt[field] != *(bool*)value)
        {
            m_xy_bool_opt[field] = *(bool*)value;
            hr = OnOptionChanged(field);
        }
        break;
    case OPTION_TYPE_INT:
        if (m_xy_int_opt[field] != *(int*)value)
        {
            m_xy_int_opt[field] = *(int*)value;
            hr = OnOptionChanged(field);
        }
        break;
    case OPTION_TYPE_SIZE:
        if (m_xy_size_opt[field] != *(SIZE*)value)
        {
            m_xy_size_opt[field] = *(SIZE*)value;
            hr = OnOptionChanged(field);
        }
        break;
    case OPTION_TYPE_RECT:
        if (m_xy_rect_opt[field] != *(RECT*)value)
        {
            m_xy_rect_opt[field] = *(RECT*)value;
            hr = OnOptionChanged(field);
        }
        break;
    case OPTION_TYPE_ULONGLONG:
        if (m_xy_ulonglong_opt[field] != *(ULONGLONG*)value)
        {
            m_xy_ulonglong_opt[field] = *(ULONGLONG*)value;
            hr = OnOptionChanged(field);
        }
        break;
    case OPTION_TYPE_DOUBLE:
        if (m_xy_double_opt[field] != *(double*)value)
        {
            m_xy_double_opt[field] = *(double*)value;
            hr = OnOptionChanged(field);
        }
        break;
    case OPTION_TYPE_STRING:
        if (m_xy_str_opt[field] != *(CStringW*)value)
        {
            m_xy_str_opt[field] = *(CStringW*)value;
            hr = OnOptionChanged(field);
        }
        break;
    case OPTION_TYPE_BIN:
    case OPTION_TYPE_BIN2:
        hr = E_NOTIMPL;
        break;
    }
    return hr;
}

[[nodiscard]] HRESULT XyOptionsImpl::OnOptionChanged(const unsigned field)
{
    return S_OK;
}
