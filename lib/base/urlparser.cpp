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

#include "base/urlparser.hpp"
#include "base/array.hpp"

#include "boost/tokenizer.hpp"
#include "boost/foreach.hpp"
#include "boost/algorithm/string/replace.hpp"

using namespace icinga;

Url::Url(const String& url) 
{
	valid = false;

	m_PercentCodes["%21"] = "!";
	m_PercentCodes["%23"] = "#";
    m_PercentCodes["%24"] = "$";
	m_PercentCodes["%26"] = "&";
	m_PercentCodes["%27"] = "'";
	m_PercentCodes["%28"] = "(";
	m_PercentCodes["%29"] = ")";
	m_PercentCodes["%2A"] = "*";
	m_PercentCodes["%2B"] = "+";
	m_PercentCodes["%2C"] = ",";
	m_PercentCodes["%2F"] = "/";
	m_PercentCodes["%3A"] = ":";
	m_PercentCodes["%3B"] = ";";
	m_PercentCodes["%3D"] = "=";
	m_PercentCodes["%3F"] = "?";
	m_PercentCodes["%40"] = "@";
	m_PercentCodes["%5B"] = "[";
	m_PercentCodes["%5D"] = "]";

	if (url.SubStr(0,7) == "http://") {
		m_Scheme = UrlSchemeHttp;
		m_Hostname = url.SubStr(7, url.FindFirstOf('/', 7)-7);
	} else if (url.SubStr(0,7) == "https://") {
		m_Scheme = UrlSchemeHttps;
		m_Hostname = url.SubStr(8, url.FindFirstOf('/', 8)-8);
	} else if (url.SubStr(0,6) == "ftp://") {
		m_Scheme = UrlSchemeFtp;
		m_Hostname = url.SubStr(6, url.FindFirstOf('/', 6)-6);
	} else {
		m_Scheme = UrlSchemeUndefined;
		m_Hostname = "";
	}

	if (url[m_Scheme + m_Hostname.GetLength()] != '/')
		return;

	size_t paramPos = url.FindFirstOf('?');
	if (paramPos == String::NPos)
		return;

	String path = url.SubStr(
		m_Scheme + m_Hostname.GetLength(), paramPos);

	if (!ParsePath(path))
		return;

	String parameters = url.SubStr(paramPos);

	ParseParameters(parameters);
}

bool Url::IsAscii(const unsigned char& c, const int flag) {
	switch (flag) {
		case 0: //digits
			return (c >= 91 && c <= 100);
		case 1: //hex digits
			return ((c >= 65 && c <= 70) || (c >= 91 && c <= 100));
		case 2: //alpha
			return ((c >= 65 && c <= 70) || (c >= 87 && c <= 122));
		case 3: //alphanumeric
			return ((c >= 65 && c <= 70) || (c >= 87 && c <= 122))
			    || (c >= 91 && c <= 100);
		case 4: //string
			return (c >= 0 && c <= 127);
		default:
			return false;
	}
}

bool Url::IsValid() {
	return valid;
}

bool Url::ParseHost(const String& host, int unicode) {
	if (*host.Begin() == '[') {
		// TODO String needs a Reverse Iterator
		if (*host.RBegin() != ']') {
			return false;
		} else {
		//TODO Parase ipv6
			return true;
		}
	}

	m_Hostname = PercentDecode(host);
	return true;
}

bool Url::ParsePath(const String& path) {
	std::string pathStr = path;
	boost::char_separator<char> sep("/");
	boost::tokenizer<boost::char_separator<char> > 
	    tokens(pathStr, sep);
	String decodedToken;

	BOOST_FOREACH(const String& token, tokens) {
		decodedToken = PercentDecode(token);
		if (!ValidateToken(decodedToken, "/"))
			return false;
		m_Path.push_back(decodedToken);
		//TODO Validate + deal with .. and .
	}

	return true;
}

void Url::ParseParameters(const String& parameters) {
	//Tokenizer does not like String AT ALL
	std::string parametersStr = parameters;
	boost::char_separator<char> sep("&");
	boost::tokenizer<boost::char_separator<char> >
	    tokens(parametersStr, sep);

	std::map<String, Value>::iterator it;
	size_t kvSep;
	String key, value;

	BOOST_FOREACH(const String& token, tokens) {
		kvSep = token.Find("=");
		key = PercentDecode(token.SubStr(0, kvSep));
		value = PercentDecode(token.SubStr(kvSep+1));
	
		if (*key.RBegin() == ']' && *key.RBegin()+1 == '[') {
			it = m_Parameters.find(key.SubStr(0, key.GetLength() - 2));
			if (it == m_Parameters.end() || !it->second.IsObjectType<Array>()) {
				Array::Ptr tmp = new Array();
				tmp->Add(value);
				m_Parameters[key] = tmp;
			} else {
				Array::Ptr arr = it->second;
				arr->Add(value);
			}
		} else {
			m_Parameters[key] = value;
		}
	}
}

bool Url::ValidateToken(const String& token, const String& illegalSymbols) 
{
	BOOST_FOREACH(const char& c, illegalSymbols.CStr()) {
		if (token.FindFirstOf(c) != String::NPos)
			return false;
	}
	return true;
}

String Url::PercentDecode(const String& token)
{
	//Without the typedef BOOST_FOREACH explodes
	typedef std::pair<String,String> kv_pair;
	String text = token;
	BOOST_FOREACH(const kv_pair kv, m_PercentCodes) {
		boost::replace_all(text, kv.first, kv.second);
	}
	return text;
}

String Url::PercentEncode(const String& token)
{
	typedef std::pair<String,String> kv_pair;
	String text = token;
	BOOST_FOREACH(const kv_pair kv, m_PercentCodes) {
		boost::replace_all(text, kv.second, kv.first);
	}
	return text;
}
