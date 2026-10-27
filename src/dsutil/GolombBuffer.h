/*
 * (C) 2008-2012 see Authors.txt
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

class CGolombBuffer
{
public:
    CGolombBuffer(BYTE* pBuffer, int nSize);
    constexpr ~CGolombBuffer() noexcept;

    [[nodiscard]] constexpr UINT64 BitRead(int nBits, bool fPeek = false) noexcept;
    UINT64 UExpGolombRead();
    INT64 SExpGolombRead();
    constexpr void BitByteAlign() noexcept;

    [[nodiscard]] inline BYTE ReadByte() { return (BYTE)BitRead(8); };
    [[nodiscard]] inline short ReadShort() { return (short)BitRead(16); };
    [[nodiscard]] inline DWORD ReadDword() { return (DWORD)BitRead(32); };
    void ReadBuffer(BYTE* pDest, int nSize);

    void Reset();
    void Reset(BYTE* pNewBuffer, int nNewSize);

    void SetSize(const int nValue) { m_nSize = nValue; };
    [[nodiscard]] int GetSize() const { return m_nSize; };
    [[nodiscard]] constexpr int RemainingSize() const noexcept { return m_nSize - m_nBitPos; };
    [[nodiscard]] constexpr bool IsEOF() const noexcept { return m_nBitPos >= m_nSize; };
    [[nodiscard]] constexpr int GetPos() noexcept;
    [[nodiscard]] constexpr BYTE* GetBufferPos() noexcept { return m_pBuffer + m_nBitPos; };

    constexpr void SkipBytes(int nCount) noexcept;

private:
    BYTE* m_pBuffer;
    int   m_nSize;
    int   m_nBitPos;
    int   m_bitlen;
    INT64 m_bitbuff;
};
