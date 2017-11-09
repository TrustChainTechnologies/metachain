/*********************************************************************
* Copyright (c) 2017 TCT DEVs	                                     *
* Distributed under the GPLv3.0 software license					 *
* contact us before using our code									 *
**********************************************************************/

#include "SubChain.h"
#include "../../tinyformat.h"

namespace MCP02
{
	SubChain::SubChain()
		:m_pPoP(nullptr),
		m_pDB(nullptr)
	{
	}

	SubChain::~SubChain()
	{
		RELEASE(m_pPoP);
		RELEASE(m_pDB);
	}

	SubChain::SubChain(SubChain& obj)
	{
		makeDeepCopy(obj);
	}

	SubChain& SubChain::operator=(SubChain& obj)
	{
		makeDeepCopy(obj);
		return *this;
	}

	void SubChain::makeDeepCopy(SubChain & obj)
	{
		m_uint16ChainIdentifier = obj.m_uint16ChainIdentifier;
		strncpy(m_caChainName, obj.m_caChainName, MAX_CHAINNAME_LENGTH);
		strncpy(m_caSubChainClassName, obj.m_caSubChainClassName, MAX_SUBCHAIN_CLASSNAME_LENGTH);
		strncpy(m_caPoP, obj.m_caPoP, MAX_POP_NAME);

		m_mapParams.insert(obj.m_mapParams.begin(), obj.m_mapParams.end());

		// we transfer the instances to us and remove them from the old obj
		m_pPoP = obj.m_pPoP;
		obj.m_pPoP = nullptr;
		m_pDB = obj.m_pDB;
		obj.m_pDB = nullptr;
	}

	uint16_t SubChain::init( char caChainName[MAX_CHAINNAME_LENGTH], char caSubChainClassName[MAX_SUBCHAIN_CLASSNAME_LENGTH], char caPoP[MAX_POP_NAME], std::map< std::string, std::string > mapParams)
	{
		MetaChain::getInstance().getStorageManager()->incMetaValue(MC_NEXT_CI, (uint16_t)0, &m_uint16ChainIdentifier);
		strncpy(m_caChainName, caChainName, MAX_CHAINNAME_LENGTH);
		strncpy(m_caSubChainClassName, caSubChainClassName, MAX_SUBCHAIN_CLASSNAME_LENGTH);
		strncpy(m_caPoP, caPoP, MAX_POP_NAME);

		m_mapParams.insert(mapParams.begin(), mapParams.end());
		
		m_pPoP = MetaChain::getInstance().getStorageManager()->getSubChainManager()->getPoPInstance(m_caPoP);
		m_pDB = MetaChain::getInstance().getStorageManager()->createDBEngine(m_uint16ChainIdentifier);

		// register this instance at the storage manager
		if (!MetaChain::getInstance().getStorageManager()->registerSC(m_uint16ChainIdentifier))
		{
			LOG_ERROR("Couldn't register this SC at the SM.", "SC");
			return (std::numeric_limits<uint16_t>::max)();
		}

		return m_uint16ChainIdentifier;
	}

	std::string SubChain::toString()
	{
		std::string strBuffer;

		strBuffer = strprintf("SC #%u - '%.5s'\n", m_uint16ChainIdentifier + 1, m_caChainName);
		strBuffer += strprintf("   SubChain Class Name: %.50s\n", m_caSubChainClassName);
		strBuffer += strprintf("   PoP: '%.5s'\n", m_caPoP);
		strBuffer += strprintf("   Height: %u\n", m_pDB->get("last_block", (unsigned int)0));
#ifdef _DEBUG
		strBuffer += strprintf("   Instance Address: %p\n", this);
		strBuffer += strprintf("   DB Address: %p\n", m_pDB);
		strBuffer += strprintf("   PoP Address: %p\n", m_pPoP);
#endif
		return strBuffer;
	}
}