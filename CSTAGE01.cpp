// CSTAGE01.cpp: 구현 파일
//

#include "pch.h"
#include "BirdShootingGame.h"
#include "afxdialogex.h"
#include "CSTAGE01.h"


// CSTAGE01 대화 상자

IMPLEMENT_DYNAMIC(CSTAGE01, CDialogEx)

CSTAGE01::CSTAGE01(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STAGE1, pParent)
{

}

CSTAGE01::~CSTAGE01()
{
}

void CSTAGE01::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSTAGE01, CDialogEx)
END_MESSAGE_MAP()


