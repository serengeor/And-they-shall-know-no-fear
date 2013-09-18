/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

#include "../CSSDocument/css_property_value.h"

namespace clan
{
/// \addtogroup clanCSSLayout_PropertyValues clanCSSLayout Property Values
/// \{

class CSSValueCursor : public CSSPropertyValue
{
public:
	CSSValueCursor();
	void compute(const CSSValueCursor *parent, CSSResourceCache *layout, float em_size, float ex_size);
	std::string to_string() const;
	void apply(CSSComputedValuesUpdater *updater);

	enum Type
	{
		type_uris,
		type_auto,
		type_crosshair,
		type_default,
		type_pointer,
		type_move,
		type_e_resize,
		type_ne_resize,
		type_nw_resize,
		type_n_resize,
		type_se_resize,
		type_sw_resize,
		type_s_resize,
		type_w_resize,
		type_text,
		type_wait,
		type_help,
		type_progress,
		type_inherit
	};
	Type type;
	std::vector<std::string> urls;
};

/// \}
}
