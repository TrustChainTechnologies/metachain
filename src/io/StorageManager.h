#pragma once

/*********************************************************************
* Copyright (c) 2017 TCT DEVs	                                     *
* Distributed under the GPLv3.0 software license					 *
* contact us before using our code									 *
**********************************************************************/

#ifndef __STORAGEMANAGER_H__
#define __STORAGEMANAGER_H__ 1

#include <string>
#include <fstream>
#include <rocksdb/db.h>
#include <boost/filesystem/path.hpp>
#include "../MetaChain.h"
#include "../SimpleIni.h"
#include "db/dbEngine.h"
#include "../MCP/MCP02/SubChainManager.h"

class MetaChain;

/*
this class handles the storage of the blocks and their metadata.
the functionality of this class differs in mode FN and CL, it also instances the db backend configured in the .ini File
*/

class StorageManager
{
	private:
		MetaChain							*m_pMC;
		// the dbEngine defines where block informations will be stored.
		// this won't store the full blocks, they are in raw format stored somewhere else.
		// the type of dbEngine can be chosen in the ini
		dbEngine							*m_pDB;

		// subchains in the metachains with their functions
		MCP02::SubChainManager				*m_pSubChainManager;

		// meta db and convenience functions
		rocksdb::DB							*m_pMetaDB;
		inline bool							getMetaValueBool(std::string strKey, bool bDefault);

		template<class obj>
		inline void							MetaSerialize(std::string strKey, obj *ptr)
		{
			LOCK(ptr->csAccess);
			std::stringstream stream(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
			boost::archive::binary_oarchive oa(stream);
			oa << ptr;
			if (!m_pMetaDB->Put(rocksdb::WriteOptions(), strKey, rocksdb::Slice(stream.str().data(), stream.tellp())).ok())
				LOG_ERROR("Unable to serialize " + strKey, "SM");
#ifdef _DEBUG
			else
				LOG_DEBUG("Serialized " + strKey, "SM");
#endif
		};

		template<class obj>
		void							MetaDeserialize(std::string strKey, obj *ptr)
		{
			if (*ptr)
			{
				delete *ptr;
				*ptr = NULL;
			}
			std::string strTmp;
			if (m_pMetaDB->Get(rocksdb::ReadOptions(), strKey, &strTmp).ok())
			{
				std::stringstream stream(strTmp);
				boost::archive::binary_iarchive ia(stream);
				ia >> *ptr;
#ifdef _DEBUG
				LOG_DEBUG("Deserialized " + strKey, "SM");
#endif
			}
			else
				LOG_ERROR("Unable to deserialize " + strKey, "SM");
		};

		boost::filesystem::path				m_pathDataDirectory;
		boost::filesystem::path				m_pathRawDirectory;

		// functions and variables for raw file output
		cCriticalSection					m_csRaw;
		std::ofstream						m_streamRaw;
		boost::filesystem::path				m_fileRaw;
		uintmax_t							m_uimRawFileSize;
		uintmax_t							m_uimRawFileSizeMax;
		bool								openRawFile();

public:
											StorageManager(MetaChain *mc);
											~StorageManager();
	bool									initialize(CSimpleIniA* iniFile);
	void									writeRaw(unsigned int uiBlockNumber, unsigned int uiLength, void *raw);
	uint16_t								getChainIdentifier(std::string strChainIdentifier) { return m_pSubChainManager->getChainIdentifier(strChainIdentifier); };
	std::string								getChainIdentifier(uint16_t uint16ChainIdentifier) { return m_pSubChainManager->getChainIdentifier(uint16ChainIdentifier); };
};

#ifndef _DEBUG
	// NullLogger class for rocksdb on release
	class RocksDBNullLogger : public rocksdb::Logger {
	public:
		using rocksdb::Logger::Logv;
		virtual void Logv(const char* format, va_list hap) override {}
		virtual size_t GetLogFileSize() const override { return 0; }
	};
#endif

#endif
