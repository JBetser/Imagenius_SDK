#pragma once

#include <atlbase.h>
#include <atlenc.h>
#include <ObjIdl.h>	// IStream
#include <sstream>
#include <fstream>

// some constants
#define XMLNODE_COMPRESSED_DATA      L"CompressedData"
#define XMLATTRIBUTE_ORIGINAL_SIZE   L"originalSize"

class IGPacker
{
public:
	IGPacker();
	~IGPacker();

public:
	static HRESULT gzipAndEncode64( const CComBSTR& contentInput, CComBSTR& base64Output);
	static HRESULT gzipAndEncode64(IStream& refIStream, CComBSTR& base64Output);

	static HRESULT decode64AndGunzip(CComBSTR& base64Input, IStream** ppStream);
	static HRESULT decode64AndGunzip(CComBSTR& base64Input, CComBSTR& contentOutput);

	/// The client is in charge of freeing the output
    static unsigned long gzipCompress(const BYTE input[], unsigned long inputLen, BYTE *output[]);
	static unsigned long gzipCompress(const CComBSTR &input, BYTE *output[]);
	static int			 gzipUncompress(const BYTE input[], unsigned long inputLen, std::ostream & output, int&) ;

	static long decode64( const wchar_t* lpszEncodedContent, char *& decodedBuffer );

	static HRESULT encode64(BYTE *compressedBuffer, unsigned long compressedDataLen, CComBSTR& base64Output);

	static HRESULT convertBase64ToStream(const CComBSTR &base64Input, IStream **ppStream);
	static HRESULT convertStreamToBase64(IStream &refIStream, CComBSTR &base64Output);

	static int gzip_compress( BYTE *dest, ULONG *destLen, const BYTE *source, ULONG sourceLen, int level = -1); // -1 = Z_DEFAULT_COMPRESSION);

	/// The client is in charge of freeing the output
    static unsigned long compressBound(const CComBSTR &input);
    static unsigned long compressBound(unsigned long inputBytesLen);
};

