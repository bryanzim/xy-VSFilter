/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#pragma once

#include "IDirectVobSub.h"
#include "IDirectVobSubXy.h"
#include "..\..\..\..\include\IFilterVersion.h"
#include "version.h"
#include "XyOptionsImpl.h"

class DirectVobSubImpl : public IDirectVobSub2, public XyOptionsImpl, public IFilterVersion
{
public:
    typedef XyOptionsImpl::Option Option;
    typedef DirectVobSubXyOptions::SubStyle SubStyle;

    enum ColorSpaceOption
    {
        YuvMatrix_AUTO = 0
        ,BT_601
        ,BT_709
        ,BT_2020
        ,GUESS
    };
    enum YuvRange
    {
        YuvRange_Auto = 0
        ,YuvRange_TV
        ,YuvRange_PC
    };
    enum LoadLevel
    {
        LOADLEVEL_WHEN_NEEDED = 0,
        LOADLEVEL_ALWAYS = 1,
        LOADLEVEL_DISABLED = 2,
        LOADLEVEL_COUNT
    };

    static const int REQUIRED_CONFIG_VERSION{706};
    static const int CUR_SUPPORTED_FILTER_VERSION{39};

    typedef DirectVobSubXyOptions::CachesInfo CachesInfo;
    typedef DirectVobSubXyOptions::XyFlyWeightInfo XyFlyWeightInfo;
    typedef DirectVobSubXyOptions::ColorSpaceOpt ColorSpaceOpt;

    struct FilterInfo
    {
        CStringW guid, friendly_name;
    };
protected:
    DirectVobSubImpl(const Option *options, CCritSec * pLock);
    virtual ~DirectVobSubImpl();

    [[nodiscard]] constexpr bool is_compatible() noexcept;
    [[nodiscard]] UINT GetCompatibleProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
    [[nodiscard]] CString GetCompatibleProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault);

    [[nodiscard]] virtual HRESULT GetCurStyles(const SubStyle sub_style[], const int count) { return E_NOTIMPL; }
    [[nodiscard]] virtual HRESULT SetCurStyles(const SubStyle sub_style[], const int count) { return E_NOTIMPL; }
protected:
    CCritSec *m_propsLock;

    int m_bt601Width, m_bt601Height;//for AUTO_GUESS

    static int const MAX_COLOR_SPACE{256};
    ColorSpaceOpt m_outputColorSpace[MAX_COLOR_SPACE];
    ColorSpaceOpt m_inputColorSpace[MAX_COLOR_SPACE];

	STSStyle m_defStyle;

	bool m_fAdvancedRenderer;
	int m_nReloaderDisableCount;
	int m_SubtitleDelay, m_SubtitleSpeedMul, m_SubtitleSpeedDiv;
    // User could specify any values for m_SubtitleSpeedMul and m_SubtitleSpeedDiv.
    // We want to normalize the user input with their greatest common divisor to minimize 
    // the chance of overflow later when we do calculations.
    int m_SubtitleSpeedNormalizedMul, m_SubtitleSpeedNormalizedDiv;

	NORMALIZEDRECT m_ZoomRect;

    CAtlArray<CStringW> m_known_source_filters_guid;
    CAtlArray<CStringW> m_known_source_filters_name;

    int m_supported_filter_verion;
    int m_config_info_version;

	CComPtr<ISubClock> m_pSubClock;

public:
    static void LoadKnownSourceFilters( CAtlArray<CStringW> *filter_guid,
                                        CAtlArray<CStringW> *filter_name );
    static void SaveKnownSourceFilters( const CAtlArray<CStringW>& filter_guid, 
                                        const CAtlArray<CStringW>& filter_name );
public:
    // XyOptionsImpl
    HRESULT DoGetField(unsigned field, void *value);
    [[nodiscard]] HRESULT DoSetField(unsigned field, void const *value);

    // IXyOptions
    [[nodiscard]] STDMETHODIMP XyGetBin      (unsigned field, LPVOID    *value, int *size );
    [[nodiscard]] STDMETHODIMP XyGetBin2     (unsigned field, void      *value, int size );

    [[nodiscard]] STDMETHODIMP XySetBin      (unsigned field, LPVOID    value, int size );

	// IDirectVobSub

    [[nodiscard]] STDMETHODIMP get_FileName(WCHAR* fn);
    [[nodiscard]] STDMETHODIMP put_FileName(WCHAR* fn);
    [[nodiscard]] STDMETHODIMP get_LanguageCount(int* nLangs);
    [[nodiscard]] STDMETHODIMP get_LanguageName(int iLanguage, WCHAR** ppName);
    [[nodiscard]] STDMETHODIMP get_SelectedLanguage(int* iSelected);
    [[nodiscard]] STDMETHODIMP put_SelectedLanguage(int iSelected);
    [[nodiscard]] STDMETHODIMP get_HideSubtitles(bool* fHideSubtitles);
    [[nodiscard]] STDMETHODIMP put_HideSubtitles(bool fHideSubtitles);
    [[nodiscard]] STDMETHODIMP get_PreBuffering(bool* fDoPreBuffering);
    [[nodiscard]] STDMETHODIMP put_PreBuffering(bool fDoPreBuffering);

    
    [[nodiscard]] constexpr STDMETHODIMP get_SubPictToBuffer(unsigned int* uSubPictToBuffer)
    noexcept
    {
        return E_NOTIMPL;
    }
    [[nodiscard]] constexpr STDMETHODIMP put_SubPictToBuffer(const unsigned int uSubPictToBuffer)
    noexcept
    {
        return E_NOTIMPL;
    }
    [[nodiscard]] constexpr STDMETHODIMP get_AnimWhenBuffering(bool* fAnimWhenBuffering)
    noexcept
    {
        return E_NOTIMPL;
    }
    [[nodiscard]] constexpr STDMETHODIMP put_AnimWhenBuffering(const bool fAnimWhenBuffering)
    noexcept
    {
        return E_NOTIMPL;
    }
    [[nodiscard]] STDMETHODIMP get_Placement(bool* fOverridePlacement, int* xperc, int* yperc);
    [[nodiscard]] STDMETHODIMP put_Placement(bool fOverridePlacement, int xperc, int yperc);
    [[nodiscard]] STDMETHODIMP get_VobSubSettings(bool* fBuffer, bool* fOnlyShowForcedSubs, bool* fPolygonize);
    [[nodiscard]] STDMETHODIMP put_VobSubSettings(bool fBuffer, bool fOnlyShowForcedSubs, bool fPolygonize);
    [[nodiscard]] STDMETHODIMP get_TextSettings(void* lf, int lflen, COLORREF* color, bool* fShadow, bool* fOutline, bool* fAdvancedRenderer);
    [[nodiscard]] STDMETHODIMP put_TextSettings(void* lf, int lflen, COLORREF color, bool fShadow, bool fOutline, bool fAdvancedRenderer);
    [[nodiscard]] STDMETHODIMP get_Flip(bool* fPicture, bool* fSubtitles);
    [[nodiscard]] STDMETHODIMP put_Flip(bool fPicture, bool fSubtitles);
    [[nodiscard]] STDMETHODIMP get_OSD(bool* fShowOSD);
    [[nodiscard]] STDMETHODIMP put_OSD(bool fShowOSD);
	[[nodiscard]] STDMETHODIMP get_SaveFullPath(bool* fSaveFullPath);
	[[nodiscard]] STDMETHODIMP put_SaveFullPath(bool fSaveFullPath);
    [[nodiscard]] constexpr STDMETHODIMP get_SubtitleTiming(int* delay, int* speedmul, int* speeddiv) noexcept;
    [[nodiscard]] STDMETHODIMP put_SubtitleTiming(int delay, int speedmul, int speeddiv);
    [[nodiscard]] constexpr STDMETHODIMP get_MediaFPS(bool* fEnabled, double* fps) noexcept;
    [[nodiscard]] constexpr STDMETHODIMP put_MediaFPS(bool fEnabled, double fps) noexcept;
	[[nodiscard]] constexpr STDMETHODIMP get_ZoomRect(NORMALIZEDRECT* rect) noexcept;
    [[nodiscard]] STDMETHODIMP put_ZoomRect(NORMALIZEDRECT* rect);
	[[nodiscard]] constexpr STDMETHODIMP get_ColorFormat(int* iPosition) noexcept {return E_NOTIMPL;}
    [[nodiscard]] constexpr STDMETHODIMP put_ColorFormat(const int iPosition) noexcept {return E_NOTIMPL;}

    STDMETHOD (get_CachesInfo)(CachesInfo* caches_info);
    STDMETHOD (get_XyFlyWeightInfo)(XyFlyWeightInfo* xy_fw_info);

	[[nodiscard]] STDMETHODIMP HasConfigDialog(int iSelected);
	[[nodiscard]] STDMETHODIMP ShowConfigDialog(int iSelected, HWND hWndParent);

	// settings for the rest are stored in the registry

	[[nodiscard]] STDMETHODIMP IsSubtitleReloaderLocked(bool* fLocked);
    [[nodiscard]] STDMETHODIMP LockSubtitleReloader(bool fLock);
	[[nodiscard]] STDMETHODIMP get_SubtitleReloader(bool* fDisabled);
    [[nodiscard]] STDMETHODIMP put_SubtitleReloader(bool fDisable);

	// the followings need a partial or full reloading of the filter

	[[nodiscard]] STDMETHODIMP get_ExtendPicture(int* horizontal, int* vertical, int* resx2, int* resx2minw, int* resx2minh);
	[[nodiscard]] STDMETHODIMP put_ExtendPicture(int horizontal, int vertical, int resx2, int resx2minw, int resx2minh);
	[[nodiscard]] STDMETHODIMP get_LoadSettings(int* level, bool* fExternalLoad, bool* fWebLoad, bool* fEmbeddedLoad);
	[[nodiscard]] STDMETHODIMP put_LoadSettings(int level, bool fExternalLoad, bool fWebLoad, bool fEmbeddedLoad);
        
	// IDirectVobSub2

	[[nodiscard]] constexpr STDMETHODIMP AdviseSubClock(ISubClock* pSubClock) noexcept;
	[[nodiscard]] STDMETHODIMP_(bool) get_Forced();
	[[nodiscard]] STDMETHODIMP put_Forced(bool fForced);
    [[nodiscard]] STDMETHODIMP get_TextSettings(STSStyle* pDefStyle);
    [[nodiscard]] STDMETHODIMP put_TextSettings(STSStyle* pDefStyle);
	[[nodiscard]] STDMETHODIMP get_AspectRatioSettings(CSimpleTextSubtitle::EPARCompensationType* ePARCompensationType);
	[[nodiscard]] STDMETHODIMP put_AspectRatioSettings(CSimpleTextSubtitle::EPARCompensationType* ePARCompensationType);

	// IFilterVersion
	
	[[nodiscard]] constexpr STDMETHODIMP_(DWORD) GetFilterVersion() noexcept;
};

// For DirectVobSubFilter
class CDirectVobSub: public DirectVobSubImpl
{
public:
    CDirectVobSub(const Option *options, CCritSec * pLock);

protected:
    STDMETHODIMP UpdateRegistry();
};

// For XySubFilter
class CDVS4XySubFilter: public DirectVobSubImpl
{
public:
    CDVS4XySubFilter(const Option *options, CCritSec * pLock);

protected:
    STDMETHODIMP UpdateRegistry();
};
