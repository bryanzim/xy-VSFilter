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

#include "NullRenderers.h"
//#include "MediaTypes.h"
#include "MediaTypeEx.h"
#include "vd.h"
#include "text.h"

// reference time helpers
#define MS2RT(t)        (10000i64 * (t))
#define RT2MS(t)        ((t) / 10000)
#define UNITS_FLOAT     (10000000.0)
// double version of UNITS in BaseClasses reftime

#define LCID_NOSUBTITLES -1

extern void DumpStreamConfig(TCHAR* fn, IAMStreamConfig* pAMVSCCap);
[[nodiscard]] extern int CountPins(IBaseFilter* pBF, int& nIn, int& nOut, int& nInC, int& nOutC);
[[nodiscard]] extern bool IsSplitter(IBaseFilter* pBF, bool fCountConnectedOnly = false);
[[nodiscard]] extern bool IsMultiplexer(IBaseFilter* pBF, bool fCountConnectedOnly = false);
[[nodiscard]] extern bool IsStreamStart(IBaseFilter* pBF);
[[nodiscard]] extern bool IsStreamEnd(IBaseFilter* pBF);
[[nodiscard]] extern bool IsVideoRenderer(IBaseFilter* pBF);
[[nodiscard]] extern bool IsAudioWaveRenderer(IBaseFilter* pBF);
[[nodiscard]] extern IBaseFilter* GetUpStreamFilter(IBaseFilter* pBF, IPin* pInputPin = nullptr);
[[nodiscard]] extern IPin* GetUpStreamPin(IBaseFilter* pBF, IPin* pInputPin = nullptr);
[[nodiscard]] extern IPin* GetFirstPin(IBaseFilter* pBF, PIN_DIRECTION dir = PINDIR_INPUT);
[[nodiscard]] extern IPin* GetFirstDisconnectedPin(IBaseFilter* pBF, PIN_DIRECTION dir);
extern void NukeDownstream(IBaseFilter* pBF, IFilterGraph* pFG);
extern void NukeDownstream(IPin* pPin, IFilterGraph* pFG);
[[nodiscard]] extern IBaseFilter* FindFilter(LPCWSTR clsid, IFilterGraph* pFG);
[[nodiscard]] extern IBaseFilter* FindFilter(const CLSID& clsid, IFilterGraph* pFG);
[[nodiscard]] extern CStringW GetFilterName(IBaseFilter* pBF);
[[nodiscard]] extern CStringW GetPinName(IPin* pPin);
[[nodiscard]] extern IFilterGraph* GetGraphFromFilter(IBaseFilter* pBF);
[[nodiscard]] extern IBaseFilter* GetFilterFromPin(IPin* pPin);
[[nodiscard]] extern IPin* AppendFilter(IPin* pPin, CString DisplayName, IGraphBuilder* pGB);
[[nodiscard]] extern IPin* InsertFilter(IPin* pPin, CString DisplayName, IGraphBuilder* pGB);
extern void ExtractMediaTypes(IPin* pPin, CAtlArray<GUID>& types);
extern void ExtractMediaTypes(IPin* pPin, CAtlList<CMediaType>& mts);
extern void ShowPPage(CString DisplayName, HWND hParentWnd);
extern void ShowPPage(IUnknown* pUnknown, HWND hParentWnd);
[[nodiscard]] extern CLSID GetCLSID(IBaseFilter* pBF);
[[nodiscard]] extern CLSID GetCLSID(IPin* pPin);
[[nodiscard]] extern bool IsCLSIDRegistered(LPCTSTR clsid);
[[nodiscard]] extern bool IsCLSIDRegistered(const CLSID& clsid);
extern void CStringToBin(CString str, CAtlArray<BYTE>& data);
[[nodiscard]] extern CString BinToCString(BYTE* ptr, int len);
typedef enum {CDROM_NotFound, CDROM_Audio, CDROM_VideoCD, CDROM_DVDVideo, CDROM_Unknown} cdrom_t;
[[nodiscard]] extern cdrom_t GetCDROMType(TCHAR drive, CAtlList<CString>& files);
[[nodiscard]] extern CString GetDriveLabel(TCHAR drive);
[[nodiscard]] extern bool GetKeyFrames(CString fn, CUIntArray& kfs);
[[nodiscard]] extern DVD_HMSF_TIMECODE RT2HMSF(REFERENCE_TIME rt, double fps = 0);
[[nodiscard]] extern REFERENCE_TIME HMSF2RT(DVD_HMSF_TIMECODE hmsf, double fps = 0);
extern void memsetd(void* dst, unsigned int c, std::size_t nbytes);
extern void memsetw(void* dst, unsigned short c, std::size_t nbytes);
[[nodiscard]] extern bool ExtractBIH(const AM_MEDIA_TYPE* pmt, BITMAPINFOHEADER* bih);
[[nodiscard]] extern bool ExtractBIH(IMediaSample* pMS, BITMAPINFOHEADER* bih);
[[nodiscard]] extern bool ExtractDim(const AM_MEDIA_TYPE* pmt, int& w, int& h, int& arx, int& ary);
[[nodiscard]] extern bool MakeMPEG2MediaType(CMediaType& mt, BYTE* seqhdr, DWORD len, int w, int h);
[[nodiscard]] extern unsigned __int64 GetFileVersion(LPCTSTR fn);
[[nodiscard]] extern bool CreateFilter(CStringW DisplayName, IBaseFilter** ppBF, CStringW& FriendlyName);
[[nodiscard]] extern IBaseFilter* AppendFilter(IPin* pPin, IMoniker* pMoniker, IGraphBuilder* pGB);
[[nodiscard]] extern CStringW GetFriendlyName(CStringW DisplayName);
[[nodiscard]] extern HRESULT LoadExternalObject(LPCTSTR path, REFCLSID clsid, REFIID iid, void** ppv);
[[nodiscard]] extern HRESULT LoadExternalFilter(LPCTSTR path, REFCLSID clsid, IBaseFilter** ppBF);
[[nodiscard]] extern HRESULT LoadExternalPropertyPage(IPersist* pP, REFCLSID clsid, IPropertyPage** ppPP);
extern void UnloadExternalObjects();
[[nodiscard]] extern CString MakeFullPath(LPCTSTR path);
[[nodiscard]] extern CString GetMediaTypeName(const GUID& guid);
[[nodiscard]] extern GUID GUIDFromCString(CString str);
[[nodiscard]] extern HRESULT GUIDFromCString(CString str, GUID& guid);
[[nodiscard]] extern CString CStringFromGUID(const GUID& guid);
[[nodiscard]] extern CStringW UTF8To16(LPCSTR utf8);
[[nodiscard]] extern CStringA UTF16To8(LPCWSTR utf16);
[[nodiscard]] extern CString ISO6391ToLanguage(LPCSTR code);
[[nodiscard]] extern CString ISO6392ToLanguage(LPCSTR code);
[[nodiscard]] extern LCID    ISO6391ToLcid(LPCSTR code);
[[nodiscard]] extern LCID    ISO6392ToLcid(LPCSTR code);
[[nodiscard]] extern CString ISO6391To6392(LPCSTR code);
[[nodiscard]] extern CString ISO6392To6391(LPCSTR code);
[[nodiscard]] extern CString LanguageToISO6392(LPCTSTR lang);
[[nodiscard]] extern int MakeAACInitData(BYTE* pData, int profile, int freq, int channels);
[[nodiscard]] extern BOOL CFileGetStatus(LPCTSTR lpszFileName, CFileStatus& status);
[[nodiscard]] extern bool DeleteRegKey(LPCTSTR pszKey, LPCTSTR pszSubkey);
[[nodiscard]] extern bool SetRegKeyValue(LPCTSTR pszKey, LPCTSTR pszSubkey, LPCTSTR pszValueName, LPCTSTR pszValue);
[[nodiscard]] extern bool SetRegKeyValue(LPCTSTR pszKey, LPCTSTR pszSubkey, LPCTSTR pszValue);
extern void RegisterSourceFilter(const CLSID& clsid, const GUID& subtype2, LPCTSTR chkbytes, LPCTSTR ext = nullptr, ...);
extern void RegisterSourceFilter(const CLSID& clsid, const GUID& subtype2, const CAtlList<CString>& chkbytes, LPCTSTR ext = nullptr, ...);
extern void UnRegisterSourceFilter(const GUID& subtype);
[[nodiscard]] extern LPCTSTR GetDXVAMode(const GUID* guidDecoder);
extern void DumpBuffer(BYTE* pBuffer, int nSize);
[[nodiscard]] extern CString ReftimeToString(const REFERENCE_TIME& rtVal);
[[nodiscard]] extern CString ReftimeToString2(const REFERENCE_TIME& rtVal);
[[nodiscard]] extern CString DVDtimeToString(const DVD_HMSF_TIMECODE& rtVal, bool bAlwaysShowHours=false);
[[nodiscard]] REFERENCE_TIME StringToReftime(LPCTSTR strVal);
[[nodiscard]] extern COLORREF YCrCbToRGB_Rec601(BYTE Y, BYTE Cr, BYTE Cb);
[[nodiscard]] extern COLORREF YCrCbToRGB_Rec709(BYTE Y, BYTE Cr, BYTE Cb);
[[nodiscard]] extern DWORD	YCrCbToRGB_Rec601(BYTE A, BYTE Y, BYTE Cr, BYTE Cb);
[[nodiscard]] extern DWORD	YCrCbToRGB_Rec709(BYTE A, BYTE Y, BYTE Cr, BYTE Cb);
extern void		TraceFilterInfo(IBaseFilter* pBF);
extern void		TracePinInfo(IPin* pPin);
extern void		SetThreadName( DWORD dwThreadID, LPCSTR szThreadName);
extern void		HexDump(CString fName, BYTE* buf, int size);
[[nodiscard]] extern DWORD	GetDefChannelMask(WORD nChannels);
extern void		CorrectComboListWidth(CComboBox& m_pComboBox);

class CPinInfo : public PIN_INFO
{
public:
	explicit CPinInfo() {pFilter = nullptr;}
	~CPinInfo() {if(pFilter)
	{
	    pFilter->Release();
	}}
};

class CFilterInfo : public FILTER_INFO
{
public:
	explicit CFilterInfo() {pGraph = nullptr;}
	~CFilterInfo() {if(pGraph)
	{
	    pGraph->Release();
	}}
};

#define BeginEnumFilters(pFilterGraph, pEnumFilters, pBaseFilter) \
	{CComPtr<IEnumFilters> pEnumFilters; \
	if(pFilterGraph && SUCCEEDED(pFilterGraph->EnumFilters(&pEnumFilters))) \
	{ \
		for(CComPtr<IBaseFilter> pBaseFilter; S_OK == pEnumFilters->Next(1, &pBaseFilter, 0); pBaseFilter = nullptr) \
		{ \

#define EndEnumFilters }}}

#define BeginEnumCachedFilters(pGraphConfig, pEnumFilters, pBaseFilter) \
	{CComPtr<IEnumFilters> pEnumFilters; \
	if(pGraphConfig && SUCCEEDED(pGraphConfig->EnumCacheFilter(&pEnumFilters))) \
	{ \
		for(CComPtr<IBaseFilter> pBaseFilter; S_OK == pEnumFilters->Next(1, &pBaseFilter, 0); pBaseFilter = nullptr) \
		{ \

#define EndEnumCachedFilters }}}

#define BeginEnumPins(pBaseFilter, pEnumPins, pPin) \
	{CComPtr<IEnumPins> pEnumPins; \
	if(pBaseFilter && SUCCEEDED(pBaseFilter->EnumPins(&pEnumPins))) \
	{ \
		for(CComPtr<IPin> pPin; S_OK == pEnumPins->Next(1, &pPin, 0); pPin = nullptr) \
		{ \

#define EndEnumPins }}}

#define BeginEnumMediaTypes(pPin, pEnumMediaTypes, pMediaType) \
	{CComPtr<IEnumMediaTypes> pEnumMediaTypes; \
	if(pPin && SUCCEEDED(pPin->EnumMediaTypes(&pEnumMediaTypes))) \
	{ \
		AM_MEDIA_TYPE* pMediaType = nullptr; \
		for(; S_OK == pEnumMediaTypes->Next(1, &pMediaType, nullptr); DeleteMediaType(pMediaType), pMediaType = nullptr) \
		{ \

#define EndEnumMediaTypes(pMediaType) } if(pMediaType) DeleteMediaType(pMediaType); }}

#define BeginEnumSysDev(clsid, pMoniker) \
	{CComPtr<ICreateDevEnum> pDevEnum4$##clsid; \
	pDevEnum4$##clsid.CoCreateInstance(CLSID_SystemDeviceEnum); \
	CComPtr<IEnumMoniker> pClassEnum4$##clsid; \
	if(SUCCEEDED(pDevEnum4$##clsid->CreateClassEnumerator(clsid, &pClassEnum4$##clsid, 0)) \
	&& pClassEnum4$##clsid) \
	{ \
		for(CComPtr<IMoniker> pMoniker; pClassEnum4$##clsid->Next(1, &pMoniker, 0) == S_OK; pMoniker = nullptr) \
		{ \

#define EndEnumSysDev }}}

#define QI(i) (riid == __uuidof(i)) ? GetInterface((i*)this, ppv) :
#define QI2(i) (riid == IID_##i) ? GetInterface((i*)this, ppv) :

template <typename T> __inline void INITDDSTRUCT(T& dd)
{
    SecureZeroMemory(&dd, sizeof(dd));
    dd.dwSize = sizeof(dd);
}

#define countof(array) (sizeof(array)/sizeof(array[0U]))

template <class T>
static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT* phr)
{
	*phr = S_OK;
    CUnknown* punk{new T(lpunk, phr)};
    if(punk == nullptr)
    {
        *phr = E_OUTOFMEMORY;
    }
	return punk;
}
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=nullptr; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=nullptr; } }
[[nodiscard]] inline int LNKO(int a, int b)

{
	if(a == 0 || b == 0) {
		return(1);
	}
	while(a != b) {
		if(a < b) {
			b -= a;
		} else if(a > b) {
			a -= b;
		}
	}
	return(a);
}
