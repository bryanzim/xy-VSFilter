#pragma once

#include "ISubPic.h"
#include "ISimpleSubPic.h"

//
// SimpleSubPicWrapper
//

class SimpleSubPicWrapper: public CUnknown, public ISimpleSubPic
{
public:
    explicit SimpleSubPicWrapper(ISubPicEx * inner_obj);
    explicit SimpleSubPicWrapper(ISubPic* inner_obj);

    DECLARE_IUNKNOWN;
    [[nodiscard]] STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    [[nodiscard]] STDMETHODIMP AlphaBlt(SubPicDesc* target);
private:
    CComPtr<ISubPicEx> m_inner_obj;
    CComPtr<ISubPic> m_inner_obj2;
};
