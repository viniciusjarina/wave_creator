//////////////////////////////////////////////////////////////////////
// Wave.cpp: implementation of the CWave class.
//
// This class implements an object that reads/writes a wave file.
//
//The following member functions are used to perform all the tasks:
//
//	UINT InOpen( CString* pPathName, WAVEFORMATEX* pWFX,
//									DWORD BufSize, PDWORD FileSize);
//	LONG InRead( PSHORT pData, INT Length );
//	void InClose();
//
//	UINT OutOpen( CString* pPathName, WAVEFORMATEX* pWFX,
//									DWORD BufSize, DWORD SampleLimit);
//	LONG OutWrite( PSHORT pData, INT Length  );
//	void OutClose();
//
//	LONGLONG GetDriveFreeSpace(CString* pPathName);
//
//////////////////////////////////////////////////////////////////////
// Copyright 1999.    Moe Wheatley AE4JY  <ae4jy@mindspring.com>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wave.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWave::CWave()
{
	m_INhmmio = FALSE;
	m_OUThmmio = FALSE;
	m_OutputOpen = FALSE;
	m_pInBuffer = NULL;
	m_pOutBuffer = NULL;
	m_InputOpen = FALSE;
	m_OutputOpen = FALSE;
}

CWave::~CWave()
{

}

//////////////////////////////////////////////////////////////////////
//  This function opens a wave file for reading.  It reads the wave file
//  information and places it in *pWFXSettings.  The file size in
//	number of samples is placed in *pFileSize. Number of samples can be
//  1,2 or 4 bytes long depending on bits per sample and number of channels.
//( ie. a 1000 sample buffer for 16 bit stereo will be a 4000 byte buffer)
//    parameters:
//		pPathName		= CString containing file path name.
//		pWFXSettings	= ptr to WAVEFORMATEX structure to put wavefile settings in.
//		BufSize			= DWORD specifies the wave file buffer size in
//							number of samples.  If the file ends before a 
//							buffer is completed, it is padded with 0's.
//						  If this value is Zero, the WAVEFORMATEX structure is
//							filled in and then the file is closed. This is useful
//							for just getting file info.
//		pFileSize		= pointer where the file length in number of samples is
//							written to.
//    returns:
//        0			if opened OK
//    ErrorCode		if not
//////////////////////////////////////////////////////////////////////
UINT CWave::InOpen( CString* pPathName, WAVEFORMATEX* pWFX, DWORD BufSize, PDWORD pFileSize)
{
LONG dwFmtSize;
MMCKINFO mmckinSubchunk;
MMCKINFO mmckinfoParent;
	m_ErrorCode = NO_ERRORS;
	m_pInBuffer = NULL;
	m_InBufPosition = 0;
	m_InputOpen = FALSE;
	if( !(m_INhmmio = mmioOpen( (LPTSTR)(LPCTSTR(*pPathName)), NULL,
									MMIO_READ|MMIO_ALLOCBUF)))
	{
		m_ErrorCode = WAVIN_ERR_OPEN;
	}
	else					//traverse the RIFF header to get info on file
	{
		mmckinfoParent.fccType = mmioFOURCC('W', 'A','V','E');
		if( mmioDescend( m_INhmmio, &mmckinfoParent, NULL,MMIO_FINDRIFF))
		{
			mmioClose(m_INhmmio,0);
			m_ErrorCode = WAVIN_ERR_NOTWAVE;
		}
		else
		{
			mmckinSubchunk.ckid = mmioFOURCC('f', 'm','t',' ');
			if( mmioDescend( m_INhmmio, &mmckinSubchunk,
					&mmckinfoParent,MMIO_FINDCHUNK))
			{
				mmioClose(m_INhmmio,0);
				m_ErrorCode = WAVIN_ERR_INVALID;
			}
			else
			{
				dwFmtSize = mmckinSubchunk.cksize;
				if( mmioRead( m_INhmmio, (HPSTR) &m_InFormat, dwFmtSize)
								!= dwFmtSize)
				{
					mmioClose(m_INhmmio,0);
					m_ErrorCode = WAVIN_ERR_INVALID;
				}
				else
				{
					mmioAscend( m_INhmmio, &mmckinSubchunk, 0);
					mmckinSubchunk.ckid = mmioFOURCC('d', 'a','t','a');
					if( mmioDescend( m_INhmmio, &mmckinSubchunk, 
							     &mmckinfoParent,MMIO_FINDCHUNK))
					{
						mmioClose(m_INhmmio,0);
						m_ErrorCode = WAVIN_ERR_INVALID;
					}
					else
					{
						*pFileSize = mmckinSubchunk.cksize;
						if ( *pFileSize == 0L )
						{
							m_ErrorCode = WAVIN_ERR_NODATA;
							mmioClose(m_INhmmio,0);
						}
						else
						{
							if( ( m_InFormat.wFormatTag != WAVE_FORMAT_PCM )
									|| ( m_InFormat.nChannels != 1 &&
											m_InFormat.nChannels != 2 )
									|| ( m_InFormat.wBitsPerSample != 8 &&
									m_InFormat.wBitsPerSample != 16 ))
							{
								m_ErrorCode = WAVIN_ERR_NOTSUPPORTED;
								mmioClose(m_INhmmio,0);
							}
							else
							{
								if( ( m_InFormat.nSamplesPerSec < 5000 ) ||
									( m_InFormat.nSamplesPerSec > 44100 ) )
								{
									m_ErrorCode = WAVIN_ERR_NOTSUPPORTED;
									mmioClose(m_INhmmio,0);
								}
							}
						}
					}
				}
			}
		}
	}
	if( m_ErrorCode == NO_ERRORS )
	{
		//  write file settings to the "pWFX" structure of the caller.
		pWFX->wFormatTag = m_InFormat.wFormatTag;
		pWFX->nChannels = m_InFormat.nChannels;
		pWFX->nSamplesPerSec = m_InFormat.nSamplesPerSec;
		pWFX->nAvgBytesPerSec = m_InFormat.nAvgBytesPerSec;
		pWFX->nBlockAlign = m_InFormat.nBlockAlign;
		pWFX->wBitsPerSample = m_InFormat.wBitsPerSample;
		m_InBytesPerSample = (m_InFormat.wBitsPerSample/8)*m_InFormat.nChannels;
		*pFileSize = *pFileSize / m_InBytesPerSample;//get # of samples
		// FileSize must be integer number of BufSizes due to padding
		if( BufSize != 0 )  // if not a file check
			*pFileSize += ( BufSize - (*pFileSize%BufSize) );
		// allocate intermediate byte buffer
		m_InBufferSize = BufSize * m_InBytesPerSample;
		m_pInBuffer = new char[ m_InBufferSize ];
		if( m_pInBuffer == NULL )
		{
			InClose();
			m_ErrorCode = MEMORY_ERROR;
		}
		else
		{
			m_InputOpen = TRUE;
		}
	}
	if( BufSize == 0 )  // see if just a file check
		InClose();	// if so close the input file
	return(m_ErrorCode);
}

///////////////////////////////////////////////////////////////////////
// Reads 'Length' samples of double data from the opened wavefile.
//		pData	= Pointer to double data array to be filled.
//		Length	= Number of samples to read into pData.
//
//	returns:
//		Length	if 'Length' samples were succesfully read.
//		0  =	if reaches the end of the file.
//		-1 =	if there is an error ( use GetError() to retrieve error )
///////////////////////////////////////////////////////////////////////
LONG CWave::InRead( double* pData , INT Length )
{
INT i;
LONG numret;
	if( !m_InputOpen )
	{
		InClose();
		m_ErrorCode = WAVIN_ERR_NOTOPEN;
		return -1;
	}
	if( m_InBufPosition == 0 )	//if need to read in new buffer's worth
	{
		numret = mmioRead( m_INhmmio, m_pInBuffer, m_InBufferSize );
		if( numret < 0 )		//read error
		{
			InClose();
			m_ErrorCode = WAVIN_ERR_READING;
			return -1;
		}
		else
		{
			if( numret == 0 )		//end of file
			{
				InClose();
				return 0;
			}
			else
			{
				if( numret < m_InBufferSize )	// got partial buffer so pad it
				{
					if(m_InFormat.wBitsPerSample == 8)
					{
						for( i=numret; i<m_InBufferSize; i++ )
						{
							*(m_pInBuffer+i) = (char)127;
						}
					}
					else
					{
						for( i=numret; i<m_InBufferSize; i++ )
						{
							*(m_pInBuffer+i) = 0;
						}
					}
				}
			}
		}
	}
	if(m_InFormat.wBitsPerSample == 8)
	{
		m_usTemp.bytes.lsb = 0;
		for( i=0; i < (Length*m_InFormat.nChannels); i++ )
		{
			m_usTemp.bytes.msb = *(m_pInBuffer + (m_InBufPosition++) ) - 128;
			*(pData+i) = (double)m_usTemp.both;
		}
	}
	else
	{
		for( i=0; i < (Length*m_InFormat.nChannels); i++ )
		{
			m_usTemp.bytes.lsb = *(m_pInBuffer + (m_InBufPosition++) );
			m_usTemp.bytes.msb = *(m_pInBuffer + (m_InBufPosition++) );
			*( pData + i) = (double)m_usTemp.both;
		}
	}
	if( m_InBufPosition >= m_InBufferSize )
		m_InBufPosition = 0;	// need to read another buffer's worth
	return(Length);
}


//////////////////////////////////////////////////////////////////////
//  Closes the Input wave file if open.
//
//////////////////////////////////////////////////////////////////////
void CWave::InClose()
{
	if( m_INhmmio )
	{
		mmioClose( m_INhmmio, 0 );
		m_INhmmio = NULL;
	}
	if( m_pInBuffer )
	{
		delete m_pInBuffer;
		m_pInBuffer = NULL;
	}
	m_InputOpen = FALSE;
}

//////////////////////////////////////////////////////////////////////
//  This function opens a wave file for writing.
//		Number of samples can be 1,2 or 4 bytes long depending on
//      bits per sample and number of channels.
//( ie. a 1000 sample buffer for 16 bit stereo will be a 4000 byte buffer)
//    parameters:
//		pPathName	= CString containing file path name
//		pWFX		= WAVEFORMATEX structure with desired wavefile settings
//		BufSize		= DWORD specifies the wave file buffer size in
//							number of samples.  
//		SampleLimit = limit on number of samples to be written.
//    returns:
//        0			if opened OK
//    ErrorCode		if not
//////////////////////////////////////////////////////////////////////
UINT CWave::OutOpen( CString* pPathName, WAVEFORMATEX* pWFX, 
										DWORD BufSize, DWORD SampleLimit)
{
	GetFreeSpace(pPathName);
	m_OutputOpen = FALSE;
	m_ErrorCode = NO_ERRORS;
	m_pOutBuffer = NULL;
	m_OutBufPosition = 0;
	m_OutSamplesWritten = 0;
	m_OutSampleLimit = SampleLimit;
	memset( &m_mmcktRIFF,0,sizeof(MMCKINFO));
	memset( &m_mmcktSubchunk,0,sizeof(MMCKINFO));
	m_OutFormat = *pWFX;
	m_OutBytesPerSample = (m_OutFormat.wBitsPerSample/8)*m_OutFormat.nChannels;
	m_OutBufferSize = BufSize * m_OutBytesPerSample;
	if( !(m_OUThmmio = mmioOpen( (LPTSTR)(LPCTSTR(*pPathName)), NULL,
									MMIO_WRITE|MMIO_ALLOCBUF|MMIO_CREATE )))
	{
		m_ErrorCode = WAVOUT_ERR_OPEN;
	}
	else
	{   // Create the output file RIFF chunk of form type 'WAVE'.
		m_mmcktRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if( mmioCreateChunk(m_OUThmmio, &m_mmcktRIFF, MMIO_CREATERIFF) != 0)
		{
			mmioClose(m_OUThmmio,0);
			m_ErrorCode = WAVOUT_ERR_WRITING;
		}
		else
		{	// Are now descended into the 'RIFF' chunk just created.
			// Now create the 'fmt ' chunk. Since the size of this chunk is known,
			// specify it in the MMCKINFO structure so MMIO doesn't have to seek
			// back and set the chunk size after ascending from the chunk.
			m_mmcktSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
			m_mmcktSubchunk.cksize = sizeof(WAVEFORMATEX);  // we know the size of this ck.
			if( mmioCreateChunk(m_OUThmmio, &m_mmcktSubchunk, 0) != 0)
			{
				mmioClose(m_OUThmmio,0);
				m_ErrorCode = WAVOUT_ERR_WRITING;
			}
			else
			{	// Write the WAVEFORMATEX structure to the 'fmt ' chunk.
				if( mmioWrite(m_OUThmmio, (HPSTR)pWFX, sizeof(WAVEFORMATEX))
							!= sizeof(WAVEFORMATEX))
				{
					mmioClose(m_OUThmmio,0);
					m_ErrorCode = WAVOUT_ERR_WRITING;
				}
				else
				{	// Ascend out of the 'fmt ' chunk, back into the 'RIFF' chunk.
					if( mmioAscend(m_OUThmmio, &m_mmcktSubchunk, 0) != 0)
					{
						mmioClose(m_OUThmmio,0);
						m_ErrorCode = WAVOUT_ERR_WRITING;
					}
					else
					{	// Create the 'data' chunk that holds the waveform samples.
						m_mmcktSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
						if (mmioCreateChunk(m_OUThmmio, &m_mmcktSubchunk, 0) != 0)
						{
							mmioClose(m_OUThmmio,0);
							m_ErrorCode = WAVOUT_ERR_WRITING;
						}
					}
				}
			}
		}
	}

	if( m_ErrorCode == NO_ERRORS )
	{
		// allocate intermediate byte buffer
		m_pOutBuffer = new char[ m_OutBufferSize ];
		if( m_pOutBuffer == NULL )
		{
			OutClose();
			m_ErrorCode = MEMORY_ERROR;
		}
		else
		{
			m_OutputOpen = TRUE;
		}
	}
	return m_ErrorCode;
}

///////////////////////////////////////////////////////////////////////
// Writes 'Length' doubles to the open wavefile.
//    parameters:
//		pData	= pointer to block of 'Length' SHORT INT's to output.
//		Length	= Number of samples to write from pData. If is zero
//					then the output file is closed.
//
// Returns:
//		'Length' if data was succesfully placed in the output buffers.
//       0 if output has finished( reached the specified sample limit )
//      -1		 if error ( use GetError() to retrieve error )
///////////////////////////////////////////////////////////////////////
LONG CWave::OutWrite( double* pData, INT Length)
{
INT i;
	if( !m_OutputOpen )
	{
		OutClose();
		m_ErrorCode = WAVOUT_ERR_NOTOPEN;
		return -1;
	}
	if( Length == 0 )
	{
		if( mmioWrite( m_OUThmmio, m_pOutBuffer, m_OutBufPosition ) < 0 )
		{
			OutClose();
			m_ErrorCode = WAVOUT_ERR_WRITING;
			return -1;
		}
		OutClose();
		return 0;
	}
	if( Length > 0 )
	{
		if(m_OutFormat.wBitsPerSample == 16)
		{
			for( i=0; i < (Length*m_OutFormat.nChannels); i++ )
			{
				m_usTemp.both = (SHORT)(*( pData + i));
				*(m_pOutBuffer + (m_OutBufPosition++)) = m_usTemp.bytes.lsb;
				*(m_pOutBuffer + (m_OutBufPosition++)) = m_usTemp.bytes.msb;
			}
		}
		else
		{
			for( i=0; i < (Length*m_OutFormat.nChannels); i++ )
			{
				m_usTemp.both = (SHORT)(*(pData + i) + 32768.0);
				*(m_pOutBuffer + (m_OutBufPosition++)) = (char)m_usTemp.bytes.lsb;
			}
		}
	}
	if( m_OutBufPosition >= m_OutBufferSize )
	{
		m_OutBufPosition = 0;	// need to write another buffer's worth
		if( mmioWrite( m_OUThmmio, m_pOutBuffer, m_OutBufferSize ) < 0 )
		{
			OutClose();
			m_ErrorCode = WAVOUT_ERR_WRITING;
			return -1;
		}
	}
	m_OutSamplesWritten += Length;
	if( m_OutSamplesWritten >= m_OutSampleLimit )
	{
		OutClose();
		return 0;
	}
	return Length;
}

//////////////////////////////////////////////////////////////////////
//  Closes the Output wave file by first updating the information chunk
//    with the length of data that was written then closing it.
//////////////////////////////////////////////////////////////////////
void CWave::OutClose()
{
   if( m_OUThmmio )
   {
	   if( m_OutputOpen )	// need to update file and data lengths in
	   {					// the wave header if the file was opened
	   		if( mmioWrite( m_OUThmmio, m_pOutBuffer, m_OutBufPosition ) >= 0 )
			{
				// Ascend out of the 'data' chunk, back into the 'RIFF' chunk.
				if( mmioAscend(m_OUThmmio, &m_mmcktSubchunk, 0) == 0)
					// Ascend out of the 'RIFF' chunk, writing the RIFF file length.
					mmioAscend(m_OUThmmio, &m_mmcktRIFF, 0);
			}
	   }
		mmioClose( m_OUThmmio, 0 );
		m_OUThmmio = FALSE;
	}
	if( m_pOutBuffer )
	{
		delete m_pOutBuffer;
		m_pOutBuffer = NULL;
	}
	m_OutputOpen = FALSE;
}

/////////////////////////////////////////////////////////////////
// returns free disk space on drive specified by pPathName.
// The extra logic is for dealing with early Win95 versions that don't
// support the extended disk space function "GetDiskFreeSpaceEx(..)".
/////////////////////////////////////////////////////////////////
LONGLONG CWave::GetDriveFreeSpace(CString* pPathName)
{
typedef BOOL (CALLBACK* LPFNDLLFUNC)(LPCTSTR, PULARGE_INTEGER,
					 PULARGE_INTEGER, PULARGE_INTEGER);

LONGLONG SpaceFree;
	SpaceFree = 0;					// see if it exists
	HMODULE hKernel = GetModuleHandle( _T("kernel32.dll"));
	LPFNDLLFUNC pFunct = (LPFNDLLFUNC)GetProcAddress( hKernel,"GetDiskFreeSpaceExA");
	if(pFunct != NULL)		//if function is in kernel32.dll
	{
		ULARGE_INTEGER TotalSpace;
		ULARGE_INTEGER FreeSpace;
		pFunct( pPathName->Left(3), &FreeSpace, &TotalSpace, NULL);
		SpaceFree = FreeSpace.QuadPart;
	}
	else					//else is WIn95 pre OSR2 so use old function
	{
		DWORD SectorsPerCluster, BytesPerSector;
		DWORD NumberOfFreeClusters, TotalNumberOfClusters;
		if( GetDiskFreeSpace( pPathName->Left(3),
					&SectorsPerCluster, &BytesPerSector,
					&NumberOfFreeClusters, &TotalNumberOfClusters ) ) 
		SpaceFree = (LONGLONG)(NumberOfFreeClusters*SectorsPerCluster*BytesPerSector);
	}
	return SpaceFree;
}
