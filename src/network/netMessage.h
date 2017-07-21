#pragma once


// the struct of the header
// only needed in this class, thus no single file
struct sHeader {
	uint32_t		ui32tPayloadSize;
	uint16_t		ui16tSubject;
	uint8_t			ui8tChecksum[CHECKSUM_SIZE];
};

// our communication message which is delivered via network
class netMessage
{
	private:
		bool							m_bComplete;
		bool							m_bHeaderComplete;
		sHeader							m_sHeader;
		cDataStream						m_vRecv;
		cHash256						m_Hasher256;
		uint256							m_ui256DataHash;
		unsigned int					m_uiPosition;

		char							*m_pBuffer;

		void							makeDeepCopy(const netMessage& obj);

	public:
		// these are the communication definers we use as ui16tSubject
		/*
		* we use a hex coding scheme to categorize our actions for easier splitting into groups.
		* the following binary scheme will be used globally and updated every single time we add new categories and subjects
		*
		* 0 0 0 0		0 0 0 0		0 0 0 0		0 0 0 0
		*	8bit CATEGORY		 |	8bit SUBJECT
		*
		* LIST:
		* 0 0 0 0		0 0 0 1		->		GENERAL NET COMMUNICATION
		*							0 0 0 0		0 0 0 1		->		0x0101		VERSION NUMBER
		*							0 0 0 0		0 0 1 0		->		0x0102		NEWER VERSION AVAILABLE
		* 0 0 0 0		0 0 1 0		->		PEER AND BAN LIST COMMUNICATION
		*							0 0 0 0		0 0 0 1		->		0x0201		GET PEER LIST
		*							0 0 0 0		0 0 1 0		->		0x0202		SEND PEER LIST
		*							0 0 0 0		0 0 1 1		->		0x0203		GET BAN LIST
		*							0 0 0 0		0 1 0 0		->		0x0204		SEND BAN LIST
		*/
		enum SUBJECT {
			NET_VERSION = 0x0101,
			NET_VERSION_NEWER = 0x0102,
			NET_PEER_LIST_GET = 0x0201,
			NET_PEER_LIST_SEND = 0x0202,
			NET_BAN_LIST_GET = 0x0203,
			NET_BAN_LIST_SEND = 0x0204
		};

							netMessage();
							netMessage(netMessage::SUBJECT subj, void *ptrData, uint32_t uiDataSize, bool bDirectSend);
							netMessage(const netMessage& obj);
							~netMessage();
		netMessage&			operator=(const netMessage& obj);

		bool				complete() { return m_bComplete; };
		int					readData( const char *pch, unsigned int nBytes );
		uint256&			GetMessageHash();
		int64_t				i64tTimeStart;
		int64_t				i64tTimeDelta;

		unsigned int		getPackageSize() { return sizeof(sHeader) + m_sHeader.ui32tPayloadSize; };
		void*				getPackage() { return m_pBuffer; };
		char*				getData() { return (m_pBuffer+sizeof(sHeader)); };

		sHeader				getHeader() { return m_sHeader; };
};