#pragma once

/*********************************************************************
* Copyright (c) 2017 TCT DEVs	                                     *
* Distributed under the GPLv3.0 software license					 *
* contact us before using our code									 *
**********************************************************************/

#ifndef __MCP04_POMC_H__
#define __MCP04_POMC_H__ 1

#include "PoPInterface.h"
#include "../MCP03/MC/mcTransaction.h"
#include "../MCP03/MC/mcBlock.h"
#include "../MCP02/SubChainManager.h"

namespace MCP04
{
	class PoMC : public PoPInterface
	{
		protected:
			static const std::string			m_strName;

		public:
			static bool							registerFactory() { return ::MetaChain::getInstance().getStorageManager()->getSubChainManager()->registerPoPFactory(m_strName, &createInstance); };
			static bool							registerFactory(MCP02::SubChainManager *ptr) { return ptr->registerPoPFactory(m_strName, &createInstance); };
			static PoPInterface					*createInstance() { return new PoMC(); };

			MCP03::Transaction*					createTXElement() { return new MCP03::MetaChain::mcTransaction; };
			MCP03::Block*						createBlockElement() { return new MCP03::MetaChain::mcBlock; };
	};
}
#endif