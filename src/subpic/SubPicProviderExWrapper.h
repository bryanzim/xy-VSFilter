/************************************************************************/
/* author: xy                                                           */
/* date: 20111013                                                       */
/************************************************************************/

#pragma once

#include "ISubPic.h"

class CSubPicProviderExWrapper : public CUnknown, public ISubPicProviderEx
{
private:
    explicit CSubPicProviderExWrapper(ISubPicProvider* const inner_provider); // disable inheriting

    CComPtr<ISubPicProvider> _inner_provider;

public:
    static CSubPicProviderExWrapper* GetSubPicProviderExWrapper(ISubPicProvider* const inner_provider);
    virtual ~CSubPicProviderExWrapper();

    DECLARE_IUNKNOWN
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // ISubPicProvider

    DECLSPEC_NOTHROW STDMETHODIMP Lock();
    DECLSPEC_NOTHROW STDMETHODIMP Unlock();

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(POSITION) GetStartPosition(REFERENCE_TIME rt, double fps);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(POSITION) GetNext(POSITION pos);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetStart(POSITION pos, double fps);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetStop(POSITION pos, double fps);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) IsAnimated(POSITION pos);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Render(SubPicDesc& spd, REFERENCE_TIME rt, double fps, RECT& bbox);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetTextureSize(POSITION pos, SIZE& MaxTextureSize, SIZE& VirtualSize, POINT& VirtualTopLeft);

    // ISubPicProviderEx

    DECLSPEC_NOTHROW STDMETHODIMP_(VOID) GetStartStop(POSITION pos, double fps, /*out*/ REFERENCE_TIME& start, /*out*/ REFERENCE_TIME& stop);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP RenderEx(SubPicDesc& spd, REFERENCE_TIME rt, double fps, CAtlList<CRect>& rectList);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) IsColorTypeSupported(int type);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(int) SetOutputColorType(int type); // Important! May failed!
};
