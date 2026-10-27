#pragma once

class CMediaTypeEx : public CMediaType
{
public:
	explicit CMediaTypeEx();
	constexpr explicit CMediaTypeEx(const CMediaType& mt) noexcept {CMediaType::operator = (mt);}

	[[nodiscard]] CString ToString(IPin* pPin = NULL);

	[[nodiscard]] static CString GetVideoCodecName(const GUID& subtype, DWORD biCompression);
	[[nodiscard]] static CString GetAudioCodecName(const GUID& subtype, WORD wFormatTag);
	[[nodiscard]] static CString GetSubtitleCodecName(const GUID& subtype);

	void Dump(CAtlList<CString>& sl);
};
