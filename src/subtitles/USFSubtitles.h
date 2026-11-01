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

#include <atlcoll.h>
#include "STS.h"

// metadata
using author_t = struct
{
    CStringW name, email, url;
};
using language_t = struct
{
    CStringW code, text;
};
using metadata_t = struct
{
    CStringW title, date, comment;
    author_t author;
    language_t language, languageext;
};
// style
using posattriblist_t = struct
{
    CStringW alignment, relativeto, horizontal_margin, vertical_margin, rotate[3U];
};
using fontstyle_t = struct
{
    CStringW face, size, color[4U], weight, italic, underline, alpha, outline, shadow, wrap;
};
using style_t = struct
{
    CStringW name;
    fontstyle_t fontstyle;
    posattriblist_t pal;
};
// effect
using keyframe_t = struct
{
    CStringW position;
    fontstyle_t fontstyle;
    posattriblist_t pal;
};
using effect_t = struct
{
    CStringW name;
    CAutoPtrList<keyframe_t> keyframes;
};
// subtitle/text
using text_t = struct
{
    int start, stop;
    CStringW effect, style, str;
    posattriblist_t pal;
};

class CUSFSubtitles
{
    [[nodiscard]] bool ParseUSFSubtitles(CComPtr<IXMLDOMNode> pNode);
    void ParseMetadata(CComPtr<IXMLDOMNode> pNode, metadata_t& m);
    void ParseStyle(CComPtr<IXMLDOMNode> pNode, style_t* s);
    void ParseFontstyle(CComPtr<IXMLDOMNode> pNode, fontstyle_t& fs);
    void ParsePal(CComPtr<IXMLDOMNode> pNode, posattriblist_t& pal);
    void ParseEffect(CComPtr<IXMLDOMNode> pNode, effect_t* e);
    void ParseKeyframe(CComPtr<IXMLDOMNode> pNode, keyframe_t* k);
    void ParseSubtitle(CComPtr<IXMLDOMNode> pNode, int start, int stop);
    void ParseText(CComPtr<IXMLDOMNode> pNode, CStringW& assstr);
    void ParseShape(CComPtr<IXMLDOMNode> pNode);

public:
    explicit CUSFSubtitles();
    virtual ~CUSFSubtitles();

    [[nodiscard]] bool Read(LPCTSTR fn);
    //	bool Write(LPCTSTR fn); // TODO

    metadata_t metadata;
    CAutoPtrList<style_t> styles;
    CAutoPtrList<effect_t> effects;
    CAutoPtrList<text_t> texts;

    [[nodiscard]] bool ConvertToSTS(CSimpleTextSubtitle& sts);
    //	bool ConvertFromSTS(CSimpleTextSubtitle& sts); // TODO
};
