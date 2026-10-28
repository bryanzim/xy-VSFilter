#pragma once

#include "../filters/transform/vsfilter/XySubRenderIntf.h"

interface ISubPicProviderEx;
interface IXyOptions;
interface ISubPicEx;
interface ISubPicProviderEx2;

class XySubRenderProviderWrapper: public CUnknown, public IXySubRenderProvider
{
public:
    XySubRenderProviderWrapper( ISubPicProviderEx *provider, HRESULT* phr/*=NULL*/ );
    virtual ~XySubRenderProviderWrapper() {}

    DECLARE_IUNKNOWN
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // IXySubRenderProvider
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Connect( IXyOptions *consumer );
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP RequestFrame(IXySubRenderFrame**subRenderFrame, REFERENCE_TIME now);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Invalidate(REFERENCE_TIME rtInvalidate = -1);

private:
    HRESULT Render( REFERENCE_TIME now, POSITION pos, double fps );
    HRESULT ResetAllocator();
private:
    IXyOptions *m_consumer;
    CComPtr<ISubPicProviderEx> m_provider;
    CComPtr<ISubPicExAllocator> m_allocator;
    CComPtr<ISubPicEx> m_pSubPic;
    CComPtr<IXySubRenderFrame> m_xy_sub_render_frame;
    CSize m_original_video_size;
    bool m_use_dst_alpha;
};

class XySubRenderProviderWrapper2: public CUnknown, public IXySubRenderProvider
{
public:
    XySubRenderProviderWrapper2( ISubPicProviderEx2 *provider, HRESULT* phr/*=NULL*/ );
    virtual ~XySubRenderProviderWrapper2() {}

    DECLARE_IUNKNOWN
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // IXySubRenderProvider
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Connect(IXyOptions *consumer);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP RequestFrame(IXySubRenderFrame**subRenderFrame, REFERENCE_TIME now);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Invalidate(REFERENCE_TIME rtInvalidate = -1);

private:
    HRESULT Render( REFERENCE_TIME now, POSITION pos );
    [[nodiscard]] HRESULT CombineBitmap(REFERENCE_TIME now);
private:
    IXyOptions *m_consumer;
    CComPtr<ISubPicProviderEx2> m_provider;
    CComPtr<IXySubRenderFrame> m_xy_sub_render_frame;
    REFERENCE_TIME m_start, m_stop;
    CRect m_output_rect, m_subtitle_target_rect;
    CSize m_original_video_size;
    bool m_use_dst_alpha;
    bool m_render_to_original_video_size;

    double m_fps;

    int m_max_bitmap_count2;
    CComPtr<ISubPicExAllocator> m_allocator;
    CComPtr<ISubPicEx> m_subpic;
};
