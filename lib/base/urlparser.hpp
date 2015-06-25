/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2015 Icinga Development Team (http://www.icinga.org)    *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#ifndef URLPARSER_H
#define URLPARSER_H

#include <string>
#include <map>

namespace icinga
{

/**
 * A url parser to use with the API
 *
 * @ingroup base
 */

class I2_BASE_API Url : public ObjectImpl<Url>
{
public:
	DECLARE_OBJECT(Url);
	DECLARE_OBJECTNAME(Url);

	Url(std::string);

private:
    map<std::string,std::string> percentCodes {
		{ "%21", "!" },
		{ "%23", "#" },
		{ "%24", "$" },
		{ "%26", "&" },
		{ "%27", "'" },
		{ "%28", "(" },
		{ "%29", ")" },
		{ "%2A", "*" },
		{ "%2B", "+" },
		{ "%2C", "," },
		{ "%2F", "/" },
		{ "%3A", ":" },
		{ "%3B", ";" },
		{ "%3D", "=" },
		{ "%3F", "?" },
		{ "%40", "@" },
		{ "%5B", "[" },
		{ "%5D", "]" },
	};  

	std::string percentDecode(const std::string&);
	std::string percentEncode(const std::string&);
};

}

#endif /* URIPARSER_H */
