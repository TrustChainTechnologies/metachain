#pragma once

using namespace std;

/*
This is the main class that'll hold all information about the chain and handle all it's core parts.
*/
class NetworkManager
{
	private:
		MetaChain							*m_pMC;
		CService							*m_pServiceLocal;
		int									m_iNetConnectTimeout;
		int									m_iTimeBetweenConnects;
		int									m_iMaxOutboundConnections;
		int									m_iMaxInboundConnections;
		bool								m_bActiveNetwork;

		// thread interrupts and message processing variables
		CThreadInterrupt					m_interruptNet;
		atomic<bool>						m_abflagInterruptMsgProc;
		bool								m_bfMsgProcWake;
		condition_variable					m_condMsgProc;
		mutex								m_mutexMsgProc;
		void								WakeMessageHandler();

		// socket functions and variables
		SOCKET								m_hListenSocket;
		bool								startListeningSocket();

		// thread functions and variables
		thread								threadSocketHandler;
		thread								threadOpenConnections;
		thread								threadMessageHandler;
		void								startThreads();
		void								ThreadSocketHandler();
		void								ThreadOpenConnections();
		void								ThreadMessageHandler();

		// message handling
		inline void							handleMessage(ipContainer< netPeers> *peers, cCriticalSection *cs, bool bInbound);
		bool								ProcessMessage(netMessage msg, list< netPeers >::iterator peer, bool bInbound);

		// functions to update the peers and ban lists
		void								DumpData();

		// functions and variables for our peers list and their management
		ipContainer< netPeers >				m_lstPeerListOut;
		mutable cCriticalSection			m_csPeerListOut;
		cSemaphore							*m_pSemOutbound;
		ipContainer< netPeers >				m_lstPeerListIn;
		mutable cCriticalSection			m_csPeerListIn;
		cSemaphore							*m_pSemInbound;
		list< netPeers >::iterator			getNextOutNode(bool bConnected = true, bool bCheckTimeDelta = true);
		inline void							handlePeers(ipContainer< netPeers> *peers, cCriticalSection *cs);

		// functions and variables for our banned list and their management
		ipContainer< CNetAddr >				m_lstBanList;
		bool								isBanned(string strAddress);

		// destructor functions
		void								Interrupt();
		void								Stop();


	public:
											NetworkManager(MetaChain *mc);
											~NetworkManager();
		bool								initialize(CSimpleIniA* iniFile);
		int									getTimeout() { return m_iNetConnectTimeout; };
};