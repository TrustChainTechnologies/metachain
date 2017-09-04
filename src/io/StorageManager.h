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
#include "rocksdb/db.h"
#include <boost/filesystem/path.hpp>
#include "../MetaChain.h"
#include "../SimpleIni.h"
#include "db/dbEngine.h"

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
		// the rdb specified here stores meta information
		rocksdb::DB							*m_pMetaDB;

		bool								m_bModeFN;
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
};

#ifndef _DEBUG
	// NullLogger class for rocksdb on release
	class RocksDBNullLogger : public rocksdb::Logger {
	public:
		using rocksdb::Logger::Logv;
		virtual void Logv(const char* format, va_list ap) override {}
		virtual size_t GetLogFileSize() const override { return 0; }
	};
#endif

#endif