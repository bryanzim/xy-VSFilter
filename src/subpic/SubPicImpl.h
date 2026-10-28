/*
 *  $Id: SubPicImpl.h 2786 2010-12-17 16:42:55Z XhmikosR $
 *
 *  (C) 2003-2006 Gabest
 *  (C) 2006-2010 see AUTHORS
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

#include "ISubPic.h"

class CSubPicImplHelper
{
protected:
    REFERENCE_TIME m_rtStart, m_rtStop;
    REFERENCE_TIME m_rtSegmentStart, m_rtSegmentStop;

    CRect m_rcDirty;

    CSize m_maxsize, m_size;
    CRect m_vidrect;
	CSize	m_VirtualTextureSize;
	CPoint	m_VirtualTextureTopLeft;

	/*

	                          Texture
	        +-------+---------------------------------+
	        |       .                                 |   .
	        |       .             m_maxsize           |       .
	 TextureTopLeft .<=============================== |======>    .           Video
	        | . . . +-------------------------------- | -----+       +-----------------------------------+
	        |       |                         .       |      |       |  m_vidrect                        |
	        |       |                         .       |      |       |                                   |
	        |       |                         .       |      |       |                                   |
	        |       |        +-----------+    .       |      |       |                                   |
	        |       |        | m_rcDirty |    .       |      |       |                                   |
	        |       |        |           |    .       |      |       |                                   |
	        |       |        +-----------+    .       |      |       |                                   |
	        |       +-------------------------------- | -----+       |                                   |
	        |                    m_size               |              |                                   |
	        |       <=========================>       |              |                                   |
	        |                                         |              |                                   |
	        |                                         |              +-----------------------------------+
	        |                                         |          .
	        |                                         |      .
	        |                                         |   .
	        +-----------------------------------------+
	                   m_VirtualTextureSize
	        <=========================================>

	*/

    explicit CSubPicImplHelper();
    
    // ISubPic

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetStart() const;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetStop() const;
    DECLSPEC_NOTHROW STDMETHODIMP_(void) SetStart(REFERENCE_TIME rtStart);
    DECLSPEC_NOTHROW STDMETHODIMP_(void) SetStop(REFERENCE_TIME rtStop);

    //DECLSPEC_NOTHROW STDMETHODIMP GetDesc(SubPicDesc& spd) const = 0;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CopyTo(ISubPic* pSubPic);

    //DECLSPEC_NOTHROW STDMETHODIMP ClearDirtyRect(DWORD color) = 0;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetDirtyRect(RECT* pDirtyRect) const;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetDirtyRect(RECT* pDirtyRect /*[in]*/);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetMaxSize(SIZE* pMaxSize) const;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetSize(SIZE size, RECT vidrect);

    //DECLSPEC_NOTHROW STDMETHODIMP Lock(SubPicDesc& spd) = 0;
    //DECLSPEC_NOTHROW STDMETHODIMP Unlock(RECT* pDirtyRect /*[in]*/) = 0;

    //DECLSPEC_NOTHROW STDMETHODIMP AlphaBlt(const RECT* pSrc, const RECT* pDst, SubPicDesc* pTarget) = 0;

	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetVirtualTextureSize (SIZE pSize, POINT pTopLeft);
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetSourceAndDest(SIZE* pSize, RECT* pRcSource, RECT* pRcDest);

	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetSegmentStart();
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetSegmentStop();
	DECLSPEC_NOTHROW STDMETHODIMP_(void) SetSegmentStart(REFERENCE_TIME rtStart);
	DECLSPEC_NOTHROW STDMETHODIMP_(void) SetSegmentStop(REFERENCE_TIME rtStop);
};

class CSubPicImpl : public CUnknown, public CSubPicImplHelper, public ISubPic
{
public:
    explicit CSubPicImpl();

    DECLARE_IUNKNOWN
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // ISubPic

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetStart() const
    {
        return CSubPicImplHelper::GetStart();
    }
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetStop() const
    {
        return CSubPicImplHelper::GetStop();
    }
    DECLSPEC_NOTHROW STDMETHODIMP_(void) SetStart(REFERENCE_TIME rtStart)
    {
        return CSubPicImplHelper::SetStart(rtStart);
    }
    DECLSPEC_NOTHROW STDMETHODIMP_(void) SetStop(REFERENCE_TIME rtStop)
    {
        return CSubPicImplHelper::SetStop(rtStop);
    }

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetDesc(SubPicDesc& spd) const = 0;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CopyTo(ISubPic* pSubPic)
    {
        return CSubPicImplHelper::CopyTo(pSubPic);
    }

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP ClearDirtyRect(DWORD color) = 0;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetDirtyRect(RECT* pDirtyRect) const
    {
        return CSubPicImplHelper::GetDirtyRect(pDirtyRect);
    }
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetDirtyRect(RECT* pDirtyRect /*[in]*/)
    {
        return CSubPicImplHelper::SetDirtyRect(pDirtyRect);
    }

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetMaxSize(SIZE* pMaxSize) const
    {
        return CSubPicImplHelper::GetMaxSize(pMaxSize);
    }
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetSize(SIZE size, RECT vidrect)
    {
        return CSubPicImplHelper::SetSize(size, vidrect);
    }

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Lock(SubPicDesc& spd) = 0;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Unlock(RECT* pDirtyRect /*[in]*/) = 0;

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP AlphaBlt(const RECT* pSrc, const RECT* pDst, SubPicDesc* pTarget) = 0;

	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetVirtualTextureSize (SIZE pSize, POINT pTopLeft)
    {
        return CSubPicImplHelper::SetVirtualTextureSize(pSize, pTopLeft);
    }
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetSourceAndDest(SIZE* pSize, RECT* pRcSource, RECT* pRcDest)
    {
        return CSubPicImplHelper::GetSourceAndDest(pSize, pRcSource, pRcDest);
    }

	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetSegmentStart()
    {
        return CSubPicImplHelper::GetSegmentStart();
    }
	[[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetSegmentStop()
    {
        return CSubPicImplHelper::GetSegmentStop();
    }
	DECLSPEC_NOTHROW STDMETHODIMP_(void) SetSegmentStart(REFERENCE_TIME rtStart)
    {
        return CSubPicImplHelper::SetSegmentStart(rtStart);
    }
	DECLSPEC_NOTHROW STDMETHODIMP_(void) SetSegmentStop(REFERENCE_TIME rtStop)
    {
        return CSubPicImplHelper::SetSegmentStop(rtStop);
    }
};

class CSubPicExImpl : public CUnknown, public CSubPicImplHelper, public ISubPicEx
{
protected:
    CAtlList<CRect> m_rectListDirty;

public:	
    explicit CSubPicExImpl();

    DECLARE_IUNKNOWN
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);
    
    // ISubPic

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetStart() const
    {
        return CSubPicImplHelper::GetStart();
    }
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetStop() const
    {
        return CSubPicImplHelper::GetStop();
    }
    DECLSPEC_NOTHROW STDMETHODIMP_(void) SetStart(REFERENCE_TIME rtStart)
    {
        return CSubPicImplHelper::SetStart(rtStart);
    }
    DECLSPEC_NOTHROW STDMETHODIMP_(void) SetStop(REFERENCE_TIME rtStop)
    {
        return CSubPicImplHelper::SetStop(rtStop);
    }

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetDesc(SubPicDesc& spd) const = 0;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CopyTo(ISubPic* pSubPic)
    {
        return CSubPicImplHelper::CopyTo(pSubPic);
    }

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP ClearDirtyRect(DWORD color) = 0;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetDirtyRect(RECT* pDirtyRect) const
    {
        return CSubPicImplHelper::GetDirtyRect(pDirtyRect);
    }
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetDirtyRect(RECT* pDirtyRect /*[in]*/);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetMaxSize(SIZE* pMaxSize) const
    {
        return CSubPicImplHelper::GetMaxSize(pMaxSize);
    }
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetSize(SIZE size, RECT vidrect)
    {
        return CSubPicImplHelper::SetSize(size, vidrect);
    }

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Lock(SubPicDesc& spd) = 0;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Unlock(RECT* pDirtyRect /*[in]*/);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP AlphaBlt(const RECT* pSrc, const RECT* pDst, SubPicDesc* pTarget) = 0;

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetVirtualTextureSize (SIZE pSize, POINT pTopLeft)
    {
        return CSubPicImplHelper::SetVirtualTextureSize(pSize, pTopLeft);
    }
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetSourceAndDest(SIZE* pSize, RECT* pRcSource, RECT* pRcDest)
    {
        return CSubPicImplHelper::GetSourceAndDest(pSize, pRcSource, pRcDest);
    }

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetSegmentStart()
    {
        return CSubPicImplHelper::GetSegmentStart();
    }
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(REFERENCE_TIME) GetSegmentStop()
    {
        return CSubPicImplHelper::GetSegmentStop();
    }
    DECLSPEC_NOTHROW STDMETHODIMP_(void) SetSegmentStart(REFERENCE_TIME rtStart)
    {
        return CSubPicImplHelper::SetSegmentStart(rtStart);
    }
    DECLSPEC_NOTHROW STDMETHODIMP_(void) SetSegmentStop(REFERENCE_TIME rtStop)
    {
        return CSubPicImplHelper::SetSegmentStop(rtStop);
    }

    // ISubPicEx
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP CopyTo(ISubPicEx* pSubPicEx);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetDirtyRects(CAtlList<const CRect>& dirtyRectList /*[out]*/) const;
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetDirtyRectEx(CAtlList<CRect>* dirtyRectList /*[in]*/);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP UnlockEx(CAtlList<CRect>* dirtyRectList /*[in]*/) = 0;
};

class CSubPicAllocatorImpl : public CUnknown, public ISubPicAllocator
{
    CComPtr<ISubPic> m_pStatic;

private:
    CSize m_cursize;
    CRect m_curvidrect;
    bool m_fDynamicWriteOnly;

    [[nodiscard]] virtual bool Alloc(bool fStatic, ISubPic** ppSubPic) = 0;
protected:
    bool m_fPow2Textures;
public:
    CSubPicAllocatorImpl(SIZE cursize, bool fDynamicWriteOnly, bool fPow2Textures);

    DECLARE_IUNKNOWN
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // ISubPicAllocator

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetCurSize(SIZE cursize);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetCurVidRect(RECT curvidrect);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetStatic(ISubPic** ppSubPic);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP AllocDynamic(ISubPic** ppSubPic);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) IsDynamicWriteOnly();
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP ChangeDevice(IUnknown* pDev);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetMaxTextureSize(SIZE MaxTextureSize) {
        return E_NOTIMPL;
    }
};

class CSubPicExAllocatorImpl : public CUnknown, public ISubPicExAllocator
{
private:
    CSize m_cursize;
    CRect m_curvidrect;
    bool m_fDynamicWriteOnly;
    
    [[nodiscard]] bool Alloc(bool fStatic, ISubPic** ppSubPic);
    
    [[nodiscard]] virtual bool AllocEx(bool fStatic, ISubPicEx** ppSubPic) = 0;
protected:
    CComPtr<ISubPicEx> m_pStatic;
    bool m_fPow2Textures;
public:
    CSubPicExAllocatorImpl(SIZE cursize, bool fDynamicWriteOnly, bool fPow2Textures);

    DECLARE_IUNKNOWN
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // ISubPicAllocator

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetCurSize(SIZE cursize);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetCurVidRect(RECT curvidrect);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetStatic(ISubPic** ppSubPic);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP AllocDynamic(ISubPic** ppSubPic);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) IsDynamicWriteOnly();
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP ChangeDevice(IUnknown* pDev);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP SetMaxTextureSize(SIZE MaxTextureSize) {
        return E_NOTIMPL;
    }

    // ISubPicExAllocator
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(bool) IsSpdColorTypeSupported(int type);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP_(int) SetSpdColorType(int color_type);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetStaticEx(ISubPicEx** ppSubPic);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP AllocDynamicEx(ISubPicEx** ppSubPic);
};
