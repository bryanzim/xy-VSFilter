/************************************************************************/
/* author: xy                                                           */
/* date: 20111013                                                       */
/************************************************************************/

#include "stdafx.h"
#include "SubPicProviderExWrapper.h"
#include "../DSUtil/DSUtil.h"

CSubPicProviderExWrapper* CSubPicProviderExWrapper::GetSubPicProviderExWrapper(ISubPicProvider* const inner_provider)
{
    return DEBUG_NEW CSubPicProviderExWrapper{inner_provider};
}

CSubPicProviderExWrapper::CSubPicProviderExWrapper(ISubPicProvider* const inner_provider)
    : CUnknown(NAME("CSubPicProviderExWrapper"), NULL),
    _inner_provider(inner_provider)
{
}

constexpr CSubPicProviderExWrapper::~CSubPicProviderExWrapper()
noexcept
{
}


// ISubPicProvider

[[nodiscard]] STDMETHODIMP CSubPicProviderExWrapper::NonDelegatingQueryInterface( const REFIID riid, void** ppv )
{
    return
        QI(ISubPicProvider)
        QI(ISubPicProviderEx)
        __super::NonDelegatingQueryInterface(riid, ppv);    
}

[[nodiscard]] STDMETHODIMP CSubPicProviderExWrapper::Lock()
{
    return _inner_provider->Lock();
}

[[nodiscard]] STDMETHODIMP CSubPicProviderExWrapper::Unlock()
{
    return _inner_provider->Unlock();
}

[[nodiscard]] STDMETHODIMP_(POSITION) CSubPicProviderExWrapper::GetStartPosition( const REFERENCE_TIME rt, const double fps )
{
    return _inner_provider->GetStartPosition(rt, fps);
}

[[nodiscard]] STDMETHODIMP_(POSITION) CSubPicProviderExWrapper::GetNext( const POSITION pos )
{
    return _inner_provider->GetNext(pos);
}

[[nodiscard]] STDMETHODIMP_(REFERENCE_TIME) CSubPicProviderExWrapper::GetStart( const POSITION pos, const double fps )
{
    return _inner_provider->GetStart(pos, fps);
}

[[nodiscard]] STDMETHODIMP_(REFERENCE_TIME) CSubPicProviderExWrapper::GetStop( const POSITION pos, const double fps )
{
    return _inner_provider->GetStop(pos, fps);
}

[[nodiscard]] STDMETHODIMP_(bool) CSubPicProviderExWrapper::IsAnimated( const POSITION pos )
{
    return _inner_provider->IsAnimated(pos);
}

[[nodiscard]] STDMETHODIMP CSubPicProviderExWrapper::Render( SubPicDesc& spd, const REFERENCE_TIME rt, const double fps, RECT& bbox )
{
    return _inner_provider->Render( spd, rt, fps, bbox );
}

[[nodiscard]] STDMETHODIMP CSubPicProviderExWrapper::GetTextureSize( const POSITION pos, SIZE& MaxTextureSize, SIZE& VirtualSize, POINT& VirtualTopLeft )
{
    return _inner_provider->GetTextureSize( pos, MaxTextureSize, VirtualSize, VirtualTopLeft );
}

// ISubPicProviderEx

[[nodiscard]] STDMETHODIMP_(VOID) CSubPicProviderExWrapper::GetStartStop( const POSITION pos, const double fps, /*out*/REFERENCE_TIME& start, /*out*/REFERENCE_TIME& stop )
{
    start = _inner_provider->GetStart(pos, fps);
    stop = _inner_provider->GetStop(pos, fps);
}

[[nodiscard]] STDMETHODIMP CSubPicProviderExWrapper::RenderEx( SubPicDesc& spd, const REFERENCE_TIME rt, const double fps, CAtlList<CRect>& rectList )
{
    CRect cRect;
    HRESULT hr{Render(spd, rt, fps, cRect)};
    if(SUCCEEDED(hr))
    {
        rectList.AddTail(cRect);
    }
    return hr;
}

[[nodiscard]] constexpr STDMETHODIMP_(bool) CSubPicProviderExWrapper::IsColorTypeSupported( const int type )
noexcept
{
    return type==MSP_RGBA;
}

[[nodiscard]] constexpr STDMETHODIMP_(int) CSubPicProviderExWrapper::SetOutputColorType( const int type )
noexcept
{
    (type);
    return MSP_RGBA;
}
