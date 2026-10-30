#define CSRIAPI extern "C" __declspec(dllimport)
#include "csri.h"
#include <cstdint>
#include <vector>

using namespace std;


csri_inst* g_csri_inst_yyy{ nullptr };

void OpenTestScript(const char* filename)
{
    csri_rend* csri_rend_xxx{ csri_renderer_default() };

    g_csri_inst_yyy = csri_open_file(csri_rend_xxx, filename, nullptr);
}

void OverallTest(const float fps /*= 25*/, const int width /*=1280*/, const int height /*=720*/, const double start /*=0*/, const double end /*=60*/)
{
    csri_fmt fmt = { CSRI_F_BGR_, static_cast<unsigned>(width), static_cast<unsigned>(height) };

    csri_request_fmt(g_csri_inst_yyy, &fmt);

    vector<unsigned char> buf(4 * (width + 15) * (height + 1) + 256);
    csri_frame frame;
    std::memset(&frame, 0, sizeof(frame));
    frame.pixfmt = CSRI_F_BGR_;
    frame.planes[0U] = &buf[0U] + 16;
    frame.planes[0U] = reinterpret_cast<unsigned char*>(reinterpret_cast<std::uintptr_t>(frame.planes[0U]) & ~std::uintptr_t{ 15 });
    frame.strides[0U] = ((width + 15) & ~15);

    for (double time{ start }; time < end; time += 1 / fps)
    {
        csri_render(g_csri_inst_yyy, &frame, time);
    }
    csri_close(g_csri_inst_yyy);
}
