// DlgDisplay.cpp: 구현 파일
//

#include "pch.h"
#include "FtpClient.h"
#include "DlgDisplay.h"
#include "afxdialogex.h"


// CDlgDisplay 대화 상자

IMPLEMENT_DYNAMIC(CDlgDisplay, CDialogEx)

CDlgDisplay::CDlgDisplay(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DISPLAY, pParent)
{
	m_pImage = NULL;
}

CDlgDisplay::~CDlgDisplay()
{
	m_pImage = NULL;
}

void CDlgDisplay::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISPLAY, m_stDisplay);
}

BOOL CDlgDisplay::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	InitControl();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BEGIN_MESSAGE_MAP(CDlgDisplay, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgDisplay::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDisplay::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgDisplay 메시지 처리기


void CDlgDisplay::InitControl()
{
	this->ShowWindow(SW_SHOWMAXIMIZED);

	SetTimer(1, 1000, NULL);
}

void CDlgDisplay::MoveControl()
{
	if (m_stDisplay.GetSafeHwnd() == NULL)
		return;

	CRect rcDlg;
	GetClientRect(rcDlg);

	// 수집시작 버튼
	CRect rcStaticDisp;
	m_stDisplay.GetWindowRect(rcStaticDisp);
	ScreenToClient(rcStaticDisp);
	m_stDisplay.SetWindowPos(NULL,
							 1,
							 1,
							 rcDlg.Width() - 2,
							 rcDlg.Height() - 2,
							 SWP_NOACTIVATE | SWP_NOZORDER);
	m_stDisplay.GetWindowRect(rcStaticDisp);
	ScreenToClient(rcStaticDisp);
}

void CDlgDisplay::DisplayImage()
{
	if (m_pcrImage == NULL || m_pImage == NULL || m_pImage->IsNull())
		return;

	CSingleLock cr(m_pcrImage);
	if (cr.IsLocked())
		return;
	CRect rect;
	m_stDisplay.GetWindowRect(&rect); // 컨트롤 크기 얻기
	CDC *pDc = m_stDisplay.GetDC();
	::SetStretchBltMode(*pDc, HALFTONE); // HALFTONE모드로 설정해서 크기 축소로 인한 이미지 깨짐 현상 최소화
	m_pImage->Draw(*pDc, 0, 0, rect.Width(), rect.Height(), 0, 0, m_pImage->GetWidth(), m_pImage->GetHeight());
}

void CDlgDisplay::SetImage(CImage * pImg)
{
	m_pImage = pImg;
}

void CDlgDisplay::SetImageCriticalSection(CCriticalSection* pcrImage)
{
	m_pcrImage = pcrImage;
}

void CDlgDisplay::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DisplayImage();
}


void CDlgDisplay::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}



void CDlgDisplay::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if ((nType != SIZE_MINIMIZED) && (cx > 0) && (cy > 0))
		MoveControl();
}


void CDlgDisplay::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
		case 1:
			DisplayImage();
			break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
