#pragma once

#include "Rasterizer.h"
#include "../SubPic/SubPicProviderImpl.h"
#include "HdmvSub.h"
#include "BaseSub.h"

class HdmvSubtitleProviderImpl
{
public:
    explicit HdmvSubtitleProviderImpl (CBaseSub* pSub);
    ~HdmvSubtitleProviderImpl(void);

    [[nodiscard]] STDMETHODIMP Connect     (IXyOptions *consumer);
    [[nodiscard]] STDMETHODIMP RequestFrame(IXySubRenderFrame**subRenderFrame, REFERENCE_TIME now);
    [[nodiscard]] STDMETHODIMP Invalidate  (REFERENCE_TIME rtInvalidate = -1);
private:
    HRESULT ResetAllocator();
    [[nodiscard]] HRESULT Render(REFERENCE_TIME now, POSITION pos);
private :
    CBaseSub*                   m_pSub;

    IXyOptions                 *m_consumer;
    CSize                       m_cur_output_size;
    bool m_use_dst_alpha;
    CComPtr<ISubPicExAllocator> m_allocator;
    CComPtr<ISubPicEx>          m_pSubPic;
    CComPtr<IXySubRenderFrame>  m_xy_sub_render_frame;
};

//This is for embedded subtitles
class __declspec(uuid("F316F98F-BDB5-42AA-A253-8D6C8AA20754"))
HdmvSubtitleProvider : public CUnknown, public IXySubRenderProvider, public ISubStream
{
public:
    HdmvSubtitleProvider(CCritSec* pLock, SUBTITLE_TYPE nType, const CString& name, LCID lcid);
    ~HdmvSubtitleProvider(void);

    [[nodiscard]] DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // IXySubRenderProvider
    [[nodiscard]] STDMETHODIMP Connect     (IXyOptions *consumer);
    [[nodiscard]] STDMETHODIMP RequestFrame(IXySubRenderFrame**subRenderFrame, REFERENCE_TIME now);
    [[nodiscard]] STDMETHODIMP Invalidate  (REFERENCE_TIME rtInvalidate = -1);

    // IPersist
    [[nodiscard]] STDMETHODIMP GetClassID(CLSID* pClassID);

    // ISubStream
    [[nodiscard]] STDMETHODIMP_(int) GetStreamCount();
    [[nodiscard]] STDMETHODIMP GetStreamInfo(int i, WCHAR** ppName, LCID* pLCID);
    [[nodiscard]] STDMETHODIMP_(int) GetStream();
    [[nodiscard]] STDMETHODIMP SetStream(int iStream);
    STDMETHODIMP Reload();

    //fix me: add to an interface
    [[nodiscard]] HRESULT ParseSample(IMediaSample* pSample);
    [[nodiscard]] HRESULT NewSegment (REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
    [[nodiscard]] HRESULT EndOfStream(void);

    [[nodiscard]] HRESULT SetYuvType(CBaseSub::ColorType colorType, CBaseSub::YuvRangeType yuvRangeType);
private :
    CString         m_name;
    LCID            m_lcid;
    REFERENCE_TIME  m_rtStart;

    CBaseSub*       m_pSub;
    CCritSec        m_csCritSec;
    HdmvSubtitleProviderImpl *m_helper;
};

//This is for external subtitles
class __declspec(uuid("B5C880E5-A702-4F1B-B79D-410149874726"))
SupFileSubtitleProvider : public CUnknown, public IXySubRenderProvider, public ISubStream, protected CAMThread
{
public:
    SupFileSubtitleProvider();
    virtual ~SupFileSubtitleProvider();

    [[nodiscard]] bool Open(CString fn, CString subName = _T(""));

    [[nodiscard]] DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // IXySubRenderProvider
    [[nodiscard]] STDMETHODIMP Connect     (IXyOptions *consumer);
    [[nodiscard]] STDMETHODIMP RequestFrame(IXySubRenderFrame**subRenderFrame, REFERENCE_TIME now);
    [[nodiscard]] STDMETHODIMP Invalidate  (REFERENCE_TIME rtInvalidate = -1);

    // IPersist
    [[nodiscard]] STDMETHODIMP GetClassID(CLSID* pClassID);

    // ISubStream
    [[nodiscard]] STDMETHODIMP_(int) GetStreamCount();
    [[nodiscard]] STDMETHODIMP GetStreamInfo(int i, WCHAR** ppName, LCID* pLCID);
    [[nodiscard]] STDMETHODIMP_(int) GetStream();
    [[nodiscard]] STDMETHODIMP SetStream(int iStream);
    STDMETHODIMP Reload();

    [[nodiscard]] HRESULT SetYuvType(CBaseSub::ColorType colorType, CBaseSub::YuvRangeType yuvRangeType);
protected:
    DWORD ThreadProc();
private:
    CString     m_fname, m_subname;
    CHdmvSub*   m_pSub;
    CCritSec    m_csCritSec;
    HdmvSubtitleProviderImpl *m_helper;
};

