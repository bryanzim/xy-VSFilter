#ifndef __XY_BITMAP_674EB983_2A49_432D_A750_62C3B3E9DA67_H__
#define __XY_BITMAP_674EB983_2A49_432D_A750_62C3B3E9DA67_H__

#include <atltypes.h>
#include <boost/shared_ptr.hpp>
#include "XySubRenderIntf.h"

class XyBitmap
{
public:
    enum MemLayout { PACK, PLANNA, RESERVED };

    int type;
    
    int x, y, w, h;
    int pitch, bpp;
    void* bits;
    BYTE* plans[4U];
public:
    explicit XyBitmap()
    {
        x = y = w = h = 0;
        type = 0;
        pitch = 0;
        bits = NULL;
        plans[0U] = plans[1U] = plans[2U] = plans[3U] = NULL;
    }
    ~XyBitmap();

    [[nodiscard]] static XyBitmap *CreateBitmap(const CRect& target_rect, MemLayout layout);

    static void FlipAlphaValue( LPVOID pixels, int w, int h, int pitch );
    static void AlphaBltPack(SubPicDesc& spd, POINT pos, SIZE size, LPCVOID pixels, int pitch);
    static void AlphaBltPlannar(SubPicDesc& spd, POINT pos, SIZE size, const XyPlannerFormatExtra& pixels, int pitch);

    static void BltPack( SubPicDesc& spd, POINT pos, SIZE size, LPCVOID pixels, int pitch );
private:
    static void ClearBitmap(XyBitmap *bitmap);
};

class XySubRenderFrame: public CUnknown, public IXySubRenderFrame
{
public:
    DECLARE_IUNKNOWN;
    [[nodiscard]] STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    [[nodiscard]] STDMETHODIMP GetOutputRect(RECT *outputRect);
    [[nodiscard]] STDMETHODIMP GetClipRect(RECT *clipRect);
    [[nodiscard]] STDMETHODIMP GetXyColorSpace(int *xyColorSpace);
    [[nodiscard]] STDMETHODIMP GetBitmapCount(int *count);
    [[nodiscard]] STDMETHODIMP GetBitmap(int index, ULONGLONG *id, POINT *position, SIZE *size, LPCVOID *pixels, int *pitch);
    [[nodiscard]] STDMETHODIMP GetBitmapExtra(int index, LPVOID extra_info);
public:
    explicit XySubRenderFrame();
    ~XySubRenderFrame();

    typedef ::boost::shared_ptr<XyBitmap> SharedBitmap;

    void MoveTo(int x, int y);
public:
    CAtlArray<SharedBitmap> m_bitmaps;
    CAtlArray<int> m_bitmap_ids;
    CRect m_output_rect;
    CRect m_clip_rect;
    XyColorSpace m_xy_color_space;

    int m_left, m_top;
};

typedef ::boost::shared_ptr<XySubRenderFrame> SharedPtrXySubRenderFrame;

class XySubRenderFrameCreater
{
public:
    static XySubRenderFrameCreater* GetDefaultCreater();

    [[nodiscard]] HRESULT SetOutputRect(const RECT& output_rect);
    [[nodiscard]] HRESULT SetClipRect(const RECT& clip_rect);
    [[nodiscard]] HRESULT SetColorSpace(XyColorSpace color_space);
    [[nodiscard]] HRESULT SetVsfilterCompactRgbCorrection(bool value);
    [[nodiscard]] HRESULT SetRgbOutputTvLevel(bool value);

    [[nodiscard]] HRESULT GetOutputRect(RECT *output_rect);
    [[nodiscard]] HRESULT GetClipRect(RECT *clip_rect);
    [[nodiscard]] HRESULT GetColorSpace(XyColorSpace *color_space);
    [[nodiscard]] bool GetVsfilterCompactRgbCorrection();
    [[nodiscard]] bool GetRgbOutputTvLevel();

    [[nodiscard]] XySubRenderFrame* NewXySubRenderFrame(UINT bitmap_count);
    [[nodiscard]] XyBitmap* CreateBitmap(const RECT& target_rect);
    [[nodiscard]] DWORD TransColor(DWORD argb);

    explicit XySubRenderFrameCreater():m_xy_color_space(XY_CS_ARGB)
        , m_vsfilter_compact_rgb_correction(false), m_rgb_output_tv_level(false), m_bitmap_layout(XyBitmap::PACK){}
private:
    CRect m_output_rect;
    CRect m_clip_rect;
    XyColorSpace m_xy_color_space;
    bool m_vsfilter_compact_rgb_correction;
    bool m_rgb_output_tv_level;
    XyBitmap::MemLayout m_bitmap_layout;
};


#endif // __XY_BITMAP_674EB983_2A49_432D_A750_62C3B3E9DA67_H__
