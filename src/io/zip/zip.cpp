/*********************************************************************
* Copyright (c) 2017 TCT DEVs	                                     *
* Distributed under the GPLv3.0 software license					 *
* contact us before using our code									 *
**********************************************************************/

#include "zip.h"
#include <zlib.h>
#include <algorithm>
#include <sstream>
#include <time.h>

namespace ZIP
{
	const unsigned int BUFSIZE = 2048;

	
	// Default constructor
	Zip::Zip() :
		zipFile_( 0 ), 
		entryOpen_( false )
	{
	}

	// Default destructor
	Zip::~Zip(void)
	{
		close();
	}

	// Create a new zip file.
	// param:
	//		filename	path and the filename of the zip file to open
	//		append		set true to append the zip file
	// return:
	//		true if open, false otherwise
	bool Zip::open( const char* filename, bool append ) 
	{
		close();
		zipFile_ = zipOpen64( filename, append?APPEND_STATUS_ADDINZIP:0 );

		return isOpen();
	}

	// Close the zip file
	void Zip::close()
	{
		if ( zipFile_ )
		{
			closeEntry();
			zipClose( zipFile_, 0 );
			zipFile_ = 0;
		}
	}

	// Check if a zipfile is open.
	// return:
	//		true if open, false otherwise
	bool Zip::isOpen()
	{
		return zipFile_ != 0;
	}

	// Create a zip entry; either file or folder. Folder has to 
	// end with a slash or backslash.
	// return:
	//		true if open, false otherwise
	bool Zip::addEntry( const char* filename )
	{
		if ( isOpen() )
		{
			closeEntry();

			while( filename[0] == '\\' || filename[0] == '/' )
			{
				filename++;
			}
			
			//?? we dont need the stinking time
			zip_fileinfo zi = {0};
			getTime( zi.tmz_date );

			int err = zipOpenNewFileInZip( zipFile_, filename, &zi,
				NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION );

			entryOpen_ = (err == ZIP_OK);
		}
		return entryOpen_;
	}

	// Close the currently open zip entry.
	void Zip::closeEntry()
	{
		if ( entryOpen_ )
		{
			zipCloseFileInZip( zipFile_ );
			entryOpen_ = false;
		}
	}

	// Check if there is a currently open file zip entry.
	// return:
	//		true if open, false otherwise
	bool Zip::isOpenEntry()
	{
		return entryOpen_;
	}
	
	// Stream operator for dumping data from an input stream to the 
	// currently open zip entry.
	Zip& Zip::operator<<( std::istream& is )
	{
		int err = ZIP_OK;
		char buf[BUFSIZE];
		unsigned long nRead = 0;

		if ( isOpenEntry() )
		{
			while (err == ZIP_OK && is.good() )
			{
				is.read( buf, BUFSIZE );
				unsigned int nRead = (unsigned int)is.gcount();

				if ( nRead )
				{
					err = zipWriteInFileInZip( zipFile_, buf, nRead );
				}
				else
				{
					break;
				}
			}
		}
		return *this;
	}

	// Fill the zip time structure
	// param:
	//		tmZip	time structure to be filled
	void Zip::getTime(tm_zip& tmZip)
	{
		time_t rawtime;
		time (&rawtime);
		auto timeinfo = localtime(&rawtime);
		tmZip.tm_sec = timeinfo->tm_sec;
		tmZip.tm_min = timeinfo->tm_min;
		tmZip.tm_hour = timeinfo->tm_hour;
		tmZip.tm_mday = timeinfo->tm_mday;
		tmZip.tm_mon = timeinfo->tm_mon;
		tmZip.tm_year = timeinfo->tm_year;
	}
};
