/************************************************************************/
/* author: xy                                                           */
/* date: 20110514                                                       */
/************************************************************************/
#include "xy_malloc.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>

std::size_t g_xy_malloc_used_size{ 0U };

// gathered from x264
/****************************************************************************
 * xy_malloc:
 ****************************************************************************/
[[nodiscard]] void* xy_malloc(const std::size_t i_size, int align_shift)
{
    const int ALIGNMENT{ 16 };
    const int MASK{ ALIGNMENT - 1 };
    std::uint8_t* buf;
    std::uint8_t* align_buf;
    align_shift &= MASK;
    buf = reinterpret_cast<std::uint8_t*>(std::malloc(i_size + MASK + sizeof(void**) +
                                                      sizeof(i_size)));
    if (!buf)
    {
        return nullptr;
    }
    align_buf = buf + MASK + sizeof(void**) + sizeof(i_size);
    align_buf -= (ALIGNMENT + (reinterpret_cast<std::intptr_t>(align_buf) & MASK) - align_shift) & MASK;
    *reinterpret_cast<void**>(align_buf - sizeof(void**)) = buf;
    *reinterpret_cast<std::size_t*>(align_buf - sizeof(void**) - sizeof(i_size)) = i_size;
    g_xy_malloc_used_size += i_size;
    return align_buf;
}

/****************************************************************************
 * xy_free:
 ****************************************************************************/
void xy_free(void* p)

{
    if (p)
    {
        g_xy_malloc_used_size -= *reinterpret_cast<std::size_t*>(reinterpret_cast<std::uint8_t*>(p) - sizeof(void**) - sizeof(std::size_t));
        std::free(*(reinterpret_cast<void**>(p) - 1));
    }
}

/****************************************************************************
 * xy_realloc:
 ****************************************************************************/
[[nodiscard]] void* xy_realloc(void* p, const std::size_t i_size, const int align_shift)
{
    std::size_t i_old_size{ 0U };
    std::uint8_t* p_new{ nullptr };
    p_new = reinterpret_cast<std::uint8_t*>(xy_malloc(i_size, align_shift));
    if (p_new && p)
    {
        i_old_size = *reinterpret_cast<std::size_t*>(reinterpret_cast<std::uint8_t*>(p) - sizeof(void**) - sizeof(i_size));
        std::memcpy(p_new, p, (i_old_size < i_size) ? i_old_size : i_size);
        xy_free(p);
    }
    return p_new;
}
