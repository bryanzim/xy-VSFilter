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

#include <atlsync.h>
#include "DirectVobSub.h"
#include "../basevideofilter/BaseVideoFilter.h"
#include "../../../subtitles/VobSubFile.h"
#include "../../../subtitles/RTS.h"
#include "../../../subtitles/SSF.h"
#include "../../../subpic/ISimpleSubPic.h"


/* This is for graphedit */

class __declspec(uuid("93A22E7A-5091-45ef-BA61-6DA26156A5D0")) CDirectVobSubFilter
	: public CBaseVideoFilter
	, public CDirectVobSub
	, public ISpecifyPropertyPages
	, public IAMStreamSelect
	, public CAMThread
{
    friend class CTextInputPin;

	CComPtr<ISimpleSubPicProvider> m_simple_provider;

	void InitSubPicQueue();
	SubPicDesc m_spd;

	[[nodiscard]] bool AdjustFrameSize(CSize& s);

    [[nodiscard]] HRESULT TryNotCopy( IMediaSample* pIn, const CMediaType& mt, const BITMAPINFOHEADER& bihIn );

    ColorConvTable::YuvMatrixType m_video_yuv_matrix_decided_by_sub;
    ColorConvTable::YuvRangeType m_video_yuv_range_decided_by_sub;

    void SetYuvMatrix();
protected:
	void GetOutputSize(int& w, int& h, int& arx, int& ary);
	[[nodiscard]] HRESULT Transform(IMediaSample* pIn);    
    [[nodiscard]] HRESULT GetIsEmbeddedSubStream(int iSelected, bool *fIsEmbedded);

    void UpdateLanguageCount();
public:
    CDirectVobSubFilter(LPUNKNOWN punk, HRESULT* phr, const GUID& clsid = __uuidof(CDirectVobSubFilter));
	virtual ~CDirectVobSubFilter();

    DECLARE_IUNKNOWN
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // CBaseFilter

	[[nodiscard]] CBasePin* GetPin(int n);
	[[nodiscard]] int GetPinCount();

	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP JoinFilterGraph(IFilterGraph* pGraph, LPCWSTR pName);
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP QueryFilterInfo(FILTER_INFO* pInfo);

    // CTransformFilter
	HRESULT SetMediaType(PIN_DIRECTION dir, const CMediaType* pMediaType),
			CheckConnect(PIN_DIRECTION dir, IPin* pPin),
			CompleteConnect(PIN_DIRECTION dir, IPin* pReceivePin),
			BreakConnect(PIN_DIRECTION dir),
			StartStreaming(), 
			StopStreaming(),
			NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

    CAtlArray<CTextInputPin*> m_pTextInput;

    //CBaseVideoFilter
    void GetInputColorspaces(ColorSpaceId *preferredOrder, UINT *count);
    void GetOutputColorspaces(ColorSpaceId *preferredOrder, UINT *count);

    // XyOptionsImpl
    [[nodiscard]] virtual HRESULT OnOptionChanged(unsigned field);
    [[nodiscard]] virtual HRESULT DoGetField(unsigned field, void *value);

    // IXyOptions
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetBool     (unsigned field, bool      value);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetInt      (unsigned field, int       value);

    // DirectVobSubImpl
    [[nodiscard]] HRESULT GetCurStyles( SubStyle sub_style[], int count );
    [[nodiscard]] HRESULT SetCurStyles( const SubStyle sub_style[], int count );

    // IDirectVobSub
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP get_LanguageName(int iLanguage, WCHAR** ppName);
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP put_PreBuffering(bool fDoPreBuffering);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP get_CachesInfo(CachesInfo* caches_info);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP get_XyFlyWeightInfo(XyFlyWeightInfo* xy_fw_info);

	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP HasConfigDialog(int iSelected);
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP ShowConfigDialog(int iSelected, HWND hWndParent);

	// IDirectVobSub2
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP put_TextSettings(STSStyle* pDefStyle);

    // ISpecifyPropertyPages
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetPages(CAUUID* pPages);

	// IAMStreamSelect
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Count(DWORD* pcStreams); 
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Enable(long lIndex, DWORD dwFlags); 
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Info(long lIndex, AM_MEDIA_TYPE** ppmt, DWORD* pdwFlags, LCID* plcid, DWORD* pdwGroup, WCHAR** ppszName, IUnknown** ppObject, IUnknown** ppUnk);  

    // CPersistStream
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetClassID(CLSID* pClsid);

protected:
    //OSD
    void ZeroObj4OSD();
    void DeleteObj4OSD();
    void InitObj4OSD();
    void PrintMessages(BYTE* pOut);

    HDC m_hdc;
    HBITMAP m_hbm;
    HFONT m_hfont;
    
protected:
	HRESULT ChangeMediaType(int iPosition);

    /* ResX2 */
	CAutoVectorPtr<BYTE> m_pTempPicBuff;
	[[nodiscard]] HRESULT Copy(BYTE* pSub, BYTE* pIn, CSize sub, CSize in, int bpp, const GUID& subtype, DWORD black);
	// segment start time, absolute time
	CRefTime m_tPrev;
	REFERENCE_TIME CalcCurrentTime();

	double m_fps;

	// 3.x- versions of microsoft's mpeg4 codec output flipped image
	bool m_fMSMpeg4Fix;

	// don't set the "hide subtitles" stream until we are finished with loading
	bool m_fLoading;

	[[nodiscard]] bool Open();

	[[nodiscard]] int FindPreferedLanguage(bool fHideToo = true);
	void UpdatePreferedLanguages(CString lang);

	CCritSec m_csSubLock;

	CInterfaceList<ISubStream> m_pSubStreams;
    CAtlList<bool> m_fIsSubStreamEmbeded;

	DWORD_PTR m_nSubtitleId;
	void UpdateSubtitle(bool fApplyDefStyle = true);
	void SetSubtitle(ISubStream* pSubStream, bool fApplyDefStyle = true);
	void InvalidateSubtitle(REFERENCE_TIME rtInvalidate = -1, DWORD_PTR nSubtitleId = -1);

	// the text input pin is using these
	void AddSubStream(ISubStream* pSubStream);
	void RemoveSubStream(ISubStream* pSubStream);
	void Post_EC_OLE_EVENT(CString str, DWORD_PTR nSubtitleId = -1);

private:
	class CFileReloaderData
	{
	public:
		ATL::CEvent ThreadStartedEvent, EndThreadEvent, RefreshEvent;
		CAtlList<CString> files;
		CAtlArray<CTime> mtime;
	} m_frd;

	void SetupFRD(CStringArray& paths, CAtlArray<HANDLE>& handles);
	DWORD ThreadProc();
private:
	HANDLE m_hSystrayThread;
	SystrayIconData m_tbid;

	VIDEOINFOHEADER2 m_CurrentVIH2;

    bool m_bExternalSubtitle{false};
	std::vector<ISubStream*> m_ExternalSubstreams;
};

/* The "auto-loading" version */

class __declspec(uuid("9852A670-F845-491b-9BE6-EBD841B8A613")) CDirectVobSubFilter2 : public CDirectVobSubFilter
{
    [[nodiscard]] bool IsAppBlackListed();
	[[nodiscard]] bool ShouldWeAutoload(IFilterGraph* pGraph);
	void GetRidOfInternalScriptRenderer();

public:
    CDirectVobSubFilter2(LPUNKNOWN punk, HRESULT* phr, const GUID& clsid = __uuidof(CDirectVobSubFilter2));

	[[nodiscard]] HRESULT CheckConnect(PIN_DIRECTION dir, IPin* pPin);
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP JoinFilterGraph(IFilterGraph* pGraph, LPCWSTR pName);
    [[nodiscard]] HRESULT CheckInputType(const CMediaType* mtIn);
};

