#pragma once

#include <afxinet.h>
#include <set>

using namespace std;

class CFtpFileInfo
{
protected:
	CTime m_tmLastWrite;
	CString m_strName;
	CString m_strPath;

public:
	CFtpFileInfo(CTime tmLastWrite, CString strName, CString strPath)
		: m_tmLastWrite(tmLastWrite)
		, m_strName(strName)
		, m_strPath(strPath)
		{}

	CTime GetLastWriteTime() { return m_tmLastWrite; }
	CString GetFileName() { return m_strName; }
	CString GetFilePath() { return m_strPath; }
	CString GetFilePathName() {
		CString strPathName;
		strPathName.Format(_T("%s/%s"), m_strPath, m_strName);
		return strPathName; }
};

class CFtpInterface
{
public:
	CFtpInterface();
	~CFtpInterface();

protected:
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
	BOOL GetFtpFileList(CPtrArray& arrFileList);

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
};