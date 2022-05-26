#include "server_logger.h"

#include "server.h"

CServerLogger::CServerLogger(CServer *pServer) :
	m_pServer(pServer),
	m_MainThread(std::this_thread::get_id())
{
	dbg_assert(pServer != nullptr, "server pointer must not be null");
}

void CServerLogger::Log(const CLogMessage *pMessage)
{
	m_PendingLock.lock();
	if(m_MainThread == std::this_thread::get_id())
	{
		if(!m_aPending.empty())
		{
			if(m_pServer)
			{
				for(const auto &Message : m_aPending)
				{
					m_pServer->SendLogLine(&Message);
				}
			}
			m_aPending.clear();
		}
		m_PendingLock.unlock();
		m_pServer->SendLogLine(pMessage);
	}
	else
	{
		m_aPending.push_back(*pMessage);
		m_PendingLock.unlock();
	}
}

void CServerLogger::OnServerDeletion()
{
	dbg_assert(m_MainThread == std::this_thread::get_id(), "CServerLogger::OnServerDeletion not called from the main thread");
	m_pServer = nullptr;
}
