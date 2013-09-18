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

#include "../api_csslayout.h"
#include <memory>

namespace clan
{
/// \addtogroup clanCSSLayout_Tokenizer clanCSSLayout Tokenizer
/// \{

class CSSToken;
class IODevice;
class CSSTokenizer_Impl;

/// \brief Tokenizer class that breaks CSS text into tokens
///
/// !group=Core/CSS! !header=core.h!
class CL_API_CSSLAYOUT CSSTokenizer
{
/// \name Construction
/// \{
public:
	CSSTokenizer(IODevice &device);
	CSSTokenizer(const std::string &text);
/// \}

/// \name Operations
/// \{
public:
	/// \brief Reads the next CSS token
	void read(CSSToken &out_token, bool eat_whitespace, bool eat_comments = true);

	/// \brief Reads all import directives specified in this document
	std::vector<std::string> read_import_urls();
/// \}

/// \name Implementation
/// \{
private:
	std::shared_ptr<CSSTokenizer_Impl> impl;
/// \}
};

/// \}
}
