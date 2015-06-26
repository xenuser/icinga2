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

#include "base/i2-base.hpp"
#include "base/string.hpp"
#include "base/value.hpp"
#include <map>
#include <vector>

namespace icinga
{

enum UrlScheme {
	UrlSchemeHttp = 7,
	UrlSchemeHttps = 8,
	UrlSchemeFtp = 6,
	UrlSchemeUndefined = 0,
};

/**
 * A url parser to use with the API
 *
 * @ingroup base
 */

class I2_BASE_API Url
{
public:

	Url(const String& url);
	bool IsValid();

private:
	bool valid;

	String m_Hostname;
	UrlScheme m_Scheme;
	std::map<String,String> m_PercentCodes;
	std::map<String,Value> m_parameters;
	std::vector<String> m_Path;

	bool ValidateToken(const String& token, const String& illegalSymbols);
	bool ParsePath(const String& path);
	bool ParseParameters(const String& path);

	String PercentDecode(const String& token);
	String PercentEncode(const String& token);
};

}

#endif /* URIPARSER_H */
