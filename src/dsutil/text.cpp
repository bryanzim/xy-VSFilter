#include "stdafx.h"
#include "text.h"

/*
CString Explode(CString str, CAtlList<CString>& sl, TCHAR sep, int limit)
{
	sl.RemoveAll();

	if(limit == 1) {sl.AddTail(str); return _T("");}

	if(!str.IsEmpty() && str[str.GetLength()-1] != sep)
		str += sep;

	for(int i = 0, j = 0; (j = str.Find(sep, i)) >= 0; i = j+1)
	{
		CString tmp = str.Mid(i, j-i);
		tmp.TrimLeft(sep); tmp.TrimRight(sep);
		tmp.TrimLeft(); tmp.TrimRight();
		sl.AddTail(tmp);
		if(limit > 0 && sl.GetCount() == limit-1)
		{
			if(j+1 < str.GetLength()) 
			{
				CString tmp = str.Mid(j+1);
				tmp.TrimLeft(sep); tmp.TrimRight(sep);
				tmp.TrimLeft(); tmp.TrimRight();
				sl.AddTail(tmp);
			}
			break;
		}
	}

	if(sl.IsEmpty())
	{
		str.TrimLeft(sep); str.TrimRight(sep);
		str.TrimLeft(); str.TrimRight();
		sl.AddTail(str);
	}

	return sl.GetHead();
}

CString ExplodeMin(CString str, CAtlList<CString>& sl, TCHAR sep, int limit)
{
	Explode(str, sl, sep, limit);
	POSITION pos = sl.GetHeadPosition();
	while(pos) 
	{
		POSITION tmp = pos;
		if(sl.GetNext(pos).IsEmpty())
			sl.RemoveAt(tmp);
	}
	if(sl.IsEmpty()) sl.AddTail(CString()); // eh

	return sl.GetHead();
}

CString Implode(CAtlList<CString>& sl, TCHAR sep)
{
	CString ret;
	POSITION pos = sl.GetHeadPosition();
	while(pos)
	{
		ret += sl.GetNext(pos);
		if(pos) ret += sep;
	}
	return(ret);
}
*/

DWORD CharSetToCodePage(DWORD dwCharSet)
{
	if(dwCharSet == CP_UTF8)
	{
	    return CP_UTF8;
	}
	if(dwCharSet == CP_UTF7)
	{
	    return CP_UTF7;
	}
	CHARSETINFO cs{};
	::TranslateCharsetInfo(reinterpret_cast<DWORD*>(static_cast<UINT_PTR>(dwCharSet)), &cs, TCI_SRCCHARSET);
	return cs.ciACP;
}

[[nodiscard]] CStringA ConvertMBCS(CStringA str, const DWORD SrcCharSet, const DWORD DstCharSet)
{
	WCHAR* utf16{new WCHAR[str.GetLength()+1]};
	std::memset(utf16, 0, (str.GetLength()+1)*sizeof(WCHAR));

	CHAR* mbcs{new CHAR[str.GetLength()*6+1]};
	std::memset(mbcs, 0, str.GetLength()*6+1);

	int len{MultiByteToWideChar(
		CharSetToCodePage(SrcCharSet), 0, 
		str.GetBuffer(str.GetLength()), str.GetLength(), 
		utf16, (str.GetLength()+1)*sizeof(WCHAR))};

	len = WideCharToMultiByte(
		CharSetToCodePage(DstCharSet), 0, 
		utf16, len, 
		mbcs, str.GetLength()*6,
		nullptr, nullptr);

	str = mbcs;

	delete [] utf16;
	delete [] mbcs;

	return str;
}

[[nodiscard]] CStringA UrlEncode(CStringA str, const bool fRaw)
{
	CStringA urlstr;

	for(int i{0}; i < str.GetLength(); ++i)
	{
		CHAR c{str[i]};
		if(fRaw && c == '+')
		{
		    urlstr += "%2B";
		}
		else if(c > 0x20 && c < 0x7f && c != '&')
		{
		    urlstr += c;
		}
		else if(c == 0x20)
		{
		    urlstr += fRaw ? ' ' : '+';
		}
		else {CStringA tmp; tmp.Format("%%%02x", (BYTE)c); urlstr += tmp;}
	}

	return urlstr;
}

[[nodiscard]] CStringA UrlDecode(CStringA str, const bool fRaw)
{
	str.Replace("&amp;", "&");

	CHAR* s{str.GetBuffer(str.GetLength())};
	CHAR* e{s + str.GetLength()};
	CHAR* s1{s};
	CHAR* s2{s};
	while(s1 < e)
	{
		CHAR s11 = (s1 < e-1) ? (__isascii(s1[1U]) && std::isupper(s1[1U]) ? std::tolower(s1[1U]) : s1[1U]) : 0;
		CHAR s12 = (s1 < e-2) ? (__isascii(s1[2U]) && std::isupper(s1[2U]) ? std::tolower(s1[2U]) : s1[2U]) : 0;

		if(*s1 == '%' && s1 < e-2
		&& ((s1[1U] >= '0' && s1[1U] <= '9') || (s11 >= 'a' && s11 <= 'f'))
		&& ((s1[2U] >= '0' && s1[2U] <= '9') || (s12 >= 'a' && s12 <= 'f')))
		{
			s1[1U] = s11;
			s1[2U] = s12;
			*s2 = 0;
			if(s1[1U] >= '0' && s1[1U] <= '9')
			{
			    *s2 |= s1[1U]-'0';
			}
			else if(s1[1U] >= 'a' && s1[1U] <= 'f')
			{
			    *s2 |= s1[1U]-'a'+10;
			}
			*s2 <<= 4;
			if(s1[2U] >= '0' && s1[2U] <= '9')
			{
			    *s2 |= s1[2U]-'0';
			}
			else if(s1[2U] >= 'a' && s1[2U] <= 'f')
			{
			    *s2 |= s1[2U]-'a'+10;
			}
			s1 += 2;
		}
		else 
		{
			*s2 = *s1 == '+' && !fRaw ? ' ' : *s1;
		}

		++s1;
		++s2;
	}

	str.ReleaseBuffer(s2 - s);

	return str;
}

[[nodiscard]] CString ExtractTag(CString tag, CMapStringToString& attribs, bool& fClosing)
{
	tag.Trim();
	attribs.RemoveAll();

	fClosing = !tag.IsEmpty() ? tag[0U] == '/' : false;
	tag.TrimLeft('/');

	int i{tag.Find(' ')};
	if(i < 0)
	{
	    i = tag.GetLength();
	}
	CString type{tag.Left(i).MakeLower()};
	tag = tag.Mid(i).Trim();

	while((i = tag.Find('=')) > 0)
	{
		CString attrib{tag.Left(i).Trim().MakeLower()};
		tag = tag.Mid(i+1);
		for(i = 0; i < tag.GetLength() && _istspace(tag[i]); ++i)
		{
		    ;
		}
		tag = i < tag.GetLength() ? tag.Mid(i) : _T("");
		if(!tag.IsEmpty() && tag[0U] == '\"') {tag = tag.Mid(1); i = tag.Find('\"');}
		else
		{
		    i = tag.Find(' ');
		}
		if(i < 0)
		{
		    i = tag.GetLength();
		}
		CString param{tag.Left(i).Trim()};
		if(!param.IsEmpty())
		{
			attribs[attrib] = param;
		}
		tag = i+1 < tag.GetLength() ? tag.Mid(i+1) : _T("");
	}

	return(type);
}

[[nodiscard]] CAtlList<CString>& MakeLower(CAtlList<CString>& sl)
{
	POSITION pos{sl.GetHeadPosition()};
	while(pos)
	{
	    sl.GetNext(pos).MakeLower();
	}
	return sl;
}

[[nodiscard]] CAtlList<CString>& MakeUpper(CAtlList<CString>& sl)
{
	POSITION pos{sl.GetHeadPosition()};
	while(pos)
	{
	    sl.GetNext(pos).MakeUpper();
	}
	return sl;
}

[[nodiscard]] CAtlList<CString>& RemoveStrings(CAtlList<CString>& sl, const int minlen, const int maxlen)
{
	POSITION pos{sl.GetHeadPosition()};
	while(pos)
	{
		POSITION tmp{pos};
		CString& str{sl.GetNext(pos)};
		int len{str.GetLength()};
		if(len < minlen || len > maxlen)
		{
		    sl.RemoveAt(tmp);
		}
	}
	return sl;
}
