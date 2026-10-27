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

#include "..\subpic\ISubPic.h"

//
// CSubtitleInputPinHelper
//
class CSubtitleInputPinHelper
{
public:
    virtual ~CSubtitleInputPinHelper(){}
    STDMETHOD (NewSegment)(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) = 0;
    STDMETHOD (Receive)(IMediaSample* pSample) = 0;
    STDMETHOD (EndOfStream)(void) = 0;

    STDMETHOD_(ISubStream*, GetSubStream) () = 0;
};

//
// CSubtitleInputPinHelperImpl
//

class CSubtitleInputPinHelperImpl: public CSubtitleInputPinHelper
{
public:
    explicit CSubtitleInputPinHelperImpl(CComPtr<ISubStream> pSubStream) : m_pSubStream(pSubStream){}

    [[nodiscard]] STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
    [[nodiscard]] STDMETHODIMP_(ISubStream*) GetSubStream() { return m_pSubStream; }
    [[nodiscard]] STDMETHODIMP EndOfStream() { return S_FALSE; }
protected:
    CComPtr<ISubStream> m_pSubStream;
    REFERENCE_TIME m_tStart, m_tStop;
    double m_dRate;
};

//
// CSubtitleInputPin
//
class CSubtitleInputPin : public CBaseInputPin
{
protected:
    CCritSec* m_pSubLock;
    CSubtitleInputPinHelper *m_helper;
protected:
    virtual void AddSubStream(ISubStream* pSubStream) = 0;
    virtual void RemoveSubStream(ISubStream* pSubStream) = 0;
    virtual void InvalidateSubtitle(REFERENCE_TIME rtStart, ISubStream* pSubStream) = 0;

    STDMETHOD_(CSubtitleInputPinHelper*, CreateHelper) ( const CMediaType& mt, IPin* pReceivePin );
    [[nodiscard]] bool IsHdmvSub(const CMediaType* pmt);
public:
    CSubtitleInputPin(CBaseFilter* pFilter, CCritSec* pLock, CCritSec* pSubLock, HRESULT* phr);

    [[nodiscard]] HRESULT CheckMediaType(const CMediaType* pmt);
    [[nodiscard]] HRESULT CompleteConnect(IPin* pReceivePin);
    HRESULT BreakConnect();
    [[nodiscard]] STDMETHODIMP ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt);
    [[nodiscard]] STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
    [[nodiscard]] STDMETHODIMP Receive(IMediaSample* pSample);
    [[nodiscard]] STDMETHODIMP EndOfStream(void);

    [[nodiscard]] ISubStream* GetSubStream();
};
