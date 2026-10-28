//------------------------------------------------------------------------------
// File: VideoCtl.h
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1992-2002 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __VIDEOCTL__
#define __VIDEOCTL__

// These help with property page implementations. The first can be used to
// load any string from a resource file. The buffer to load into is passed
// as an input parameter. The same buffer is the return value if the string
// was found otherwise it returns TEXT(""). The GetDialogSize is passed the
// resource ID of a dialog box and returns the size of it in screen pixels

#define STR_MAX_LENGTH 256
const TCHAR * WINAPI StringFromResource(TCHAR *pBuffer, int iResourceID);

#ifdef UNICODE
#define WideStringFromResource StringFromResource
const char* WINAPI StringFromResource(char* pBuffer, int iResourceID);
#else
WCHAR * WINAPI WideStringFromResource(WCHAR *pBuffer, int iResourceID);
#endif


BOOL WINAPI GetDialogSize(int iResourceID,     // Dialog box resource identifier
                          DLGPROC pDlgProc,    // Pointer to dialog procedure
                          LPARAM lParam,       // Any user data wanted in pDlgProc
                          SIZE *pResult);      // Returns the size of dialog box

// Class that aggregates an IDirectDraw interface

class CAggDirectDraw : public IDirectDraw, public CUnknown
{
protected:

    LPDIRECTDRAW m_pDirectDraw;

public:

    DECLARE_IUNKNOWN
    DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);

    // Constructor and destructor

    CAggDirectDraw(TCHAR *pName,LPUNKNOWN pUnk) :
        CUnknown(pName,pUnk),
        m_pDirectDraw(NULL) { }

    virtual ~CAggDirectDraw() { }

    // Set the object we should be aggregating
    void SetDirectDraw(LPDIRECTDRAW pDirectDraw) {
        m_pDirectDraw = pDirectDraw;
    }

    // IDirectDraw methods

    DECLSPEC_NOTHROW STDMETHODIMP Compact();
    DECLSPEC_NOTHROW STDMETHODIMP CreateClipper(DWORD dwFlags,LPDIRECTDRAWCLIPPER *lplpDDClipper,IUnknown *pUnkOuter);
    DECLSPEC_NOTHROW STDMETHODIMP CreatePalette(DWORD dwFlags,LPPALETTEENTRY lpColorTable,LPDIRECTDRAWPALETTE *lplpDDPalette,IUnknown *pUnkOuter);
    DECLSPEC_NOTHROW STDMETHODIMP CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc,LPDIRECTDRAWSURFACE *lplpDDSurface,IUnknown *pUnkOuter);
    DECLSPEC_NOTHROW STDMETHODIMP DuplicateSurface(LPDIRECTDRAWSURFACE lpDDSurface,LPDIRECTDRAWSURFACE *lplpDupDDSurface);
    DECLSPEC_NOTHROW STDMETHODIMP EnumDisplayModes(DWORD dwSurfaceDescCount,LPDDSURFACEDESC lplpDDSurfaceDescList,LPVOID lpContext,LPDDENUMMODESCALLBACK lpEnumCallback);
    DECLSPEC_NOTHROW STDMETHODIMP EnumSurfaces(DWORD dwFlags,LPDDSURFACEDESC lpDDSD,LPVOID lpContext,LPDDENUMSURFACESCALLBACK lpEnumCallback);
    DECLSPEC_NOTHROW STDMETHODIMP FlipToGDISurface();
    DECLSPEC_NOTHROW STDMETHODIMP GetCaps(LPDDCAPS lpDDDriverCaps,LPDDCAPS lpDDHELCaps);
    DECLSPEC_NOTHROW STDMETHODIMP GetDisplayMode(LPDDSURFACEDESC lpDDSurfaceDesc);
    DECLSPEC_NOTHROW STDMETHODIMP GetFourCCCodes(LPDWORD lpNumCodes,LPDWORD lpCodes);
    DECLSPEC_NOTHROW STDMETHODIMP GetGDISurface(LPDIRECTDRAWSURFACE *lplpGDIDDSurface);
    DECLSPEC_NOTHROW STDMETHODIMP GetMonitorFrequency(LPDWORD lpdwFrequency);
    DECLSPEC_NOTHROW STDMETHODIMP GetScanLine(LPDWORD lpdwScanLine);
    DECLSPEC_NOTHROW STDMETHODIMP GetVerticalBlankStatus(LPBOOL lpblsInVB);
    DECLSPEC_NOTHROW STDMETHODIMP Initialize(GUID *lpGUID);
    DECLSPEC_NOTHROW STDMETHODIMP RestoreDisplayMode();
    DECLSPEC_NOTHROW STDMETHODIMP SetCooperativeLevel(HWND hWnd,DWORD dwFlags);
    DECLSPEC_NOTHROW STDMETHODIMP SetDisplayMode(DWORD dwWidth,DWORD dwHeight,DWORD dwBpp);
    DECLSPEC_NOTHROW STDMETHODIMP WaitForVerticalBlank(DWORD dwFlags,HANDLE hEvent);
};


// Class that aggregates an IDirectDrawSurface interface

class CAggDrawSurface : public IDirectDrawSurface, public CUnknown
{
protected:

    LPDIRECTDRAWSURFACE m_pDirectDrawSurface;

public:

    DECLARE_IUNKNOWN
    DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);

    // Constructor and destructor

    CAggDrawSurface(TCHAR *pName,LPUNKNOWN pUnk) :
        CUnknown(pName,pUnk),
        m_pDirectDrawSurface(NULL) { }

    virtual ~CAggDrawSurface() { }

    // Set the object we should be aggregating
    void SetDirectDrawSurface(LPDIRECTDRAWSURFACE pDirectDrawSurface) {
        m_pDirectDrawSurface = pDirectDrawSurface;
    }

    // IDirectDrawSurface methods

    DECLSPEC_NOTHROW STDMETHODIMP AddAttachedSurface(LPDIRECTDRAWSURFACE lpDDSAttachedSurface);
    DECLSPEC_NOTHROW STDMETHODIMP AddOverlayDirtyRect(LPRECT lpRect);
    DECLSPEC_NOTHROW STDMETHODIMP Blt(LPRECT lpDestRect,LPDIRECTDRAWSURFACE lpDDSrcSurface,LPRECT lpSrcRect,DWORD dwFlags,LPDDBLTFX lpDDBltFx);
    DECLSPEC_NOTHROW STDMETHODIMP BltBatch(LPDDBLTBATCH lpDDBltBatch,DWORD dwCount,DWORD dwFlags);
    DECLSPEC_NOTHROW STDMETHODIMP BltFast(DWORD dwX,DWORD dwY,LPDIRECTDRAWSURFACE lpDDSrcSurface,LPRECT lpSrcRect,DWORD dwTrans);
    DECLSPEC_NOTHROW STDMETHODIMP DeleteAttachedSurface(DWORD dwFlags,LPDIRECTDRAWSURFACE lpDDSAttachedSurface);
    DECLSPEC_NOTHROW STDMETHODIMP EnumAttachedSurfaces(LPVOID lpContext,LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback);
    DECLSPEC_NOTHROW STDMETHODIMP EnumOverlayZOrders(DWORD dwFlags,LPVOID lpContext,LPDDENUMSURFACESCALLBACK lpfnCallback);
    DECLSPEC_NOTHROW STDMETHODIMP Flip(LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride,DWORD dwFlags);
    DECLSPEC_NOTHROW STDMETHODIMP GetAttachedSurface(LPDDSCAPS lpDDSCaps,LPDIRECTDRAWSURFACE *lplpDDAttachedSurface);
    DECLSPEC_NOTHROW STDMETHODIMP GetBltStatus(DWORD dwFlags);
    DECLSPEC_NOTHROW STDMETHODIMP GetCaps(LPDDSCAPS lpDDSCaps);
    DECLSPEC_NOTHROW STDMETHODIMP GetClipper(LPDIRECTDRAWCLIPPER *lplpDDClipper);
    DECLSPEC_NOTHROW STDMETHODIMP GetColorKey(DWORD dwFlags,LPDDCOLORKEY lpDDColorKey);
    DECLSPEC_NOTHROW STDMETHODIMP GetDC(HDC *lphDC);
    DECLSPEC_NOTHROW STDMETHODIMP GetFlipStatus(DWORD dwFlags);
    DECLSPEC_NOTHROW STDMETHODIMP GetOverlayPosition(LPLONG lpdwX,LPLONG lpdwY);
    DECLSPEC_NOTHROW STDMETHODIMP GetPalette(LPDIRECTDRAWPALETTE *lplpDDPalette);
    DECLSPEC_NOTHROW STDMETHODIMP GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat);
    DECLSPEC_NOTHROW STDMETHODIMP GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc);
    DECLSPEC_NOTHROW STDMETHODIMP Initialize(LPDIRECTDRAW lpDD,LPDDSURFACEDESC lpDDSurfaceDesc);
    DECLSPEC_NOTHROW STDMETHODIMP IsLost();
    DECLSPEC_NOTHROW STDMETHODIMP Lock(LPRECT lpDestRect,LPDDSURFACEDESC lpDDSurfaceDesc,DWORD dwFlags,HANDLE hEvent);
    DECLSPEC_NOTHROW STDMETHODIMP ReleaseDC(HDC hDC);
    DECLSPEC_NOTHROW STDMETHODIMP Restore();
    DECLSPEC_NOTHROW STDMETHODIMP SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper);
    DECLSPEC_NOTHROW STDMETHODIMP SetColorKey(DWORD dwFlags,LPDDCOLORKEY lpDDColorKey);
    DECLSPEC_NOTHROW STDMETHODIMP SetOverlayPosition(LONG dwX,LONG dwY);
    DECLSPEC_NOTHROW STDMETHODIMP SetPalette(LPDIRECTDRAWPALETTE lpDDPalette);
    DECLSPEC_NOTHROW STDMETHODIMP Unlock(LPVOID lpSurfaceData);
    DECLSPEC_NOTHROW STDMETHODIMP UpdateOverlay(LPRECT lpSrcRect,LPDIRECTDRAWSURFACE lpDDDestSurface,LPRECT lpDestRect,DWORD dwFlags,LPDDOVERLAYFX lpDDOverlayFX);
    DECLSPEC_NOTHROW STDMETHODIMP UpdateOverlayDisplay(DWORD dwFlags);
    DECLSPEC_NOTHROW STDMETHODIMP UpdateOverlayZOrder(DWORD dwFlags,LPDIRECTDRAWSURFACE lpDDSReference);
};


// DirectShow must work on multiple platforms.  In particular, it also runs on
// Windows NT 3.51 which does not have DirectDraw capabilities. The filters
// cannot therefore link statically to the DirectDraw library. To make their
// lives that little bit easier we provide this class that manages loading
// and unloading the library and creating the initial IDirectDraw interface

typedef DWORD (WINAPI *PGETFILEVERSIONINFOSIZE)(LPTSTR,LPDWORD);
typedef BOOL (WINAPI *PGETFILEVERSIONINFO)(LPTSTR,DWORD,DWORD,LPVOID);
typedef BOOL (WINAPI *PVERQUERYVALUE)(LPVOID,LPTSTR,LPVOID,PUINT);

class CLoadDirectDraw
{
    LPDIRECTDRAW m_pDirectDraw;     // The DirectDraw driver instance
    HINSTANCE m_hDirectDraw;        // Handle to the loaded library

public:

    CLoadDirectDraw();
    ~CLoadDirectDraw();

    HRESULT LoadDirectDraw(LPSTR szDevice);
    void ReleaseDirectDraw();
    HRESULT IsDirectDrawLoaded();
    LPDIRECTDRAW GetDirectDraw();
    BOOL IsDirectDrawVersion1();
};

#endif // __VIDEOCTL__

