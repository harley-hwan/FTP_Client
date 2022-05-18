
// FtpClientDlg.h: 헤더 파일
//

#pragma once
#include "FtpInterface.h"
#include "DlgDisplay.h"

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
public:

protected:
	CFtpInterface m_ftp; // FTP 인터페이스 클래스
	CPtrArray m_arrFileList; // FTP 파일 리스트 정보를 담는 클래스

	CIPAddressCtrl m_ctrlFtpIP; // FTP IP
	CEdit m_edFtpPort; // FTP Port
	CEdit m_edFtpID; // FTP ID
	CEdit m_edFtpPass; // FTP Password
	CEdit m_edFtpPath; // FTP Path

	CListCtrl m_lcFileList; // 화면 표시용 리스트 컨트롤
	CStatic m_Image; // 이미지 표시 컨트롤

	CWinThread *m_pThreadLastImage;
	BOOL m_bFlagLastImage;

	CImage m_imgDisplay;
	CDlgDisplay *m_pDlgDisplay;

	CCriticalSection m_cr;

public:
	void InitDialog(); // 대화상자 초기화
	void RemoveFileList(); // 리스트 컨트롤 모두 지우기
	void DisplayFileList(); // 파일 리스트 표시
	void DisplayLastWriteImage(); // 가장 최근 이미지 파일 표시
protected:
	int SetFileInfoToList(int iIndex, CFtpFileInfo *pFileInfo); // 파일 정보 리스트에 입력
	void MoveControl();

protected:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonFtpConn(); // FTP 연결 버튼
	afx_msg void OnBnClickedButtonFtpDisconn();
	afx_msg void OnBnClickedButtonGetLastImage();
	afx_msg void OnBnClickedButtonStartThread();
	afx_msg void OnBnClickedButtonEndThread();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClickListFileList(NMHDR *pNMHDR, LRESULT *pResult); // 파일 리스트 클릭
	afx_msg void OnStnDblclickStaticLastWrtieTimeImage(); // Display 화면 더블클릭

	static int CALLBACK CompareTime(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); // 가장 최근 이미지를 리스트 최상단에 올리기 위한 정렬 함수
	static UINT ThreadLastImage(LPVOID lpvoid);
public:
};
