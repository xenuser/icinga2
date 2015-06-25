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

#include "urlparser.hpp"

#include <string>
#include <iostream>
#include <map>
#include "boost/tokenizer.hpp"
#include "boost/foreach.hpp"
#include "boost/any.hpp"
#include "boost/algorithm/string/replace.hpp"

using namespace icinga;

Url::Url(std::string) 
{
	if (!url.compare(0,7,"http://")) {
		scheme = url.substr(0,7);
		hostname = url.substr(7, url.find_first_of('/', 7)-7);
	}

	if (url[scheme.length() + hostname.length()] != '/')
		return;

	size_t paramPos = url.find_first_of('?');
	if (paramPos == string::npos)
		return;

	string path = url.substr(
		scheme.length() + hostname.length(), paramPos);

	string parameters = url.substr(paramPos);

	//TODO Validate Strings

}

std::string Url::percentDecode(const std::string&)
{        
	typedef pair<string,string> kv_pair;
	BOOST_FOREACH(const kv_pair kv, percentCode) {
		boost::replace_all(token, kv.first, kv.second);
	}

}

std::string Url::percentEncode(const std::string&)
{
	typedef pair<string,string> kv_pair;
	BOOST_FOREACH(const kv_pair kv, percentCode) {
		boost::replace_all(token, kv.second, kv.first);
	}

}
