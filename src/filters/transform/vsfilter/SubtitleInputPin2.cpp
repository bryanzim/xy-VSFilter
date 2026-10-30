#include "StdAfx.h"
#include "xy_sub_filter.h"
#include "SubtitleInputPin2.h"
#include "../../../subtitles/hdmv_subtitle_provider.h"
#include "moreuuids.h"
//
// CHdmvInputPinHepler
//
class CHdmvInputPinHepler2 : public CSubtitleInputPinHelperImpl
{
public:
    CHdmvInputPinHepler2(HdmvSubtitleProvider* pHdmvSubtitle, const CMediaType& mt);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Receive(IMediaSample* pSample);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP EndOfStream(void);

private:
    HdmvSubtitleProvider* m_pHdmvSubtitle;
};

CHdmvInputPinHepler2::CHdmvInputPinHepler2(HdmvSubtitleProvider* pHdmvSubtitle, const CMediaType& mt)
    : CSubtitleInputPinHelperImpl(pHdmvSubtitle), m_pHdmvSubtitle(pHdmvSubtitle)
{
    (void)mt;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CHdmvInputPinHepler2::NewSegment(const REFERENCE_TIME tStart, const REFERENCE_TIME tStop, const double dRate)
{
    return m_pHdmvSubtitle->NewSegment(tStart, tStop, dRate);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CHdmvInputPinHepler2::Receive(IMediaSample* pSample)
{
    return m_pHdmvSubtitle->ParseSample(pSample);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CHdmvInputPinHepler2::EndOfStream()
{
    return m_pHdmvSubtitle->EndOfStream();
}

//
// SubtitleInputPin2
//
SubtitleInputPin2::SubtitleInputPin2(XySubFilter* pFilter, CCritSec* pLock, CCritSec* pSubLock, HRESULT* phr)
    : CSubtitleInputPin(pFilter, pLock, pSubLock, phr), xy_sub_filter(pFilter)
{
    ASSERT(pFilter);
}

void SubtitleInputPin2::AddSubStream(ISubStream* pSubStream)
{
    xy_sub_filter->AddSubStream(pSubStream);
}

void SubtitleInputPin2::RemoveSubStream(ISubStream* pSubStream)
{
    xy_sub_filter->RemoveSubStream(pSubStream);
}

void SubtitleInputPin2::InvalidateSubtitle(const REFERENCE_TIME rtStart, ISubStream* pSubStream)
{
    xy_sub_filter->InvalidateSubtitle(rtStart, (DWORD_PTR)(ISubStream*)pSubStream);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(CSubtitleInputPinHelper*) SubtitleInputPin2::CreateHelper(const CMediaType& mt, IPin* pReceivePin)
{
    if (IsHdmvSub(&mt))
    {
        XY_LOG_INFO("Create CHdmvInputPinHepler2");
        SUBTITLEINFO* psi{ (SUBTITLEINFO*)mt.pbFormat };
        CString name;
        LCID lcid{ 0 };

        if (psi != nullptr)
        {
            name = ISO6392ToLanguage(psi->IsoLang);
            lcid = ISO6392ToLcid(psi->IsoLang);

            if (std::wcslen(psi->TrackName) > 0)
            {
                name += (!name.IsEmpty() ? _T(", ") : _T("")) + CString(psi->TrackName);
            }
            if (name.IsEmpty())
            {
                name = _T("Unknown");
            }
        }

        HdmvSubtitleProvider* hdmv_sub{ DEBUG_NEW HdmvSubtitleProvider(m_pSubLock,
                                                                       (mt.subtype == MEDIASUBTYPE_DVB_SUBTITLES) ? ST_DVB : ST_HDMV, name, lcid) };
        return DEBUG_NEW CHdmvInputPinHepler2{ hdmv_sub, m_mt };
    }
    else
    {
        return __super::CreateHelper(mt, pReceivePin);
    }
    return nullptr;
}

[[nodiscard]] HRESULT SubtitleInputPin2::CheckMediaType(const CMediaType* pmt)
{
    HRESULT hr{ xy_sub_filter->CheckInputType(pmt) };
    if (FAILED(hr))
    {
        return hr;
    }
    return __super::CheckMediaType(pmt);
}
