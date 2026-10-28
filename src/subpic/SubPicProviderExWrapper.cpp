/************************************************************************/
/* author: xy                                                           */
/* date: 20111013                                                       */
/************************************************************************/

#include "stdafx.h"
#include "SubPicProviderExWrapper.h"
#include "../dsutil/DSUtil.h"

CSubPicProviderExWrapper* CSubPicProviderExWrapper::GetSubPicProviderExWrapper(ISubPicProvider* const inner_provider)
{
    return DEBUG_NEW CSubPicProviderExWrapper{inner_provider};
}

CSubPicProviderExWrapper::CSubPicProviderExWrapper(ISubPicProvider* const inner_provider)
    : CUnknown(NAME("CSubPicProviderExWrapper"), NULL),
    _inner_provider(inner_provider)
{
}

CSubPicProviderExWrapper::~CSubPicProviderExWrapper()

{
}


// ISubPicProvider

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CSubPicProviderExWrapper::NonDelegatingQueryInterface( REFIID riid, void** ppv )
{
    return
        QI(ISubPicProvider)
        QI(ISubPicProviderEx)
        __super::NonDelegatingQueryInterface(riid, ppv);    
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CSubPicProviderExWrapper::Lock()
{
    return _inner_provider->Lock();
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CSubPicProviderExWrapper::Unlock()
{
    return _inner_provider->Unlock();
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(POSITION) CSubPicProviderExWrapper::GetStartPosition( const REFERENCE_TIME rt, const double fps )
{
    return _inner_provider->GetStartPosition(rt, fps);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(POSITION) CSubPicProviderExWrapper::GetNext( POSITION pos )
{
    return _inner_provider->GetNext(pos);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) CSubPicProviderExWrapper::GetStart( POSITION pos, const double fps )
{
    return _inner_provider->GetStart(pos, fps);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) CSubPicProviderExWrapper::GetStop( POSITION pos, const double fps )
{
    return _inner_provider->GetStop(pos, fps);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) CSubPicProviderExWrapper::IsAnimated( POSITION pos )
{
    return _inner_provider->IsAnimated(pos);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CSubPicProviderExWrapper::Render( SubPicDesc& spd, const REFERENCE_TIME rt, const double fps, RECT& bbox )
{
    return _inner_provider->Render( spd, rt, fps, bbox );
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CSubPicProviderExWrapper::GetTextureSize( POSITION pos, SIZE& MaxTextureSize, SIZE& VirtualSize, POINT& VirtualTopLeft )
{
    return _inner_provider->GetTextureSize( pos, MaxTextureSize, VirtualSize, VirtualTopLeft );
}

// ISubPicProviderEx

DECLSPEC_NOTHROW STDMETHODIMP_(VOID) CSubPicProviderExWrapper::GetStartStop( POSITION pos, const double fps, /*out*/REFERENCE_TIME& start, /*out*/REFERENCE_TIME& stop )
{
    start = _inner_provider->GetStart(pos, fps);
    stop = _inner_provider->GetStop(pos, fps);
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CSubPicProviderExWrapper::RenderEx( SubPicDesc& spd, const REFERENCE_TIME rt, const double fps, CAtlList<CRect>& rectList )
{
    CRect cRect;
    HRESULT hr{Render(spd, rt, fps, cRect)};
    if(SUCCEEDED(hr))
    {
        rectList.AddTail(cRect);
    }
    return hr;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) CSubPicProviderExWrapper::IsColorTypeSupported( const int type )

{
    return type==MSP_RGBA;
}

[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(int) CSubPicProviderExWrapper::SetOutputColorType( const int type )

{
    (void)type;
    return MSP_RGBA;
}
