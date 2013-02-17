/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012 Icinga Development Team (http://www.icinga.org/)        *
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

#include "i2-base.h"

using namespace icinga;

Stream::Stream(void)
	: m_Connected(false)
{ }

Stream::~Stream(void)
{
	assert(!m_Running);
}

bool Stream::IsConnected(void) const
{
	return m_Connected;
}

void Stream::SetConnected(bool connected)
{
	m_Connected = connected;

	if (m_Connected)
		OnConnected(GetSelf());
	else
		OnClosed(GetSelf());
}

/**
 * Checks whether an exception is available for this socket and re-throws
 * the exception if there is one.
 */
void Stream::CheckException(void)
{
	if (m_Exception)
		rethrow_exception(m_Exception);
}

void Stream::SetException(boost::exception_ptr exception)
{
	m_Exception = exception;
}

boost::exception_ptr Stream::GetException(void)
{
	return m_Exception;
}

void Stream::Start(void)
{
	m_Running = true;
}

void Stream::Close(void)
{
	assert(m_Running);
	m_Running = false;

	SetConnected(false);
}
