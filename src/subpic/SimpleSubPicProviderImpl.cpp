#include "stdafx.h"
#include "SimpleSubPicProviderImpl.h"
#include "SimpleSubPicWrapper.h"
#include "../subtitles/xy_bitmap.h"
#include "SimpleSubpicImpl.h"
#include "PooledSubPic.h"
#include "IDirectVobSubXy.h"

//////////////////////////////////////////////////////////////////////////
//
// SimpleSubPicProvider
// 


SimpleSubPicProvider::SimpleSubPicProvider( int alpha_blt_dst_type, SIZE spd_size, RECT video_rect, IXyOptions *consumer
    , HRESULT* phr/*=NULL*/ )
    : CUnknown(NAME("CSubPicQueueImpl"), nullptr)
    , m_alpha_blt_dst_type(alpha_blt_dst_type)
    , m_spd_size(spd_size)
    , m_video_rect(video_rect)
    , m_fps(25.0)
    , m_rtNow(0)
    , m_consumer(consumer)
{
    if(phr) {
        *phr = consumer ? S_OK : E_INVALIDARG;
    }

    m_prefered_colortype.AddTail(MSP_AYUV_PLANAR);
    m_prefered_colortype.AddTail(MSP_AYUV);
    m_prefered_colortype.AddTail(MSP_XY_AUYV);
    m_prefered_colortype.AddTail(MSP_RGBA);
}

SimpleSubPicProvider::~SimpleSubPicProvider()

{

}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider::NonDelegatingQueryInterface( REFIID riid, void** ppv )
{
    return
        QI(ISimpleSubPicProvider)
        __super::NonDelegatingQueryInterface(riid, ppv);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider::SetSubPicProvider( IUnknown* subpic_provider )
{
    if (subpic_provider!=nullptr)
    {
        CComQIPtr<ISubPicProviderEx2> tmp{subpic_provider};
        if (tmp)
        {
            CAutoLock cAutoLock(&m_csSubPicProvider);
            m_pSubPicProviderEx = tmp;

            if(m_pSubPicProviderEx!=nullptr)
            {
                POSITION pos{m_prefered_colortype.GetHeadPosition()};
                while(pos!=nullptr)
                {
                    int color_type{m_prefered_colortype.GetNext(pos)};
                    if( m_pSubPicProviderEx->IsColorTypeSupported( color_type ) &&
                        IsSpdColorTypeSupported( color_type ) )
                    {
                        m_spd_type = color_type;
                        break;
                    }
                }
            }
            (void)Invalidate();

            return S_OK;
        }

        return E_NOTIMPL;
    }
    
    return S_OK;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider::GetSubPicProvider( IUnknown** subpic_provider )
{
    if(!subpic_provider) {
        return E_POINTER;
    }

    CAutoLock cAutoLock(&m_csSubPicProvider);

    if(m_pSubPicProviderEx) {
        *subpic_provider = m_pSubPicProviderEx;
        (*subpic_provider)->AddRef();
    }

    return !!*subpic_provider ? S_OK : E_FAIL;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider::SetFPS( const double fps )
{
    m_fps = fps;

    return S_OK;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider::SetTime( const REFERENCE_TIME rtNow )
{
    m_rtNow = rtNow;

    return S_OK;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider::Invalidate( const REFERENCE_TIME rtInvalidate /*= -1*/ )
{
    CAutoLock cQueueLock(&m_csLock);

    if( m_pSubPic && m_subpic_stop >= rtInvalidate)
    {
        m_pSubPic = nullptr;
    }
    return S_OK;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) SimpleSubPicProvider::LookupSubPic( const REFERENCE_TIME now /*[in]*/, ISimpleSubPic** output_subpic /*[out]*/ )
{    
    if(output_subpic!=nullptr)
    {
        *output_subpic = nullptr;
        CComPtr<IXySubRenderFrame> temp = nullptr;
        bool result{LookupSubPicEx(now, &temp)};
        if (result && temp)
        {
            (*output_subpic = DEBUG_NEW SimpleSubpic(temp, m_alpha_blt_dst_type))->AddRef();
        }
        return result;
    }
    return false;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider::GetStats( int& nSubPics, REFERENCE_TIME& rtNow, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop )
{
    CAutoLock cAutoLock(&m_csLock);

    nSubPics = 0;
    rtNow = m_rtNow;
    rtStart = rtStop = 0;

    if(m_pSubPic)
    {
        nSubPics = 1;
        rtStart = m_subpic_start;
        rtStop = m_subpic_stop;
    }

    return S_OK;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider::GetStats( const int nSubPic, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop )
{
    CAutoLock cAutoLock(&m_csLock);

    if(!m_pSubPic || nSubPic != 0)
    {
        return E_INVALIDARG;
    }

    rtStart = m_subpic_start;
    rtStop = m_subpic_stop;

    return S_OK;
}

[[nodiscard]] bool SimpleSubPicProvider::LookupSubPicEx(const REFERENCE_TIME rtNow, IXySubRenderFrame** sub_render_frame)
{
    if(!sub_render_frame)
    {
        return(false);
    }

    *sub_render_frame = nullptr;

    if(m_subpic_start <= rtNow && rtNow < m_subpic_stop && m_pSubPic)
    {
        (*sub_render_frame = m_pSubPic)->AddRef();
    }
    else
    {
        CComPtr<ISubPicProviderEx2> pSubPicProvider;
        if(SUCCEEDED(GetSubPicProviderEx(&pSubPicProvider)) && pSubPicProvider
            && SUCCEEDED(pSubPicProvider->Lock()))
        {
            double fps{m_fps};

            if(POSITION pos = pSubPicProvider->GetStartPosition(rtNow, fps))
            {
                REFERENCE_TIME rtStart{pSubPicProvider->GetStart(pos, fps)};
                REFERENCE_TIME rtStop{pSubPicProvider->GetStop(pos, fps)};

                if(rtStart <= rtNow && rtNow < rtStop)
                {
                    (void)RenderTo(sub_render_frame, rtNow, rtNow+1, fps, true);
                }
            }

            pSubPicProvider->Unlock();

            if(*sub_render_frame)
            {
                CAutoLock cAutoLock(&m_csLock);

                m_pSubPic = *sub_render_frame;
            }
        }
    }
    return(!!*sub_render_frame);
}

[[nodiscard]] HRESULT SimpleSubPicProvider::GetSubPicProviderEx( ISubPicProviderEx2** pSubPicProviderEx )
{
    if(!pSubPicProviderEx) {
        return E_POINTER;
    }

    CAutoLock cAutoLock(&m_csSubPicProvider);

    if(m_pSubPicProviderEx) {
        *pSubPicProviderEx = m_pSubPicProviderEx;
        (*pSubPicProviderEx)->AddRef();
    }

    return !!*pSubPicProviderEx ? S_OK : E_FAIL;
}

[[nodiscard]] HRESULT SimpleSubPicProvider::RenderTo( IXySubRenderFrame** pSubPic, const REFERENCE_TIME rtStart, const REFERENCE_TIME rtStop, const double fps, const BOOL bIsAnimated )
{
    HRESULT hr{E_FAIL};

    if(!pSubPic) {
        return hr;
    }

    CComPtr<ISubPicProviderEx2> pSubPicProviderEx;
    if(FAILED(GetSubPicProviderEx(&pSubPicProviderEx)) || !pSubPicProviderEx) {
        return hr;
    }

    if(FAILED(pSubPicProviderEx->Lock())) {
        return hr;
    }

    CAtlList<CRect> rectList;

    CComPtr<IXySubRenderFrame> sub_render_frame;
    SIZE size_render_with;
    ASSERT(m_consumer);
    hr = m_consumer->XyGetSize(DirectVobSubXyOptions::SIZE_LAYOUT_WITH, &size_render_with);
    if (FAILED(hr))
    {
        pSubPicProviderEx->Unlock();
        return hr;
    }

    hr = pSubPicProviderEx->RenderEx(pSubPic, m_spd_type,
        CRect(CPoint(),m_spd_size), CRect(CPoint(),m_spd_size),
        size_render_with, 
        bIsAnimated ? rtStart : ((rtStart+rtStop)/2), fps);

    POSITION pos{pSubPicProviderEx->GetStartPosition(rtStart, fps)};

    if (!pSubPicProviderEx->IsAnimated(pos))
    {
        pSubPicProviderEx->GetStartStop(pos, fps, m_subpic_start, m_subpic_stop);
    }
    else
    {
        m_subpic_start = rtStart;
        m_subpic_stop = rtStart+1;
    }

    pSubPicProviderEx->Unlock();

    return hr;
}

[[nodiscard]] bool SimpleSubPicProvider::IsSpdColorTypeSupported( const int type )

{
    if( (type==MSP_RGBA) 
        ||
        (type==MSP_XY_AUYV &&  m_alpha_blt_dst_type == MSP_YUY2)
        ||
        (type==MSP_AYUV &&  m_alpha_blt_dst_type == MSP_AYUV)//ToDo: fix me MSP_AYUV 
        ||
        (type==MSP_AYUV_PLANAR && (m_alpha_blt_dst_type == MSP_IYUV ||
                                    m_alpha_blt_dst_type == MSP_YV12 ||
                                    m_alpha_blt_dst_type == MSP_P010 ||
                                    m_alpha_blt_dst_type == MSP_P016 ||
                                    m_alpha_blt_dst_type == MSP_YV16 ||
                                    m_alpha_blt_dst_type == MSP_YV24 ||
                                    m_alpha_blt_dst_type == MSP_P210 ||
                                    m_alpha_blt_dst_type == MSP_P216 ||
                                    m_alpha_blt_dst_type == MSP_NV12 ||
                                    m_alpha_blt_dst_type == MSP_NV21)) )
    {
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
//
// SimpleSubPicProvider2
//

SimpleSubPicProvider2::SimpleSubPicProvider2( int alpha_blt_dst_type, SIZE max_size, SIZE cur_size, RECT video_rect, 
    IXyOptions *consumer, HRESULT* phr/*=NULL*/)
    : CUnknown(NAME("SimpleSubPicProvider2"), nullptr)
    , m_alpha_blt_dst_type(alpha_blt_dst_type)
    , m_max_size(max_size)
    , m_cur_size(cur_size)
    , m_video_rect(video_rect)
    , m_fps(25.0)
    , m_now(0)
    , m_consumer(consumer)
{
    if (phr)
    {
        *phr= consumer ? S_OK : E_INVALIDARG;
    }
    m_ex_provider = nullptr;
    m_old_provider = nullptr;
    m_cur_provider = nullptr;
}

SimpleSubPicProvider2::~SimpleSubPicProvider2()

{
    delete m_ex_provider;
    delete m_old_provider;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider2::NonDelegatingQueryInterface( REFIID riid, void** ppv )
{
    return
        QI(ISimpleSubPicProvider)
        __super::NonDelegatingQueryInterface(riid, ppv);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider2::SetSubPicProvider( IUnknown* subpic_provider )
{
    const int MAX_SUBPIC_QUEUE_LENGTH{1};

    if (subpic_provider!=nullptr)
    {
        HRESULT hr;
        CComQIPtr<ISubPicProviderEx2> tmp{subpic_provider};
        if (tmp)
        {
            m_cur_provider = nullptr;
            delete m_old_provider;
            m_old_provider = nullptr;
            if (!m_ex_provider)
            {
                m_ex_provider = DEBUG_NEW SimpleSubPicProvider(m_alpha_blt_dst_type, m_cur_size, m_video_rect, m_consumer, &hr);
                (void)m_ex_provider->SetFPS(m_fps);
                (void)m_ex_provider->SetTime(m_now);
            }
            if (m_ex_provider==nullptr)
            {
                ASSERT(m_ex_provider!=nullptr);
                return E_FAIL;
            }
            m_cur_provider = m_ex_provider;
        }
        else
        {
            m_cur_provider = nullptr;
            delete m_ex_provider;
            m_ex_provider = nullptr;
            if (!m_old_provider)
            {
                CComPtr<ISubPicExAllocator> pSubPicAllocator = DEBUG_NEW CPooledSubPicAllocator(m_alpha_blt_dst_type, 
                    m_max_size, MAX_SUBPIC_QUEUE_LENGTH + 1);
                ASSERT(pSubPicAllocator);
                pSubPicAllocator->SetCurSize(m_cur_size);
                pSubPicAllocator->SetCurVidRect(m_video_rect);
                m_old_provider = DEBUG_NEW CSubPicQueueNoThread(pSubPicAllocator, &hr);
                (void)m_old_provider->SetFPS(m_fps);
                (void)m_old_provider->SetTime(m_now);
                if (FAILED(hr) || m_old_provider==nullptr)
                {
                    ASSERT(SUCCEEDED(hr));
                    return hr;
                }
            }
            m_cur_provider = m_old_provider;
        }
        return m_cur_provider->SetSubPicProvider(subpic_provider);
    }
    else
    {
        if (m_cur_provider!=nullptr)
        {
            m_cur_provider->SetSubPicProvider(nullptr);
        }
        m_cur_provider = nullptr;
    }
    return S_OK;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider2::GetSubPicProvider( IUnknown** subpic_provider )
{
    if (!m_cur_provider)
    {
        return S_FALSE;
    }
    return m_cur_provider->GetSubPicProvider(subpic_provider);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider2::SetFPS( const double fps )
{
    m_fps = fps;
    if (!m_cur_provider)
    {
        return S_FALSE;
    }
    return m_cur_provider->SetFPS(fps);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider2::SetTime( const REFERENCE_TIME rtNow )
{
    m_now = rtNow;
    if (!m_cur_provider)
    {
        return S_FALSE;
    }
    return m_cur_provider->SetTime(rtNow);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider2::Invalidate( const REFERENCE_TIME rtInvalidate /*= -1*/ )
{
    //ASSERT(m_cur_provider);
    if (!m_cur_provider)
    {
        return S_FALSE;
    }
    return m_cur_provider->Invalidate(rtInvalidate);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) SimpleSubPicProvider2::LookupSubPic( const REFERENCE_TIME now /*[in]*/, ISimpleSubPic** output_subpic/*[out]*/ )
{
    if (!m_cur_provider)
    {
        return false;
    }
    return m_cur_provider->LookupSubPic(now, output_subpic);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider2::GetStats( int& nSubPics, REFERENCE_TIME& rtNow, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop )
{
    nSubPics = 0;
    if (!m_cur_provider)
    {
        return S_FALSE;
    }
    return m_cur_provider->GetStats(nSubPics, rtNow, rtStart, rtStop);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SimpleSubPicProvider2::GetStats( const int nSubPic, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop )
{
    if (!m_cur_provider)
    {
        return S_FALSE;
    }
    return m_cur_provider->GetStats(nSubPic, rtStart, rtStop);
}
