/************************************************************************/
/* author: xy                                                           */
/* date: 20111013                                                       */
/************************************************************************/

#pragma once

#include "ISubPic.h"

class CSubPicProviderExWrapper : public CUnknown, public ISubPicProviderEx
{
private:
    explicit CSubPicProviderExWrapper(ISubPicProvider* const inner_provider);//disable inheriting
    
    CComPtr<ISubPicProvider> _inner_provider;
public:
    static CSubPicProviderExWrapper* GetSubPicProviderExWrapper(ISubPicProvider* const inner_provider);
    virtual ~CSubPicProviderExWrapper();

    DECLARE_IUNKNOWN;
    [[nodiscard]] STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // ISubPicProvider

    STDMETHODIMP Lock();
    STDMETHODIMP Unlock();

    [[nodiscard]] STDMETHODIMP_(POSITION) GetStartPosition(REFERENCE_TIME rt, double fps);
    [[nodiscard]] STDMETHODIMP_(POSITION) GetNext(POSITION pos);

    [[nodiscard]] STDMETHODIMP_(REFERENCE_TIME) GetStart(POSITION pos, double fps);
    [[nodiscard]] STDMETHODIMP_(REFERENCE_TIME) GetStop(POSITION pos, double fps);    

    [[nodiscard]] STDMETHODIMP_(bool) IsAnimated (POSITION pos);

    [[nodiscard]] STDMETHODIMP Render(SubPicDesc& spd, REFERENCE_TIME rt, double fps, RECT& bbox);
    [[nodiscard]] STDMETHODIMP GetTextureSize (POSITION pos, SIZE& MaxTextureSize, SIZE& VirtualSize, POINT& VirtualTopLeft);

    // ISubPicProviderEx

    [[nodiscard]] STDMETHODIMP_(VOID) GetStartStop(POSITION pos, double fps, /*out*/REFERENCE_TIME& start, /*out*/REFERENCE_TIME& stop);

    [[nodiscard]] STDMETHODIMP RenderEx(SubPicDesc& spd, REFERENCE_TIME rt, double fps, CAtlList<CRect>& rectList);

    [[nodiscard]] STDMETHODIMP_(bool) IsColorTypeSupported(int type);
    [[nodiscard]] STDMETHODIMP_(int) SetOutputColorType(int type);//Important! May failed!
};
