# xy-VSFilter v4.0.0.0 (20261028)
https://github.com/bryanzim/xy-VSFilter/

Active branch: master

Pulled from:

- [pinterf/xy-VSFilter](https://github.com/pinterf/xy-VSFilter/): pfmod, including the Avisynth filters
- [Masaiki/xy-VSFilter](https://github.com/Masaiki/xy-VSFilter/): classic-renderer fixes for font fallback, HDR side data, color conversion, and SUP seeking

Official upstream:

- [Cyberbeing/xy-VSFilter](https://github.com/Cyberbeing/xy-VSFilter/tree/xy_sub_filter_rc5), branch `xy_sub_filter_rc5`

# Useful links:
[Doom9 forum](https://forum.doom9.org/showthread.php?t=168282)

[Avisynth wiki](http://avisynth.nl/index.php/Xy-VSFilter)

# Info:

The setup installs into a folder you choose. The default is Program Files\xy-VSFilter. A 64-bit Windows install includes the x86 and x64 copies and registers both, so 32-bit and 64-bit players can load the filters. A 32-bit Windows install includes the x86 copy. The setup runs regsvr32 on the installed DLLs, and uninstall unregisters them before the files are removed.

When this setup finds a previous installation, it unregisters those DLLs and removes their files before copying the new ones. If the new folder is different, the previous folder is removed once those files are gone.

- XySubFilter.dll is always installed. Registration covers XySubFilter and XySubFilter AutoLoader. The autoloader inserts the filter into a player graph for external subtitles.
- VSFilter.dll is optional and included in the full setup. It registers DirectVobSub and its auto-loading version, and it is the Avisynth plugin.
- xy_sub_filter_consumer.dll is optional and included in the full setup. It is for a player that takes the rendered subtitle frames from XySubFilter and blends them itself.

VSFilter.dll Avisynth filters (name and parameter signature):

      VobSub "cs"
      TextSub "c[file]s[charset]i[fps]f[vfr]s"
      TextSubSwapUV "b"
      MaskSub "[file]s[width]i[height]i[fps]f[length]i[charset]i[vfr]s[pixel_type]s"

# Build

Visual Studio 2026, Win32 and x64, MSVC or ClangCL. Release builds use the static CRT. CMake presets: `msvc-x64`, `msvc-win32`, `clang-x64`, `clang-win32`.

    cmake --preset clang-x64
    cmake --build --preset clang-x64
    ctest --preset clang-x64

Filter DLLs are written to `build/<preset>/bin/Release`.

The installer script is `installer/xy-vsfilter.iss` (Inno Setup 7). Compile it with Inno Setup 7 after the Clang Win32 and x64 Release builds. XySubFilter and its autoloader are always installed. DirectVobSub and the subtitle consumer are optional.

# Change log:

## v4.0.0.0 (20261028)

* Update: Default branch is master.
* Enhancement: Visual Studio 2026 builds for MSVC and ClangCL, Win32 and x64, through CMake presets. CTest covers the subtitle scripts.
* Enhancement: Static CRT, so the filter DLLs do not need the Visual C++ redistributable.
* Enhancement: Inno Setup 7 installer. The destination folder is selectable. A previous installation from this setup is unregistered and its filter files are removed before the new files are copied. XySubFilter and the autoloader are registered; DirectVobSub and the subtitle consumer are optional.
* Update: Vendored libraries:
  * Boost 1.57.0 to 1.92.0
  * log4cplus 1.0.4 to 2.2.1
  * GoogleTest 1.7.0 to 1.18.0
  * AviSynth+ headers to 3.7.5 (interface 8 to 11)
  * UnRAR 5.21 to 7.30 beta 2
  * VirtualDub to VirtualDub2 2.5.7
  * MFC sources to the Visual Studio 2026 toolset
  * Windows Driver Kit headers in `include/winddk` refreshed
* Fix: SSE2 subtitle blending builds its all-ones masks from a zeroed register. Release builds used an uninitialized XMM register, so the mask was not reliably all ones. This covers YV12, NV12, P010, and P210 mixing.
* Fix: Interleaved UV conversion keeps `packus_epi32` on the SSE4.1 path. Clang had marked the whole function SSE4.1, so a CPU with only SSE2 could crash there.
* Fix: Property-page subtitle styles are destroyed as `STSStyle`. They were deleted through `void*`, so the font-name string was not freed.
* Fix: Karaoke sweep progress starts at 0 when the karaoke type is not fill, sweep, or outline.
* Fix: UnRAR keeps SSSE3 and AES-NI on the paths that check for them. Whole files had been compiled with those instruction sets, including Reed-Solomon ECC, BLAKE2s, and AES decryption.
* Fix: A failed `GetPath` is treated as an empty outline. A missing glyph used to return -1, and that count was used as the path-buffer size. (Masaiki/xy-VSFilter)
* Fix: An external SUP file keeps presentation segments across a backward seek. Embedded PGS still drops segments that have already ended. (Masaiki/xy-VSFilter)
* Fix: Subtitle rendering unlocks the provider when the layout size cannot be read. (Masaiki/xy-VSFilter)
* Fix: YUV conversion rounds the fixed-point scale instead of truncating it. The same rounding is used for PC and TV range conversion. (Masaiki/xy-VSFilter)
* Fix: `IAMStreamSelect::Info` returns `E_INVALIDARG` when a caller asks for a media type or object this filter cannot fill. (Masaiki/xy-VSFilter)
* Fix: HDR10, HDR10+, and Dolby Vision side data is copied onto the sample that leaves DirectVobSub. VideoInfo2 color flags are copied when they still describe the output. The same subtype keeps them, YUV-to-YUV drops chroma siting, and YUV-to-RGB drops the matrix and chroma siting. (Masaiki/xy-VSFilter)
* Fix: Avisynth TextSub no longer forces BT.601 when the script matrix is Auto. It reads `_Matrix` and `_ColorRange` from the frame when AviSynth+ provides them, and otherwise chooses BT.709 or BT.601 from the picture size.
* Enhancement: Text rendering can stay on legacy GDI, use Uniscribe, or fall back to Uniscribe only when the selected font is missing a glyph. The choice is on the More property page. The default remains legacy GDI. (Masaiki/xy-VSFilter)
* Update: First-party sources follow the repository C++ style. Upstream drops under `src/thirdparty`, BaseClasses, and the SDK headers are left as they are.

## v3.2.0.810 (20240123)

* Fix VsFilter crash on load/release (as external DirectVobSub in players). Regression since v804.

## v3.2.0.809 (20240102)

Updates, fixes and some backports from Cyberbeing/xy-VSFilter by TheOneric, Masaiki and Cyberbeing

* Gracefully handle negative Encoding values
  more on this: https://github.com/pinterf/xy-VSFilter/commit/102897189902658ec68ef691eb3dd8d89f099985
* Update CSRI name
* Use distinct CSRI name and update CSRI version:
  Adjust the name so users can distinguish Cyberbeing/xy-VSFilter
  from pinterf/xy-VSFilter if builds from both are placed into Aegisub’s csri directory
* fix: time shift when playing external sup files
* fix: close sup file and read stop time of sample
* Introduce LayoutRes{X,Y} script headers. See details here:
  https://github.com/pinterf/xy-VSFilter/commit/8a46df19666191f6fcaa44811123c34f165f358e

## v3.2.0.808 (20221116)
* Fix Possible unhandled exception in Masksub (https://github.com/pinterf/xy-VSFilter/issues/36)
* Fix: no more vertically flipped MaskSub result for RGB32
* remove latest scaling workaround
  More on that: https://github.com/pinterf/xy-VSFilter/pull/34

## v3.2.0.807 (20210522) - WIP
* fix: scale between subtitle and video on CWord (Masaiki) (Issue: frx and fry tags wrong scaling)

## v3.2.0.806 (20210412)

* proper WebVTT support fixes by adipose/MPC-HC

## v3.2.0.805 (20210412)

* WebVTT support, embedded subtitles as well.
  99% of it extracted from MPC-HC https://github.com/clsid2/mpc-hc, thanks to MPC-HC developers.
* Changed the Subtitles library timestamp to REFERENCE_TIME (64 bit) internally.
  For very-very long clips (use case: streaming) (slydiman)
* Fix: ASS subtitles: styles with space after their names would fail to work (MPC HC/BE + external XySubFilter)
  (report: MatiasMovie, solution by astiob)
* Various code syntax fixes (clang compiler)

## v3.2.0.804 (20210306 - pinterf)
* Add native YV16, YV24, P210 (YUV422P10) and P216 (YUV422P16) support.
  From TextSub Avisynth filter they even work.

## v3.2.0.803 (20210305 - pinterf)
* Source: new code base in new branch which finally works with Visual Studio 2019:\
  https://github.com/Cyberbeing/xy-VSFilter/tree/xy_sub_filter_rc5
* upstream:
  * (cyberbeing) Revert MPC-HC Tag Cache, restore xy Tag Cache
  
   Fixes a significant performance regression on certain SSA/ASS samples.
  * (jesec) subtitles, subpic: enable SSE2 unconditionally
     Target OS (10.0) required users to have SSE2
     It is meaningless to maintain backward compatibility
  * (jesec) Unicode conversion for file src/dsutil/DSUtil.cpp
     Remove support for DX7
  * (jesec) Rasterizer::Rasterize: use optimized memsetSSE2.
    Source: https://www.agner.org/optimize/
  * (cyberbeing) Add VS2019 environment
* Active branch changed to xy_sub_filter_rc5 on my (pinterf) side as well
* Avisynth plugin
  * re-add my changes from 2018
  * Add support for YV16
  * Throw error on unsupported formats
  * Avisynth plugin: keep frame properties when Avisynth+ interface is v8
* version set manually in src/filters/transform/vsfilter/version_in.h

## v3.2.0.802 (20181031)
* Change version numbering.
  
  Cyberbeing has released a patch for their original xy-VSFilter, their version stepped to v3.1.0.752
  
  Since I thought the project was full-dead, my version numbering kept - after a range skip - the original numbering.
  
  Now I'm changing my versioning scheme to 3.2.x.x to avoid possible future collision.
  
* Fix TextSub crash that could occur in rare circumstances (shssoichiro)

* Fix for SSA/ASS repositioning becoming permanently disabled after typesetting was displayed\
  (madshi -> Cyberbeing v3.1.0.752 7th October 2018)
  
* Add BT.2020 Support\
  (madshi -> Cyberbeing v3.1.0.751 27th September 2018)

## v3.1.0.801 (20180904)
* fix random crash in x64 build (address truncated to 32 bits, Rasterizer::Draw and Rasterizer::FillSolidRect were affected)
* XySubFilter.DLL now compiled with v141_xp toolset instead of v141 (like VSFilter.DLL)
* From upstream (madshi, Cyberbeing)
  * Fix a possible infinite loop in the Real Text subtitle parser on 64-bit
  * Fix external SRT subtitles with italic tag not being flagged as repositionable
  * Disable repositioning for SSA/ASS subtitles with default position level
  * Add setting to allow repositioning of SSA/ASS dialog

- v3.1.0.800 (20180411)
* Bring source to XySubFilter-3.1.0.746 level:\
  Apply file differences between Cyberbeing XySubFilter-3.1.0.746 (source from the release pack) \
  and xy-VSFilter 3.0.0.306 mod.
* make the source VS2017 compatible again
* make VSFilter work again as an Avisynth plugin
* Versioning: 3.1.0 prefix is kept, build number goes from 800 and up for my mod manually in src/filters/transform/vsfilter/version_in.h

## v3.0.0.400 (20180405) (work in February 2018)
* Get the last known xy-vsfilter version (3.0.0.306) which was still working as the Avisynth plugin.\
  https://github.com/Cyberbeing/xy-VSFilter/releases/tag/3.0.0.306\
  Plan: have a native 10/16 bit compatible Avisynth TextSub filter.\
* xy-vsfilter 3.0.0.306 rebuilt with Visual Studio 2017
* Avisynth+ headers
* Native support for YUV420P10 and YUV420P16 formats in Avisynth filter TextSub and probably VobSub.
  Internal conversion to and from P010 and P016 with SSE2 support
* Still doesn't work in any MT mode (I think): automatically registers itself as MT_SERIALIZED to prevent troubles
* Avisynth filter MaskSub: new pixel_type (default: "RGB32") parameter. Can be set to "YUV420P10" or "YUV420P16"
* Versioning: 3.0.0 prefix kept, build number goes from 400 and up for my mod manually in src/filters/transform/vsfilter/version_in.h
