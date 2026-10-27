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

struct OutputFormatBase
{
    const GUID* subtype; 
    WORD biPlanes, biBitCount; 
    DWORD biCompression;
};

typedef int ColorSpaceId;
enum ColorSpaceDir
{
    INPUT_COLOR_SPACE = 0,
    OUTPUT_COLOR_SPACE = 1
};
[[nodiscard]] CString GetColorSpaceName(ColorSpaceId colorSpace, ColorSpaceDir inputOrOutput);
UINT GetOutputColorSpaceNumber();
UINT GetInputColorSpaceNumber();
[[nodiscard]] ColorSpaceId Subtype2OutputColorSpaceId( const GUID& subtype, ColorSpaceId startPos =0 );

class CBaseVideoFilter : public CTransformFilter
{
private:
    HRESULT Receive(IMediaSample* pIn);
	[[nodiscard]] HRESULT ReconnectOutput(int w, int h);

	// these are private for a reason, don't bother them
	DWORD m_win, m_hin, m_arxin, m_aryin;
	DWORD m_wout, m_hout, m_arxout, m_aryout;

	long m_cBuffers;

protected:
	CCritSec m_csReceive;
    bool m_donot_follow_upstream_preferred_order;

	int m_w, m_h, m_arx, m_ary;

    static const int MAX_COLOR_SPACE_NUM{256};
    const GUID* m_inputFmt[MAX_COLOR_SPACE_NUM];
    int m_inputFmtCount;

    const OutputFormatBase* m_outputFmt[MAX_COLOR_SPACE_NUM];
    int m_outputFmtCount;

	[[nodiscard]] HRESULT GetDeliveryBuffer(int w, int h, IMediaSample** ppOut);
	[[nodiscard]] HRESULT CopyBuffer(BYTE* pOut, BYTE* pIn, int w, int h, int pitchIn, const GUID& subtype, bool fInterlaced = false);
	[[nodiscard]] HRESULT CopyBuffer(BYTE* pOut, BYTE** ppIn, int w, int h, int pitchIn, const GUID& subtype, bool fInterlaced = false);

	virtual void GetOutputSize(int& w, int& h, int& arx, int& ary) {}
	[[nodiscard]] virtual HRESULT Transform(IMediaSample* pIn) = 0;
	[[nodiscard]] constexpr virtual HRESULT IsVideoInterlaced() noexcept {return false;}

    void InitInputColorSpaces();
    void InitOutputColorSpaces();
    virtual void GetInputColorspaces(ColorSpaceId *preferredOrder, UINT *count);
    virtual void GetOutputColorspaces(ColorSpaceId *preferredOrder, UINT *count);
    [[nodiscard]] HRESULT GetUpstreamOutputPriority(int *priorities, UINT count);
    [[nodiscard]] HRESULT CombineOutputPriority( ColorSpaceId *preferredOrder, UINT *count );
    [[nodiscard]] int GetInputSubtypePosition(const GUID& subtype);
    [[nodiscard]] int GetOutputSubtypePosition( const GUID& subtype, int startPos =0 );    
public:
	CBaseVideoFilter(TCHAR* pName, LPUNKNOWN lpunk, HRESULT* phr, REFCLSID clsid, long cBuffers = 1);
	constexpr virtual ~CBaseVideoFilter() noexcept;

	[[nodiscard]] int GetPinCount();
	[[nodiscard]] CBasePin* GetPin(int n);

    [[nodiscard]] HRESULT CheckInputType(const CMediaType* mtIn);
	[[nodiscard]] HRESULT CheckOutputType(const CMediaType& mtOut);
    [[nodiscard]] HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut);
    [[nodiscard]] HRESULT CheckReconnect(const CMediaType* mtIn, const CMediaType* mtOut);
    [[nodiscard]] HRESULT DecideBufferSize(IMemAllocator* pAllocator, ALLOCATOR_PROPERTIES* pProperties);
    [[nodiscard]] HRESULT GetMediaType(int iPosition, CMediaType* pMediaType);    
	[[nodiscard]] HRESULT SetMediaType(PIN_DIRECTION dir, const CMediaType* pmt);
    
    friend class CBaseVideoOutputPin;
};

class CBaseVideoInputAllocator : public CMemAllocator
{
	CMediaType m_mt;

public:
	explicit CBaseVideoInputAllocator(HRESULT* phr);
	void SetMediaType(const CMediaType& mt);
	[[nodiscard]] STDMETHODIMP GetBuffer(IMediaSample** ppBuffer, REFERENCE_TIME* pStartTime, REFERENCE_TIME* pEndTime, DWORD dwFlags);
};

class CBaseVideoInputPin : public CTransformInputPin
{
	CBaseVideoInputAllocator* m_pAllocator;

public:
	CBaseVideoInputPin(TCHAR* pObjectName, CBaseVideoFilter* pFilter, HRESULT* phr, LPCWSTR pName);
	constexpr ~CBaseVideoInputPin() noexcept;

	[[nodiscard]] STDMETHODIMP GetAllocator(IMemAllocator** ppAllocator);
	[[nodiscard]] STDMETHODIMP ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt);
};

class CBaseVideoOutputPin : public CTransformOutputPin
{
public:
	CBaseVideoOutputPin(TCHAR* pObjectName, CBaseVideoFilter* pFilter, HRESULT* phr, LPCWSTR pName);

    [[nodiscard]] HRESULT CheckMediaType(const CMediaType* mtOut);
    //STDMETHODIMP EnumMediaTypes( IEnumMediaTypes **ppEnum );

    [[nodiscard]] HRESULT CompleteConnect(IPin *pReceivePin);
protected:
    CBaseVideoFilter * m_pFilter;
};
