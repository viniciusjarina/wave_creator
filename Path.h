// hlpPath.h: interface for the CPath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGPATH_H__679525A5_0914_11D3_9D01_00A0C9E70BD0__INCLUDED_)
#define AFX_ENGPATH_H__679525A5_0914_11D3_9D01_00A0C9E70BD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Shlwapi.h>


// These are all supported special folders
typedef enum {
	currentFolder = 0,
	windowsFolder,
	systemFolder,
	tempFolder,
	tempFileCreate,
	appFolder,
	programFilesFolder,
	commonFilesFolder,
	startMenuProgramsFolder,
	startMenuFolder,
	startupFolder,
	recentDocsFolder,
	cookiesFolder,
	desktopFolder,
	fontsFolder,
	myDocsFolder,
	appsData,
	commonAppData,
	commonDocuments
}	enumSpecialFolders;


/// These are the supported drive types
typedef enum {
	unknown		= DRIVE_UNKNOWN,
	no_root_dir	= DRIVE_NO_ROOT_DIR,
	removable	= DRIVE_REMOVABLE,
	fixed		= DRIVE_FIXED,
	remote		= DRIVE_REMOTE,
	cdrom		= DRIVE_CDROM,
	ramdisk		= DRIVE_RAMDISK,
} enumDriveType;


/// These are DelTreeEx() options
typedef enum
{
	/// Delete files only in given Folder
	treeFilesFolder,
	/// Same as above including ReadOnly files
	treeFilesFolderRO,
	/// Delete all files on given dir and subfolders
	treeFilesInSubFolders,
	/// Delete all files and subfolders (if empty)
	treeDelAll,
}	enumDelTreeMode;


class CPath : public CString  
{
public:
	CPath();
	CPath(const CPath &rPath);
	CPath(LPCTSTR lpszPath);
	CPath(enumSpecialFolders eInitialFolder);
	~CPath();

// Adding file parts
public:
	/// Adds a file part
	void AddFile(LPCTSTR rFile);
	/// Adds a path part
	void AddPath(LPCTSTR rPath);
	/// Copies only the root from the origin
	void AddRoot(LPCTSTR rPath);

// Normalizing paths
public:
	/// Static version to normalize a path
	static void	Canonicalize(CString &rResult, LPCTSTR szSource);
	/// Normalizes the path for this object
	void Canonicalize();
	/// Outputs the normalized path ionto another string
	void Canonicalize(CString &rPath) const;
	/// Converts this object into an absolute path
	void MakeAbsolute();
	/// Outputs the short path name into another string
	void MakeShortPathName(CString &rPath) const;
	/// Obtains the short path name version
	void MakeShortPathName();
	/// Outputs the short path name into another string
	void MakeLongPathName(CString &rPath) const;
	/// Obtains the short path name version
	void MakeLongPathName();
	/// Forces the contents to be a folder (adds the backslash)
	void MakeFolderNotation();
	/// Removes the backslash from the current string contents
	void RemoveBackSlash();

	/// Changes the current Folder
	void SetCurrentFolder() const;

// Drive stats
public:
	/// Gets size and free space for this folder
	ULONGLONG GetFolderFreeSpace(ULONGLONG *pTotalSpace = NULL,
			ULONGLONG *pTotalAvail = NULL) const;
	/// Gets drive stats for this object
	DWORD GetDiskFreeSpace(DWORD *pdwSecPerCluster, DWORD *pdwBytesPerCluster,
				DWORD *pdwTotalClusters=NULL) const;

	/// Gets drive type for this object
	enumDriveType GetDriveType() const;

	/// Immediate CString conversion (fast)
	static CPath &FromString(CString &other)
	{
		return (CPath &)other;
	}

// Properties
public:
	/// Tests for backslash presence
	bool HasBackSlash() const;
	/// Tests for wildcard
	static bool	HasWildCards(LPCTSTR szPath);
	/// Tests for wildcard in this object
	bool HasWildCards() const;
	/// Verifies if path is a folder and exists
	bool IsFolderAndExists() const;
	/// Verifies if contents is a file name
	bool IsFile() const;
	/// Verifies if folder is empty
	bool IsFolderEmpty() const;
	/// Verifies if path uses UNC conventions
	bool IsUNC() const;
	/// Verifies if path has invalid characters
	static bool IsSyntaxValid(LPCTSTR szFilePath);
	/// Verifies if this path has invalid characters
	bool IsSyntaxValid() const;
	/// Tests for folder or file presence
	bool Exists() const;
	/// Tests for media presence
	bool IsMediaMounted() const;

// File/Folder operations
public:
	/// Deletes a folder
	bool DeleteFolder() const;

	/// Deletes this file
	bool DeleteFile() const;

	/// Deletes this file or folder.
	/** If you know the kind of path you have it is more efficient to
		use DeleteFile() or DeleteFolder(). */
	bool Delete() const;

	/// Moves the file
	bool MoveFile(LPCTSTR szNewFileName) const;

	/// Copies a file
	bool CopyFile(LPCTSTR szDestFileName, bool bFailIfExists=true) const;

	/// Sets attributes for this file
	bool SetFileAttributes(DWORD dwFileAttributes);

	/// Deletes files of a folder and subfolders
	void DelTreeEx(enumDelTreeMode nMode=treeFilesFolder);
	/// Inconditionally deletes all files of a folder and its subfolders
	void DelTree();
	/// Empty the directory contents
	void EmptyFolder();

// Retrieving Drive/Path/File components
public:
	/// Obtains path components
	void	GetComponents(CString *pRoot, CString *pDir, CString *pName=NULL, CString *pExt=NULL) const;
	/// Gets directory information (without root)
	CString	GetDirectory() const;
	/// Gets directory information (without root)
	void	ToDirectory();
	/// Gets root information
	CString	GetRoot() const;
	/// Gets root information
	void	ToRoot();
	/// Gets file name and extension
	CString	GetNameExtension() const;
	/// Gets file name and extension
	void	ToNameExtension();
	/// Gets the folder name
	CString	GetFolder() const;
	/// Gets the folder name
	void	ToFolder();
	/// Gets the file extension
	CString	GetExtension() const;
	/// Gets the file extension
	void	ToExtension();
	/// Gets the file name without extension.
	CString	GetName() const;
	/// Gets the file name without extension.
	void	ToName();

// Setting Drive/Path/File components
public:
	/// Sets path components
	void SetComponents(LPCTSTR szDrive, LPCTSTR szDir, LPCTSTR szName, LPCTSTR szExt);
	/// Changes root of this path
	void ChangeRoot(LPCTSTR szDrive);
	/// Changes extension of this path
	void ChangeExtension(LPCTSTR szNewExt);
	/// Changes file name and extension
	void ChangeNameExtension(LPCTSTR szNewFile);
	/// Changes the directory portion of the file path, preserving root
	void ChangeDirectory(LPCTSTR szNewDir);
	/// Changes the folder of the file; only name and extension are preserved
	void ChangeFolder(LPCTSTR szNewFolder);

	/// Adds or changes the file part
	void SetFilespec(LPCTSTR rFile)
	{//DROPPED:
		if(IsFile())
			ChangeNameExtension(rFile);
		else
			AddFile(rFile);
	}

// Folder creation
public:
	/// Creates a Folder.
	bool CreateFolder() const;
	/// Creates all the folder tree of a given path.
	bool CreateFolderEx() const;

// File/Folder Selection
public:
	/// Opens a dialog to select the root.
	bool SelRoot();
	/// These are SelFolder() options
	enum enumSelOptions 
	{
		mustExist = 1,				// Folder must exist.
		mustContainFiles,			// Folder must have files.
		mustExistAndFilterFiles,	// Folder must exist, and shows how many
									// filtered exists in the current folder.
	};
	/// Opens a dialog to select a folder
	bool	SelFolder(enumSelOptions eOptions, LPCTSTR lpszTitle = NULL);
	/// Opens a dialog to select a file
	bool	SelFile(LPCTSTR szFilter = NULL);

// Init to predefined paths
public:
	/// Sets contents to the application folder
	void	SetToAppFolder();
	/// Sets contents to the current folder
	void	SetToCurrentFolder();
	/// Sets contents to the Windows folder
	void	SetToWindowsFolder();
	/// Sets contents to the System folder
	void	SetToSystemFolder();
	/// Sets contents to the system TEMP folder
	void	SetToTempFolder();
	/// Creates a temporary file name and sets to object.
	void	SetToTempFileCreate();
	/// Sets contents to the given special folder
	void	SetToSpecialFolder(enumSpecialFolders nFolder);
	/// Canonically compares path names
	bool operator ==(const CPath &other) const;
	/// Canonically assigns path names
	CPath &operator=(LPCTSTR szOther);
	/// Canonically assigns path names
	CPath &operator=(const CString &other);

private:
	/// For file browsing implementation
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp,
					LPARAM pData);
};

#endif // !defined(AFX_ENGPATH_H__679525A5_0914_11D3_9D01_00A0C9E70BD0__INCLUDED_)
