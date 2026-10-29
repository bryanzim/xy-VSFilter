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
#include "xy_sub_filter.h"


#if ENABLE_XY_LOG_REG_CONFIG
#  define TRACE_REG_CONFIG(msg) XY_LOG_TRACE(msg)
#else
#  define TRACE_REG_CONFIG(msg)
#endif

/////////////////////////////////////////////////////////////////////////////
// CVSFilterApp 

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
BEGIN_MESSAGE_MAP(CVSFilterApp, CWinApp)
END_MESSAGE_MAP()
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

CVSFilterApp::CVSFilterApp()
{
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#if ENABLE_XY_LOG
    LPTSTR  strDLLPath{DEBUG_NEW TCHAR[_MAX_PATH]};
    ::GetModuleFileName( reinterpret_cast<HINSTANCE>(&__ImageBase), strDLLPath, _MAX_PATH);
    CString dllPath{strDLLPath};
    dllPath += ".properties";
    xy_logger::doConfigure( dllPath.GetString() );
    delete [] strDLLPath;
#endif

#if (defined XY_SUB_FILTER_DLL)
    std::free((void*)m_pszProfileName);
    m_pszProfileName = _tcsdup(_T("XySubFilter"));
#endif
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

[[nodiscard]] BOOL CVSFilterApp::InitInstance()
{
	if(!CWinApp::InitInstance())
	{
		return FALSE;
	}

	SetRegistryKey(_T("Gabest"));
#if (defined XY_SUB_FILTER_DLL)
    std::free((void*)m_pszProfileName);
    m_pszProfileName = _tcsdup(_T("XySubFilter"));//restore m_pszProfileName overwrite by SetRegistryKey
#endif

	DllEntryPoint(AfxGetInstanceHandle(), DLL_PROCESS_ATTACH, 0); // "DllMain" of the dshow baseclasses

	STARTUPINFO si;
	GetStartupInfo(&si);
	m_AppName = CString(si.lpTitle);
	m_AppName.Replace('\\', '/');
	m_AppName = m_AppName.Mid(m_AppName.ReverseFind('/')+1);
	m_AppName.MakeLower();

	return TRUE;
}

[[nodiscard]] int CVSFilterApp::ExitInstance()
{
	DllEntryPoint(AfxGetInstanceHandle(), DLL_PROCESS_DETACH, 0); // "DllMain" of the dshow baseclasses

	return CWinApp::ExitInstance();
}

[[nodiscard]] HINSTANCE CVSFilterApp::LoadAppLangResourceDLL()
{
    CString fn;
    fn.ReleaseBufferSetLength(::GetModuleFileName(m_hInstance, fn.GetBuffer(MAX_PATH), MAX_PATH));
    fn = fn.Mid(fn.ReverseFind('\\')+1);
    fn = fn.Left(fn.ReverseFind('.')+1);
    fn = fn + _T("lang");
    return ::LoadLibrary(fn);
}

[[nodiscard]] UINT CVSFilterApp::GetProfileInt( const LPCTSTR lpszSection, const LPCTSTR lpszEntry, const int nDefault )
{
    UINT rv{__super::GetProfileInt(lpszSection, lpszEntry, nDefault)};
    TRACE_REG_CONFIG(lpszSection<<" "<<lpszEntry<<" "<<rv);
    return rv;
}

[[nodiscard]] CString CVSFilterApp::GetProfileString( const LPCTSTR lpszSection, const LPCTSTR lpszEntry, const LPCTSTR lpszDefault /*= NULL*/ )
{
    CString rv{__super::GetProfileString(lpszSection, lpszEntry, lpszDefault)};
    TRACE_REG_CONFIG(lpszSection<<" "<<lpszEntry<<" '"<<rv.GetString()<<"'");
    return rv;
}

CVSFilterApp theApp;
