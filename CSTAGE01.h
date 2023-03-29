#pragma once
#include "afxdialogex.h"


// CSTAGE01 대화 상자

class CSTAGE01 : public CDialogEx
{
	DECLARE_DYNAMIC(CSTAGE01)

public:
	CSTAGE01(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CSTAGE01();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STAGE1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
