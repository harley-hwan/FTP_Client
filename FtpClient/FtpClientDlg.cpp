
// FtpClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "FtpClient.h"
#include "FtpClientDlg.h"
#include "afxdialogex.h"
#include <string>
#include <fstream>

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
	m_iFtpShowFileType = 0;
	m_iListCtrlFileType = 0;

	m_ftp.SetOwner(this);
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

	int iCount = (int)m_arrFileList.GetCount();
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
	//DDX_Control(pDX, IDC_EDIT_FTP_PATH, m_edFtpPath);
	DDX_Control(pDX, IDC_LIST_FILE_LIST, m_lcFileList);
	DDX_Control(pDX, IDC_LIST_WIFI_LIST, m_lcWifiList);
	DDX_Control(pDX, IDC_STATIC_LAST_WRTIE_TIME_IMAGE, m_stDispImage);
	DDX_Control(pDX, IDC_COMBO_FTP_PATH, m_cbFtpPath);
	DDX_Control(pDX, IDC_RADIO_IMAGE, m_rdImage);
	DDX_Control(pDX, IDC_EDIT_LAST_WRTIE_TIME_IMAGE, m_edDispText);
}

BEGIN_MESSAGE_MAP(CFtpClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CFtpClientDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFtpClientDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_FTP_CONN, &CFtpClientDlg::OnBnClickedButtonFtpConn)
	ON_BN_CLICKED(IDC_BUTTON_GET_LAST_IMAGE, &CFtpClientDlg::OnBnClickedButtonGetLastFile)
	ON_BN_CLICKED(IDC_BUTTON_FTP_DISCONN, &CFtpClientDlg::OnBnClickedButtonFtpDisconn)
	ON_BN_CLICKED(IDC_BUTTON_START_THREAD, &CFtpClientDlg::OnBnClickedButtonStartThread)
	ON_BN_CLICKED(IDC_BUTTON_END_THREAD, &CFtpClientDlg::OnBnClickedButtonEndThread)
	ON_WM_SIZE()
	ON_STN_DBLCLK(IDC_STATIC_LAST_WRTIE_TIME_IMAGE, &CFtpClientDlg::OnStnDblclickStaticLastWrtieTimeImage)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_IMAGE, IDC_RADIO_TEXT, &CFtpClientDlg::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_BUTTON_DOWN_ALL_FILE, &CFtpClientDlg::OnBnClickedButtonDownAllFile)
	//ON_NOTIFY(NM_CLICK, IDC_LIST_FILE_LIST, &CFtpClientDlg::OnNMClickListFileList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILE_LIST, &CFtpClientDlg::OnLvnItemchangedListFileList)
	//ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_WIFI_LIST, &CFtpClientDlg::OnLvnItemchangedListWifiList)
	ON_CBN_SELCHANGE(IDC_COMBO_FTP_PATH, &CFtpClientDlg::OnCbnSelchangeComboFtpPath)
	ON_BN_CLICKED(IDC_BUTTON_WIFI_SCAN, &CFtpClientDlg::OnBnClickedButtonWifiScan)
	//ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_WIFI_LIST, &CFtpClientDlg::OnLvnItemchangedListWifiList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_WIFI_LIST, &CFtpClientDlg::OnDblclkListWifiList)
	ON_BN_CLICKED(IDC_BUTTON_WIFI_CONNECT, &CFtpClientDlg::OnBnClickedButtonWifiConnect)
	ON_BN_CLICKED(IDC_BUTTON_RTC_SET, &CFtpClientDlg::OnBnClickedButtonRtcSet)
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
    m_cbFtpPath.InsertString(0, _T("/Result"));
    m_cbFtpPath.InsertString(1, _T("/Log"));
    m_cbFtpPath.InsertString(2, _T("/BT_Log"));
    m_cbFtpPath.InsertString(3, _T("/ShotDB"));

    m_cbFtpPath.SetCurSel(GetFtpPathFromConfig());

    m_lcFileList.SetExtendedStyle(/*LVS_EX_GRIDLINES | */LVS_EX_FULLROWSELECT); // 리스트 스타일
    m_lcFileList.InsertColumn(0, _T("Time"), LVCFMT_CENTER, 150);
    m_lcFileList.InsertColumn(1, _T("File Name"), LVCFMT_CENTER, 250);

    // WiFi 리스트 컨트롤 스타일 수정
    m_lcWifiList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES); // 확장 스타일 설정
    m_lcWifiList.ModifyStyle(0, WS_HSCROLL | WS_VSCROLL); // 가로/세로 스크롤바 추가
    m_lcWifiList.InsertColumn(0, _T("WiFi Name"), LVCFMT_LEFT, 250);

    m_iFtpShowFileType = 0;
    m_rdImage.SetCheck(TRUE);
    ShowControlByFileType(m_iFtpShowFileType);

    RemoveFileList();

    MoveControl();

    InitEnableControl();
}


void CFtpClientDlg::RemoveFileList()
{
	CSingleLock cr(&m_cr);
	cr.Lock();
	m_lcFileList.DeleteAllItems();
	int iCount = (int)m_arrFileList.GetCount();
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

	for (int iIndex = 0; iIndex < iCount; iIndex++)
	{
		CFtpFileInfo* pFileInfo = (CFtpFileInfo*)m_arrFileList.GetAt(iIndex);
		int iListCount = m_lcFileList.GetItemCount();
		SetFileInfoToList(iListCount, pFileInfo);
	}
	m_lcFileList.SortItems(CompareTime, (LPARAM)&m_lcFileList);
	m_lcFileList.SetRedraw(TRUE);
}

void CFtpClientDlg::DisplayLastFile(int iFileType)
{
	if (iFileType == 0)
		DisplayLastWriteImage();
	else if (iFileType == 1)
		DisplayLastWriteText();
	else
		TRACE(_T("Unknown File Type(%d)\n"), iFileType);
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

		CString strFilePathName = pFileInfo->GetFileDownPathName();
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
				m_stDispImage.GetWindowRect(&rect); // 컨트롤 크기 얻기
				CDC *pDc = m_stDispImage.GetDC();
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

void CFtpClientDlg::DisplayLastWriteText()
{
	int iCount = m_lcFileList.GetItemCount();
	if (0 < iCount)
	{
		CSingleLock cr(&m_cr);
		cr.Lock();
		CFtpFileInfo *pFileInfo = (CFtpFileInfo *)m_lcFileList.GetItemData(0);
		if (pFileInfo == NULL)
			AfxMessageBox(_T("Data Load Fail"));

		CString strFilePathName = pFileInfo->GetFileDownPathName();
		CString strLocalPathName;
		strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());
		BOOL bDown = m_ftp.DownloadFile(strFilePathName, strLocalPathName);
		if (bDown)
		{
			CString strLocalPathName;
			strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());
			if (m_fileText.m_hFile != CStdioFile::hFileNull)
				m_fileText.Close();
			BOOL bOpen = m_fileText.Open(strLocalPathName,
										 CStdioFile::modeNoTruncate |
										 CStdioFile::typeText |
										 CStdioFile::modeReadWrite |
										 CStdioFile::shareDenyNone);
			if (bOpen)
			{
				CString strTotal(_T("")), strRead(_T(""));
				while (m_fileText.ReadString(strRead))
					strTotal += strRead + _T("\r\n");
				m_edDispText.SetWindowText(strTotal);
				m_fileText.Close();

				DeleteFile(strLocalPathName);
			}
			else
				AfxMessageBox(_T("Text File Load Fail"));
		}
		else
			AfxMessageBox(_T("파일 다운로드 실패"));
	}
}

void CFtpClientDlg::InitEnableControl()
{
	GetDlgItem(IDC_IPADDRESS_FTP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_FTP_PORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_FTP_PATH)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_FTP_PATH)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_FTP_ID)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_FTP_PASS)->EnableWindow(TRUE);

	GetDlgItem(IDC_BUTTON_FTP_CONN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_FTP_DISCONN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GET_LAST_IMAGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_THREAD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_END_THREAD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DOWN_ALL_FILE)->EnableWindow(FALSE);

	GetDlgItem(IDC_RADIO_IMAGE)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_TEXT)->EnableWindow(TRUE);
}

void CFtpClientDlg::LoginEnableControl(BOOL bLogin)
{
	if (bLogin)
	{
		GetDlgItem(IDC_IPADDRESS_FTP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FTP_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FTP_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_FTP_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FTP_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FTP_PASS)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_FTP_CONN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FTP_DISCONN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_GET_LAST_IMAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_START_THREAD)->EnableWindow(!m_bFlagLastImage);
		GetDlgItem(IDC_BUTTON_END_THREAD)->EnableWindow(m_bFlagLastImage);
		GetDlgItem(IDC_BUTTON_DOWN_ALL_FILE)->EnableWindow(TRUE);

		GetDlgItem(IDC_RADIO_IMAGE)->EnableWindow(TRUE);
		if (m_cbFtpPath.GetCurSel() == 3)
			GetDlgItem(IDC_RADIO_TEXT)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_RADIO_TEXT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_IPADDRESS_FTP)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FTP_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FTP_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_FTP_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FTP_ID)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FTP_PASS)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUTTON_FTP_CONN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FTP_DISCONN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_GET_LAST_IMAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_START_THREAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_END_THREAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWN_ALL_FILE)->EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_IMAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TEXT)->EnableWindow(TRUE);
	}
}

void CFtpClientDlg::ThreadEnableControl(BOOL bStart)
{
	if (bStart)
	{
		GetDlgItem(IDC_IPADDRESS_FTP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FTP_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FTP_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_FTP_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FTP_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FTP_PASS)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_FTP_CONN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FTP_DISCONN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_GET_LAST_IMAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_START_THREAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_END_THREAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DOWN_ALL_FILE)->EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_IMAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_TEXT)->EnableWindow(FALSE);
	}
	else
		LoginEnableControl(m_ftp.IsConnected());
}

void CFtpClientDlg::RadioEnableControl(int iFileType)
{
	if (iFileType == 0)
	{
		ThreadEnableControl(m_bFlagLastImage);
	}
	else if (iFileType == 1)
	{
		GetDlgItem(IDC_BUTTON_START_THREAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_END_THREAD)->EnableWindow(FALSE);
	}
}

void CFtpClientDlg::ShowControlByFileType(int iFileType)
{
	// 0: Image, 1: Text
	if (iFileType == 0) 
	{
		m_stDispImage.ShowWindow(SW_SHOW);
		m_edDispText.ShowWindow(SW_HIDE);
	}
	else
	{
		m_edDispText.SetWindowText(_T(""));
		m_stDispImage.ShowWindow(SW_HIDE);
		m_edDispText.ShowWindow(SW_SHOW);
	}
}

int CFtpClientDlg::GetFtpPathType()
{
	return m_cbFtpPath.GetCurSel();
}

int CFtpClientDlg::GetFtpShowFileType()
{
	return m_iFtpShowFileType;
}

int CFtpClientDlg::GetFtpPathFromConfig()
{
	TCHAR atcRead[MAX_PATH] ={0,};
	DWORD dwReadSize = GetPrivateProfileString(SECTION_CONFIG, KEY_FTP_PATH, _T("0"), atcRead, sizeof(atcRead), FILE_NAME_PATH);
	int iFtpPath = _ttoi(atcRead);
	return iFtpPath;
}

void CFtpClientDlg::SetFtpPathToConfig(int iIndex)
{
	CString strFtpPath(_T(""));
	strFtpPath.Format(_T("%d"), iIndex);
	WritePrivateProfileString(SECTION_CONFIG, KEY_FTP_PATH, strFtpPath, FILE_NAME_PATH);
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
	//GROUPBOX        "WiFi List",IDC_STATIC_GROUP_WIFI,7,7,157,185
	//GROUPBOX        "FTP Info",IDC_STATIC_GROUP_FTP,7,196,157,118
	//RTEXT           "Server IP : ",IDC_STATIC_IP,12,208,59,14,SS_CENTERIMAGE
	//CONTROL         "",IDC_IPADDRESS_FTP,"SysIPAddress32",WS_TABSTOP,78,208,79,15
	//RTEXT           "Port No : ",IDC_STATIC_PORT,12,224,59,14,SS_CENTERIMAGE
	//EDITTEXT        IDC_EDIT_FTP_PORT,78,224,79,14,ES_AUTOHSCROLL
	//RTEXT           "FTP Path : ",IDC_STATIC_PATH,12,240,59,14,SS_CENTERIMAGE
	//EDITTEXT        IDC_EDIT_FTP_PATH,147,258,24,14,ES_AUTOHSCROLL | NOT WS_VISIBLE
	//RTEXT           "User ID : ",IDC_STATIC_USER_ID,12,256,59,14,SS_CENTERIMAGE
	//EDITTEXT        IDC_EDIT_FTP_ID,78,256,79,14,ES_AUTOHSCROLL
	//RTEXT           "User Password : ",IDC_STATIC_USER_PASS,13,272,56,14,SS_CENTERIMAGE
	//EDITTEXT        IDC_EDIT_FTP_PASS,78,273,79,14,ES_AUTOHSCROLL
	//PUSHBUTTON      "Login",IDC_BUTTON_FTP_CONN,7,319,45,14
	//PUSHBUTTON      "Logout",IDC_BUTTON_FTP_DISCONN,55,319,45,14
	//GROUPBOX        "File List",IDC_STATIC_GROUP_FILE_LIST,168,7,241,345
	//CONTROL         "",IDC_LIST_FILE_LIST,"SysListView32",LVS_REPORT | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP,175,17,226,326
	//GROUPBOX        "Last Write Time Image",IDC_STATIC_GROUP_IMAGE,412,7,386,345
	//CONTROL         "",IDC_STATIC_LAST_WRTIE_TIME_IMAGE,"Static",SS_BLACKFRAME | SS_NOTIFY | SS_CENTERIMAGE,420,19,372,322
	//PUSHBUTTON      "최근 파일 표시",IDC_BUTTON_GET_LAST_IMAGE,107,319,45,14
	//PUSHBUTTON      "수집 시작",IDC_BUTTON_START_THREAD,7,338,45,14
	//PUSHBUTTON      "수집 종료",IDC_BUTTON_END_THREAD,55,338,45,14
	//COMBOBOX        IDC_COMBO_FTP_PATH,78,241,79,30,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
	//CONTROL         "Image",IDC_RADIO_IMAGE,"Button",BS_AUTORADIOBUTTON | WS_GROUP,24,295,35,10
	//CONTROL         "Text",IDC_RADIO_TEXT,"Button",BS_AUTORADIOBUTTON,84,293,31,10
	//EDITTEXT        IDC_EDIT_LAST_WRTIE_TIME_IMAGE,419,19,68,58,ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | NOT WS_VISIBLE | WS_VSCROLL
	//PUSHBUTTON      "모든 파일 다운",IDC_BUTTON_DOWN_ALL_FILE,106,338,45,14
	int iGap10 = 10, iGap5 = 5, iGap2 = 2;
	CWnd* pBtnWiFiScan = GetDlgItem(IDC_BUTTON_WIFI_SCAN);
	CWnd* pBtnWifiConnect = GetDlgItem(IDC_BUTTON_WIFI_CONNECT);
	CWnd* pBtnRtcSet = GetDlgItem(IDC_BUTTON_RTC_SET);

	CWnd* pBtnFtpCon = GetDlgItem(IDC_BUTTON_FTP_CONN);
	CWnd* pBtnFtpDiscon = GetDlgItem(IDC_BUTTON_FTP_DISCONN);
	CWnd* pBtnGetImg = GetDlgItem(IDC_BUTTON_GET_LAST_IMAGE);
	CWnd* pBtnStart = GetDlgItem(IDC_BUTTON_START_THREAD);
	CWnd* pBtnEnd = GetDlgItem(IDC_BUTTON_END_THREAD);
	CWnd* pBtnDownAllFile = GetDlgItem(IDC_BUTTON_DOWN_ALL_FILE);
	CWnd* pGroupWifi = GetDlgItem(IDC_STATIC_GROUP_WIFI);
	CWnd* pAddrFtpIP = GetDlgItem(IDC_IPADDRESS_FTP);
	CWnd* pEdFtpPort = GetDlgItem(IDC_EDIT_FTP_PORT);
	//CWnd *pEdFtpPath = GetDlgItem(IDC_EDIT_FTP_PATH);
	CWnd* pCbFtpPath = GetDlgItem(IDC_COMBO_FTP_PATH);
	CWnd* pEdFtpID = GetDlgItem(IDC_EDIT_FTP_ID);
	CWnd* pEdFtpPass = GetDlgItem(IDC_EDIT_FTP_PASS);
	CWnd* pGroupFtp = GetDlgItem(IDC_STATIC_GROUP_FTP);
	CWnd* pGroupFileList = GetDlgItem(IDC_STATIC_GROUP_FILE_LIST);
	CWnd* pGroupImage = GetDlgItem(IDC_STATIC_GROUP_IMAGE);
	CWnd* pStaticIP = GetDlgItem(IDC_STATIC_IP);
	CWnd* pStaticPort = GetDlgItem(IDC_STATIC_PORT);
	CWnd* pStaticPath = GetDlgItem(IDC_STATIC_PATH);
	CWnd* pStaticID = GetDlgItem(IDC_STATIC_USER_ID);
	CWnd* pStaticPass = GetDlgItem(IDC_STATIC_USER_PASS);
	CWnd* pLcFile = GetDlgItem(IDC_LIST_FILE_LIST);
	CWnd* pLcWifi = GetDlgItem(IDC_LIST_WIFI_LIST);
	CWnd* pPcImage = GetDlgItem(IDC_STATIC_LAST_WRTIE_TIME_IMAGE);
	CWnd* pedImage = GetDlgItem(IDC_EDIT_LAST_WRTIE_TIME_IMAGE);
	CWnd* pRdImage = GetDlgItem(IDC_RADIO_IMAGE);
	CWnd* pRdText = GetDlgItem(IDC_RADIO_TEXT);

	if (pBtnStart == NULL || pBtnStart->GetSafeHwnd() == NULL ||
		pBtnFtpCon == NULL || pBtnFtpCon->GetSafeHwnd() == NULL ||
		pBtnFtpDiscon == NULL || pBtnFtpDiscon->GetSafeHwnd() == NULL ||
		pBtnGetImg == NULL || pBtnGetImg->GetSafeHwnd() == NULL ||
		pBtnStart == NULL || pBtnStart->GetSafeHwnd() == NULL ||
		pBtnEnd == NULL || pBtnEnd->GetSafeHwnd() == NULL ||
		pBtnDownAllFile == NULL || pBtnDownAllFile->GetSafeHwnd() == NULL ||
		pGroupWifi == NULL || pGroupWifi->GetSafeHwnd() == NULL ||
		pAddrFtpIP == NULL || pAddrFtpIP->GetSafeHwnd() == NULL ||
		pEdFtpPort == NULL || pEdFtpPort->GetSafeHwnd() == NULL ||
		pCbFtpPath == NULL || pCbFtpPath->GetSafeHwnd() == NULL ||
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
		pedImage == NULL || pedImage->GetSafeHwnd() == NULL ||
		pRdImage == NULL || pRdImage->GetSafeHwnd() == NULL ||
		pRdText == NULL || pRdText->GetSafeHwnd() == NULL ||
		pBtnWiFiScan == NULL || pBtnWifiConnect == NULL ||
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

	// 모든 파일 다운로드 버튼
	CRect rcBtnDownAll;
	pBtnDownAllFile->GetWindowRect(rcBtnDownAll);
	ScreenToClient(rcBtnDownAll);
	pBtnDownAllFile->SetWindowPos(NULL,
		rcBtnEnd.right + iGap10,
		rcBtnStart.top,
		rcBtnDownAll.Width(),
		rcBtnDownAll.Height(),
		SWP_NOACTIVATE | SWP_NOZORDER);
	pBtnDownAllFile->GetWindowRect(rcBtnDownAll);
	ScreenToClient(rcBtnDownAll);

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
							 rcGroupFtp.top - iGap10*6,
							 SWP_NOACTIVATE | SWP_NOZORDER);
	pGroupWifi->GetWindowRect(rcGroupWifi);
	ScreenToClient(rcGroupWifi);

	// Wifi List Control 
	CRect rcLcWifiList;
	pLcWifi->GetWindowRect(rcLcWifiList);
	ScreenToClient(rcLcWifiList);
	pLcWifi->SetWindowPos(NULL,
		rcGroupWifi.left + iGap10,
		rcGroupWifi.top + iGap10*2,
		rcGroupWifi.Width() - iGap10*2,
		rcGroupWifi.Height() - iGap10*3,
		SWP_NOACTIVATE | SWP_NOZORDER);
	pLcWifi->GetWindowRect(rcLcWifiList);
	ScreenToClient(rcLcWifiList);

	// WiFiScan
	CRect rcBtnWifiScan;
	pBtnWiFiScan->GetWindowRect(rcBtnWifiScan);
	ScreenToClient(rcBtnWifiScan);
	pBtnWiFiScan->SetWindowPos(NULL,
		iGap10,
		rcGroupWifi.bottom + iGap10,
		rcBtnConn.Width(),
		rcBtnConn.Height(),
		SWP_NOACTIVATE | SWP_NOZORDER);
	pBtnWiFiScan->GetWindowRect(rcBtnWifiScan);
	ScreenToClient(rcBtnWifiScan);

	// WiFi Connect
	CRect rcBtnWifiConnect;
	if (pBtnWifiConnect && pBtnWifiConnect->GetSafeHwnd())
	{
		pBtnWifiConnect->GetWindowRect(rcBtnWifiConnect);

		ScreenToClient(rcBtnWifiConnect);
		pBtnWifiConnect->SetWindowPos(NULL,
			rcBtnWifiScan.right + iGap10,
			rcBtnWifiScan.top,
			rcBtnConn.Width(),
			rcBtnConn.Height(),
			SWP_NOACTIVATE | SWP_NOZORDER);
		pBtnWifiConnect->GetWindowRect(rcBtnWifiConnect);
		ScreenToClient(rcBtnWifiConnect);

		pBtnWifiConnect->Invalidate();
	}

	// RTC Setting

	if (pBtnRtcSet && pBtnRtcSet->GetSafeHwnd())
	{
		CRect rcBtnRtcSet;
		pBtnRtcSet->GetWindowRect(rcBtnRtcSet);
		ScreenToClient(rcBtnRtcSet);
		pBtnRtcSet->SetWindowPos(NULL,
			rcBtnWifiConnect.right + iGap10,
			rcBtnWifiConnect.top,
			rcBtnConn.Width(),
			rcBtnConn.Height(),
			SWP_NOACTIVATE | SWP_NOZORDER);
		pBtnRtcSet->GetWindowRect(rcBtnRtcSet);
		ScreenToClient(rcBtnRtcSet);

		pBtnRtcSet->Invalidate();
	}
	
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
	pCbFtpPath->GetWindowRect(rcPath);
	ScreenToClient(rcPath);
	pCbFtpPath->SetWindowPos(NULL,
							 rcIP.left,
							 rcStaticPath.top,
							 rcPath.Width(),
							 rcPath.Height(),
							 SWP_NOACTIVATE | SWP_NOZORDER);
	pCbFtpPath->GetWindowRect(rcPath);
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

	// Radio Image
	CRect rcRadioImage;
	pRdImage->GetWindowRect(rcRadioImage);
	ScreenToClient(rcRadioImage);
	pRdImage->SetWindowPos(NULL,
						   rcIP.left,
						   rcStaticPass.bottom + iGap5,
						   rcRadioImage.Width(),
						   rcRadioImage.Height(),
						   SWP_NOACTIVATE | SWP_NOZORDER);
	pRdImage->GetWindowRect(rcRadioImage);
	ScreenToClient(rcRadioImage);

	// Radio Text
	CRect rcRadioText;
	pRdText->GetWindowRect(rcRadioText);
	ScreenToClient(rcRadioText);
	pRdText->SetWindowPos(NULL,
						  rcRadioImage.right + iGap10,
						  rcRadioImage.top,
						  rcRadioText.Width(),
						  rcRadioText.Height(),
						  SWP_NOACTIVATE | SWP_NOZORDER);
	pRdText->GetWindowRect(rcRadioText);
	ScreenToClient(rcRadioText);

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

	// Text
	CRect rcText;
	pedImage->GetWindowRect(rcText);
	ScreenToClient(rcText);
	pedImage->SetWindowPos(NULL,
						   rcGroupImage.left + iGap10,
						   rcGroupImage.top + iGap10*2,
						   rcGroupImage.Width() - iGap10*2,
						   rcGroupImage.Height() - iGap10*3,
						   SWP_NOACTIVATE | SWP_NOZORDER);
	pedImage->GetWindowRect(rcText);
	ScreenToClient(rcText);
	

	// invalidate
	pBtnFtpCon->Invalidate();
	pBtnFtpDiscon->Invalidate();
	pBtnGetImg->Invalidate();
	pBtnStart->Invalidate();
	pBtnEnd->Invalidate();
	pGroupWifi->Invalidate();
	pAddrFtpIP->Invalidate();
	pEdFtpPort->Invalidate();
	pCbFtpPath->Invalidate();
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
	pedImage->Invalidate();
	pRdImage->Invalidate();
	pRdText->Invalidate();

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
	//m_edFtpPath.GetWindowText(strFtpPath);
	m_cbFtpPath.GetWindowText(strFtpPath);
	m_cbFtpPath.GetWindowText(strFtpPath);
	m_edFtpID.GetWindowText(strFtpID);
	m_edFtpPass.GetWindowText(strFtpPass);

	// 로그인 되어있으면 로그아웃
	if (m_ftp.IsConnected())
	{
		m_ftp.Logout();
	}

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
	//if (m_ftp.IsConnected())
	//	m_ftp.Logout();
	m_ftp.Logout();
}

void CFtpClientDlg::OnBnClickedButtonGetLastFile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 로그인 되어있으면 로그아웃
	if (!m_ftp.IsConnected())
		m_ftp.Login();

	// 경로 설정
	CString strFtpPath(_T(""));
	m_cbFtpPath.GetWindowText(strFtpPath);
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

	if (m_arrFileList.GetCount() <= 0)
	{
		AfxMessageBox(_T("파일이 없습니다."));
		return;
	}

	// 파일 리스트 화면에 표시
	DisplayFileList();

	// 가장 최근 파일 표시
	DisplayLastFile(m_iFtpShowFileType);

	m_iListCtrlFileType = m_iFtpShowFileType;
}

void CFtpClientDlg::OnBnClickedButtonStartThread()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_iFtpShowFileType != 0)
	{
		AfxMessageBox(_T("Image 라디오 버튼 선택시만 가능합니다."));
		return;
	}

	if (m_pThreadLastImage == NULL)
	{
		m_bFlagLastImage = TRUE;
		m_pThreadLastImage = AfxBeginThread(ThreadLastImage, this);
		ThreadEnableControl(m_bFlagLastImage);
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
		ThreadEnableControl(m_bFlagLastImage);
	}
}


void CFtpClientDlg::OnBnClickedRadio(UINT uiResID)
{
	if (uiResID == IDC_RADIO_IMAGE)
	{
		m_iFtpShowFileType = 0;
	}
	else if (uiResID == IDC_RADIO_TEXT)
	{
		if (m_bFlagLastImage == TRUE)
		{
			AfxMessageBox(_T("수집 중에는 라디오버튼을 변경할 수 없습니다."));
			((CButton *)GetDlgItem(IDC_RADIO_TEXT))->SetCheck(FALSE);
			return;
		}
		m_iFtpShowFileType = 1;
	}
	ShowControlByFileType(m_iFtpShowFileType);
	RadioEnableControl(m_iFtpShowFileType);
}

void CFtpClientDlg::OnBnClickedButtonDownAllFile()
{
	if (m_bFlagLastImage == TRUE)
	{
		AfxMessageBox(_T("수집 중에는 다운로드할 수 없습니다."));
		return;
	}

	// 경로 설정
	CString strFtpPath(_T(""));
	m_cbFtpPath.GetWindowText(strFtpPath);
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

	CString strDownDir = _T(".\\Download");
	CString strDirType, strDownFullPath;
	strDirType = strFtpPath;
	strDirType.Replace(_T("/"), _T(""));
	strDownFullPath.Format(_T("%s\\%s"), strDownDir, strDirType);

	CString strReturn = _T("");
	int iPos = 1;
	BOOL bCreate = TRUE;
	CString strCurrent(_T("."));
	while (AfxExtractSubString(strReturn, strDownFullPath, iPos++, _T('\\')))
	{
		strCurrent += _T("\\") + strReturn;
		if (!PathFileExists(strCurrent))
		{
			BOOL bCreateDir = CreateDirectory(strCurrent, NULL);
			if (!bCreateDir)
			{
				bCreate = FALSE;
				break;
			}
		}
	}

	if (!bCreate)
	{
		AfxMessageBox(_T("다운로드 폴더 생성 불가"));
		return;
	}

	CSingleLock cr(&m_cr);
	cr.Lock();
	CPtrArray arrFileList; // FTP 파일 리스트 정보를 담는 클래스
	bRes = m_ftp.GetFtpFileList(arrFileList, TRUE);
	if (bRes)
	{
		int iCount = (int)arrFileList.GetCount();
		int iSuccess = 0, iFail = 0;
		for (int iIndex = 0 ; iIndex < iCount ; iIndex++)
		{
			CFtpFileInfo *pFileInfo = (CFtpFileInfo *)arrFileList.GetAt(iIndex);
			if (pFileInfo == NULL)
				continue;

			CString strFilePathName = pFileInfo->GetFilePathName();
			CString strLocalPathName;
			strLocalPathName.Format(_T("%s\\%s"), strDownFullPath, pFileInfo->GetFileName());
			BOOL bDown = m_ftp.DownloadFile(strFilePathName, strLocalPathName);
			if (bDown)
				iSuccess++;
			else
				iFail++;

			delete pFileInfo;
			pFileInfo = NULL;
		}

		if (iCount == 0)
			AfxMessageBox(_T("다운로드할 파일이 없습니다."));
		else
			AfxMessageBox(_T("다운로드 완료"));
	}
	arrFileList.RemoveAll();
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

//void CFtpClientDlg::OnNMClickListFileList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	if (pNMHDR->idFrom == IDC_LIST_FILE_LIST && m_pThreadLastImage == NULL)
//	{
//		CSingleLock cr(&m_cr);
//		cr.Lock();
//		int iIndex = pNMItemActivate->iItem;
//		int iCount = m_lcFileList.GetItemCount();
//		if (0 <= iIndex && iIndex < iCount)
//		{
//			CFtpFileInfo *pFileInfo = (CFtpFileInfo *)m_lcFileList.GetItemData(iIndex);
//			if (pFileInfo != NULL)
//			{
//				CString strFilePathName = pFileInfo->GetFilePathName();
//				CString strLocalPathName;
//				strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());
//				BOOL bDown = m_ftp.DownloadFile(strFilePathName, strLocalPathName);
//				if (bDown)
//				{
//					CString strLocalPathName;
//					strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());
//
//					if (!m_imgDisplay.IsNull())
//						m_imgDisplay.Destroy();
//					HRESULT hr = m_imgDisplay.Load(strLocalPathName);
//					if (SUCCEEDED(hr))
//					{
//						CRect rect;
//						m_stDispImage.GetWindowRect(&rect); // 컨트롤 크기 얻기
//						CDC *pDc = m_stDispImage.GetDC();
//						::SetStretchBltMode(*pDc, HALFTONE); // HALFTONE모드로 설정해서 크기 축소로 인한 이미지 깨짐 현상 최소화
//						m_imgDisplay.Draw(*pDc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_imgDisplay.GetWidth(), m_imgDisplay.GetHeight());
//
//						DeleteFile(strLocalPathName);
//					}
//				}
//			}
//		}
//	}
//	*pResult = 0;
//}

void CFtpClientDlg::OnLvnItemchangedListFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMHDR->idFrom == IDC_LIST_FILE_LIST)
	{
		if ((pNMLV->uNewState & LVIS_FOCUSED) == LVIS_FOCUSED ||
			(pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
		{
			//if (m_pThreadLastImage == NULL)	// 수집 중이 아닐 때  (수집 중일 때 파일 리스트 클릭 시 이미지 출력 안함)
			{

				CSingleLock cr(&m_cr);
				cr.Lock();
				int iCount = m_lcFileList.GetItemCount();
				int iIndex = pNMLV->iItem/*m_lcFileList.GetSelectionMark()*/;
				if (0 <= iIndex && iIndex < iCount)
				{
					CFtpFileInfo *pFileInfo = (CFtpFileInfo *)m_lcFileList.GetItemData(iIndex);
					if (pFileInfo != NULL)
					{
						CString strFilePathName = pFileInfo->GetFileDownPathName();
						CString strLocalPathName;
						strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());
						BOOL bDown = m_ftp.DownloadFile(strFilePathName, strLocalPathName);
						TRACE(_T("strFilePathName(%s), strLocalPathName(%s), bDown(%d)\n"), strFilePathName, strLocalPathName, bDown);
						if (bDown)
						{
							CString strLocalPathName;
							strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());
							CString strExt = PathFindExtension(strLocalPathName);
							int iFtpPathType = pFileInfo->GetFtpPathType();
							if (_tcscmp(_T(".jpg"), strExt) == 0 || _tcscmp(_T(".rbf"), strExt) == 0)
							{
								if (!m_imgDisplay.IsNull())
									m_imgDisplay.Destroy();
								HRESULT hr = m_imgDisplay.Load(strLocalPathName);
								if (SUCCEEDED(hr))
								{
									CRect rect;
									m_stDispImage.GetWindowRect(&rect); // 컨트롤 크기 얻기
									CDC *pDc = m_stDispImage.GetDC();
									::SetStretchBltMode(*pDc, HALFTONE); // HALFTONE모드로 설정해서 크기 축소로 인한 이미지 깨짐 현상 최소화
									m_imgDisplay.Draw(*pDc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_imgDisplay.GetWidth(), m_imgDisplay.GetHeight());

									DeleteFile(strLocalPathName);
								}
							}
							if (_tcscmp(_T(".log"), strExt) == 0 || _tcscmp(_T(".txt"), strExt) == 0)
							{
								CString strLocalPathName;
								strLocalPathName.Format(_T(".\\%s"), pFileInfo->GetFileName());
								if (m_fileText.m_hFile != CStdioFile::hFileNull)
									m_fileText.Close();
								BOOL bOpen = m_fileText.Open(strLocalPathName,
															 CStdioFile::modeNoTruncate |
															 CStdioFile::typeText |
															 CStdioFile::modeReadWrite |
															 CStdioFile::shareDenyNone);
								if (bOpen)
								{
									CString strTotal(_T("")), strRead(_T(""));
									while (m_fileText.ReadString(strRead))
										strTotal += strRead + _T("\r\n");
									m_edDispText.SetWindowText(strTotal);
									m_fileText.Close();

									DeleteFile(strLocalPathName);
								}
								else
									AfxMessageBox(_T("Text File Load Fail"));
							}
						}
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
				pThis->m_cbFtpPath.GetWindowText(strFtpPath);
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


void CFtpClientDlg::OnCbnSelchangeComboFtpPath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iFtpPath = m_cbFtpPath.GetCurSel();
	if (iFtpPath == 3)
	{
		((CButton*)GetDlgItem(IDC_RADIO_IMAGE))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_TEXT))->SetCheck(FALSE);
		m_iFtpShowFileType = 0;
		RadioEnableControl(m_iFtpShowFileType); // 라디오 버튼 - 컨트롤 활성화/비활성화
		ShowControlByFileType(m_iFtpShowFileType); // 파일 타입에 따른 컨트롤 표시
	}
	else
	{
		GetDlgItem(IDC_RADIO_IMAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TEXT)->EnableWindow(TRUE);
	}
	SetFtpPathToConfig(iFtpPath);
	TRACE(_T("%d\n"), iFtpPath);
}


// wifi list
void CFtpClientDlg::OnBnClickedButtonWifiScan()
{
	std::vector<std::tuple<CString, LONG, CString>> v_Wifilist;
	m_lcWifiList.DeleteAllItems();

	v_Wifilist = ListAvailableWifiNetworks();

	// RSSI 값 기준으로 내림차순 정렬
	std::sort(v_Wifilist.begin(), v_Wifilist.end(),
		[](const auto& a, const auto& b) {
			return std::get<1>(a) > std::get<1>(b);
		});

	int nIndex = 0;
	for (const auto& item : v_Wifilist) {
		CString ssid, listItem;
		LONG rssi;
		CString linkTime;
		std::tie(ssid, rssi, linkTime) = item;

		// WAVE로 시작하는 SSID만 리스트에 추가
		if (ssid.Find(_T("WAVE")) == 0)
		{
			listItem.Format(_T("%s - RSSI: %d - First Connect: %s"),
				ssid, rssi, linkTime);
			m_lcWifiList.InsertItem(nIndex, listItem);
			nIndex++;
		}
	}
}

std::vector<std::tuple<CString, LONG, CString>> CFtpClientDlg::ListAvailableWifiNetworks()
{
	std::vector<std::tuple<CString, LONG, CString>> availableNetworks;
	DWORD negotiatedVersion;
	HANDLE clientHandle = NULL;

	DWORD ret = WlanOpenHandle(2, NULL, &negotiatedVersion, &clientHandle);
	if (ret != ERROR_SUCCESS) {
		return availableNetworks;
	}

	PWLAN_INTERFACE_INFO_LIST ifList = NULL;
	ret = WlanEnumInterfaces(clientHandle, NULL, &ifList);
	if (ret != ERROR_SUCCESS) {
		WlanCloseHandle(clientHandle, NULL);
		return availableNetworks;
	}

	for (DWORD i = 0; i < ifList->dwNumberOfItems; i++) {
		PWLAN_INTERFACE_INFO pIfInfo = &ifList->InterfaceInfo[i];
		PWLAN_BSS_LIST pBssList = NULL;
		ret = WlanGetNetworkBssList(clientHandle, &pIfInfo->InterfaceGuid, NULL,
			dot11_BSS_type_any, FALSE, NULL, &pBssList);

		if (ret != ERROR_SUCCESS) {
			continue;
		}

		for (DWORD j = 0; j < pBssList->dwNumberOfItems; j++) {
			PWLAN_BSS_ENTRY pBssEntry = &pBssList->wlanBssEntries[j];
			DOT11_SSID ssid = pBssEntry->dot11Ssid;
			std::wstring networkName = ConvertSSID(ssid.ucSSID, ssid.uSSIDLength);
			LONG rssi = pBssEntry->lRssi;

			ULARGE_INTEGER ftSystemTime1970;
			ftSystemTime1970.QuadPart = 116444736000000000ULL;
			ULARGE_INTEGER ftTimestamp;
			ftTimestamp.QuadPart = ftSystemTime1970.QuadPart + (pBssEntry->ullHostTimestamp * 10);

			FILETIME ftFirstAvailableTime;
			ftFirstAvailableTime.dwHighDateTime = ftTimestamp.HighPart;
			ftFirstAvailableTime.dwLowDateTime = ftTimestamp.LowPart;

			SYSTEMTIME stFirstAvailableTime;
			FileTimeToSystemTime(&ftFirstAvailableTime, &stFirstAvailableTime);

			CString firstAvailableTime;
			firstAvailableTime.Format(_T("%02u:%02u:%02u"),
				stFirstAvailableTime.wHour,
				stFirstAvailableTime.wMinute,
				stFirstAvailableTime.wSecond);

			CStringW networkNameW = CStringW(networkName.c_str());
			CString networkNameT = CString(networkNameW);
			availableNetworks.push_back(std::make_tuple(networkNameT, rssi, firstAvailableTime));
		}
		WlanFreeMemory(pBssList);
	}

	WlanFreeMemory(ifList);
	WlanCloseHandle(clientHandle, NULL);
	return availableNetworks;
}


std::wstring CFtpClientDlg::ConvertSSID(const unsigned char* ssid, size_t ssidLength)
{
	int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
		reinterpret_cast<const char*>(ssid), ssidLength, NULL, 0);

	// UTF-8로 변환 실패시 시스템 기본 코드페이지로 시도
	if (len == 0 && GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
		len = MultiByteToWideChar(CP_ACP, 0,
			reinterpret_cast<const char*>(ssid), ssidLength, NULL, 0);
	}

	if (len > 0) {
		std::wstring networkName(len, L'\0');
		if (MultiByteToWideChar(CP_UTF8, 0,
			reinterpret_cast<const char*>(ssid), ssidLength,
			&networkName[0], len) > 0) {
			return networkName;
		}
	}

	// 변환 실패시 빈 문자열 반환
	return std::wstring();
}

//void CFtpClientDlg::OnLvnItemchangedListWifiList(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	string m_editNum2;
//	DWORD mousePos = GetMessagePos();
//	//m_editNume2.SetWindowText(m_lcWifiList.GetItemText(pNMLV->iItem, 1));
//	POINT p;
//	CPoint cp;
//	p.x = GET_X_LPARAM(mousePos);
//	p.x = GET_X_LPARAM(mousePos);
//
//	ScreenToClient(&p);
//	cp.x = p.x;
//	cp.y = p.y;
//	UINT flag;
//
//	if (m_lcWifiList.HitTest(cp, &flag) == -1)
//	{
//		MessageBox(L"hi", L"crap", MB_OK);
//	}
//
//
//	//MessageBox(L"김장환", L"알림", MB_OKCANCEL | MB_ICONINFORMATION);
//	*pResult = 0;
//}

void CFtpClientDlg::OnDblclkListWifiList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here



	*pResult = 0;
}


// wifi connect command (cmd)
// system("netsh wlan connect ssid=WAVE_ea:05:9f:b4 name=WAVE_ea:05:9f:b4 interface=\"Wi-Fi 2\"");
// 
// netsh wlan connect ssid=WAVE_ea:05:9f:b4 name=WAVE_ea:05:9f:b4 interface="Wi-Fi 2"

//string WIFI_NAME = "WAVE_ea:05:9f:b4";
//string WIFI_NUM = "\"Wi-Fi 2\"";
//string command = "netsh wlan connect ssid=" + WIFI_NAME + " name=" + WIFI_NAME + " interface=" + WIFI_NUM;

void CFtpClientDlg::OnBnClickedButtonWifiConnect()
{
	POSITION pos = m_lcWifiList.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		int selectedIndex = m_lcWifiList.GetNextSelectedItem(pos);
		CString selectedNetwork = m_lcWifiList.GetItemText(selectedIndex, 0);

		// SSID 부분만 추출 (SSID - RSSI: XX 형식에서)
		std::wregex ssidPattern(L"^([^ ]+)");
		std::wsmatch match;
		std::wstring selectedNetworkW = selectedNetwork.GetString();
		std::regex_search(selectedNetworkW, match, ssidPattern);
		std::wstring networkName = match.str(1);
		std::wstring password = L"wave1234";  // 기본 비밀번호

		if (ConnectToSelectedWifi(networkName, password)) {
			AfxMessageBox(_T("Wi-Fi에 연결되었습니다!"));

			// 윈도우 타이틀 업데이트
			CString windowTitle;
			windowTitle.Format(_T("FTP Client - Connected to %s"),
				CString(WStringToString(networkName).c_str()));
			this->SetWindowText(windowTitle);
		}
		else {
			AfxMessageBox(_T("Wi-Fi 연결에 실패했습니다."));
		}
	}
	else {
		AfxMessageBox(_T("선택된 Wi-Fi가 없습니다."));
	}
}

std::wstring CFtpClientDlg::StringToWString(const std::string& str)
{
	wstring wstr;
	size_t size;
	wstr.resize(str.length());
	mbstowcs_s(&size, &wstr[0], wstr.size() + 1, str.c_str(), str.size());
	return wstr;
}

std::string CFtpClientDlg::WStringToString(const std::wstring& wstr)
{
	string str;
	size_t size;
	str.resize(wstr.length());
	wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
	return str;
}

bool CFtpClientDlg::ConnectToSelectedWifi(const std::wstring& networkName, const std::wstring& password)
{
	std::string name(networkName.begin(), networkName.end());
	std::string pass(password.begin(), password.end());
	std::string fileName = "myWlan.xml";

	std::ofstream xmlFile;
	xmlFile.open(fileName.c_str());
	if (!xmlFile.is_open()) {
		return false;
	}

	// XML 파일 작성
	xmlFile << "<?xml version=\"1.0\"?>\n";
	xmlFile << "<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">\n";
	xmlFile << "<name>" << name << "</name>\n";
	xmlFile << "<SSIDConfig>\n<SSID>\n<hex>";
	for (int i = 0; i < name.length(); i++)
		xmlFile << std::hex << (int)name.at(i);
	xmlFile << "</hex>\n<name>" << name << "</name>\n</SSID>\n</SSIDConfig>\n";
	xmlFile << "<connectionType>ESS</connectionType>\n";
	xmlFile << "<connectionMode>auto</connectionMode>\n<MSM>\n<security>\n";
	xmlFile << "<authEncryption>\n<authentication>WPA2PSK</authentication>\n";
	xmlFile << "<encryption>AES</encryption>\n<useOneX>false</useOneX>\n";
	xmlFile << "</authEncryption>\n<sharedKey>\n<keyType>passPhrase</keyType>\n";
	xmlFile << "<protected>false</protected>\n<keyMaterial>" << pass << "</keyMaterial>\n";
	xmlFile << "</sharedKey>\n</security>\n</MSM>\n";
	xmlFile << "<MacRandomization xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v3\">\n";
	xmlFile << "<enableRandomization>false</enableRandomization>\n</MacRandomization>\n";
	xmlFile << "</WLANProfile>";
	xmlFile.close();

	// 시스템 프로파일에 XML 파일 추가
	std::string command = "netsh wlan add profile filename=" + fileName;
	if (system(command.c_str()) != 0) {
		return false;
	}

	// 네트워크 연결
	command = "netsh wlan connect name=" + name;
	if (system(command.c_str()) == 0) {
		return true;
	}

	return false;
}


std::string CFtpClientDlg::executeRemoteSshCommand(const char* command, bool isVersionCheck)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return "";
	}

	const char* ip = "192.168.8.1";
	const char* username = "root";
	const char* password = "fa";
	int port = 22;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		WSACleanup();
		return "";
	}

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	if (inet_pton(AF_INET, ip, &(sin.sin_addr)) != 1) {
		closesocket(sock);
		WSACleanup();
		return "";
	}

	if (connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
		closesocket(sock);
		WSACleanup();
		return "";
	}

	if (libssh2_init(0) != 0) {
		closesocket(sock);
		WSACleanup();
		return "";
	}

	LIBSSH2_SESSION* session = libssh2_session_init();
	if (libssh2_session_handshake(session, (int)sock) != 0) {
		closesocket(sock);
		libssh2_exit();
		WSACleanup();
		return "";
	}

	if (libssh2_userauth_password(session, username, password) != 0) {
		closesocket(sock);
		libssh2_session_free(session);
		libssh2_exit();
		WSACleanup();
		return "";
	}

	LIBSSH2_CHANNEL* channel = libssh2_channel_open_session(session);
	if (channel == NULL) {
		closesocket(sock);
		libssh2_session_free(session);
		libssh2_exit();
		WSACleanup();
		return "";
	}

	if (libssh2_channel_request_pty(channel, "xterm") != 0) {
		libssh2_channel_free(channel);
		closesocket(sock);
		libssh2_session_free(session);
		libssh2_exit();
		WSACleanup();
		return "";
	}

	libssh2_channel_setenv(channel, "TERM", "xterm");
	if (libssh2_channel_exec(channel, command) != 0) {
		libssh2_channel_free(channel);
		closesocket(sock);
		libssh2_session_free(session);
		libssh2_exit();
		WSACleanup();
		return "";
	}

	std::string output;
	char buffer[4096];
	ssize_t bytecount;
	while ((bytecount = libssh2_channel_read(channel, buffer, sizeof(buffer) - 1)) > 0) {
		buffer[bytecount] = '\0';
		output += buffer;
	}

	libssh2_channel_free(channel);
	libssh2_session_disconnect(session, "Finished session");
	libssh2_session_free(session);
	libssh2_exit();
	closesocket(sock);
	WSACleanup();

	return output;
}


void CFtpClientDlg::OnBnClickedButtonRtcSet()
{
	CWnd* pRtcButton = GetDlgItem(IDC_BUTTON_RTC_SET);
	if (!pRtcButton || !pRtcButton->GetSafeHwnd()) {
		AfxMessageBox(_T("UI 컨트롤 오류가 발생했습니다."));
		return;
	}

	CWaitCursor wait;
	pRtcButton->EnableWindow(FALSE);
	pRtcButton->UpdateWindow();

	try {
		SYSTEMTIME st;
		GetLocalTime(&st);

		char command[256] = { 0 };
		if (_snprintf_s(command, sizeof(command),
			"cd /home/pi/test && ./date_set.sh %02d %02d %02d %02d %02d %02d",
			st.wYear % 100, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond) < 0) {
			throw std::runtime_error("Command string formation failed");
		}

		std::string result = executeRemoteSshCommand(command, false);

		if (!result.empty()) {
			CString resultMsg;
			resultMsg.Format(_T("[Result]\n%s"), CString(result.c_str()));
			if (AfxMessageBox(resultMsg, MB_ICONINFORMATION | MB_OK) == IDOK) {
				// 현재 시스템 시간 가져오기
				SYSTEMTIME currentSt;
				GetLocalTime(&currentSt);

				// RTC 시간 확인
				const char* checkTimeCmd = "date '+%Y,%m,%d,%H,%M,%S'";
				std::string timeResult = executeRemoteSshCommand(checkTimeCmd, false);

				if (!timeResult.empty()) {
					// RTC 시간 파싱
					int rtcYear, rtcMonth, rtcDay, rtcHour, rtcMin, rtcSec;
					sscanf_s(timeResult.c_str(), "%d,%d,%d,%d,%d,%d",
						&rtcYear, &rtcMonth, &rtcDay, &rtcHour, &rtcMin, &rtcSec);

					// 시간 차이 계산 (초 단위)
					CTime rtcTime(rtcYear, rtcMonth, rtcDay, rtcHour, rtcMin, rtcSec);
					CTime sysTime(currentSt.wYear, currentSt.wMonth, currentSt.wDay,
						currentSt.wHour, currentSt.wMinute, currentSt.wSecond);

					CTimeSpan diff = sysTime - rtcTime;
					int diffSeconds = abs((int)diff.GetTotalSeconds());

					// 허용 오차 범위 (3초)
					const int ALLOWED_DIFF_SECONDS = 3;
					bool isWithinRange = diffSeconds <= ALLOWED_DIFF_SECONDS;

					CString timeMsg;
					timeMsg.Format(_T("[Current Time]\n%s\n\n[Time Verification]\n")
						_T("System Time: %04d,%02d,%02d,%02d,%02d,%02d\n")
						_T("RTC Time: %04d,%02d,%02d,%02d,%02d,%02d\n")
						_T("Time Difference: %d seconds\n")
						_T("Status: %s"),
						CString(timeResult.c_str()),
						currentSt.wYear, currentSt.wMonth, currentSt.wDay,
						currentSt.wHour, currentSt.wMinute, currentSt.wSecond,
						rtcYear, rtcMonth, rtcDay, rtcHour, rtcMin, rtcSec,
						diffSeconds,
						isWithinRange ? _T("OK (Within allowed range)") : _T("ERROR (Outside allowed range)"));

					AfxMessageBox(timeMsg, isWithinRange ? MB_ICONINFORMATION : MB_ICONWARNING);
				}
				else {
					throw std::runtime_error("시간 확인 실패");
				}
			}
		}
		else {
			throw std::runtime_error("RTC 시간 설정 실패");
		}
	}
	catch (const std::exception& e) {
		CString errorMsg;
		errorMsg.Format(_T("오류: %s"), CString(e.what()));
		AfxMessageBox(errorMsg, MB_ICONERROR);
	}
	catch (...) {
		AfxMessageBox(_T("알 수 없는 오류가 발생했습니다."), MB_ICONERROR);
	}

	// UI 상태 복구
	pRtcButton->EnableWindow(TRUE);
	pRtcButton->UpdateWindow();
}