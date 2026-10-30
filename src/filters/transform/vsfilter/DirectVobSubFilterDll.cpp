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

#include "StdAfx.h"
#include "DirectVobSubFilter.h"
#include "DirectVobSubPropPage.h"
#include "VSFilter.h"
#include "..\..\..\dsutil\MediaTypes.h"

#include <initguid.h>
#include "..\..\..\..\include\moreuuids.h"

const AMOVIESETUP_MEDIATYPE sudPinTypesIn[] = {
    {  &MEDIATYPE_NULL,   &MEDIASUBTYPE_NULL },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_YUY2 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_YV12 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_YV16 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_YV24 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_I420 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_IYUV },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_P010 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_P016 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_P210 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_P216 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_NV12 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_NV21 },
    { &MEDIATYPE_Video,   &MEDIASUBTYPE_AYUV },
    { &MEDIATYPE_Video,  &MEDIASUBTYPE_RGB32 },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB565 },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB555 },
    { &MEDIATYPE_Video,  &MEDIASUBTYPE_RGB24 },
};

const AMOVIESETUP_MEDIATYPE sudPinTypesIn2[] = {
    {     &MEDIATYPE_Text,          &MEDIASUBTYPE_None },
    {     &MEDIATYPE_Text,                  &GUID_NULL },
    { &MEDIATYPE_Subtitle,          &MEDIASUBTYPE_None },
    { &MEDIATYPE_Subtitle,          &MEDIASUBTYPE_UTF8 },
    { &MEDIATYPE_Subtitle,           &MEDIASUBTYPE_SSA },
    { &MEDIATYPE_Subtitle,           &MEDIASUBTYPE_ASS },
    { &MEDIATYPE_Subtitle,          &MEDIASUBTYPE_ASS2 },
    { &MEDIATYPE_Subtitle,           &MEDIASUBTYPE_SSF },
    { &MEDIATYPE_Subtitle,        &MEDIASUBTYPE_VOBSUB },
    { &MEDIATYPE_Subtitle,       &MEDIASUBTYPE_HDMVSUB },
    { &MEDIATYPE_Subtitle,        &MEDIASUBTYPE_WEBVTT },
    { &MEDIATYPE_Subtitle, &MEDIASUBTYPE_DVB_SUBTITLES }
};

const AMOVIESETUP_MEDIATYPE sudPinTypesOut[] = {
    { &MEDIATYPE_Video, &MEDIASUBTYPE_None },
};

static WCHAR sudNameIn[] = L"Input";
static WCHAR sudNameOut[] = L"Output";
static WCHAR sudNameIn2[] = L"Input2";

const AMOVIESETUP_PIN sudpPins[] = {
    {  sudNameIn, FALSE, FALSE, FALSE, FALSE, &CLSID_NULL, nullptr,  countof(sudPinTypesIn),  sudPinTypesIn },
    { sudNameOut, FALSE,  TRUE, FALSE, FALSE, &CLSID_NULL, nullptr, countof(sudPinTypesOut), sudPinTypesOut },
    { sudNameIn2,  TRUE, FALSE, FALSE,  TRUE, &CLSID_NULL, nullptr, countof(sudPinTypesIn2), sudPinTypesIn2 }
};

/*const*/ AMOVIESETUP_FILTER sudFilter[] = {
    {  &__uuidof(CDirectVobSubFilter),                        L"DirectVobSub",    MERIT_DO_NOT_USE, countof(sudpPins), sudpPins },
    { &__uuidof(CDirectVobSubFilter2), L"DirectVobSub (auto-loading version)", MERIT_PREFERRED + 2, countof(sudpPins), sudpPins },
};

CFactoryTemplate g_Templates[] = {
    { sudFilter[0U].strName,         sudFilter[0U].clsID,  CreateInstance<CDirectVobSubFilter>, nullptr, &sudFilter[0U] },
    { sudFilter[1U].strName,         sudFilter[1U].clsID, CreateInstance<CDirectVobSubFilter2>, nullptr, &sudFilter[1U] },
    {       L"DVSMainPPage",    &__uuidof(CDVSMainPPage),        CreateInstance<CDVSMainPPage>, nullptr,        nullptr },
    {    L"DVSGeneralPPage", &__uuidof(CDVSGeneralPPage),     CreateInstance<CDVSGeneralPPage>, nullptr,        nullptr },
    {       L"DVSMiscPPage",    &__uuidof(CDVSMiscPPage),        CreateInstance<CDVSMiscPPage>, nullptr,        nullptr },
    {       L"DVSMorePPage",    &__uuidof(CDVSMorePPage),        CreateInstance<CDVSMorePPage>, nullptr,        nullptr },
    {     L"DVSTimingPPage",  &__uuidof(CDVSTimingPPage),      CreateInstance<CDVSTimingPPage>, nullptr,        nullptr },
    {       L"DVSZoomPPage",    &__uuidof(CDVSZoomPPage),        CreateInstance<CDVSZoomPPage>, nullptr,        nullptr },
    {      L"DVSColorPPage",   &__uuidof(CDVSColorPPage),       CreateInstance<CDVSColorPPage>, nullptr,        nullptr },
    {      L"DVSPathsPPage",   &__uuidof(CDVSPathsPPage),       CreateInstance<CDVSPathsPPage>, nullptr,        nullptr },
    {      L"DVSAboutPPage",   &__uuidof(CDVSAboutPPage),       CreateInstance<CDVSAboutPPage>, nullptr,        nullptr },
};

int g_cTemplates{ countof(g_Templates) };

//////////////////////////////
/*removeme*/
extern void JajDeGonoszVagyok();

STDAPI DllRegisterServer()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (theApp.GetProfileInt(ResStr(IDS_R_GENERAL), ResStr(IDS_RG_SEENDIVXWARNING), 0) != 1)
    {
        theApp.WriteProfileInt(ResStr(IDS_R_GENERAL), ResStr(IDS_RG_SEENDIVXWARNING), 0);
    }

    /*removeme*/
    JajDeGonoszVagyok();

    return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
    //  DVS_WriteProfileInt2(IDS_R_GENERAL, IDS_RG_SEENDIVXWARNING, 0);

    // ToDo: reset auto subtitle provider setting

    return AMovieDllRegisterServer2(FALSE);
}

void CALLBACK DirectVobSub(const HWND hwnd, const HINSTANCE hinst, const LPSTR lpszCmdLine, const int nCmdShow)
{
    if (FAILED(::CoInitialize(0)))
    {
        return;
    }

    CComPtr<IBaseFilter> pFilter;
    CComQIPtr<ISpecifyPropertyPages> pSpecify;

    if (SUCCEEDED(pFilter.CoCreateInstance(__uuidof(CDirectVobSubFilter))) && (pSpecify = pFilter))
    {
        ShowPPage(pFilter, hwnd);
    }

    ::CoUninitialize();
}
