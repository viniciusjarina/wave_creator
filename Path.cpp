// Path.cpp: implementation of the CPath class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Path.h"
#include "resource.h"

#include <shlobj.h>
#include <stdlib.h>
#include <io.h> 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma comment(lib, "Shlwapi.lib")
#pragma warning (disable : 4706)

//////////////////////////////////////////////////////////////////////
// CallBack for SHBrowseForFolder
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPath::CPath()
{
}


CPath::CPath(const CPath &rPath)
:CString(rPath)
{
	Canonicalize();
}


CPath::CPath(LPCTSTR lpszPath)
:CString(lpszPath)
{
	Canonicalize();
}


CPath::CPath(enumSpecialFolders eInitialFolder)
{
	SetToSpecialFolder(eInitialFolder);
}


CPath::~CPath()
{
}


bool CPath::HasBackSlash() const
{
	TCHAR ch;
	// Last char must be a backslash
	return ((ch = *(operator LPCTSTR() + GetLength() - 1)) == '\\')
		|| (ch == '/');
}


bool CPath::HasWildCards() const
{
	return HasWildCards(operator LPCTSTR());
}


bool CPath::IsFile() const
{
	TCHAR ch;
	// We must have contents and last char
	// cannot be '\' or '/'
	return (*(operator LPCTSTR())
		&& ((ch = *(operator LPCTSTR() + GetLength() - 1)) != '\\')
		&& (ch != '/'));
}


bool CPath::IsSyntaxValid() const
{
	return IsSyntaxValid(operator LPCTSTR());
}


bool CPath::Exists() const
{
	return ::GetFileAttributes(operator LPCTSTR()) 
		!= INVALID_FILE_ATTRIBUTES;
}


bool CPath::DeleteFile() const
{
	ASSERT(IsFile());
	return ::DeleteFile(operator LPCTSTR()) != 0;
}


bool CPath::Delete() const
{
	if(IsFile())
		return ::DeleteFile(operator LPCTSTR()) != 0;
	return DeleteFolder();
}


bool CPath::MoveFile(LPCTSTR szNewFileName) const
{
	ASSERT(IsFile());
	return ::MoveFile(operator LPCTSTR(), szNewFileName) != 0;
}


bool CPath::CopyFile(LPCTSTR szDestFileName, bool bFailIfExists) const
{
	ASSERT(IsFile());
	return ::CopyFile(operator LPCTSTR(), szDestFileName, bFailIfExists) != 0;
}


bool CPath::SetFileAttributes(DWORD dwFileAttributes)
{
	ASSERT(IsFile());
	return ::SetFileAttributes(operator LPCTSTR(), dwFileAttributes) != 0;
}


void CPath::DelTree()
{
	DelTreeEx(treeDelAll);
}



//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////

void CPath::SetToSpecialFolder(enumSpecialFolders eInitialFolder)
{
	DWORD dwType = REG_SZ;
	DWORD dwSize = _MAX_PATH;
	HKEY hKey;

	if(eInitialFolder == tempFileCreate)
	{
		SetToTempFileCreate();
		return;
	}

	LPTSTR	pBuf = GetBuffer(_MAX_PATH);
	switch(eInitialFolder)
	{
	case currentFolder:
		::GetCurrentDirectory(_MAX_PATH, pBuf);
		break;
	case windowsFolder:
		::GetWindowsDirectory(pBuf, _MAX_PATH);
		break;
	case appFolder:
		::GetModuleFileName(NULL, pBuf, _MAX_PATH);
		*(::PathFindFileName(pBuf)) = 0;
		break;
	case systemFolder:
		::GetSystemDirectory(pBuf, _MAX_PATH);
		break;
	case tempFolder:
		::GetTempPath(_MAX_PATH, pBuf);
		break;
	case startMenuFolder:
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_STARTMENU, false);
		break;
	case startupFolder:
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_STARTUP, false);
		break;
	case startMenuProgramsFolder:
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_PROGRAMS, false);
		break;
	case recentDocsFolder:
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_RECENT, false);
		break;
	case cookiesFolder:
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_COOKIES, false);
		break;
	case desktopFolder:
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_DESKTOPDIRECTORY, false);
		break;
	case fontsFolder:
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_FONTS, false);
		break;
	case myDocsFolder:
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_PERSONAL, false);
		break;
	case commonFilesFolder:
		if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"),
				0,
				KEY_QUERY_VALUE,&hKey) == ERROR_SUCCESS)
		{
			::RegQueryValueEx(
					hKey,
					_T("CommonFilesDir"),
					NULL,
					&dwType,
					(LPBYTE)pBuf,
					&dwSize);
			::RegCloseKey(hKey);
		}
		break;
	case programFilesFolder:
		if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"),
				0,
				KEY_QUERY_VALUE,&hKey) == ERROR_SUCCESS)
		{
			::RegQueryValueEx(
					hKey,
					_T("ProgramFilesDir"),
					NULL,
					&dwType,
					(LPBYTE)pBuf,
					&dwSize);
			::RegCloseKey(hKey);
		}
		break;
	case appsData:
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_APPDATA, false);
		break;
	case commonAppData:
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_COMMON_APPDATA, false);
		if(_tcslen(pBuf) == 0)	// Windows98 patch
			::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_APPDATA, false);
		break;

	case commonDocuments:         // All Users\Documents
#ifndef _SHFOLDER_H_
		::SHGetSpecialFolderPath(NULL, pBuf, CSIDL_COMMON_DOCUMENTS, false);
#else
	#error _SHFOLDER_H_ defined.
		// TODO:
#endif
		break;

	default:
		// Unrecognized option
		ASSERT(FALSE);
		break;
	}
	::PathAddBackslash( pBuf );
	ReleaseBuffer();
}


void CPath::SetToCurrentFolder()
{
	TCHAR *pBuf = GetBuffer(_MAX_PATH);
	::GetCurrentDirectory(_MAX_PATH, pBuf);
	::PathAddBackslash(pBuf);
	ReleaseBuffer();
}


void CPath::SetToWindowsFolder()
{
	TCHAR *pBuf = GetBuffer(_MAX_PATH);
	::GetWindowsDirectory(pBuf, _MAX_PATH);
	::PathAddBackslash( pBuf );
	ReleaseBuffer();
}


void CPath::SetToSystemFolder()
{
	TCHAR *pBuf = GetBuffer(_MAX_PATH);
	::GetSystemDirectory(pBuf, _MAX_PATH);
	::PathAddBackslash( pBuf );
	ReleaseBuffer();
}


void CPath::SetToAppFolder() 
{
	TCHAR *pBuf = GetBuffer(_MAX_PATH);
	::GetModuleFileName(NULL, pBuf, _MAX_PATH);
	ReleaseBuffer();
	ToFolder();
}


void CPath::SetToTempFolder() 
{
	TCHAR *pBuf = GetBuffer(_MAX_PATH);
	::GetTempPath(_MAX_PATH, pBuf);
	::PathAddBackslash( pBuf );
	ReleaseBuffer();
}



// Function name	: CPath::SetToTempFileCreate
// Description	    : Set object to a temporary filename in Windows Temp Folder.
//						A file with that name is created and should be deleted
//						after use.
// Return type		: void 
void CPath::SetToTempFileCreate()
{
	CPath aux;
	aux.SetToTempFolder();

	TCHAR *pBuf = GetBuffer(_MAX_PATH);
	::GetTempFileName(aux, AfxGetApp()->m_pszAppName, 0, pBuf);
	ReleaseBuffer();
}


void CPath::SetCurrentFolder() const
{
	// Must have contents
	if(!IsEmpty())
	{
		// Must we strip Folder info ?
		if(!HasBackSlash())
		{
			CString s;
			s = GetFolder();
			::SetCurrentDirectory(s);
		}
		else
			::SetCurrentDirectory(operator LPCTSTR());
	}
}



// Function name	: CPath::GetDiskFreeSpace
// Description	    : Calls ::GetDiskFreeSpace() API function. This is only
//						useful to get drive's cluster size; if not you should
//						use GetFolderFreeSpace(), that is FAT32 compliant.
// Return type		: DWORD : The number of free clusters
// Argument         : DWORD *pdwSecPerCluster: optional, the number of sectors
//						per cluster.
// Argument         : DWORD *pdwBytesPerCluster: optional, the number of bytes
//						per cluster.
// Argument         : DWORD *pdwTotalClusters=NULL: optional, the number of
//						free clusters.
DWORD CPath::GetDiskFreeSpace(DWORD *pdwSecPerCluster, DWORD *pdwBytesPerCluster,
								 DWORD *pdwTotalClusters) const
{
	// Temp storage
	DWORD dwSectPerClust;
	DWORD dwBytesPerSect;
	DWORD dwFreeClusters;
	DWORD dwTotalClusters;

	// Strip root information
	CString strFolder;
	strFolder = GetRoot();
	// Get Free space
	::GetDiskFreeSpace (strFolder, &dwSectPerClust, &dwBytesPerSect,
		&dwFreeClusters, &dwTotalClusters);
	// Copy required data
	if(pdwSecPerCluster)
		*pdwSecPerCluster = dwSectPerClust;
	if(pdwBytesPerCluster)
		*pdwBytesPerCluster = dwBytesPerSect;
	if(pdwTotalClusters)
		*pdwTotalClusters = dwTotalClusters;
	return dwFreeClusters;
}


// Function name	: CPath::GetFolderFreeSpace
// Description	    : Get free/used space of a folder, according to user
//						privileges. ::GetDiskFreeSpaceEx() API is used if OS
//						provides support; ::GetDiskFreeSpace() will be used
//						in case not (pre Win95 OSR2 releases)
// Return type		: ULONGLONG : User free space
// Argument         : ULONGLONG *pTotalSpace: Total drive space
// Argument         : ULONGLONG *pTotalAvail: Drive Free space. This is usually
//						the same as the returns type, except on some NT
//						privileged settings.
ULONGLONG CPath::GetFolderFreeSpace(ULONGLONG *pTotalSpace, ULONGLONG *pTotalAvail) const
{
	// Temp storage
	ULONGLONG	i64UserAvail;
	ULONGLONG	i64TotalSpace;
	ULONGLONG	i64TotalAvail;

	// Strip root information
	CPath strFolder(*this);

	// Try to use GetDiskFreeSpaceEx() function to handle big drives
	BOOL (WINAPI *pGetDiskFreeSpaceEx)(LPCTSTR, ULONGLONG *, ULONGLONG *, ULONGLONG *);
	pGetDiskFreeSpaceEx = (BOOL (WINAPI *)(LPCTSTR, ULONGLONG *, ULONGLONG *, ULONGLONG *))
			::GetProcAddress (GetModuleHandle(_T("kernel32.dll")), "GetDiskFreeSpaceExA");
	BOOL fResult;
	// Use GetDiskFreeSpaceEx() if found
	if (pGetDiskFreeSpaceEx)
	{
		strFolder.ToFolder();
		if(!strFolder.IsFolderAndExists())
			strFolder.ToRoot();
		fResult = pGetDiskFreeSpaceEx(strFolder, &i64UserAvail, &i64TotalSpace, &i64TotalAvail);
	}
	else	// ... use old API
	{
		strFolder.ToRoot();
		// Temp storage
		DWORD dwSectPerClust;
		DWORD dwBytesPerSect;
		DWORD dwFreeClusters;
		DWORD dwTotalClusters;
		// Get Free space
		fResult = ::GetDiskFreeSpace(strFolder, &dwSectPerClust, &dwBytesPerSect,
			&dwFreeClusters, &dwTotalClusters);
		// Bytes per cluster
		i64TotalAvail = dwSectPerClust * dwBytesPerSect;
		// Total bytes
		i64TotalSpace = i64TotalAvail * dwTotalClusters;
		// Free and User bytes
		i64UserAvail = i64TotalAvail * dwFreeClusters;
		i64TotalAvail = i64UserAvail;
	}
	// Copy desired information to the user
	if(pTotalAvail)
		*pTotalAvail = i64TotalAvail;
	if(pTotalSpace)
		*pTotalSpace = i64TotalSpace;
	// Return Free disk space
	return i64UserAvail;
}


bool CPath::IsFolderEmpty() const
{
	CFileFind   finder;
	CString patm = operator LPCTSTR();
	BOOL bWorking = finder.FindFile(patm + _T("*.*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if(!finder.IsDots())
			return false;
	}
	return true;
}


bool CPath::IsUNC() const
{
	return (bool)(::PathIsUNC(operator LPCTSTR()) == TRUE);
}


bool CPath::IsMediaMounted() const 
{
	// Must have any contents
	ASSERT(!IsEmpty());

	TCHAR bufName[MAX_PATH+1];
	CString strRoot;

	strRoot = GetRoot();

	return ::GetVolumeInformation(strRoot, bufName, MAX_PATH, 
			NULL, NULL, NULL, NULL, 0) != 0;
}



/**
	Call this method to obtain the type of drive for this path. It extracts
	the root part of the path and uses GetDriveType() API to obtain the type
	of drive.
	
\return enumDriveType  : enumeration describing the type of drive detected.

\sa	enumDriveType, GetRoot(), ::GetDriveType(). */
enumDriveType CPath::GetDriveType() const
{
	CString strRoot;

	// GetDriveType() API does not accept the full path
	strRoot = GetRoot();
	return (enumDriveType)::GetDriveType(strRoot); 
}


bool CPath::operator ==(const CPath &other) const
{
	CPath fullThis(*this), fullOther(other);
	fullThis.Canonicalize();
	fullOther.Canonicalize();
	return fullThis.CompareNoCase(fullOther) == 0;
}


void CPath::AddRoot(LPCTSTR rPath)
{
	TCHAR *pBuf = GetBuffer(_MAX_PATH);
	_tcscpy(pBuf, rPath);
	::PathStripToRoot(pBuf);
	ReleaseBuffer();
}


void CPath::AddPath(LPCTSTR rPath)
{
	TCHAR *pBuf = GetBuffer(_MAX_PATH);
	_tcsncat(pBuf, rPath, _MAX_PATH);
	::PathAddBackslash(pBuf);
	ReleaseBuffer();
}


// Function name	: CPath::AddFile
// Description	    : Add a file.
// Return type		: void 
// Argument         : LPCTSTR rFile
void CPath::AddFile(LPCTSTR rFile)
{
	ASSERT(!IsFile());

	int nLen = GetLength();

 	TCHAR *pBuf = GetBuffer(_MAX_PATH);
	_tcsncat(pBuf, rFile, _MAX_PATH - _tcslen(pBuf));
	ReleaseBuffer();
}


/**
	Copy the root/drive into rDrive

\param &rDrive : The extracted root path
 */
CString CPath::GetRoot() const
{
	CString rDrive;
	rDrive = *this;
	LPTSTR pBuf = rDrive.GetBuffer(_MAX_PATH);
	::PathStripToRoot(pBuf);
	::PathAddBackslash(pBuf);
	rDrive.ReleaseBuffer();
	return rDrive;
}



/**
	Removes everything, except the root/drive.
 */
void CPath::ToRoot()
{
	LPTSTR pBuf = GetBuffer(_MAX_PATH);
	::PathStripToRoot(pBuf);
	::PathAddBackslash(pBuf);
	ReleaseBuffer();
}


// Function name	: CPath::GetDirectory
// Description	    : This function returns only the Directory information
//						without root and filespec
// Return type		: void 
// Argument         : CString &rDir
CString CPath::GetDirectory() const
{
	CString rDir;
	// Skip root and copy folder+filespec
	rDir = ::PathSkipRoot(operator LPCTSTR());
	// Access buffer directly
	LPTSTR pBuf = rDir.GetBuffer(_MAX_PATH);
	// Remove file specification
	::PathRemoveFileSpec(pBuf);
	// Folder must have backslash (class convention)
	::PathAddBackslash(pBuf);
	// Clean up
	rDir.ReleaseBuffer();
	return rDir;
}


// Function name	: CPath::GetDirectory
// Description	    : This function removes root and filespec from the
//						contents. Only the Directory information is
//						preserved.
// Return type		: void 
void CPath::ToDirectory()
{
	// Skip root and copy folder+filespec
	CString rDir(::PathSkipRoot(operator LPCTSTR()));
	// Access buffer directly
	LPTSTR pBuf = rDir.GetBuffer(_MAX_PATH);
	// Remove file specification
	::PathRemoveFileSpec(pBuf);
	// Folder must have backslash (class convention)
	::PathAddBackslash(pBuf);
	// Clean up
	rDir.ReleaseBuffer();
	*this = rDir;
}



// Function name	: CPath::GetName
// Description	    : This function return the filename without folder or
//						extension.
// Return type		: void 
// Argument         : CString &rName
CString CPath::GetName() const
{
	CString rName;
	// Must have a file name
	ASSERT(IsFile());

	LPCTSTR pPos = ::PathFindFileName(operator LPCTSTR());
	// Get pointer to file extension
	LPCTSTR pExt = ::PathFindExtension(operator LPCTSTR());

	// PathFindExtension() should not return NULL
	ASSERT(pPos != NULL);
	ASSERT(pExt != NULL);
	// Does we have something to copy ?
	if(pPos < pExt)
	{
		// Get string buffer
		TCHAR *pBuf = rName.GetBuffer(pExt - pPos + 1);
		// Copy filename
		while(pPos < pExt)
			*pBuf++ = *pPos++;
		*pBuf = 0;
		rName.ReleaseBuffer();
	}
	return rName;
}



// Function name	: CPath::GetName
// Description	    : This function removes Folder and extension information.
//						Only the filename is preserved.
// Return type		: void 
void CPath::ToName()
{
	// Must have a file name
	ASSERT(IsFile());

	LPTSTR pBuf = GetBuffer(_MAX_PATH);
	LPCTSTR pPos = ::PathFindFileName(pBuf);
	// Get pointer to file extension
	LPTSTR pExt = ::PathFindExtension(pBuf);

	// PathFindExtension() should not return NULL
	ASSERT(pPos != NULL);
	ASSERT(pExt != NULL);
	// Does we have something to copy ?
	if(pPos == pBuf)
	{
		if(pPos < pExt)
			*pExt = 0;
	}
	else if(pPos < pExt)
	{
		ASSERT(pPos >= pBuf);
		// Copy filename
		while(pPos < pExt)
			*pBuf++ = *pPos++;
		*pBuf = 0;
	}
	ReleaseBuffer();
}



// Function name	: CPath::GetExtension
// Description	    : This function returns the file extension without
//						the period (.)
// Return type		: void 
// Argument         : CString &rExt
CString CPath::GetExtension() const
{
	CString rExt;
	rExt = "";
	if(IsFile())
	{
		// Get pointer to file extension
		LPCTSTR pExt = ::PathFindExtension(operator LPCTSTR());
		// PathFindExtension() should not return NULL
		ASSERT(pExt != NULL);
		if(*pExt == '.')
			pExt++;
		rExt = pExt;
	}
	return rExt;
}



// Function name	: CPath::GetExtension
// Description	    : This function removes all information except the
//						file extension. The extension period is also
//						removed.
// Return type		: void 
void CPath::ToExtension()
{
	CString rExt;
	if(IsFile())
	{
		// Get pointer to file extension
		LPCTSTR pExt = ::PathFindExtension(operator LPCTSTR());
		// PathFindExtension() should not return NULL
		ASSERT(pExt != NULL);
		if(*pExt == '.')
			pExt++;
		rExt = pExt;
	}
	*this = rExt;
}


void CPath::GetComponents(CString *pRoot, CString *pDir, CString *pName, CString *pExt) const
{
	// Get root component
	if(pRoot)
		*pRoot = GetRoot();
	// Get Directory component
	if(pDir)
		*pDir = GetDirectory();
	// Get File Name component
	if(pName)
		*pName = GetName();
	// File extension
	if(pExt)
		*pExt = GetExtension();
}



// Function name	: CPath::GetFolder
// Description	    : This function return the root/folder of a filename.
// Return type		: void 
// Argument         : CString &rPathPart
CString CPath::GetFolder() const
{
	CString rPathPart;
	if(IsFile())
	{
		TCHAR *pBuf = rPathPart.GetBuffer(_MAX_PATH);
		::PathCanonicalize(pBuf, operator LPCTSTR());
		if(::PathRemoveFileSpec(pBuf))
			::PathAddBackslash(pBuf);
		rPathPart.ReleaseBuffer();
	}
	else
		rPathPart = (LPCTSTR)*this;
	return rPathPart;
}



// Function name	: CPath::GetFolder
// Description	    : This function removes the filespec. Only the root
//						and directory is preserved.
// Return type		: void 
void CPath::ToFolder()
{
	CString s(operator LPCTSTR());
	if(IsFile())
	{
		LPTSTR pBuf = GetBuffer(_MAX_PATH);
		::PathCanonicalize(pBuf, s);
		if(::PathRemoveFileSpec(pBuf))
			::PathAddBackslash(pBuf);
		ReleaseBuffer();
	}
}



// Function name	: CPath::IsFolderAndExists
// Description	    : Test and verify if it is a valid folder
// Return type		: bool 
bool CPath::IsFolderAndExists() const
{
	if(!HasBackSlash())
		return false;
	CString s(operator LPCTSTR());
	s += '.';
	return _taccess((LPCTSTR)s, 0) == 0;
}


// Function name	: CPath::SelFile
// Description	    : This function opens a File Open Dialog and returns
//						the selected path.
// Return type		: bool 
// Argument         : LPCTSTR szFilter
bool CPath::SelFile(LPCTSTR szFilter)
{
	CString strFile((LPCTSTR)*this);
	CString strOutputFile;

	CString strTempFileName;

	CPath strTempPath;
	CString folder;
	CString file;

	strOutputFile.Empty();

	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hInstance = AfxGetInstanceHandle();

	CString strFilter;

	if(szFilter)
	{
		if(HIWORD(szFilter) == 0)
		{
			strFilter.LoadString((UINT)szFilter);
		}
		else
		{
			strFilter = szFilter;
		}
	}
	else
	{
		strFilter.LoadString(IDS_FILE_OPEN_ALL);
	}	

	ofn.Flags = OFN_ENABLESIZING |OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
	ofn.hwndOwner = AfxGetMainWnd()->m_hWnd;

	LPTSTR pch = strFilter.GetBuffer(0); // modify the buffer in place
	// MFC delimits with '|' not '\0'
	while ((pch = _tcschr(pch, '|')) != NULL)
		*pch++ = '\0';

	ofn.lpstrFilter = strFilter;
	ofn.nFilterIndex = 1;

	if(!strFile.IsEmpty())
	{
		strTempPath = strFile;
		folder = strTempPath.GetFolder();
		if(!folder.IsEmpty())
			ofn.lpstrInitialDir = folder;
		file = strTempPath.GetNameExtension();
		if(!file.IsEmpty())
			strTempFileName = file;
	}

	ofn.nMaxFile  = MAX_PATH;
	ofn.lpstrFile = strTempFileName.GetBufferSetLength(MAX_PATH);

	if(::GetOpenFileName(&ofn))
	{
		strTempFileName.ReleaseBuffer();

		strOutputFile = (LPCTSTR)strTempFileName;
	}

	if(!strOutputFile.IsEmpty())
	{
		*this = (LPCTSTR)strOutputFile;
		return true;
	}
	return false;
}



// Function name	: CPath::CreateFolder
// Description	    : Creates a directory level. You can also use a
//						fully qualified path+filespec.
// Return type		: bool 
bool CPath::CreateFolder() const
{
	CString s;
	// Strip folder from file?
	if(IsFile())
	{
		s = GetFolder();
	}
	else
		s = operator LPCTSTR();
	// Cannot create unknown folder
	if(s.IsEmpty())
		return false;
	// Remove backslash
	::PathRemoveBackslash(s.GetBuffer(1));
	s.ReleaseBuffer();
	return ::CreateDirectory(s, NULL) == TRUE;
}



/**
	Creates all folder tree of the specified path. The filespec is ignored.
	
\return bool  : false if failed.
*/
bool CPath::CreateFolderEx() const
{
	CString tmpPath, strFolder;
	LPCTSTR pPartLim, pSource;
	LPTSTR pDest, pDestBuf = tmpPath.GetBuffer(_MAX_PATH);

	// Gets the root path
	strFolder = GetFolder();
	pSource = strFolder;
	pDest	= pDestBuf;
	pPartLim = strFolder;
	while(pPartLim = ::PathFindNextComponent(pPartLim))
	{
		while(pSource < pPartLim)
			*pDest++ = *pSource++;
		*pDest = 0;
		if ( !::PathIsDirectory(pDestBuf))
			::CreateDirectory(pDestBuf, NULL);
	}
	return ::PathIsDirectory(pDestBuf) != 0;
}



// Function name	: CPath::GetNameExtension
// Description	    : This function returns the filespec
// Return type		: void 
// Argument         : CString &rFile
CString CPath::GetNameExtension() const
{
	CString rFile;

	if(IsFile())
	{
		rFile = operator LPCTSTR();
		LPTSTR pBuf = rFile.GetBuffer(1);
		::PathStripPath(pBuf);
		rFile.ReleaseBuffer();
	}
	else
		rFile = "";
	return rFile;
}



// Function name	: CPath::GetNameExtension
// Description	    : This function removes folder information.
//						Only the filespec is preserved.
// Return type		: void 
void CPath::ToNameExtension()
{
	bool bIsFile = IsFile();
	LPTSTR pBuf = GetBuffer(1);
	if(bIsFile)
		::PathStripPath(pBuf);
	else
		*pBuf = 0;
	ReleaseBuffer();
}



/**
	This function normalizes the szSource and writes the result into 
	rResult. szSource cannot point to the same buffer of rResult.

\param &rResult : Variable to receive the converted string;
\param szSource : 
 */
void CPath::Canonicalize(CString &rResult, LPCTSTR szSource)
{
	if(!szSource || *szSource == 0)
		rResult = "";
	else
	{
		::PathCanonicalize(rResult.GetBuffer(_MAX_PATH), szSource);
		rResult.ReleaseBuffer();
	}
}



/**
	This function normalizes the current path/file and returns the result in rPath
	
	\param &rPath : String to be normalized.
 */
void CPath::Canonicalize(CString &rPath) const
{
	if(IsEmpty())
		rPath = "";
	else
	{
		::PathCanonicalize(rPath.GetBuffer(_MAX_PATH), operator LPCTSTR());
		rPath.ReleaseBuffer();
	}
}



/**
	This function normalizes the stored path/file.
 */
void CPath::Canonicalize()
{
	if(IsEmpty())
		return;
	// Create a temp buffer
	CString s = operator LPCTSTR();
	// Perform normalization
	::PathCanonicalize(GetBuffer(_MAX_PATH), s);
	// Release buffer
	ReleaseBuffer();
}



/**
	This function normalizes the stored path/file. It resolves relative 
	specs.
 */
void CPath::MakeAbsolute()
{
	if(::PathIsRelative(operator LPCTSTR()))
	{
		CPath aux(currentFolder);
		aux += operator LPCTSTR();
		::PathCanonicalize(GetBuffer(_MAX_PATH), aux);
		// Release buffer
		ReleaseBuffer();
	}
	else
		Canonicalize();
}


/**
	Returns the short path name (DOS compatible)

\param &rPath : Path to be converted
 */
void CPath::MakeShortPathName(CString &rPath) const
{
	ASSERT(Exists());
	if(IsEmpty())
		rPath = "";
	else
	{
		::GetShortPathName(operator LPCTSTR(), rPath.GetBuffer(_MAX_PATH), _MAX_PATH);
		rPath.ReleaseBuffer();
	}
}


/**
	Returns the short path name (DOS compatible)
 */
void CPath::MakeShortPathName()
{
	ASSERT(Exists());

	if(IsEmpty())
		return;
	CString s = operator LPCTSTR();
	::GetShortPathName(s, GetBuffer(_MAX_PATH), _MAX_PATH);
	ReleaseBuffer();
}


/**
	Returns the long path name (DOS->Win32)

\param &rPath : Path to be converted
 */
void CPath::MakeLongPathName(CString &rPath) const
{
	ASSERT(Exists());
	if(IsEmpty())
		rPath = "";
	else
	{
		::GetLongPathName(operator LPCTSTR(), rPath.GetBuffer(_MAX_PATH), _MAX_PATH);
		rPath.ReleaseBuffer();
	}
}


/**
	Returns the short path name (DOS->Win32)
 */
void CPath::MakeLongPathName()
{
	ASSERT(Exists());

	if(IsEmpty())
		return;
	CString s = operator LPCTSTR();
	::GetLongPathName(s, GetBuffer(_MAX_PATH), _MAX_PATH);
	ReleaseBuffer();
}


/**
	This method appends a backslash at the end of the string buffer.

	So it forces the contents of the object to use the CPath folder 
	notation. In CPath folders ends always with a backslash.
 */
void CPath::MakeFolderNotation()
{
	::PathAddBackslash(GetBuffer(_MAX_PATH));
	ReleaseBuffer();
}


/**
	This method remove the trailing backslash from the string contents.
	Sometimes this is required, because other API may expect a Root/Drive
	or folder without the backslash
*/
void CPath::RemoveBackSlash()
{
	/*
	**	We DO NOT use ::PathRemoveBackslash(), since it refuses to remove
	**	backslashes in case of a volume name, such as "G:\" --> "G:".
	*/
	if(HasBackSlash() && GetLength())
	{
		LPTSTR p = GetBuffer(_MAX_PATH);
		p[GetLength()-1] = 0;
		ReleaseBuffer();
	}
}


// Function name	: CPath::IsSyntaxValid
// Description	    : Check if all chars are valid
// Return type		: bool 
// Argument         : LPCTSTR pBuf
bool CPath::IsSyntaxValid(LPCTSTR pBuf)
{
	for(; *pBuf; pBuf++)
	{
		int nTyp = ::PathGetCharType(*pBuf);
		if(nTyp == GCT_INVALID || nTyp == GCT_WILD)
			return false;
	}
	return true;
}



// Function name	: CPath::SetComponents
// Description	    : Set entire path information. one or more elements can
//						be NULL pointers.
// Return type		: void 
// Argument         : LPCTSTR szDrive
// Argument         : LPCTSTR szDir
// Argument         : LPCTSTR szName
// Argument         : LPCTSTR szExt
void CPath::SetComponents(LPCTSTR szDrive, LPCTSTR szDir, LPCTSTR szName, LPCTSTR szExt)
{
	LPTSTR pBuf = GetBuffer(_MAX_PATH);
	if(szDrive)
		::PathAppend(pBuf, szDrive);
	if(szDir)
		::PathAppend(pBuf, szDir);
	if(szName)
		::PathAppend(pBuf, szName);
	if(szExt)
	{
		::_tcscat(pBuf, _T("."));
		if(*szExt == '.')
			::_tcscat(pBuf, szExt+1);
		else
			::_tcscat(pBuf, szExt);
	}
	ReleaseBuffer();
}



// Function name	: CPath::HasWildCards
// Description	    : Searches the filespec for wildcards characters.
// Return type		: bool 
// Argument         : LPCTSTR pBuf
bool CPath::HasWildCards(LPCTSTR pBuf)
{
	for(; *pBuf; pBuf++)
	{
		if(::PathGetCharType(*pBuf) == GCT_WILD)
			return true;
	}
	return false;
}



// Function name	: CPath::ChangeRoot
// Description	    : Changes the root part of the filespec. Directory and
//						filename are preserved.
// Return type		: void 
// Argument         : LPCTSTR szDrive
void CPath::ChangeRoot(LPCTSTR szDrive)
{
	CString strDir, strName, strExt;
	strDir = GetDirectory();
	if(IsFile())
	{
		strName = GetName();
		strExt = GetExtension();
		SetComponents(szDrive, strDir, strName, strExt);
	}
	else
		SetComponents(szDrive, strDir, NULL, NULL);
}


// This structure is used to interface with BrowseCallbackProc
typedef struct tagFOLDERPARAMS
{
	CPath::enumSelOptions m_eOptions;
	LPCTSTR	szPathName;
} _FOLDERPARAMS, *P_FOLDERPARAMS;


int CALLBACK CPath::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	P_FOLDERPARAMS pParams = (P_FOLDERPARAMS)pData;

    switch(uMsg)
	{
		case BFFM_INITIALIZED: 
		{
			// Copy path information
			CPath folder(pParams->szPathName);
			// Get folder information
			folder.ToFolder();
			// Is there a folder ?
			if(!folder.IsEmpty())
			{
				// Remove Backslash
				folder.SetAt(folder.GetLength()-1, 0);
				// WParam is TRUE since you are passing a path.
                // It would be FALSE if you were passing a pidl.
				::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)folder);
            }
			break;
        }

        case BFFM_SELCHANGED:
		{
			// Search filespec for wildcards
			CPath filespec(pParams->szPathName);
			filespec.ToNameExtension();

			// If no filespec we can open any folder, even empty
			bool fMustHaveFiles = (!filespec.IsEmpty() && (pParams->m_eOptions != mustExistAndFilterFiles));
			if(filespec.IsEmpty() || !filespec.HasWildCards())
				filespec = _T("*.*");
			// Temp buffer
			CString s, s2;
			// Set the status window to the currently selected path.
            if(::SHGetPathFromIDList((LPITEMIDLIST)lp, s.GetBuffer(_MAX_PATH)))
			{
				::PathAddBackslash((LPTSTR)(LPCTSTR)s);
				s.ReleaseBuffer();

				s += filespec;
				// Count files in folder
				CFileFind finder;
				BOOL bWorking = finder.FindFile(s);
				int nCount = 0;
				while (bWorking)
				{
					bWorking = finder.FindNextFile();
					if(!finder.IsDirectory() && !finder.IsDots())
						nCount++;
				}

				s2 = _T("Selected file count changed");
				::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)(LPCTSTR)s2);
				// Set OK button status if dir must have files
				if(fMustHaveFiles)
					::SendMessage(hwnd, BFFM_ENABLEOK, 0, nCount);
			}
			else
				s.ReleaseBuffer();
            break;
        }

        case BFFM_VALIDATEFAILED:
			return 1;

        default:
			break;
    }
    return 0;
}


// Function name	: CPath::SelFolder
// Description	    : Select a folder and put the selected value in this
//						object. If you specify a filespec the directory must
//						contain files to allow selection. You can also specify
//						a specific wildcard.
// Return type		: bool : true if folder has been selected. NOTE: If the
//						operation is canceled the return value is false and
//						the contents of this object will be unmodified.
// Argument         : enumSelOptions eOptions
// Argument         : LPCTSTR lpszTitle : Address of a null-terminated string that
//						is displayed above the tree view control in the dialog box.
//						This string can be used to specify instructions to the user.
bool CPath::SelFolder(enumSelOptions eOptions, LPCTSTR lpszTitle /*=NULL*/)
{
	bool bResult = false;
	BROWSEINFO bi;
    LPITEMIDLIST pidl;
    LPMALLOC pMalloc;
	_FOLDERPARAMS params;
	CString strTitle;

	if((HIWORD(lpszTitle) == 0) && (lpszTitle != NULL))
		strTitle.LoadString((UINT)lpszTitle);
	else
		strTitle = lpszTitle;

	// Work on a safe copy
	CPath path(*this);

	// filespec is not compatible with mustExist
	if((eOptions == mustExist) && path.IsFile())
		path.ToFolder();

	// Add filespec if mustContainFiles
	if((eOptions == mustContainFiles) && path.HasBackSlash())
		path.AddFile(_T("*.*"));

	if(SUCCEEDED(::SHGetMalloc(&pMalloc)))
	{
		// Zero Browse structure
		ZeroMemory(&bi, sizeof(bi));
		// Owner window
		CWnd *pWnd = ::AfxGetMainWnd();
		if(pWnd)
			bi.hwndOwner = pWnd->m_hWnd;
		// Flags: Valid Dirs and Display status text

		// ?WARNING?: BIF_STATUSTEXT doesn't work with BIF_NEWDIALOGSTYLE
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_EDITBOX | BIF_VALIDATE;
		// Set callback and starting dir
        bi.lpfn = BrowseCallbackProc;
		// Set Parameters
		params.m_eOptions = eOptions;
		params.szPathName = path;
		bi.lpszTitle = (LPCTSTR)strTitle;
		bi.lParam = (LPARAM)&params;
		// Open dialog
		pidl = ::SHBrowseForFolder(&bi);
		// User pressed OK?
        if(pidl)
		{
			// Get Selected path
			LPTSTR pBuf = GetBuffer(_MAX_PATH);
            if(::SHGetPathFromIDList(pidl, pBuf))
			{
				::PathAddBackslash(pBuf);
				bResult = true;
			}
			ReleaseBuffer();
            pMalloc->Free(pidl);
            pMalloc->Release();
        }
    }
    return bResult;
}


bool CPath::SelRoot()
{
	bool bResult = false;
	BROWSEINFO bi;
    LPITEMIDLIST pidl;
    LPMALLOC pMalloc;
	_FOLDERPARAMS params;

	// Work on a safe copy
	CPath path(*this);

	path.ToRoot();
	if(SUCCEEDED(::SHGetMalloc(&pMalloc)))
	{
		// Zero Browse structure
		ZeroMemory(&bi, sizeof(bi));
		// Owner window
		CWnd *pWnd = ::AfxGetMainWnd();
		if(pWnd)
			bi.hwndOwner = pWnd->m_hWnd;
		// Flags: Valid Dirs and Display status text

        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_VALIDATE;
		// Set callback and starting dir
        bi.lpfn = BrowseCallbackProc;
		// Set Paramaters
		params.m_eOptions = mustExist;
		params.szPathName = path;
		bi.lParam = (LPARAM)&params;
		// Open dialog
		pidl = ::SHBrowseForFolder(&bi);
		// User pressed OK?
        if(pidl)
		{
			// Get Selected path
			LPTSTR pBuf = GetBuffer(_MAX_PATH);
            if(::SHGetPathFromIDList(pidl, pBuf))
			{
				::PathStripToRoot(pBuf);
				::PathAddBackslash(pBuf);
				bResult = true;
			}
			ReleaseBuffer();
            pMalloc->Free(pidl);
            pMalloc->Release();
        }
    }
    return bResult;
}


CPath &CPath::operator=(LPCTSTR szOther)
{
	CString::operator =(szOther);
	// Canonicalize destroys paths beginning with '../'
	if(szOther[0] != '.' 
		|| szOther[1] != '.' 
		|| ::PathGetCharType(szOther[2]) != GCT_SEPARATOR)
		Canonicalize();
	return *this;
}


CPath &CPath::operator=(const CString &other)
{
	// Nothing to do if self-assignment
	if(this != &other)
	{
		CString::operator =(other);
		Canonicalize();
	}
	return *this;
}


// Function name	: CPath::ChangeExtension
// Description	    : Changes the extension from a fully qualified path.
//					  If the string suplied is null or empty, this method
//					  removes the extension, including the right-most dot ('.').
// Return type		: void 
// Argument         : LPCTSTR szNewExt
void CPath::ChangeExtension(LPCTSTR pszNewExt)
{
	ASSERT(IsFile());
	LPTSTR pBuf = ::PathFindExtension(GetBuffer(_MAX_PATH));
	if(pszNewExt == NULL)
		pszNewExt = _T("");

	// Note: we add 1 because pszNewExt may come without '.'
	if((_tcslen(pszNewExt)+1 + GetLength()) > _MAX_PATH)
	{
		ASSERT(FALSE);
		return;
	}
	if(*pszNewExt == (TCHAR)'\0')
	{
		if(*pBuf == (TCHAR)'.')
			// Removes any extension
			*pBuf = (TCHAR)'\0';
		ReleaseBuffer();
		return;
	}
	else if(*pszNewExt != (TCHAR)'.')
	{
		if(!*pBuf)
			*pBuf = (TCHAR)'.';
		pBuf++;
	}

	::_tcscpy(pBuf, pszNewExt);	// Adds the new extension
	ReleaseBuffer();
}


// Function name	: CPath::ChangeNameExtension
// Description	    : Changes filespec from a fully qualified path
// Return type		: void 
// Argument         : LPCTSTR szNewFile
void CPath::ChangeNameExtension(LPCTSTR szNewFile)
{
	if(IsFile())
	{
		LPTSTR pBuf = ::PathFindFileName(GetBuffer(_MAX_PATH));
		::_tcscpy(pBuf, szNewFile);
		ReleaseBuffer();
	}
	else
		AddFile(szNewFile);
}


// Function name	: CPath::ChangeDirectory
// Description	    : Changes the path portion of the filepath. Root
//						and filespec are preserved
// Return type		: void 
// Argument         : LPCTSTR szNewDir
void CPath::ChangeDirectory(LPCTSTR szNewDir)
{
	ASSERT(IsFile());

	CString strRoot, strName;

	strRoot = GetRoot();
	strName = GetNameExtension();
	// Rebuild new filename
	*this = strRoot;
	AddPath(szNewDir);
	AddFile(strName);
}


// Function name	: CPath::ChangeFolder
// Description	    : Changes the root and pathspec of a filespec. Will also
//						works without FileExt.
// Return type		: void 
// Argument         : LPCTSTR szNewFolder
void CPath::ChangeFolder(LPCTSTR szNewFolder)
{
	CString strFile;
	strFile = GetNameExtension();

	LPTSTR pBuf = GetBuffer(_MAX_PATH);
	LPTSTR pBufCpy = pBuf;

	do
	{
		*pBufCpy++ = *szNewFolder;
	}
	while(*szNewFolder++ != '\0');

	::PathAddBackslash(pBuf);

	_tcscat(pBufCpy, strFile);
	ReleaseBuffer();
}
#pragma warning (default : 4706)


bool CPath::DeleteFolder() const
{
	ASSERT(HasBackSlash());
	CString s = *this;

	// Remove backslash
	LPTSTR pS = s.GetBuffer(1);
	while(*pS)
		pS++;
	pS--;
	*pS = 0;
	s.ReleaseBuffer();

	return ::RemoveDirectory(s) != 0;
}


void CPath::DelTreeEx(enumDelTreeMode nMode)
{
	BOOL fWorking;
	CPath strFolder(*this);
	CFileFind finder;

	// Add default Wildcard if not specified
	if(HasBackSlash())
		strFolder += "*.*";

	fWorking = finder.FindFile(strFolder);
	while(fWorking)
	{ 
		fWorking = finder.FindNextFile();
		
		// Ignore . and .. files
		if(finder.IsDots())
			continue;

		if( nMode >= treeFilesFolderRO &&
			finder.MatchesMask(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))
		{
			CFileStatus status;
			CFile::GetStatus(finder.GetFilePath(), status);
			status.m_attribute &= ~(CFile::readOnly | CFile::system | CFile::hidden);
			CFile::SetStatus(finder.GetFilePath(), status);
		}
		// Delete files
		if(!finder.IsDirectory())
			VERIFY(::DeleteFile(finder.GetFilePath()));
	}

	// Search for the directories...
	if(nMode >= treeFilesInSubFolders)
	{
		CStringArray arrRD;
		CString strWildCard;

		strWildCard = strFolder.GetNameExtension();
		strFolder.ChangeNameExtension(_T("*.*"));
		fWorking = finder.FindFile(strFolder);
		while(fWorking)
		{ 
			fWorking = finder.FindNextFile();

			// skip . and .. files
			if(finder.IsDots())
				continue;

			// Found a Folder
			if(finder.IsDirectory())
			{
				// Temp storage
				CPath strFoundFolder = finder.GetFilePath();
				// Save to RemoveDir array
				if(nMode == treeDelAll)
					arrRD.Add((LPCTSTR)strFoundFolder);
				// Folder must have backslash to work
				if(!strFoundFolder.HasBackSlash())
					strFoundFolder += '\\';
				// Apply wildcard for that folder
				strFoundFolder.AddFile(strWildCard);
				strFoundFolder.DelTreeEx(nMode);
			}
		}
		// Try to Remove the directory list
		const int m = arrRD.GetSize();
		for(int i = 0; i < m; i++)
			::RemoveDirectory(arrRD[i]);
	}
	// Remove start dir
	if(HasBackSlash())
		DeleteFolder();
}

void CPath::EmptyFolder()
{
	BOOL fWorking;
	CPath strFolder(*this);
	CFileFind finder;

	// Add default Wildcard if not specified
	if(HasBackSlash())
		strFolder += "*.*";

	fWorking = finder.FindFile(strFolder);
	while(fWorking)
	{ 
		fWorking = finder.FindNextFile();

		// Ignore . and .. files
		if(finder.IsDots())
			continue;

		if(finder.MatchesMask(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))
		{
			CFileStatus status;
			CFile::GetStatus(finder.GetFilePath(), status);
			status.m_attribute &= ~(CFile::readOnly | CFile::system | CFile::hidden);
			CFile::SetStatus(finder.GetFilePath(), status);
		}
		// Delete files
		if(!finder.IsDirectory())
			VERIFY(::DeleteFile(finder.GetFilePath()));
	}

	// Search for the directories...

	CStringArray arrRD;
	CString strWildCard;

	strWildCard = strFolder.GetNameExtension();
	strFolder.ChangeNameExtension(_T("*.*"));
	fWorking = finder.FindFile(strFolder);
	while(fWorking)
	{ 
		fWorking = finder.FindNextFile();

		// skip . and .. files
		if(finder.IsDots())
			continue;

		// Found a Folder
		if(finder.IsDirectory())
		{
			// Temp storage
			CPath strFoundFolder = finder.GetFilePath();
			// Save to RemoveDir array
			arrRD.Add((LPCTSTR)strFoundFolder);
			// Folder must have backslash to work
			if(!strFoundFolder.HasBackSlash())
				strFoundFolder += '\\';
			// Apply wildcard for that folder
			strFoundFolder.AddFile(strWildCard);
			strFoundFolder.DelTreeEx(treeDelAll);
		}
	}
	// Try to Remove the directory list
	const int m = arrRD.GetSize();
	for(int i = 0; i < m; i++)
		::RemoveDirectory(arrRD[i]);

}