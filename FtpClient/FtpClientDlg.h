
// FtpClientDlg.h: 헤더 파일
//

#pragma once
#include "FtpInterface.h"
#include "DlgDisplay.h"

#include <iostream>

#include <wlanapi.h>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#pragma comment(lib, "wlanapi.lib")

#include <regex>
#include <fstream>

#include <libssh2/include/libssh2.h>
#include <libssh2/include/libssh2_sftp.h>

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssh2.lib")

// CFtpClientDlg 대화 상자
class CFtpClientDlg : public CDialogEx
{
// 생성입니다.
public:
	CFtpClientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	virtual ~CFtpClientDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FTPCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

protected:
	CFtpInterface m_ftp; // FTP 인터페이스 클래스
	CPtrArray m_arrFileList; // FTP 파일 리스트 정보를 담는 클래스

	CIPAddressCtrl m_ctrlFtpIP; // FTP IP
	CEdit m_edFtpPort; // FTP Port
	CEdit m_edFtpID; // FTP ID
	CEdit m_edFtpPass; // FTP Password
	//CEdit m_edFtpPath; // FTP Path // 콤보로 변경
	CComboBox m_cbFtpPath; // FTP Path

	CListCtrl m_lcFileList; // 화면 표시용 리스트 컨트롤
	CListCtrl m_lcWifiList; // 화면 표시용 리스트 컨트롤
	CStatic m_stDispImage; // 이미지 표시 컨트롤
	CEdit m_edDispText; // 텍스트 표시 컨트롤
	int m_iListCtrlFileType;

	CWinThread *m_pThreadLastImage;
	BOOL m_bFlagLastImage;

	CImage m_imgDisplay; // 이미지
	CStdioFile m_fileText; // 텍스트
	CDlgDisplay *m_pDlgDisplay;

	CCriticalSection m_cr; // 파일 표시에 대한 크리티컬 섹션

	int m_iFtpShowFileType;
	CButton m_rdImage;

public:
	void InitDialog(); // 대화상자 초기화
	void RemoveFileList(); // 리스트 컨트롤 모두 지우기
	void DisplayFileList(); // 파일 리스트 표시
	void DisplayLastFile(int iFileType); // 가장 최근 파일 표시
	void DisplayLastWriteImage(); // 가장 최근 이미지 파일 표시
	void DisplayLastWriteText(); // 가장 최근 텍스트 파일 표시

	void InitEnableControl(); // 초기화 - 컨트롤 활성화/비활성화
	void LoginEnableControl(BOOL bLogin); // 로그인/로그아웃 - 컨트롤 활성화/비활성화
	void ThreadEnableControl(BOOL bStart); // 최근파일조회 시작/종료 - 컨트롤 활성화/비활성화
	void RadioEnableControl(int iFileType); // 라디오 버튼 - 컨트롤 활성화/비활성화
	void ShowControlByFileType(int iFileType); // 파일 타입에 따른 컨트롤 표시

	int GetFtpPathType(); // 선택한 FTP 경로 얻기
	int GetFtpShowFileType(); // 선택한 라디오 타입 얻기

	int GetFtpPathFromConfig();
	void SetFtpPathToConfig(int iIndex);
protected:
	int SetFileInfoToList(int iIndex, CFtpFileInfo *pFileInfo); // 파일 정보 리스트에 입력
	void MoveControl();

private:
	// 2025-01-03 : Wifi Scan
	std::vector<std::tuple<CString, LONG, CString>> ListAvailableWifiNetworks();
	std::wstring ConvertSSID(const unsigned char* ssid, size_t ssidLength);

	// 2025-01-03 : Wifi Connect
	std::wstring StringToWString(const std::string& str);
	std::string WStringToString(const std::wstring& wstr);
	bool ConnectToSelectedWifi(const std::wstring& networkName, const std::wstring& password);

	// 2025-01-03 : RTC Setting
	std::string executeRemoteSshCommand(const char* command, bool isVersionCheck = false);

protected:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonFtpConn(); // FTP 로그인
	afx_msg void OnBnClickedButtonFtpDisconn(); // FTP 로그아웃
	afx_msg void OnBnClickedButtonGetLastFile(); // 최근 파일 표시
	afx_msg void OnBnClickedButtonStartThread(); // 수집 시작
	afx_msg void OnBnClickedButtonEndThread(); // 수집 종료
	afx_msg void OnBnClickedRadio(UINT uiResID); // 라디오 버튼클릭
	afx_msg void OnBnClickedButtonDownAllFile(); // 모든 파일 다운롣르
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnNMClickListFileList(NMHDR *pNMHDR, LRESULT *pResult); // 파일 리스트 클릭
	afx_msg void OnLvnItemchangedListFileList(NMHDR *pNMHDR, LRESULT *pResult); // 리스트에서 선택한 파일이 변경되었을때
	afx_msg void OnStnDblclickStaticLastWrtieTimeImage(); // Display 화면 더블클릭

	static int CALLBACK CompareTime(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); // 가장 최근 이미지를 리스트 최상단에 올리기 위한 정렬 함수
	static UINT ThreadLastImage(LPVOID lpvoid);
public:
	afx_msg void OnCbnSelchangeComboFtpPath(); // ftp path 콤보박스 변경
	afx_msg void OnBnClickedButtonWifiScan();
	afx_msg void OnDblclkListWifiList(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnLvnItemchangedListWifiList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonWifiConnect();
	afx_msg void OnBnClickedButtonRtcSet();
};
