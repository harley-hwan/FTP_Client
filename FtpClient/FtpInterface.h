#pragma once

#include <afxinet.h>
#include <set>

using namespace std;

class CFtpFileInfo
{
protected:
	int m_iFtpPathType;
	CTime m_tmLastWrite;
	CString m_strName;
	CString m_strPath;

public:
	CFtpFileInfo(int iType, CTime tmLastWrite, CString strName, CString strPath)
		: m_iFtpPathType(iType)
		, m_tmLastWrite(tmLastWrite)
		, m_strName(strName)
		, m_strPath(strPath)
		{}

	int GetFtpPathType() { return m_iFtpPathType; }
	CTime GetLastWriteTime() { return m_tmLastWrite; }
	CString GetFileName() { return m_strName; }
	CString GetFileNameExceptExt()
	{
		CString strExt = PathFindExtension(m_strName);
		CString strName = m_strName;
		strName.Replace(strExt, _T(""));
		return strName;
	}
	CString GetFileExt() { return PathFindExtension(m_strName); }
	CString GetFilePath() { return m_strPath; }
	CString GetFilePathName() {
		CString strPathName;
		strPathName.Format(_T("%s/%s"), m_strPath, m_strName);
		return strPathName;
	}
	CString GetFileDownPathName() {
		CString strPathName;
		if (m_iFtpPathType == 3) // ShotDB
			strPathName.Format(_T("/Result/%s.jpg"), GetFileNameExceptExt());
		else
			strPathName = GetFilePathName();

		return strPathName;
	}
};

class CFtpInterface
{
public:
	CFtpInterface();
	~CFtpInterface();

protected:
	void *m_pOwner;

	CInternetSession* m_pSession;
	CFtpConnection* m_pFtp;

	CString m_strIP; // ���Ӿ�����
	CString m_strID; // ���̵�
	CString m_strPasswd; // ��ȣ

	UINT m_nPort; // FTP ���� ��Ʈ(�⺻ 21)
	BOOL m_bPassive; // passive, active mode

	CString m_strCurFtpDir;

 	HANDLE m_hEventClose;
	HANDLE m_hEventTransferringFile; // ���� ���� �� Ȯ���� ���� �̺�Ʈ

	CCriticalSection m_crLastError;
	DWORD m_dwErrorCode;
	CString m_strErrorMsg;
protected:
	// �α���
	BOOL Login(CString strIP, UINT nPort, CString strID, CString strPass, BOOL bPassive);

public:
	// �α���
	BOOL Login(CString strIP, CString strID, CString strPass, UINT nPort = INTERNET_DEFAULT_FTP_PORT, BOOL bPassive = FALSE);
	// �α���(�ڵ� �α��ο�)
	BOOL Login();
	// �α׾ƿ�
	void Logout();
	// ���� Ȯ��
	BOOL IsConnected();
	// ���� Ȯ�� �� �ڵ� �α���
	BOOL IsPossibleUse();

	// FTP���� ���� ���
	BOOL GetCurrentDir(CString& strDirectory);
	// FTP���� ��� ����
	BOOL SetCurrentDir(CString strDirectory);

	// ���� ��� ���ϸ��(����)�� ���ϱ�
	BOOL GetFtpFileList(CPtrArray& arrFileList, BOOL bFileAll = FALSE);

	// �ٿ�ε�
	BOOL DownloadFile(CString strFtpFilePath, CString strLocalPath);

public:
	// ���ڿ� üũ
	void CheckString(CString& strCheck);
	// try catch error ó��
	void SetLastErrorMsg(CException* e);
	// ������ ���� ���
	void GetLastErrorMsg(DWORD& dwErrorCode, CString& strErrorMsg);
	// ���� ���� ������ üũ
	BOOL IsFtpFileTransferring();
	// ���� ���� ���� ��� ���
	CString GetCurrentModulePath();

public:
	void SetOwner(void *pOwner);
};