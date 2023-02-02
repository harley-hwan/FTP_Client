#include "pch.h"
#include "FtpInterface.h"
#pragma comment(lib, "Wininet.lib")
#include "FtpClientDlg.h"

#define LEN_ERR_MSG 1024

#define MAX_BUFFER_SIZE 4096

CFtpInterface::CFtpInterface(void)
{
	m_pOwner = NULL;
	m_pSession = NULL;
	m_pFtp = NULL;

	m_strIP = _T("");
	m_strID = _T("");
	m_strPasswd = _T("");

	m_nPort = INTERNET_DEFAULT_FTP_PORT;
	m_bPassive = FALSE;

	m_strCurFtpDir = _T("");


	m_hEventClose = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventTransferringFile = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_dwErrorCode = 0;
	m_strErrorMsg = _T("");
}

CFtpInterface::~CFtpInterface(void)
{
	Logout();

	CloseHandle(m_hEventClose);
	m_hEventClose = NULL;
	CloseHandle(m_hEventTransferringFile);
	m_hEventTransferringFile = NULL;
}

BOOL CFtpInterface::Login(CString strIP, UINT nPort, CString strID, CString strPass, BOOL bPassive)
{
	Logout();

	try
	{
		m_pSession = new CInternetSession;

		m_pFtp = m_pSession->GetFtpConnection(strIP, strID, strPass, nPort, bPassive);
		ResetEvent(m_hEventClose);

		if (m_pOwner != NULL && ((CFtpClientDlg*)m_pOwner)->GetSafeHwnd() != NULL)
			((CFtpClientDlg*)m_pOwner)->LoginEnableControl(TRUE);

		return TRUE;
	}
	catch (CException* e)
	{
		SetLastErrorMsg(e);
		return FALSE;
	}
}


BOOL CFtpInterface::Login(CString strIP, CString strID, CString strPass, UINT nPort/* = INTERNET_DEFAULT_FTP_PORT*/, BOOL bPassive/* = FALSE*/)
{
	// 저장
	m_strIP = strIP;
	m_strID = strID;
	m_strPasswd = strPass;
	m_nPort = nPort;
	m_bPassive = bPassive;

	BOOL bLogin = Login(m_strIP, m_nPort, m_strID, m_strPasswd, m_bPassive);

	// 로그인 시 현재 경로 읽어오기
	if (TRUE == bLogin)
		GetCurrentDir(m_strCurFtpDir);

	return bLogin;
}

BOOL CFtpInterface::Login()
{
	BOOL bLogin = Login(m_strIP, m_nPort, m_strID, m_strPasswd, m_bPassive);

	// 재로그인 시 디렉토리 다시 설정
	if (TRUE == bLogin)
		SetCurrentDir(m_strCurFtpDir);

	return bLogin;
}

void CFtpInterface::Logout()
{
	if (NULL != m_hEventClose)
	{
		SetEvent(m_hEventClose);
	}

	if (NULL != m_pFtp)
	{
		m_pFtp->Close();
		delete m_pFtp;
		m_pFtp = NULL;
	}

	if (NULL != m_pSession)
	{
		m_pSession->Close();
		delete m_pSession;
		m_pSession = NULL;
	}

	if (m_pOwner != NULL && ((CFtpClientDlg*)m_pOwner)->GetSafeHwnd() != NULL)
		((CFtpClientDlg*)m_pOwner)->LoginEnableControl(FALSE);
}

BOOL CFtpInterface::IsConnected()
{
	if ((NULL == m_pFtp) || (NULL == m_pSession))
		return FALSE;

	// 서버마다 종료메시지가 안올 수 있기 때문에
	// 체크할 때 현재 디렉토리 위치 읽어서 파악.
	try
	{
		CString strDir;
		BOOL bRes = m_pFtp->GetCurrentDirectory(strDir);

		return bRes;
	}
	catch (CException* e)
	{
		SetLastErrorMsg(e);
		return FALSE;
	}
}

BOOL CFtpInterface::IsPossibleUse()
{
	// 파일 전송 중이면 사용 못함
	if (TRUE == IsFtpFileTransferring())
		return FALSE;

	BOOL bRes = IsConnected();
	// 연결 안되어 있으면
	if (FALSE == bRes)
	{
		Sleep(100);


	return bRes;
		// 재로그인
		bRes = Login();
	}
}

BOOL CFtpInterface::GetCurrentDir(CString& strDirectory)
{
	strDirectory = _T("");

	BOOL bRes = IsPossibleUse();
	if (FALSE == bRes)
		return bRes;

	try
	{
		CString strDir;
		bRes = m_pFtp->GetCurrentDirectory(strDirectory);

		return bRes;
	}
	catch (CException* e)
	{
		SetLastErrorMsg(e);
		return FALSE;
	}		

	return bRes;
}

BOOL CFtpInterface::SetCurrentDir(CString strDirectory)
{
	BOOL bRes = IsPossibleUse();
	if (FALSE == bRes)
		return bRes;

	try
	{
		bRes = m_pFtp->SetCurrentDirectory(strDirectory);
		if (TRUE == bRes)
			m_strCurFtpDir = strDirectory;

		return bRes;
	}
	catch (CException* e)
	{
		SetLastErrorMsg(e);
		return FALSE;
	}

	return bRes;
}

BOOL CFtpInterface::GetFtpFileList(CPtrArray& arrFileList, BOOL bFileAll/* = FALSE*/)
{
	try
	{
		CFtpFileFind finder(m_pFtp);
		BOOL bWorking = finder.FindFile(_T("*"), INTERNET_FLAG_RELOAD);
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (finder.IsDirectory())
				continue;

			CString strName = finder.GetFileName();
			CString strExt = PathFindExtension(strName);

			int iFileType = 0;
			int iFtpPath = -1;
			if (!bFileAll)
			{
				if (m_pOwner != NULL && ((CFtpClientDlg*)m_pOwner)->GetSafeHwnd() != NULL)
				{
					iFtpPath = ((CFtpClientDlg*)m_pOwner)->GetFtpPathType();
					iFileType = ((CFtpClientDlg*)m_pOwner)->GetFtpShowFileType();
				}

				// ShotDB
				if (iFtpPath == 3)
				{
					if (_tcscmp(strExt, _T(".rbf")) !=  0)
						continue;
				}
				else
				{
					if (iFileType == 0)
					{
						if (_tcscmp(strExt, _T(".jpg")) !=  0)
							continue;
					}
					else if (iFileType == 1)
					{
						if (_tcscmp(strExt, _T(".log")) !=  0 && _tcscmp(strExt, _T(".txt")) !=  0)
							continue;
					}
				}
			}

			CTime tmLastWrite;
			finder.GetLastWriteTime(tmLastWrite);
			CString strPath = finder.GetFilePath();
			CTime tmCreation;
			finder.GetCreationTime(tmCreation);

  			CFtpFileInfo* pFileInfo = new CFtpFileInfo(iFtpPath, tmLastWrite, strName, strPath);
 			arrFileList.Add(pFileInfo);
			TRACE(_T("tmLastWrite(%s), strName(%s), strPath(%s)\n"),
				  tmLastWrite.Format(_T("%Y:%m:%d %H:%M:%S")), strName, strPath);
		}
	}
	catch (CException* e)
	{
		SetLastErrorMsg(e);
		return FALSE;
	}

	return TRUE;
}

BOOL CFtpInterface::DownloadFile(CString strFtpFilePath, CString strLocalPath)
{
	BOOL bRes = IsPossibleUse();
	if (FALSE == bRes)
		return bRes;

	try
	{
		// INTERNET_FLAG_RELOAD
		//m_pFtp->GetFile(strFtpFilePath, strLocalPath, FALSE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);

		// ftp 파일 open
		CInternetFile* pInternet = m_pFtp->OpenFile(strFtpFilePath, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD);
		if (NULL == pInternet)
			return FALSE;

		// 로컬 파일 생성
		CString strFileName = pInternet->GetFileName();
		//CString strLocalFileName;
		//strLocalFileName.Format(_T("%s/%s"), strLocalPath, strFileName);
		//CheckString(strLocalPath);
		CStdioFile file(strLocalPath, CStdioFile::modeCreate | CStdioFile::modeWrite | CStdioFile::shareExclusive | CStdioFile::typeBinary);

		do
		{
			// ftp 파일 읽기
			BYTE byBuff[MAX_BUFFER_SIZE] ={0,};
			UINT unRead = pInternet->Read(byBuff, MAX_BUFFER_SIZE);

			if (0 == unRead)
				break;
			// 로컬에 쓰기
			file.Write(byBuff, unRead);

		}
		while (true);

		pInternet->Close();
		delete pInternet;
		pInternet = NULL;

		file.Close();
	}
	catch (CException* e)
	{
		SetLastErrorMsg(e);
		return FALSE;
	}

	return TRUE;
}

void CFtpInterface::CheckString(CString& strCheck)
{
	while (0 < strCheck.Replace(_T("//"), _T("/")));
	while (0 < strCheck.Replace(_T("\\"), _T("/")));
}

void CFtpInterface::SetLastErrorMsg(CException* e)
{
	m_crLastError.Lock();

	TCHAR tzErrorMsg[LEN_ERR_MSG] = { 0, };
	e->GetErrorMessage(tzErrorMsg, LEN_ERR_MSG);
	DWORD dwErrorCode = GetLastError();

	m_dwErrorCode = dwErrorCode;
	m_strErrorMsg.Format(_T("%s"), tzErrorMsg);
	TRACE(_T("Error Code : 0x%X, Error Msg : %s\n"), dwErrorCode, tzErrorMsg);

	m_crLastError.Unlock();
}

void CFtpInterface::GetLastErrorMsg(DWORD& dwErrorCode, CString& strErrorMsg)
{
	m_crLastError.Lock();

	dwErrorCode = m_dwErrorCode;
	strErrorMsg = m_strErrorMsg;

	m_dwErrorCode = 0;
	m_strErrorMsg = _T("");

	m_crLastError.Unlock();
}

BOOL CFtpInterface::IsFtpFileTransferring()
{
	BOOL bRes = FALSE;
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hEventTransferringFile, 0))
		bRes = TRUE;
	return bRes;
}

CString CFtpInterface::GetCurrentModulePath()
{
	CString sFilePath(_T(""));
	TCHAR tzTemp[MAX_PATH] = { 0, };

	DWORD dwRes = ::GetModuleFileName((HMODULE)&__ImageBase, tzTemp, MAX_PATH);
	if (0 != dwRes)
	{
		if (TRUE == ::PathRemoveFileSpec(tzTemp))
			sFilePath.Format(_T("%s"), tzTemp);
	}

	return sFilePath;
}

void CFtpInterface::SetOwner(void * pOwner)
{
	m_pOwner = pOwner;
}
