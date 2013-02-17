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

#ifndef SOCKET_H
#define SOCKET_H

namespace icinga {

/**
 * Base class for connection-oriented sockets.
 *
 * @ingroup base
 */
class I2_BASE_API Socket : public Stream
{
public:
	typedef shared_ptr<Socket> Ptr;
	typedef weak_ptr<Socket> WeakPtr;

	~Socket(void);

	virtual void Start(void);

	virtual void Close(void);

	String GetClientAddress(void);
	String GetPeerAddress(void);

	bool IsConnected(void) const;

	virtual size_t GetAvailableBytes(void) const;
	virtual size_t Read(void *buffer, size_t size);
	virtual size_t Peek(void *buffer, size_t size);
	virtual void Write(const void *buffer, size_t size);

	void Listen(void);

	signals2::signal<void (const Socket::Ptr&, const Socket::Ptr&)> OnNewClient;

protected:
	Socket(void);

	void SetFD(SOCKET fd);
	SOCKET GetFD(void) const;

	void SetConnected(bool connected);

	int GetError(void) const;
	static int GetLastSocketError(void);

	mutable boost::mutex m_SocketMutex;

private:
	SOCKET m_FD; /**< The socket descriptor. */
	bool m_Connected;
	bool m_Listening;

	thread m_ReadThread;
	thread m_WriteThread;

	condition_variable m_WriteCV;

	void ReadThreadProc(void);
	void WriteThreadProc(void);

	void ExceptionEventHandler(void);

	static String GetAddressFromSockaddr(sockaddr *address, socklen_t len);

	mutable boost::mutex m_QueueMutex;
	FIFO::Ptr m_SendQueue;
	FIFO::Ptr m_RecvQueue;

	void HandleWritableClient(void);
	void HandleReadableClient(void);

	void HandleWritableServer(void);
	void HandleReadableServer(void);

	void HandleReadable(void);
	void HandleWritable(void);
	void HandleException(void);

	bool WantsToWriteClient(void) const;
	bool WantsToReadClient(void) const;

	bool WantsToWriteServer(void) const;
	bool WantsToReadServer(void) const;

	bool WantsToWrite(void) const;
	bool WantsToRead(void) const;

	void CloseUnlocked(void);
};

/**
 * A socket exception.
 */
class SocketException : public Exception
{
public:
	SocketException(const String& message, int errorCode);
};

}

#endif /* SOCKET_H */
