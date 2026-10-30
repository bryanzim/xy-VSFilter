/*
 * $Id: RenderedHdmvSubtitle.cpp 3650 2011-08-11 12:16:46Z XhmikosR $
 *
 * (C) 2006-2010 see AUTHORS
 *
 * This file is part of mplayerc.
 *
 * Mplayerc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mplayerc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "stdafx.h"
#include "HdmvSub.h"
#include "DVBSub.h"
#include "RenderedHdmvSubtitle.h"

CRenderedHdmvSubtitle::CRenderedHdmvSubtitle(CCritSec* pLock, SUBTITLE_TYPE nType, const CString& name, LCID lcid)
    : CSubPicProviderImpl(pLock), m_name(name), m_lcid(lcid)
{
    switch (nType)
    {
    case ST_DVB:
        m_pSub = DEBUG_NEW CDVBSub{};
        if (name.IsEmpty() || (name == _T("Unknown")))
        {
            m_name = "DVB Embedded Subtitle";
        }
        break;
    case ST_HDMV:
        m_pSub = DEBUG_NEW CHdmvSub{};
        if (name.IsEmpty() || (name == _T("Unknown")))
        {
            m_name = "HDMV Embedded Subtitle";
        }
        break;
    default:
        ASSERT(FALSE);
        m_pSub = nullptr;
    }
    m_rtStart = 0;
}

CRenderedHdmvSubtitle::~CRenderedHdmvSubtitle()

{
    delete m_pSub;
}


[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CRenderedHdmvSubtitle::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
    CheckPointer(ppv, E_POINTER);
    *ppv = nullptr;

    return QI(IPersist)
        QI(ISubStream)
            QI(ISubPicProvider) __super::NonDelegatingQueryInterface(riid, ppv);
}

// ISubPicProvider

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(POSITION) CRenderedHdmvSubtitle::GetStartPosition(const REFERENCE_TIME rt, const double fps)
{
    CAutoLock cAutoLock(&m_csCritSec);
    return m_pSub->GetStartPosition(rt - m_rtStart, fps);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(POSITION) CRenderedHdmvSubtitle::GetNext(POSITION pos)
{
    CAutoLock cAutoLock(&m_csCritSec);
    return m_pSub->GetNext(pos);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) CRenderedHdmvSubtitle::GetStart(POSITION pos, const double fps)
{
    CAutoLock cAutoLock(&m_csCritSec);
    return m_pSub->GetStart(pos) + m_rtStart;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) CRenderedHdmvSubtitle::GetStop(POSITION pos, const double fps)
{
    CAutoLock cAutoLock(&m_csCritSec);
    return m_pSub->GetStop(pos) + m_rtStart;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) CRenderedHdmvSubtitle::IsAnimated(POSITION pos)
{
    return false;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CRenderedHdmvSubtitle::Render(SubPicDesc& spd, const REFERENCE_TIME rt, const double fps, RECT& bbox)
{
    CAutoLock cAutoLock(&m_csCritSec);
    m_pSub->Render(spd, rt - m_rtStart, bbox);

    return S_OK;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CRenderedHdmvSubtitle::GetTextureSize(POSITION pos, SIZE& MaxTextureSize, SIZE& VideoSize, POINT& VideoTopLeft)
{
    CAutoLock cAutoLock(&m_csCritSec);
    HRESULT hr{ m_pSub->GetTextureSize(pos, MaxTextureSize, VideoSize, VideoTopLeft) };
    return hr;
}

// IPersist

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CRenderedHdmvSubtitle::GetClassID(CLSID* pClassID)
{
    return pClassID ? *pClassID = __uuidof(this), S_OK : E_POINTER;
}

// ISubStream

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(int) CRenderedHdmvSubtitle::GetStreamCount()
{
    return (1);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CRenderedHdmvSubtitle::GetStreamInfo(const int iStream, WCHAR** ppName, LCID* pLCID)
{
    if (iStream != 0)
    {
        return E_INVALIDARG;
    }

    if (ppName)
    {
        *ppName = (WCHAR*)CoTaskMemAlloc((m_name.GetLength() + 1) * sizeof(WCHAR));
        if (!(*ppName))
        {
            return E_OUTOFMEMORY;
        }

        wcscpy_s(*ppName, m_name.GetLength() + 1, CStringW(m_name));
    }

    if (pLCID)
    {
        *pLCID = m_lcid;
    }

    return S_OK;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(int) CRenderedHdmvSubtitle::GetStream()
{
    return (0);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CRenderedHdmvSubtitle::SetStream(const int iStream)
{
    return iStream == 0 ? S_OK : E_FAIL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CRenderedHdmvSubtitle::Reload()
{
    return S_OK;
}

[[nodiscard]] HRESULT CRenderedHdmvSubtitle::ParseSample(IMediaSample* pSample)
{
    CAutoLock cAutoLock(&m_csCritSec);
    HRESULT hr;

    hr = m_pSub->ParseSample(pSample);
    return hr;
}

[[nodiscard]] HRESULT CRenderedHdmvSubtitle::NewSegment(const REFERENCE_TIME tStart, const REFERENCE_TIME tStop, const double dRate)
{
    CAutoLock cAutoLock(&m_csCritSec);

    m_pSub->Reset();
    m_rtStart = tStart;
    return S_OK;
}

void CRenderedHdmvSubtitle::EndOfStream()
{
    CAutoLock cAutoLock(&m_csCritSec);
    m_pSub->EndOfStream();
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) CRenderedHdmvSubtitle::IsColorTypeSupported(const int type)

{
    return type == MSP_AYUV_PLANAR ||
           type == MSP_AYUV ||
           type == MSP_XY_AUYV ||
           type == MSP_RGBA;
}

[[nodiscard]] HRESULT CRenderedHdmvSubtitle::SetYuvType(const CBaseSub::ColorType colorType, const CBaseSub::YuvRangeType yuvRangeType)
{
    CAutoLock cAutoLock(&m_csCritSec);
    return m_pSub->SetYuvType(colorType, yuvRangeType);
}
