/*
 * (C) 2014 see Authors.txt
 *
 * This file is part of MPC-HC.
 *
 * MPC-HC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-HC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <afx.h>

class CStdioFile64 : public CStdioFile
{
    DECLARE_DYNAMIC(CStdioFile64)

public:
    explicit CStdioFile64() : CStdioFile() {};
    constexpr explicit CStdioFile64(CAtlTransactionManager* pTM) noexcept : CStdioFile(pTM) {};
    constexpr explicit CStdioFile64(FILE* pOpenStream) noexcept : CStdioFile(pOpenStream) {};
    CStdioFile64(LPCTSTR lpszFileName, UINT nOpenFlags) : CStdioFile(lpszFileName, nOpenFlags) {};
    CStdioFile64(LPCTSTR lpszFileName, UINT nOpenFlags, CAtlTransactionManager* pTM) : CStdioFile(lpszFileName, nOpenFlags, pTM) {};

    constexpr virtual ~CStdioFile64() noexcept {};

    [[nodiscard]] virtual ULONGLONG GetPosition() const;
    [[nodiscard]] virtual ULONGLONG GetLength() const;

    [[nodiscard]] virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
};
