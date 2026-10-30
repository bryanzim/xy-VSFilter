#include "stdafx.h"
#include "xy_clipper_paint_machine.h"
#include "RTS.h"
#include "Rasterizer.h"
#include "cache_manager.h"
#include "subpixel_position_controler.h"

//////////////////////////////////////////////////////////////////////////
//
// CClipperPaintMachine
//

void CClipperPaintMachine::Paint(SharedPtrGrayImage2* output)
{
    ASSERT(output);
    if (m_clipper != nullptr)
    {
        ClipperAlphaMaskCacheKey key{ m_clipper };
        key.UpdateHashValue();
        ClipperAlphaMaskMruCache* cache{ CacheManager::GetClipperAlphaMaskMruCache() };
        POSITION pos{ cache->Lookup(key) };
        if (pos != nullptr)
        {
            *output = cache->GetAt(pos);
            (void)cache->UpdateCache(pos);
        }
        else
        {
            (*output).reset(m_clipper->Paint());
            (void)cache->UpdateCache(key, *output);
        }
    }
}

[[nodiscard]] CRect CClipperPaintMachine::CalcDirtyRect()
{
    return CRect(0, 0, INT_MAX, INT_MAX); // fix me: not a decent state machine yet
}

[[nodiscard]] const ClipperAlphaMaskCacheKey& CClipperPaintMachine::GetHashKey()
{
    return m_hash_key;
}
