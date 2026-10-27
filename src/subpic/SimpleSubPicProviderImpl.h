#pragma once

#include "ISimpleSubPic.h"
#include "SubPicQueueImpl.h"

interface IXyOptions;

//////////////////////////////////////////////////////////////////////////
//
// SimpleSubPicProvider
// 

class SimpleSubPicProvider: public CUnknown, public ISimpleSubPicProvider
{
public:
    SimpleSubPicProvider(int alpha_blt_dst_type, SIZE spd_size, RECT video_rect, IXyOptions *consumer, HRESULT* phr=NULL);
    constexpr virtual ~SimpleSubPicProvider() noexcept;

    DECLARE_IUNKNOWN;
    [[nodiscard]] STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    [[nodiscard]] bool LookupSubPicEx(REFERENCE_TIME rtNow, IXySubRenderFrame** sub_render_frame);
    [[nodiscard]] HRESULT GetSubPicProviderEx(ISubPicProviderEx2** pSubPicProviderEx);
    [[nodiscard]] HRESULT RenderTo(IXySubRenderFrame** pSubPic, REFERENCE_TIME rtStart, REFERENCE_TIME rtStop, double fps, BOOL bIsAnimated);
    [[nodiscard]] constexpr bool IsSpdColorTypeSupported(int type) noexcept;
public:
    // ISimpleSubPicProvider

    STDMETHODIMP SetSubPicProvider(IUnknown* subpic_provider);
    [[nodiscard]] STDMETHODIMP GetSubPicProvider(IUnknown** subpic_provider);

    [[nodiscard]] STDMETHODIMP SetFPS(double fps);
    [[nodiscard]] STDMETHODIMP SetTime(REFERENCE_TIME rtNow);

    [[nodiscard]] STDMETHODIMP Invalidate(REFERENCE_TIME rtInvalidate = -1);
    [[nodiscard]] STDMETHODIMP_(bool) LookupSubPic(REFERENCE_TIME now /*[in]*/, ISimpleSubPic** output_subpic/*[out]*/);

    [[nodiscard]] STDMETHODIMP GetStats(int& nSubPics, REFERENCE_TIME& rtNow, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);
    [[nodiscard]] STDMETHODIMP GetStats(int nSubPic, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);
private:
    CCritSec m_csSubPicProvider;
    CComPtr<ISubPicProviderEx2> m_pSubPicProviderEx;

    CAtlList<int> m_prefered_colortype;
protected:
    int m_alpha_blt_dst_type;
    SIZE m_spd_size;
    RECT m_video_rect;
    int m_spd_type;
    double m_fps;
    REFERENCE_TIME m_rtNow;

    CComPtr<ISubPicExAllocator> m_pAllocator;

    CCritSec m_csLock;
    REFERENCE_TIME m_subpic_start,m_subpic_stop;
    CComPtr<IXySubRenderFrame> m_pSubPic;

    IXyOptions *m_consumer;
};

//////////////////////////////////////////////////////////////////////////
//
// SimpleSubPicProvider2
// 

class SimpleSubPicProvider2: public CUnknown, public ISimpleSubPicProvider
{
public:
    SimpleSubPicProvider2(int alpha_blt_dst_type, SIZE max_size, SIZE cur_size, RECT video_rect, 
        IXyOptions *consumer, HRESULT* phr=NULL);
    constexpr virtual ~SimpleSubPicProvider2() noexcept;

    DECLARE_IUNKNOWN;
    [[nodiscard]] STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);
public:
    SimpleSubPicProvider2(const SimpleSubPicProvider2&);
    void operator=(const SimpleSubPicProvider2&)const;

    // ISimpleSubPicProvider

    [[nodiscard]] STDMETHODIMP SetSubPicProvider(IUnknown* subpic_provider);
    [[nodiscard]] STDMETHODIMP GetSubPicProvider(IUnknown** subpic_provider);

    [[nodiscard]] STDMETHODIMP SetFPS(double fps);
    [[nodiscard]] STDMETHODIMP SetTime(REFERENCE_TIME rtNow);

    [[nodiscard]] STDMETHODIMP Invalidate(REFERENCE_TIME rtInvalidate = -1);
    [[nodiscard]] STDMETHODIMP_(bool) LookupSubPic(REFERENCE_TIME now /*[in]*/, ISimpleSubPic** output_subpic/*[out]*/);

    [[nodiscard]] STDMETHODIMP GetStats(int& nSubPics, REFERENCE_TIME& rtNow, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);
    [[nodiscard]] STDMETHODIMP GetStats(int nSubPic, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);

protected:
    int m_alpha_blt_dst_type;
    SIZE m_max_size;
    SIZE m_cur_size;
    RECT m_video_rect;

    double m_fps;
    REFERENCE_TIME m_now;

    ISimpleSubPicProvider *m_cur_provider;
    SimpleSubPicProvider *m_ex_provider;
    CSubPicQueueNoThread *m_old_provider;

    IXyOptions *m_consumer;
};
