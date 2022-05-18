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

	CString m_strIP; // 접속아이피
	CString m_strID; // 아이디
	CString m_strPasswd; // 암호

	UINT m_nPort; // FTP 접속 포트(기본 21)
	BOOL m_bPassive; // passive, active mode

	CString m_strCurFtpDir;

 	HANDLE m_hEventClose;
	HANDLE m_hEventTransferringFile; // 파일 전송 중 확인을 위한 이벤트

	CCriticalSection m_crLastError;
	DWORD m_dwErrorCode;
	CString m_strErrorMsg;
protected:
	// 로그인
	BOOL Login(CString strIP, UINT nPort, CString strID, CString strPass, BOOL bPassive);

public:
	// 로그인
	BOOL Login(CString strIP, CString strID, CString strPass, UINT nPort = INTERNET_DEFAULT_FTP_PORT, BOOL bPassive = FALSE);
	// 로그인(자동 로그인용)
	BOOL Login();
	// 로그아웃
	void Logout();
	// 연결 확인
	BOOL IsConnected();
	// 연결 확인 및 자동 로그인
	BOOL IsPossibleUse();

	// FTP서버 현재 경로
	BOOL GetCurrentDir(CString& strDirectory);
	// FTP서버 경로 변경
	BOOL SetCurrentDir(CString strDirectory);

	// 현재 경로 파일목록(파일)을 구하기
	BOOL GetFtpFileList(CPtrArray& arrFileList);

	// 다운로드
	BOOL DownloadFile(CString strFtpFilePath, CString strLocalPath);

public:
	// 문자열 체크
	void CheckString(CString& strCheck);
	// try catch error 처리
	void SetLastErrorMsg(CException* e);
	// 마지막 에러 얻기
	void GetLastErrorMsg(DWORD& dwErrorCode, CString& strErrorMsg);
	// 파일 전송 중인지 체크
	BOOL IsFtpFileTransferring();
	// 실행 파일 현재 경로 얻기
	CString GetCurrentModulePath();
};