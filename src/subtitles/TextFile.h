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

#include <afx.h>
#include <vector>

#include "StdioFile64.h"

class CTextFile : protected CStdioFile64
{
public:
    enum enc {
        DEFAULT_ENCODING,
        UTF8,
        LE16,
        BE16
    };

private:
	enc m_encoding, m_defaultencoding;
	int m_offset;

	unsigned char m_readbuffer[4096U];
	std::size_t m_bufferPos;
	std::size_t m_bufferCount;
	std::vector<wchar_t> m_convertedBuffer;

	enum charerror {CHARERR_NEED_MORE=-1, CHARERR_REOPEN=-2};

	[[nodiscard]] int NextChar();
	[[nodiscard]] bool ReadLine();

public:
	explicit CTextFile(enc e = DEFAULT_ENCODING);

	using CStdioFile64::Open;
	[[nodiscard]] virtual bool Open(LPCTSTR lpszFileName);
	[[nodiscard]] virtual bool Save(LPCTSTR lpszFileName, enc e /*= DEFAULT_ENCODING*/);

	void SetEncoding(enc e);
	[[nodiscard]] enc GetEncoding();
	[[nodiscard]] bool IsUnicode();

	// CFile
	[[nodiscard]] CString GetFilePath() const;

	// CStdioFile

    [[nodiscard]] ULONGLONG GetPosition() const;
	[[nodiscard]] ULONGLONG GetLength() const;
	[[nodiscard]] ULONGLONG Seek(LONGLONG lOff, UINT nFrom);

	void WriteString(LPCWSTR lpsz/*CStringW str*/);
	[[nodiscard]] BOOL ReadString(CStringW& str);
};

class CWebTextFile : public CTextFile
{
	LONGLONG m_llMaxSize;
	CString m_tempfn;

public:
    explicit CWebTextFile(CTextFile::enc e = DEFAULT_ENCODING, LONGLONG llMaxSize = 1024 * 1024);

	[[nodiscard]] bool Open(LPCTSTR lpszFileName);
	[[nodiscard]] bool Save(LPCTSTR lpszFileName, enc e /*= DEFAULT_ENCODING*/);
	void Close();
};
