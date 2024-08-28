/*=============================================================================
	UnIpDrvNative.h: Native function lookup table for static libraries.
	Copyright 2000 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Brandon Reinhart
=============================================================================*/

#ifndef UNIPDRVNATIVE_H
#define UNIPDRVNATIVE_H

#if __STATIC_LINK
DECLARE_NATIVE_TYPE(IpDrv,AInternetLink);
DECLARE_NATIVE_TYPE(IpDrv,AUdpLink);
DECLARE_NATIVE_TYPE(IpDrv,ATcpLink);

#define AUTO_INITIALIZE_REGISTRANTS_IPDRV		\
	AInternetLink::StaticClass();				\
	AUdpLink::StaticClass();					\
	ATcpLink::StaticClass();					\
	UTcpNetDriver::StaticClass();				\
	UTcpipConnection::StaticClass();			\
	UMasterServerCommandlet::StaticClass();		\
	UUpdateServerCommandlet::StaticClass();		\
	UHTTPDownload::StaticClass();				\
	UCompressCommandlet::StaticClass();			\
	UDecompressCommandlet::StaticClass();

#endif

#endif
