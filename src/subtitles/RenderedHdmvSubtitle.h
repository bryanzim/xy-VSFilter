/*
 * $Id: RenderedHdmvSubtitle.h 2804 2010-12-28 13:48:24Z aleksoid $
 *
 * (C) 2006-2010 see AUTHORS
 *
 * This file is part of mplayerc.
 *
 * Mplayerc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mplayerc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#pragma once

#include "Rasterizer.h"
#include "../SubPic/SubPicProviderImpl.h"
#include "HdmvSub.h"
#include "BaseSub.h"


class __declspec(uuid("FCA68599-C83E-4ea5-94A3-C2E1B0E326B9"))
	CRenderedHdmvSubtitle : public CSubPicProviderImpl, public ISubStream
{
public:
	CRenderedHdmvSubtitle(CCritSec* pLock, SUBTITLE_TYPE nType, const CString& name, LCID lcid);
	~CRenderedHdmvSubtitle(void);

	[[nodiscard]] DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	// ISubPicProvider
	[[nodiscard]] STDMETHODIMP_(POSITION) GetStartPosition(REFERENCE_TIME rt, double fps);
	[[nodiscard]] STDMETHODIMP_(POSITION) GetNext(POSITION pos);
	[[nodiscard]] STDMETHODIMP_(REFERENCE_TIME) GetStart(POSITION pos, double fps);
	[[nodiscard]] STDMETHODIMP_(REFERENCE_TIME) GetStop(POSITION pos, double fps);
	[[nodiscard]] STDMETHODIMP_(bool) IsAnimated(POSITION pos);
	[[nodiscard]] STDMETHODIMP Render(SubPicDesc& spd, REFERENCE_TIME rt, double fps, RECT& bbox);
	[[nodiscard]] STDMETHODIMP GetTextureSize (POSITION pos, SIZE& MaxTextureSize, SIZE& VirtualSize, POINT& VirtualTopLeft);

    // ISubPicProviderEx
    [[nodiscard]] STDMETHODIMP_(bool) IsColorTypeSupported(int type);

	// IPersist
	[[nodiscard]] STDMETHODIMP GetClassID(CLSID* pClassID);

	// ISubStream
	[[nodiscard]] STDMETHODIMP_(int) GetStreamCount();
	[[nodiscard]] STDMETHODIMP GetStreamInfo(int i, WCHAR** ppName, LCID* pLCID);
	[[nodiscard]] STDMETHODIMP_(int) GetStream();
	[[nodiscard]] STDMETHODIMP SetStream(int iStream);
	STDMETHODIMP Reload();

	[[nodiscard]] HRESULT ParseSample (IMediaSample* pSample);
    [[nodiscard]] HRESULT NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
    void EndOfStream();

    [[nodiscard]] HRESULT SetYuvType(CBaseSub::ColorType colorType, CBaseSub::YuvRangeType yuvRangeType);
private :
	CString			m_name;
	LCID			m_lcid;
	REFERENCE_TIME	m_rtStart;

	CBaseSub*		m_pSub;
	CCritSec		m_csCritSec;
};
