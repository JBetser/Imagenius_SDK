#include "StdAfx.h"
#include "IGPacker.h"
#include "CxImage600/zlib/zlib.h"

#define Z_GZIP_COMPLIANT (MAX_WBITS + 16) // + 16 means GZIP

//-------------------------------------------------------------------------------------------------
IGPacker::IGPacker()
//-------------------------------------------------------------------------------------------------
{}
//-------------------------------------------------------------------------------------------------
IGPacker::~IGPacker()
//-------------------------------------------------------------------------------------------------
{}


int IGPacker::gzipUncompress(const BYTE input[], unsigned long inputLen, std::ostream & output, int& outputsize)
{
	const int chunk = 256 * 1024; 

	unsigned char out[chunk]; 
	z_stream strm; 
    strm.zalloc = (alloc_func)Z_NULL;
    strm.zfree = (free_func)Z_NULL;
    strm.opaque = (voidpf)Z_NULL;
	strm.avail_in = 0; 
	strm.next_in = Z_NULL; 

	outputsize = 0;

	int ret = inflateInit2(&strm, Z_GZIP_COMPLIANT); 
    if (ret != Z_OK)
	{
        return ret; 
	}

	strm.avail_in = (uInt) inputLen; 
	strm.next_in = (Bytef*) input; 
	do        
	{ 
		strm.avail_out = chunk; 
		strm.next_out = out; 
		ret = inflate(&strm, Z_NO_FLUSH); 
		if(ret == Z_STREAM_ERROR ||
		   ret == Z_NEED_DICT ||
		   ret == Z_DATA_ERROR ||               
		   ret == Z_MEM_ERROR)            
		{ 
			inflateEnd(&strm); 
			//throw std::runtime_error("inflate"); 
			return ret ;

		} 
		output.write(reinterpret_cast<char*>(out), chunk - strm.avail_out); 
		outputsize += chunk - strm.avail_out;
	} 
	while(strm.avail_out == 0); 
	inflateEnd(&strm); 

	return Z_OK ;
}

//-------------------------------------------------------------------------------------------------
unsigned long IGPacker::gzipCompress(const BYTE input[], unsigned long inputLen, BYTE *output[])
//-------------------------------------------------------------------------------------------------
{
    /// The client is in charge of freeing the output !!!!
    if (input == NULL || output == NULL)
        return 0;

    unsigned long nCompressSize = IGPacker::compressBound(inputLen);
    BYTE *outputBuffer = new BYTE[nCompressSize];
    ::ZeroMemory( outputBuffer, nCompressSize );
    long result = IGPacker::gzip_compress( outputBuffer, &nCompressSize, input, inputLen );
    if (result == Z_BUF_ERROR)
    {
        // if the compression failed because the buffer was too small, double it!!
        delete []outputBuffer;
        nCompressSize = nCompressSize*2;
        outputBuffer = new BYTE[nCompressSize];
        ::ZeroMemory( outputBuffer, nCompressSize );
        result = IGPacker::gzip_compress( outputBuffer, &nCompressSize, input, inputLen );
    }
    if (result != Z_OK)
    {
        delete []outputBuffer;
        return 0;
    }

    *output = (BYTE*)outputBuffer;
    return nCompressSize;
}


//-------------------------------------------------------------------------------------------------
HRESULT IGPacker::encode64(BYTE *compressedBuffer, unsigned long compressedDataLen, CComBSTR &base64Output)
//-------------------------------------------------------------------------------------------------
{
	// Now we have to encode binary data to be able to transport them embedded in XML
	int encodedBufferLen = 0;
	char *encodedBuffer = NULL;

	// add an overhead of 40% for base64
	encodedBufferLen = ::Base64EncodeGetRequiredLength(compressedDataLen)+1;
	encodedBuffer = new char[encodedBufferLen];
	::ZeroMemory(encodedBuffer, encodedBufferLen*sizeof(char));
	//
	bool isEncodedSuccess = ::Base64Encode(compressedBuffer, compressedDataLen, encodedBuffer, &encodedBufferLen) != FALSE;
	if (isEncodedSuccess)
	{
		base64Output = encodedBuffer;
	}

	delete []encodedBuffer;

	return S_OK;
}

//-------------------------------------------------------------------------------------------------
long IGPacker::decode64(const wchar_t* lpszEncodedContent, char *& decodedBuffer)
//-------------------------------------------------------------------------------------------------
{
	// Prepare the encoded content to be restored
	std::string inputBuffer;
	int nbCharacters = ::WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK|WC_SEPCHARS,lpszEncodedContent ,-1,NULL,0,NULL,NULL);
	// if len is -1, length returned by MultiByteToWideChar includes zero terminal
	// else we have to add it
	--nbCharacters;
	if (nbCharacters > 0) 
	{		
		inputBuffer.resize(nbCharacters);
		::WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_SEPCHARS, lpszEncodedContent , -1, const_cast<char*>(inputBuffer.data()), nbCharacters, 0, 0);
	}

	// Now we have to encode binary data to be able to transport them embedded in XML
	int decodedBufferLen = ::Base64DecodeGetRequiredLength(nbCharacters);
	decodedBuffer = new char[decodedBufferLen];
	::ZeroMemory(decodedBuffer, decodedBufferLen*sizeof(char));
	bool isDecodedSuccess = ::Base64Decode(inputBuffer.data(), nbCharacters, (BYTE*)decodedBuffer, &decodedBufferLen) != FALSE;
	if (!isDecodedSuccess)
	{
		decodedBufferLen = 0;
		delete[] decodedBuffer;
		decodedBuffer = 0;
	}
	return decodedBufferLen;
}

/* ===========================================================================
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least 0.1% larger than sourceLen plus
   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

     compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/
int IGPacker::gzip_compress (
    BYTE *dest,
    ULONG *destLen,
    const BYTE *source,
    ULONG sourceLen,
    int level )//= Z_DEFAULT_COMPRESSION
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = deflateInit2(&stream, level, Z_DEFLATED, 
		Z_GZIP_COMPLIANT, // + 16 means GZIP 
		8, // default level
		Z_DEFAULT_STRATEGY);
    if (err != Z_OK) 
		return err;

    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) 
	{
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;

    err = deflateEnd(&stream);

//#ifdef _DEBUG
//	FILE * fTest = NULL;
//	// uncompress with 7-zip
//	_wfopen_s( &fTest, L"c:\\temp\\gzip1.gz", L"w+b");
//	fwrite( dest, sizeof( Bytef ), *destLen, fTest );
//	fclose( fTest );
//#endif

	return err;
}

//-------------------------------------------------------------------------------------------------
unsigned long IGPacker::gzipCompress(const CComBSTR &input, BYTE *output[])
//-------------------------------------------------------------------------------------------------
{
	if (input.Length() == 0)
		return 0;
	if (output == NULL)
		return 0;

	// destLen is the total size of the destination buffer,
	// which must be at least 0.1% larger than sourceLen plus 12 bytes. 
	// Upon exit, destLen is the actual size of the compressed buffer.
    unsigned long destLen = IGPacker::compressBound(input);

	char *outputBuffer = new char[destLen];
	
	::ZeroMemory(outputBuffer, destLen);	
	
	int result = IGPacker::gzip_compress((Bytef *)outputBuffer, &destLen, (Bytef *)input.m_str, input.Length()*sizeof(wchar_t));
	
	if (result != Z_OK)
	{
		delete []outputBuffer;
		return 0;
	}

	*output = (BYTE*)outputBuffer;

//#ifdef _DEBUG
//	FILE * fTest = NULL;
//	// uncompress with 7-zip
//	_wfopen_s( &fTest, L"c:\\temp\\gzip1.gz", L"w+b");
//	fwrite( outputBuffer, sizeof( BYTE ), destLen, fTest );
//	fclose( fTest );
//#endif

	return destLen;
}

//-------------------------------------------------------------------------------------------------
unsigned long IGPacker::compressBound(unsigned long inputBytesLen)
//-------------------------------------------------------------------------------------------------
{
    // note that ::compressBound() from zlib doesn't return enough when the size of the input is small    
    return (unsigned long)(inputBytesLen*sizeof(BYTE)*(1+0.1/100)+50*sizeof(BYTE));
}
//-------------------------------------------------------------------------------------------------
unsigned long IGPacker::compressBound(const CComBSTR &input)
//-------------------------------------------------------------------------------------------------
{
    // note that ::compressBound() from zlib doesn't return enough when the size of the input is small    
    return (unsigned long)((input.Length()+1)*sizeof(wchar_t)*(1+0.1/100)+50*sizeof(char));
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// CComBSTR helpers
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
HRESULT IGPacker::gzipAndEncode64(const CComBSTR& contentInput, CComBSTR& base64Output)
//-------------------------------------------------------------------------------------------------
{
	if (contentInput.Length() == 0)
		return S_FALSE;

	BYTE *compressedBuffer = NULL;
	// compress the data using zlib library
	unsigned long compressedDataLen = IGPacker::gzipCompress(contentInput, &compressedBuffer);		
	if (compressedDataLen == 0)
		return E_FAIL;

	HRESULT hr = IGPacker::encode64( compressedBuffer, compressedDataLen,  base64Output);

	delete []compressedBuffer;

	return hr;
}

//-------------------------------------------------------------------------------------------------
HRESULT IGPacker::decode64AndGunzip(CComBSTR& base64Input, CComBSTR& contentOutput)
//-------------------------------------------------------------------------------------------------
{
    if (base64Input.Length() == 0)
	{
        return S_OK;
	}

	char *decodedBuffer = NULL;
	int decodedBufferLen = IGPacker::decode64( (const wchar_t*) base64Input, decodedBuffer );
	if( decodedBufferLen <= 0 )
	{
		return E_FAIL ;
	}
	std::ostringstream outputStreamBuffer; 
	int nOutputLen;
	int result = gzipUncompress((BYTE*) decodedBuffer, decodedBufferLen, outputStreamBuffer, nOutputLen);
	delete[] decodedBuffer;
	if (result != Z_OK)
	{
		return E_FAIL ;
	}
	contentOutput.AppendBytes((const char*) outputStreamBuffer.str().c_str(), nOutputLen);
	return S_OK;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// IStream helpers
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
HRESULT IGPacker::decode64AndGunzip(CComBSTR& base64Input, IStream** ppStream)
//-------------------------------------------------------------------------------------------------
{
    if (!ppStream)
	{
        return E_POINTER;
	}
    *ppStream = NULL;

    if (base64Input.Length() == 0)
	{
        return S_OK;
	}

	char *decodedBuffer = NULL;
	int decodedBufferLen = IGPacker::decode64( (const wchar_t*) base64Input, decodedBuffer );
	if( decodedBufferLen <= 0 )
	{
		return E_FAIL ;
	}
	std::ostringstream outputStreamBuffer; 
	int nOutputLen;
	int result = gzipUncompress((BYTE*) decodedBuffer, decodedBufferLen, outputStreamBuffer, nOutputLen);
	delete[] decodedBuffer;
	if (result != Z_OK)
	{
		return E_FAIL ;
	}

    HRESULT hr = ::CreateStreamOnHGlobal(NULL, TRUE, ppStream);
    if (SUCCEEDED(hr) && *ppStream)
    {
        ULONG cbWritten = 0;
        hr = (*ppStream)->Write((const char*) outputStreamBuffer.str().c_str(), nOutputLen, &cbWritten);
		if (hr != S_OK)
		{
			return hr;
		}
		LARGE_INTEGER pos = {0};
		//the key is to reset the seek pointer
		hr = (*ppStream)->Seek((LARGE_INTEGER)pos, STREAM_SEEK_SET, NULL);
		if (hr != S_OK)
		{
			return hr;
		}
    }
	return hr;
}


//-------------------------------------------------------------------------------------------------
HRESULT IGPacker::gzipAndEncode64(IStream& refIStream, CComBSTR& base64Output)
//-------------------------------------------------------------------------------------------------
{
	base64Output.Empty();

	HRESULT hr = E_FAIL;
	ULONG nbByteRead = 0;

	// now move the pointer to the beginning of the stream
	LARGE_INTEGER lnOffset;
	lnOffset.QuadPart = 0;
	if(refIStream.Seek(lnOffset, STREAM_SEEK_SET, NULL) != S_OK)
	{
		ATLTRACE(L"ConvertStreamToBase64() - Failed to go to stream begining\n");
		return E_FAIL;
	}

	// Get the size of the stream
	STATSTG statsg;
	if (refIStream.Stat(&statsg, NULL) != S_OK)
	{
		return E_FAIL;
	}

	ULONG bufferSize = statsg.cbSize.LowPart;
	BYTE *buffer = new BYTE[bufferSize];
	::ZeroMemory(buffer, bufferSize);

	// Read the stream in one shot
	hr = refIStream.Read(buffer, bufferSize, &nbByteRead);
	if( FAILED(hr) )
	{
		delete [] buffer;
		return hr ;
	}

    BYTE *gzipBuffer = NULL;
    ULONG nCompressSize = IGPacker::gzipCompress(buffer, bufferSize, &gzipBuffer);
    delete [] buffer;
	if (nCompressSize == 0)
	{
		delete []gzipBuffer;
		return E_FAIL ;
	}

	hr = IGPacker::encode64(gzipBuffer, nCompressSize, base64Output);

	delete [] gzipBuffer;
	
	return hr ;
}


//-------------------------------------------------------------------------------------------------
HRESULT IGPacker::convertBase64ToStream(const CComBSTR &base64Input, IStream **ppStream)
//-------------------------------------------------------------------------------------------------
{
    if (!ppStream)
        return E_POINTER;
    *ppStream = NULL;

    if (base64Input.Length() == 0)
        return S_OK;

	char *decodedBuffer = NULL;
	int decodedBufferLen = IGPacker::decode64( (const wchar_t*) base64Input, decodedBuffer );
	if( decodedBufferLen <= 0 )
	{
		return E_FAIL ;
	}

    HRESULT hr = ::CreateStreamOnHGlobal(NULL, TRUE, ppStream);
    if (SUCCEEDED(hr) && *ppStream)
    {
        ULONG cbWritten = 0;
        hr = (*ppStream)->Write(decodedBuffer, decodedBufferLen, &cbWritten);
		if (hr != S_OK)
		{
			return hr;
		}
		LARGE_INTEGER pos = {0};
		//the key is to reset the seek pointer
		hr = (*ppStream)->Seek((LARGE_INTEGER)pos, STREAM_SEEK_SET, NULL);
		if (hr != S_OK)
		{
			return hr;
		}
    }

    delete [] decodedBuffer;

    return hr;
}

//-------------------------------------------------------------------------------------------------
HRESULT IGPacker::convertStreamToBase64(IStream &refIStream, CComBSTR &base64Output)
//-------------------------------------------------------------------------------------------------
{
	base64Output.Empty();

	// now move the pointer to the beginning of the stream
	LARGE_INTEGER lnOffset;
	lnOffset.QuadPart = 0;
	if(refIStream.Seek(lnOffset, STREAM_SEEK_SET, NULL) != S_OK)
	{
		ATLTRACE(L"ConvertStreamToBase64() - Failed to go to stream begining\n");
		return E_FAIL;
	}

	// Get the size of the stream
	STATSTG statsg;
	if (refIStream.Stat(&statsg, NULL) != S_OK)
	{
		return E_FAIL;
	}

	ULONG bufferSize = statsg.cbSize.LowPart;
	BYTE *buffer = new BYTE[bufferSize];
	::ZeroMemory(buffer, bufferSize);

	// Read the stream in one shot
	ULONG nbByteRead = 0;
	HRESULT hr = refIStream.Read(buffer, bufferSize, &nbByteRead);
	if (hr == S_OK)
	{
		hr = IGPacker::encode64(buffer, bufferSize, base64Output);
	}
	delete [] buffer;
	return hr ;
}

