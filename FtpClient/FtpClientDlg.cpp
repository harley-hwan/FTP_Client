
// FtpClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "FtpClient.h"
#include "FtpClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFtpClientDlg 대화 상자



CFtpClientDlg::CFtpClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FTPCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pThreadLastImage = NULL;
	m_bFlagLastImage = FALSE;
	m_pDlgDisplay = NULL;
}

CFtpClientDlg::~CFtpClientDlg()
{
	OnBnClickedButtonEndThread();

	if (m_pDlgDisplay != NULL)
		delete m_pDlgDisplay;
	m_pDlgDisplay = NULL;

	CSingleLock cr(&m_cr);
	cr.Lock();
	if (!m_imgDisplay.IsNull())
		m_imgDisplay.Destroy();
	cr.Unlock();

	int iCount = m_arrFileList.GetCount();
	for (int iIndex = 0 ; iIndex < iCount ; iIndex++)
	{
		CFtpFileInfo *pFileInfo = (CFtpFileInfo *)m_arrFileList.GetAt(iIndex);
		if (pFileInfo == NULL)
			continue;

		delete pFileInfo;
		pFileInfo = NULL;

	}
}

void CFtpClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_FTP, m_ctrlFtpIP);
	DDX_Control(pDX, IDC_EDIT_FTP_PORT, m_edFtpPort);
	DDX_Control(pDX, IDC_EDIT_FTP_ID, m_edFtpID);
	DDX_Control(pDX, IDC_EDIT_FTP_PASS, m_edFtpPass);
	DDX_Control(pDX, IDC_EDIT_FTP_PATH, m_edFtpPath);
	DDX_Control(pDX, IDC_LIST_FILE_LIST, m_lcFileList);
	DDX_Control(pDX, IDC_STATIC_LAST_WRTIE_TIME_IMAGE, m_Image);
}

BEGIN_MESSAGE_MAP(CFtpClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CFtpClientDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFtpClientDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_FTP_CONN, &CFtpClientDlg::OnBnClickedButtonFtpConn)
	ON_BN_CLICKED(IDC_BUTTON_GET_LAST_IMAGE, &CFtpClientDlg::OnBnClickedButtonGetLastImage)
	ON_BN_CLICKED(IDC_BUTTON_FTP_DISCONN, &CFtpClientDlg::OnBnClickedButtonFtpDisconn)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START_THREAD, &CFtpClientDlg::OnBnClickedButtonStartThread)
	ON_BN_CLICKED(IDC_BUTTON_END_THREAD, &CFtpClientDlg::OnBnClickedButtonEndThread)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILE_LIST, &CFtpClientDlg::OnNMClickListFileList)
	ON_STN_DBLCLK(IDC_STATIC_LAST_WRTIE_TIME_IMAGE, &CFtpClientDlg::OnStnDblclickStaticLastWrtieTimeImage)
END_MESSAGE_MAP()


// CFtpClientDlg 메시지 처리기

BOOL CFtpClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	InitDialog();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFtpClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFtpClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFtpClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	RemoveFileList();
}




void CFtpClientDlg::InitDialog()
{
	m_ctrlFtpIP.SetWindowText(_T("192.168.8.1"));
	m_edFtpPort.SetWindowText(_T("3999"));
	m_edFtpID.SetWindowText(_T("dbgftp"));
	m_edFtpPass.SetWindowText(_T("gradar"));
	m_edFtpPath.SetWindowText(_T("/Result"));

	m_lcFileList.SetExtendedStyle(/*LVS_EX_GRIDLINES | */LVS_EX_FULLROWSELECT); // 리스트 스타일
	m_lcFileList.InsertColumn(0, _T("Time"), LVCFMT_CENTER, 150);
	m_lcFileList.InsertColumn(1, _T("File Name"), LVCFMT_CENTER, 250);

	RemoveFileList();

	MoveControl();
}

void CFtpClientDlg::RemoveFileList()
{
	CSingleLock cr(&m_cr);
	cr.Lock();
	m_lcFileList.DeleteAllItems();
	int iCount = m_arrFileList.GetCount();
	for (int iIndex = 0 ; iIndex < iCount ; iIndex++)
	{
		CFtpFileInfo *pFileInfo = (CFtpFileInfo *)m_arrFileList.GetAt(iIndex);
		if (pFileInfo == NULL)
			continue;

		delete pFileInfo;
		pFileInfo = NULL;

	}
	m_arrFileList.RemoveAll();
}

void CFtpClientDlg::DisplayFileList()
{
	CSingleLock cr(&m_cr);
	cr.Lock();
	m_lcFileList.SetRedraw(FALSE);
	m_lcFileList.DeleteAllItems();
	int iCount = m_arrFileList.GetCount();

	for (int iIndex = 0 ; iIndex < iCount ; iIndex++)
	{
		CFtpFileInfo *pFileInfo = (CFtpFileInfo *)m_arrFileList.GetAt(iIndex);
		int iListCount = m_lcFileList.GetItemCount();
		SetFileInfoToList(iListCount, pFileInfo);
	}
	m_lcFileList.SortItems(CompareTime, (LPARAM)&m_lcFileList);
	m_lcFileList.SetRedraw(TRUE);
}

void CFtpClientDlg::DisplayLastWriteImage()
{
	int iCount = m_lcFileList.GetItemCount();
	if (0 < iCount)
	{
		CSingleLock cr(&m_cr);
		cr.Lock();
		CFtpFileInfo *pFileInfo = (CFtpFileInfo *)m_lcFileList.GetItemData(0);
		if (pFileInfo == NULL)
			AfxMessageBox(_T("Data Load Fail"));

		CString strFilePathName = pFileInfo->GetFilePathName();
		CString strLocalPathName;
		strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());
		BOOL bDown = m_ftp.DownloadFile(strFilePathName, strLocalPathName);
		if (bDown)
		{
			CString strLocalPathName;
			strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());
			if (!m_imgDisplay.IsNull())
				m_imgDisplay.Destroy();
			HRESULT hr = m_imgDisplay.Load(strLocalPathName);
			if (SUCCEEDED(hr))
			{
				CRect rect;
				m_Image.GetWindowRect(&rect); // 컨트롤 크기 얻기
				CDC *pDc = m_Image.GetDC();
				::SetStretchBltMode(*pDc, HALFTONE); // HALFTONE모드로 설정해서 크기 축소로 인한 이미지 깨짐 현상 최소화
				m_imgDisplay.Draw(*pDc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_imgDisplay.GetWidth(), m_imgDisplay.GetHeight() );
				
				
				DeleteFile(strLocalPathName);
			}
			else
				AfxMessageBox(_T("Image File Load Fail"));
		}
		else
			AfxMessageBox(_T("파일 다운로드 실패"));
	}
}

int CFtpClientDlg::SetFileInfoToList(int iIndex, CFtpFileInfo * pFileInfo)
{
	int iRes = -1;
	if (iIndex < 0 || pFileInfo == NULL)
		return iRes;

	CString strNo;
	strNo.Format(_T("%d"), iIndex);
	CTime tmLastWrite = pFileInfo->GetLastWriteTime();
	CString strTime = tmLastWrite.FormatGmt(_T("%Y:%m:%d %H:%M:%S"));
	CString strFileName = pFileInfo->GetFileName();

	iRes = m_lcFileList.InsertItem(iIndex, strTime);
	m_lcFileList.SetItemText(iIndex, 1, strFileName);
	m_lcFileList.SetItemData(iIndex, (DWORD_PTR)pFileInfo);

	return iRes;
}

void CFtpClientDlg::MoveControl()
{
    //GROUPBOX        "WiFi List",IDC_STATIC_GROUP_WIFI,7,7,157,206
    //GROUPBOX        "FTP Info",IDC_STATIC_GROUP_FTP,7,217,157,93
    //RTEXT           "Server IP : ",IDC_STATIC_IP,12,226,59,14,SS_CENTERIMAGE
    //CONTROL         "",IDC_IPADDRESS_FTP,"SysIPAddress32",WS_TABSTOP,72,226,87,15
    //RTEXT           "Port No : ",IDC_STATIC_PORT,12,242,59,14,SS_CENTERIMAGE
    //EDITTEXT        IDC_EDIT_FTP_PORT,72,242,87,14,ES_AUTOHSCROLL
    //RTEXT           "FTP Path : ",IDC_STATIC_PATH,12,258,59,14,SS_CENTERIMAGE
    //EDITTEXT        IDC_EDIT_FTP_PATH,72,258,87,14,ES_AUTOHSCROLL
    //RTEXT           "User ID : ",IDC_STATIC_USER_ID,12,274,59,14,SS_CENTERIMAGE
    //EDITTEXT        IDC_EDIT_FTP_ID,72,274,87,14,ES_AUTOHSCROLL
    //RTEXT           "User Password : ",IDC_STATIC_USER_PASS,12,290,59,14,SS_CENTERIMAGE
    //EDITTEXT        IDC_EDIT_FTP_PASS,72,291,87,14,ES_AUTOHSCROLL
    //PUSHBUTTON      "Login",IDC_BUTTON_FTP_CONN,7,318,45,14
    //PUSHBUTTON      "Logout",IDC_BUTTON_FTP_DISCONN,55,318,45,14
    //GROUPBOX        "File List",IDC_STATIC_GROUP_FILE_LIST,168,7,241,345
    //CONTROL         "",IDC_LIST_FILE_LIST,"SysListView32",LVS_REPORT | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP,175,17,226,326
    //GROUPBOX        "Last Write Time Image",IDC_STATIC_GROUP_IMAGE,412,7,386,345
    //CONTROL         "",IDC_STATIC_LAST_WRTIE_TIME_IMAGE,"Static",SS_BLACKFRAME | SS_CENTERIMAGE,420,19,372,322
    //PUSHBUTTON      "최근 파일 표시",IDC_BUTTON_GET_LAST_IMAGE,107,318,45,14
    //PUSHBUTTON      "수집 시작",IDC_BUTTON_START_THREAD,7,338,45,14
    //PUSHBUTTON      "수집 종료",IDC_BUTTON_END_THREAD,55,338,45,14

	int iGap10 = 10, iGap5 = 5, iGap2 = 2;
	CWnd *pBtnFtpCon = GetDlgItem(IDC_BUTTON_FTP_CONN);
	CWnd *pBtnFtpDiscon = GetDlgItem(IDC_BUTTON_FTP_DISCONN);
    CWnd *pBtnGetImg = GetDlgItem(IDC_BUTTON_GET_LAST_IMAGE);
	CWnd *pBtnStart = GetDlgItem(IDC_BUTTON_START_THREAD);
	CWnd *pBtnEnd = GetDlgItem(IDC_BUTTON_END_THREAD);
	CWnd *pGroupWifi = GetDlgItem(IDC_STATIC_GROUP_WIFI);
	CWnd *pAddrFtpIP = GetDlgItem(IDC_IPADDRESS_FTP);
	CWnd *pEdFtpPort = GetDlgItem(IDC_EDIT_FTP_PORT);
	CWnd *pEdFtpPath = GetDlgItem(IDC_EDIT_FTP_PATH);
	CWnd *pEdFtpID = GetDlgItem(IDC_EDIT_FTP_ID);
	CWnd *pEdFtpPass = GetDlgItem(IDC_EDIT_FTP_PASS);
	CWnd *pGroupFtp = GetDlgItem(IDC_STATIC_GROUP_FTP);
    CWnd *pGroupFileList = GetDlgItem(IDC_STATIC_GROUP_FILE_LIST);
	CWnd *pGroupImage = GetDlgItem(IDC_STATIC_GROUP_IMAGE);
	CWnd *pStaticIP = GetDlgItem(IDC_STATIC_IP);
	CWnd *pStaticPort = GetDlgItem(IDC_STATIC_PORT);
	CWnd *pStaticPath = GetDlgItem(IDC_STATIC_PATH);
	CWnd *pStaticID = GetDlgItem(IDC_STATIC_USER_ID);
	CWnd *pStaticPass = GetDlgItem(IDC_STATIC_USER_PASS);
	CWnd *pLcFile = GetDlgItem(IDC_LIST_FILE_LIST);
	CWnd *pPcImage = GetDlgItem(IDC_STATIC_LAST_WRTIE_TIME_IMAGE);
	
	if (pBtnStart == NULL || pBtnStart->GetSafeHwnd() == NULL ||
		pBtnFtpCon == NULL || pBtnFtpCon->GetSafeHwnd() == NULL ||
		pBtnFtpDiscon == NULL || pBtnFtpDiscon->GetSafeHwnd() == NULL ||
		pBtnGetImg == NULL || pBtnGetImg->GetSafeHwnd() == NULL ||
		pBtnStart == NULL || pBtnStart->GetSafeHwnd() == NULL ||		
		pBtnEnd == NULL || pBtnEnd->GetSafeHwnd() == NULL ||
		pGroupWifi == NULL || pGroupWifi->GetSafeHwnd() == NULL ||
		pAddrFtpIP == NULL || pAddrFtpIP->GetSafeHwnd() == NULL ||
		pEdFtpPort == NULL || pEdFtpPort->GetSafeHwnd() == NULL ||
		pEdFtpPath == NULL || pEdFtpPath->GetSafeHwnd() == NULL ||
		pEdFtpID == NULL || pEdFtpID->GetSafeHwnd() == NULL ||
		pEdFtpPass == NULL || pEdFtpPass->GetSafeHwnd() == NULL ||
		pGroupFtp == NULL || pGroupFtp->GetSafeHwnd() == NULL ||
		pGroupFileList == NULL || pGroupFileList->GetSafeHwnd() == NULL ||
		pGroupImage == NULL || pGroupImage->GetSafeHwnd() == NULL ||
		pStaticIP == NULL || pStaticIP->GetSafeHwnd() == NULL ||
		pStaticPort == NULL || pStaticPort->GetSafeHwnd() == NULL ||
		pStaticPath == NULL || pStaticPath->GetSafeHwnd() == NULL ||
		pStaticID == NULL || pStaticID->GetSafeHwnd() == NULL ||
		pStaticPass == NULL || pStaticPass->GetSafeHwnd() == NULL ||
		pLcFile == NULL || pLcFile->GetSafeHwnd() == NULL ||
		pPcImage == NULL || pPcImage->GetSafeHwnd() == NULL ||
		false)
		return;

	pGroupFtp->ShowWindow(SW_HIDE);

	CRect rcDlg;
	GetClientRect(rcDlg);

	// 수집시작 버튼
	CRect rcBtnStart;
	pBtnStart->GetWindowRect(rcBtnStart);
	ScreenToClient(rcBtnStart);
	pBtnStart->SetWindowPos(NULL,
							iGap10,
							rcDlg.Height() - rcBtnStart.Height() - iGap10,
							rcBtnStart.Width(),
							rcBtnStart.Height(),
							SWP_NOACTIVATE | SWP_NOZORDER);
	pBtnStart->GetWindowRect(rcBtnStart);
	ScreenToClient(rcBtnStart);

	// 수집 종료 버튼
	CRect rcBtnEnd;
	pBtnEnd->GetWindowRect(rcBtnEnd);
	ScreenToClient(rcBtnEnd);
	pBtnEnd->SetWindowPos(NULL,
						  rcBtnStart.right + iGap10,
						  rcBtnStart.top,
						  rcBtnEnd.Width(),
						  rcBtnEnd.Height(),
						  SWP_NOACTIVATE | SWP_NOZORDER);
	pBtnEnd->GetWindowRect(rcBtnEnd);
	ScreenToClient(rcBtnEnd);
	
	// login
	CRect rcBtnConn;
	pBtnFtpCon->GetWindowRect(rcBtnConn);
	ScreenToClient(rcBtnConn);
	pBtnFtpCon->SetWindowPos(NULL,
							 iGap10,
							 rcBtnEnd.top - rcBtnConn.Height() - iGap10,
							 rcBtnConn.Width(),
							 rcBtnConn.Height(),
							 SWP_NOACTIVATE | SWP_NOZORDER);
	pBtnFtpCon->GetWindowRect(rcBtnConn);
	ScreenToClient(rcBtnConn);

	// logout
	CRect rcBtnDisconn;
	pBtnFtpDiscon->GetWindowRect(rcBtnDisconn);
	ScreenToClient(rcBtnDisconn);
	pBtnFtpDiscon->SetWindowPos(NULL,
								rcBtnConn.right + iGap10,
								rcBtnConn.top,
								rcBtnDisconn.Width(),
								rcBtnDisconn.Height(),
								SWP_NOACTIVATE | SWP_NOZORDER);
	pBtnFtpDiscon->GetWindowRect(rcBtnDisconn);
	ScreenToClient(rcBtnDisconn);

	// 최근 이미지 표시
	CRect rcBtnGetImg;
	pBtnGetImg->GetWindowRect(rcBtnGetImg);
	ScreenToClient(rcBtnGetImg);
	pBtnGetImg->SetWindowPos(NULL,
							 rcBtnDisconn.right + iGap10,
							 rcBtnConn.top,
							 rcBtnGetImg.Width(),
							 rcBtnGetImg.Height(),
							 SWP_NOACTIVATE | SWP_NOZORDER);
	pBtnGetImg->GetWindowRect(rcBtnGetImg);
	ScreenToClient(rcBtnGetImg);

	// ftp 그룹
	CRect rcGroupFtp;
	pGroupFtp->GetWindowRect(rcGroupFtp);
	ScreenToClient(rcGroupFtp);
	pGroupFtp->SetWindowPos(NULL,
							iGap10,
							rcBtnConn.top - rcGroupFtp.Height() - iGap10,
							rcGroupFtp.Width(),
							rcGroupFtp.Height(),
							SWP_NOACTIVATE | SWP_NOZORDER);
	pGroupFtp->GetWindowRect(rcGroupFtp);
	ScreenToClient(rcGroupFtp);

	// Wifi 그룹
	CRect rcGroupWifi;
	pGroupWifi->GetWindowRect(rcGroupWifi);
	ScreenToClient(rcGroupWifi);
	pGroupWifi->SetWindowPos(NULL,
							 iGap10,
							 iGap10,
							 rcGroupWifi.Width(),
							 rcGroupFtp.top - iGap10,
							 SWP_NOACTIVATE | SWP_NOZORDER);
	pGroupWifi->GetWindowRect(rcGroupWifi);
	ScreenToClient(rcGroupWifi);

	// IP - STATIC
	CRect rcStaticIP;
	pStaticIP->GetWindowRect(rcStaticIP);
	ScreenToClient(rcStaticIP);
	pStaticIP->SetWindowPos(NULL,
							rcGroupFtp.left + iGap10,
							rcGroupFtp.top + iGap10*2,
							rcStaticIP.Width(),
							rcStaticIP.Height(),
							SWP_NOACTIVATE | SWP_NOZORDER);
	pStaticIP->GetWindowRect(rcStaticIP);
	ScreenToClient(rcStaticIP);

	// Port - STATIC
	CRect rcStaticPort;
	pStaticPort->GetWindowRect(rcStaticPort);
	ScreenToClient(rcStaticPort);
	pStaticPort->SetWindowPos(NULL,
							  rcStaticIP.left,
							  rcStaticIP.bottom + iGap2,
							  rcStaticPort.Width(),
							  rcStaticPort.Height(),
							  SWP_NOACTIVATE | SWP_NOZORDER);
	pStaticPort->GetWindowRect(rcStaticPort);
	ScreenToClient(rcStaticPort);
	
	// Path - STATIC
	CRect rcStaticPath;
	pStaticPath->GetWindowRect(rcStaticPath);
	ScreenToClient(rcStaticPath);
	pStaticPath->SetWindowPos(NULL,
							  rcStaticIP.left,
							  rcStaticPort.bottom + iGap2,
							  rcStaticPath.Width(),
							  rcStaticPath.Height(),
							  SWP_NOACTIVATE | SWP_NOZORDER);
	pStaticPath->GetWindowRect(rcStaticPath);
	ScreenToClient(rcStaticPath);

	// ID - STATIC
	CRect rcStaticID;
	pStaticID->GetWindowRect(rcStaticID);
	ScreenToClient(rcStaticID);
	pStaticID->SetWindowPos(NULL,
							rcStaticIP.left,
							rcStaticPath.bottom + iGap2,
							rcStaticID.Width(),
							rcStaticID.Height(),
							SWP_NOACTIVATE | SWP_NOZORDER);
	pStaticID->GetWindowRect(rcStaticID);
	ScreenToClient(rcStaticID);

	// PASS - STATIC
	CRect rcStaticPass;
	pStaticPass->GetWindowRect(rcStaticPass);
	ScreenToClient(rcStaticPass);
	pStaticPass->SetWindowPos(NULL,
							  rcStaticIP.left,
							  rcStaticID.bottom + iGap2,
							  rcStaticPass.Width(),
							  rcStaticPass.Height(),
							  SWP_NOACTIVATE | SWP_NOZORDER);
	pStaticPass->GetWindowRect(rcStaticPass);
	ScreenToClient(rcStaticPass);
	
	// IP
	CRect rcIP;
	pAddrFtpIP->GetWindowRect(rcIP);
	ScreenToClient(rcIP);
	pAddrFtpIP->SetWindowPos(NULL,
							 rcStaticIP.right + iGap10,
							 rcStaticIP.top,
							 rcIP.Width(),
							 rcIP.Height(),
							 SWP_NOACTIVATE | SWP_NOZORDER);
	pAddrFtpIP->GetWindowRect(rcIP);
	ScreenToClient(rcIP);
	
	// PORT
	CRect rcPort;
	pEdFtpPort->GetWindowRect(rcPort);
	ScreenToClient(rcPort);
	pEdFtpPort->SetWindowPos(NULL,
							 rcIP.left,
							 rcStaticPort.top,
							 rcPort.Width(),
							 rcPort.Height(),
							 SWP_NOACTIVATE | SWP_NOZORDER);
	pEdFtpPort->GetWindowRect(rcPort);
	ScreenToClient(rcPort);
	
	// PATH
	CRect rcPath;
	pEdFtpPath->GetWindowRect(rcPath);
	ScreenToClient(rcPath);
	pEdFtpPath->SetWindowPos(NULL,
							 rcIP.left,
							 rcStaticPath.top,
							 rcPath.Width(),
							 rcPath.Height(),
							 SWP_NOACTIVATE | SWP_NOZORDER);
	pEdFtpPath->GetWindowRect(rcPath);
	ScreenToClient(rcPath);

	// ID
	CRect rcID;
	pEdFtpID->GetWindowRect(rcID);
	ScreenToClient(rcID);
	pEdFtpID->SetWindowPos(NULL,
						   rcIP.left,
						   rcStaticID.top,
						   rcID.Width(),
						   rcID.Height(),
						   SWP_NOACTIVATE | SWP_NOZORDER);
	pEdFtpID->GetWindowRect(rcID);
	ScreenToClient(rcID);

	// Pass
	CRect rcPass;
	pEdFtpPass->GetWindowRect(rcPass);
	ScreenToClient(rcPass);
	pEdFtpPass->SetWindowPos(NULL,
							 rcIP.left,
							 rcStaticPass.top,
							 rcPass.Width(),
							 rcPass.Height(),
							 SWP_NOACTIVATE | SWP_NOZORDER);
	pEdFtpPass->GetWindowRect(rcPass);
	ScreenToClient(rcPass);

	// File List
	CRect rcGroupFileList;
	pGroupFileList->GetWindowRect(rcGroupFileList);
	ScreenToClient(rcGroupFileList);
	pGroupFileList->SetWindowPos(NULL,
								 rcGroupWifi.right + iGap5,
								 iGap10,
								 rcGroupFileList.Width(),
								 rcDlg.Height() - iGap10*2,
								 SWP_NOACTIVATE | SWP_NOZORDER);
	pGroupFileList->GetWindowRect(rcGroupFileList);
	ScreenToClient(rcGroupFileList);

	// List Control
	CRect rcLcFileList;
	pLcFile->GetWindowRect(rcLcFileList);
	ScreenToClient(rcLcFileList);
	pLcFile->SetWindowPos(NULL,
						  rcGroupFileList.left + iGap10,
						  rcGroupFileList.top + iGap10*2,
						  rcGroupFileList.Width() - iGap10*2,
						  rcGroupFileList.Height() - iGap10*3,
						  SWP_NOACTIVATE | SWP_NOZORDER);
	pLcFile->GetWindowRect(rcLcFileList);
	ScreenToClient(rcLcFileList);

	// Image Group
	CRect rcGroupImage;
	pGroupImage->GetWindowRect(rcGroupImage);
	ScreenToClient(rcGroupImage);
	pGroupImage->SetWindowPos(NULL,
							  rcGroupFileList.right + iGap5,
							  iGap10,
							  rcDlg.Width() - rcGroupFileList.right - iGap10,
							  rcDlg.Height() - iGap10*2,
							  SWP_NOACTIVATE | SWP_NOZORDER);
	pGroupImage->GetWindowRect(rcGroupImage);
	ScreenToClient(rcGroupImage);

	// Image
	CRect rcImage;
	pPcImage->GetWindowRect(rcImage);
	ScreenToClient(rcImage);
	pPcImage->SetWindowPos(NULL,
						   rcGroupImage.left + iGap10,
						   rcGroupImage.top + iGap10*2,
						   rcGroupImage.Width() - iGap10*2,
						   rcGroupImage.Height() - iGap10*3,
						   SWP_NOACTIVATE | SWP_NOZORDER);
	pPcImage->GetWindowRect(rcImage);
	ScreenToClient(rcImage);

	// invalidate
	pBtnFtpCon->Invalidate();
	pBtnFtpDiscon->Invalidate();
	pBtnGetImg->Invalidate();
	pBtnStart->Invalidate();
	pBtnEnd->Invalidate();
	pGroupWifi->Invalidate();
	pAddrFtpIP->Invalidate();
	pEdFtpPort->Invalidate();
	pEdFtpPath->Invalidate();
	pEdFtpID->Invalidate();
	pEdFtpPass->Invalidate();
	pGroupFtp->Invalidate();
	pGroupFileList->Invalidate();
	pGroupImage->Invalidate();
	pStaticIP->Invalidate();
	pStaticPort->Invalidate();
	pStaticPath->Invalidate();
	pStaticID->Invalidate();
	pStaticPass->Invalidate();
	pLcFile->Invalidate();
	pPcImage->Invalidate();

	pGroupFtp->ShowWindow(SW_SHOW);
}


void CFtpClientDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CFtpClientDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}

void CFtpClientDlg::OnBnClickedButtonFtpConn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strFtpIP(_T("")), strFtpPort(_T("")), strFtpID(_T("")), strFtpPass(_T("")), strFtpPath(_T(""));

	m_ctrlFtpIP.GetWindowText(strFtpIP);
	m_edFtpPort.GetWindowText(strFtpPort);
	m_edFtpPath.GetWindowText(strFtpPath);
	m_edFtpID.GetWindowText(strFtpID);
	m_edFtpPass.GetWindowText(strFtpPass);

	// 로그인 되어있으면 로그아웃
	if (m_ftp.IsConnected())
		m_ftp.Logout();

	// 로그인 시도
	BOOL bRes = m_ftp.Login(strFtpIP, strFtpID, strFtpPass, _ttoi(strFtpPort));
	if (!bRes)
	{
		// 실패
		DWORD dwRes;
		CString strErr;
		m_ftp.GetLastErrorMsg(dwRes, strErr);

		CString strMsg;
		strMsg.Format(_T("로그인 실패(%d, %s)"), dwRes, strMsg);
		AfxMessageBox(strMsg);
		return;
	}
	
	AfxMessageBox(_T("로그인 되었습니다."));

	// 경로 설정
	bRes = m_ftp.SetCurrentDir(strFtpPath);
	if (!bRes)
	{
		// 실패
		DWORD dwRes;
		CString strErr;
		m_ftp.GetLastErrorMsg(dwRes, strErr);

		CString strMsg;
		strMsg.Format(_T("경로 설정 실패(%d, %s)"), dwRes, strMsg);
		AfxMessageBox(strMsg);
	}
}


void CFtpClientDlg::OnBnClickedButtonFtpDisconn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 로그인 되어있으면 로그아웃
	if (m_ftp.IsConnected())
		m_ftp.Logout();
}

void CFtpClientDlg::OnBnClickedButtonGetLastImage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 로그인 되어있으면 로그아웃
	if (!m_ftp.IsConnected())
		m_ftp.Login();

	// 경로 설정
	CString strFtpPath(_T(""));
	m_edFtpPath.GetWindowText(strFtpPath);
	BOOL bRes = m_ftp.SetCurrentDir(strFtpPath);
	if (!bRes)
	{
		// 실패
		DWORD dwRes;
		CString strErr;
		m_ftp.GetLastErrorMsg(dwRes, strErr);

		CString strMsg;
		strMsg.Format(_T("경로 설정 실패(%d, %s)"), dwRes, strMsg);
		AfxMessageBox(strMsg);
		return;
	}

	// 파일 리스트 가져오기
	RemoveFileList();
	bRes = m_ftp.GetFtpFileList(m_arrFileList);
	if (!bRes)
	{
		// 실패
		DWORD dwRes;
		CString strErr;
		m_ftp.GetLastErrorMsg(dwRes, strErr);

		CString strMsg;
		strMsg.Format(_T("파일 리스트 가져오기 실패(%d, %s)"), dwRes, strMsg);
		AfxMessageBox(strMsg);
		return;
	}

	// 파일 리스트 화면에 표시
	DisplayFileList();

	// 가장 최근 이미지 파일 표시
	DisplayLastWriteImage();
}

void CFtpClientDlg::OnBnClickedButtonStartThread()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pThreadLastImage == NULL)
	{
		m_bFlagLastImage = TRUE;
		m_pThreadLastImage = AfxBeginThread(ThreadLastImage, this);
	}
}


void CFtpClientDlg::OnBnClickedButtonEndThread()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pThreadLastImage != NULL)
	{
		m_bFlagLastImage = FALSE;
		if (WaitForSingleObject(m_pThreadLastImage->m_hThread, INFINITE) != WAIT_OBJECT_0)
			TerminateThread(m_pThreadLastImage->m_hThread, 0);
		m_pThreadLastImage = NULL;
	}
}

void CFtpClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
		case 1:
			OnBnClickedButtonGetLastImage();
			break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


int CFtpClientDlg::CompareTime(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl *pList = (CListCtrl *)lParamSort;

	LVFINDINFO info1, info2;
	memset(&info1, 0, sizeof(info1));
	memset(&info2, 0, sizeof(info2));

	info1.flags = LVFI_PARAM;
	info1.lParam = lParam1;

	info2.flags = LVFI_PARAM;
	info2.lParam = lParam2;

	int nRow1 = pList->FindItem(&info1);
	int nRow2 = pList->FindItem(&info2);

	CFtpFileInfo *pItem1 = (CFtpFileInfo *)pList->GetItemData(nRow1);
	CFtpFileInfo *pItem2 = (CFtpFileInfo *)pList->GetItemData(nRow2);

	int nRes = 0;
	if (pItem1 != NULL && pItem2 != NULL)
	{
		CTime tm1 = pItem1->GetLastWriteTime();
		CTime tm2 = pItem2->GetLastWriteTime();

		CTimeSpan tmSpan = tm2 - tm1; // 시간 차이
		LONGLONG llSec = tmSpan.GetTotalSeconds();
		if (llSec == 0)
		{
			CString strTime1 = pItem1->GetFileName();
			CString strTime2 = pItem2->GetFileName();
			CString strTemp1(strTime1), strTemp2(strTime2);
			if (2 < strTime1.GetLength() && 2 < strTime2.GetLength())
			{
				// 이미지 파일 이름 앞에 PU, SW, SD 문자열이 붙어서 제거 후 정렬
				int iCompIndex = 2;
				strTemp1 = strTime1.Mid(iCompIndex, strTime1.GetLength()-iCompIndex);
				strTemp2 = strTime2.Mid(iCompIndex, strTime2.GetLength()-iCompIndex);
			}

			nRes = -_tcscmp(strTemp1, strTemp2);
		}
		else if (llSec < 0)
			nRes = -1;
		else if (0 < llSec)
			nRes = 1;
		else
			nRes = 0;
	}
	return nRes;
}

void CFtpClientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if ((nType != SIZE_MINIMIZED) && (cx > 0) && (cy > 0))
		MoveControl();
}

void CFtpClientDlg::OnNMClickListFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMHDR->idFrom == IDC_LIST_FILE_LIST && m_pThreadLastImage == NULL)
	{
		CSingleLock cr(&m_cr);
		cr.Lock();
		int iIndex = pNMItemActivate->iItem;
		int iCount = m_lcFileList.GetItemCount();
		if (0 <= iIndex && iIndex < iCount)
		{
			CFtpFileInfo *pFileInfo = (CFtpFileInfo *)m_lcFileList.GetItemData(iIndex);
			if (pFileInfo != NULL)
			{
				CString strFilePathName = pFileInfo->GetFilePathName();
				CString strLocalPathName;
				strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());
				BOOL bDown = m_ftp.DownloadFile(strFilePathName, strLocalPathName);
				if (bDown)
				{
					CString strLocalPathName;
					strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());

					if (!m_imgDisplay.IsNull())
						m_imgDisplay.Destroy();
					HRESULT hr = m_imgDisplay.Load(strLocalPathName);
					if (SUCCEEDED(hr))
					{
						CRect rect;
						m_Image.GetWindowRect(&rect); // 컨트롤 크기 얻기
						CDC *pDc = m_Image.GetDC();
						::SetStretchBltMode(*pDc, HALFTONE); // HALFTONE모드로 설정해서 크기 축소로 인한 이미지 깨짐 현상 최소화
						m_imgDisplay.Draw(*pDc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_imgDisplay.GetWidth(), m_imgDisplay.GetHeight());

						DeleteFile(strLocalPathName);
					}
				}
			}
		}
	}
	*pResult = 0;
}


void CFtpClientDlg::OnStnDblclickStaticLastWrtieTimeImage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSingleLock cr(&m_cr);
	cr.Lock();
	if (m_pDlgDisplay == NULL)
		m_pDlgDisplay = new CDlgDisplay(this);
	m_pDlgDisplay->SetImageCriticalSection(&m_cr);
	m_pDlgDisplay->SetImage(&m_imgDisplay);
	cr.Unlock();

	m_pDlgDisplay->DoModal();

	delete m_pDlgDisplay;
	m_pDlgDisplay = NULL;
}


UINT CFtpClientDlg::ThreadLastImage(LPVOID lpvoid)
{
	CFtpClientDlg *pThis = (CFtpClientDlg *)lpvoid;

	while (pThis->m_bFlagLastImage)
	{
		if (!pThis->m_ftp.IsConnected())
		{
			CString strFtpIP(_T("")), strFtpPort(_T("")), strFtpID(_T("")), strFtpPass(_T(""));

			pThis->m_ctrlFtpIP.GetWindowText(strFtpIP);
			pThis->m_edFtpPort.GetWindowText(strFtpPort);
			pThis->m_edFtpID.GetWindowText(strFtpID);
			pThis->m_edFtpPass.GetWindowText(strFtpPass);

			// 로그인 시도
			BOOL bRes = pThis->m_ftp.Login(strFtpIP, strFtpID, strFtpPass, _ttoi(strFtpPort));
			if (bRes)
			{
				CString strFtpPath = _T("");
				pThis->m_edFtpPath.GetWindowText(strFtpPath);
				bRes = pThis->m_ftp.SetCurrentDir(strFtpPath);
				if (bRes)
				{

				}
			}
		}

		if (pThis->m_ftp.IsConnected())
		{
			CSingleLock cr(&pThis->m_cr);
			cr.Lock();
			// 파일 리스트 가져오기
			pThis->RemoveFileList();
			BOOL bRes = pThis->m_ftp.GetFtpFileList(pThis->m_arrFileList);
			if (!bRes)
			{
				// 실패
				DWORD dwRes;
				CString strErr;
				pThis->m_ftp.GetLastErrorMsg(dwRes, strErr);

				CString strMsg;
				strMsg.Format(_T("파일 리스트 가져오기 실패(%d, %s)\n"), dwRes, strMsg);
				TRACE(strMsg);
				continue;
			}

			// 파일 리스트 화면에 표시
			pThis->DisplayFileList();

			// 가장 최근 이미지 파일 표시
			pThis->DisplayLastWriteImage();
		}

		Sleep(5000);
	}

	return 0;
}

