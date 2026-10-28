//------------------------------------------------------------------------------
// File: CProp.h
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1992-2002 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __CPROP__
#define __CPROP__

// Base property page class. Filters typically expose custom properties by
// implementing special control interfaces, examples are IDirectDrawVideo
// and IQualProp on renderers. This allows property pages to be built that
// use the given interface. Applications such as the ActiveMovie OCX query
// filters for the property pages they support and expose them to the user
//
// This class provides all the framework for a property page. A property
// page is a COM object that supports IPropertyPage. We should be created
// with a resource ID for the dialog which we will load when required. We
// should also be given in the constructor a resource ID for a title string
// we will load from the DLLs STRINGTABLE. The property page titles must be
// stored in resource files so that they can be easily internationalised
//
// We have a number of virtual methods (not PURE) that may be overriden in
// derived classes to query for interfaces and so on. These functions have
// simple implementations here that just return NOERROR. Derived classes
// will almost definately have to override the message handler method called
// OnReceiveMessage. We have a static dialog procedure that calls the method
// so that derived classes don't have to fiddle around with the this pointer

class AM_NOVTABLE CBasePropertyPage : public IPropertyPage, public CUnknown
{
protected:

    LPPROPERTYPAGESITE m_pPageSite;       // Details for our property site
    HWND m_hwnd;                          // Window handle for the page
    HWND m_Dlg;                           // Actual dialog window handle
    BOOL m_bDirty;                        // Has anything been changed
    int m_TitleId;                        // Resource identifier for title
    int m_DialogId;                       // Dialog resource identifier

    static INT_PTR CALLBACK DialogProc(HWND hwnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam);

private:
    BOOL m_bObjectSet ;                  // SetObject has been called or not.
public:

    CBasePropertyPage(TCHAR *pName,      // Debug only name
                      LPUNKNOWN pUnk,    // COM Delegator
                      int DialogId,      // Resource ID
                      int TitleId);      // To get tital

#ifdef UNICODE
    CBasePropertyPage(CHAR *pName,
                      LPUNKNOWN pUnk,
                      int DialogId,  
                      int TitleId);
#endif
    virtual ~CBasePropertyPage() { }
    DECLARE_IUNKNOWN

    // Override these virtual methods

    virtual HRESULT OnConnect(IUnknown *pUnknown) { return NOERROR; }
    virtual HRESULT OnDisconnect() { return NOERROR; }
    virtual HRESULT OnActivate() { return NOERROR; }
    virtual HRESULT OnDeactivate() { return NOERROR; }
    virtual HRESULT OnApplyChanges() { return NOERROR; }
    virtual INT_PTR OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

    // These implement an IPropertyPage interface

    DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
    DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) NonDelegatingRelease();
    DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) NonDelegatingAddRef();
    DECLSPEC_NOTHROW STDMETHODIMP SetPageSite(LPPROPERTYPAGESITE pPageSite);
    DECLSPEC_NOTHROW STDMETHODIMP Activate(HWND hwndParent,LPCRECT prect,BOOL fModal);
    DECLSPEC_NOTHROW STDMETHODIMP Deactivate(void);
    DECLSPEC_NOTHROW STDMETHODIMP GetPageInfo(LPPROPPAGEINFO pPageInfo);
    DECLSPEC_NOTHROW STDMETHODIMP SetObjects(ULONG cObjects, LPUNKNOWN *ppUnk);
    DECLSPEC_NOTHROW STDMETHODIMP Show(UINT nCmdShow);
    DECLSPEC_NOTHROW STDMETHODIMP Move(LPCRECT prect);
    DECLSPEC_NOTHROW STDMETHODIMP IsPageDirty(void) { return m_bDirty ? S_OK : S_FALSE; }
    DECLSPEC_NOTHROW STDMETHODIMP Apply(void);
    DECLSPEC_NOTHROW STDMETHODIMP Help(LPCWSTR lpszHelpDir) { return E_NOTIMPL; }
    DECLSPEC_NOTHROW STDMETHODIMP TranslateAccelerator(LPMSG lpMsg) { return E_NOTIMPL; }
};

#endif // __CPROP__

