/*********************************************************************
* Copyright (c) 2017 TCT DEVs	                                     *
* Distributed under the GPLv3.0 software license					 *
* contact us before using our code									 *
**********************************************************************/

#include "mcTransaction.h"
#include <sstream>
#include <boost/archive/binary_oarchive.hpp>
#include "../../../crypto/sha3.h"
#include "../../../tinyformat.h"

namespace MCP04
{
	namespace MetaChain
	{
		mcTransaction::mcTransaction(uint16_t Version)
			: Transaction(Version)
		{
		}

		mcTransaction::~mcTransaction()
		{
		}

		uint256 mcTransaction::getHash()
		{
			// serialize this transaction
			std::stringstream stream(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
			{
				boost::archive::binary_oarchive oa(stream, boost::archive::no_header | boost::archive::no_tracking);
				oa << *this;
			}

			// return the calculated hash
			SHA3 crypto;
			return crypto.hash256(SHA3::HashType::DEFAULT, (uint8_t *)stream.str().data(), stream.str().size());
		}

		uint32_t mcTransaction::getTotalSize()
		{
			// serialize this transaction
			std::stringstream stream(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
			{
				boost::archive::binary_oarchive oa(stream, boost::archive::no_header | boost::archive::no_tracking);
				oa << *this;
			}

			return stream.str().size();
		}

		std::string mcTransaction::toString()
		{
			std::stringstream s;
			s << strprintf("mcTransaction(Hash=%s, Version=%d)\n",
				getHash().ToString().substr(0, 10),
				uint16tVersion
			);
			s << "    " + txIn.toString() + "\n";

			return s.str();
		}
	}
}