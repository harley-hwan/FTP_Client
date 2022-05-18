#pragma once


// CDlgDisplay 대화 상자

class CDlgDisplay : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDisplay)

public:
	CDlgDisplay(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDisplay();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DISPLAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
	CStatic m_stDisplay;
	CImage *m_pImage;

	CCriticalSection *m_pcrImage;

	void InitControl();
	void MoveControl();
	void DisplayImage();
public:
	void SetImage(CImage *img);
	void SetImageCriticalSection(CCriticalSection* pcrImage);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
