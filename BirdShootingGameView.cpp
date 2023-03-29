#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "BirdShootingGame.h"

#endif
#include "BirdShootingGameDoc.h"
#include "BirdShootingGameView.h"
// 랜덤함수를 위한 헤더파일
#include <math.h>
#include <time.h>
// 딜레이 함수
#include <windows.h>
// 문자열
#include <iostream>
#include <string>
// Bird 객체 불러오기
#include "Bird.h"
// 음악 재생 부분
#pragma comment(lib, "winmm")
#include <mmsystem.h>
#include <Digitalv.h>

#define ID_TIMER
UINT m_uTimer;

Bird bird[10]; // 새 10마리 생성

// 메모리릭 검사
#ifdef DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // DEBUG\

#define malloc DEBUG_MALLOC

#define _CRTDBG_MAP_ALLOC 


IMPLEMENT_DYNCREATE(CBirdShootingGameView, CView)

BEGIN_MESSAGE_MAP(CBirdShootingGameView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()
CBirdShootingGameView::CBirdShootingGameView() noexcept
{
	StateInitialization();
}
CBirdShootingGameView::~CBirdShootingGameView()
{
	LobbySound(1);
	GameSound(2);
	for (int i = 0; i < 10; i++)
		KillTimer(i);
}
BOOL CBirdShootingGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

typedef struct {
	MCI_OPEN_PARMS open;
	MCI_PLAY_PARMS play;
	MCI_GENERIC_PARMS genericparm;
	MCIERROR mciError;
	DWORD_PTR m_id;
} mciParms;

// 로비 음악 사운드
void CBirdShootingGameView::LobbySound(int state) {
	mciParms LobbyBgm;

	LobbyBgm.open.lpstrElementName = _T("..\\BirdShootingGame\\res\\Sound\\BackGround\\Lobby.wav");
	LobbyBgm.open.lpstrDeviceType = _T("waveaudio");
	mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)&LobbyBgm.open);
	if (state == 0) {
		mciSendCommand(1, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD_PTR)&LobbyBgm.play); // 음악 반복 재생
	}
	else {
		mciSendCommand(1, MCI_CLOSE, 0, NULL); // 음악 끄기
	}
}

// 게임 배경음악 사운드
void CBirdShootingGameView::GameSound(int state) {
	mciParms GameBgm;

	GameBgm.open.lpstrElementName = _T("..\\BirdShootingGame\\res\\Sound\\BackGround\\GamePlay.wav");
	GameBgm.open.lpstrDeviceType = _T("waveaudio");
	mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)(&GameBgm.open));
	if (state == 0) {
		mciSendCommand(2, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD_PTR)(&GameBgm.play)); // 음악 반복 재생
	}
	else {
		mciSendCommand(2, MCI_STOP, 0, NULL); // 음악 끄기
	}
}

void CBirdShootingGameView::OnDraw(CDC* pDC)
{
	CBirdShootingGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(473);
	_CrtMemDumpAllObjectsSince(0);
	CBitmap* pold;
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	// 로비 화면 (배경화면, 플레이버튼, 옵션버튼, 닫기버튼)
	CBitmap background, play_bt, option_bt, cancle_bt;

	// 옵션 화면 (배경화면, 닫기버튼, 각종 음악 On/Off 버튼들...)
	CBitmap option_bg, option_cancle, option_lobby_on, option_lobby_off, option_stage_on, option_stage_off, option_shoot_on, option_shoot_off, option_click_on, option_click_off;
	CBitmap t_LobbySound, t_GameSound, t_LobbyClickSound, t_GameClickSound;

	// 중간중간 전환 화면
	CBitmap bg_loading, stage_ready, stage_ready_text, stage_clear_text, stage_failed_text;

	// 스테이지 실패시 화면 버튼들
	CBitmap failed_mainmenu, failed_retry, failed_cancle;

	// 스테이지 화면 (각 스테이지별 배경화면들...)
	CBitmap stage1_bg, stage2_bg, stage3_bg, stage4_bg;

	// 로비 화면
	background.LoadBitmapW(IDB_BG_LOBBY);
	play_bt.LoadBitmapW(IDB_BT_PLAY);
	option_bt.LoadBitmapW(IDB_BT_OPTION);
	cancle_bt.LoadBitmapW(IDB_BT_CANCLE);

	// 옵션 화면
	option_bg.LoadBitmapW(IDB_BG_OPTION);
	option_cancle.LoadBitmapW(IDB_BT_OPTION_CANCLE);
	option_lobby_on.LoadBitmapW(IDB_BT_ON);
	option_lobby_off.LoadBitmapW(IDB_BT_OFF);
	option_stage_on.LoadBitmapW(IDB_BT_ON);
	option_stage_off.LoadBitmapW(IDB_BT_OFF);
	option_shoot_on.LoadBitmapW(IDB_BT_ON);
	option_shoot_off.LoadBitmapW(IDB_BT_OFF);
	option_click_on.LoadBitmapW(IDB_BT_ON);
	option_click_off.LoadBitmapW(IDB_BT_OFF);
	t_LobbySound.LoadBitmapW(IDB_V_LS);
	t_GameSound.LoadBitmapW(IDB_V_GS);
	t_LobbyClickSound.LoadBitmapW(IDB_V_LCS);
	t_GameClickSound.LoadBitmapW(IDB_V_GSS);

	// 중간중간 전환 화면
	bg_loading.LoadBitmapW(IDB_BG_LOADING);
	stage_ready.LoadBitmapW(IDB_BG_READY);
	stage_ready_text.LoadBitmapW(IDB_BG_READY_TEXT);
	stage_clear_text.LoadBitmapW(IDB_BG_CLEAR_TEXT);
	stage_failed_text.LoadBitmapW(IDB_BG_FAILED);

	// 스테이지 실패시 화면 버튼들
	failed_mainmenu.LoadBitmapW(IDB_BT_RE_MAINMENU);
	failed_retry.LoadBitmapW(IDB_BT_RETRY);
	failed_cancle.LoadBitmapW(IDB_BT_RE_CANCLE);

	// 스테이지 화면
	stage1_bg.LoadBitmapW(IDB_BG_STAGE1);
	stage2_bg.LoadBitmapW(IDB_BG_STAGE2);
	stage3_bg.LoadBitmapW(IDB_BG_STAGE3);
	stage4_bg.LoadBitmapW(IDB_BG_STAGE4);

	// 커서 변수
	lobbyCursor = AfxGetApp()->LoadCursor(IDC_LOBBY);
	lobbyClickCursor = AfxGetApp()->LoadCursor(IDC_LOBBY_CLICK);
	gameCursor = AfxGetApp()->LoadCursor(IDC_AIM);
	
	// 효과음 변수
	CString gameOverMusic = _T("..\\BirdShootingGame\\res\\Sound\\GameOver\\GameOver.wav");
	// 로비 화면 ( 게임 시작 시 )
	if (lobby_check) { // 옵션 창이 열리지 않았고, 로비 화면 상태 일때,
		cursor_lobbySet = true;
		if (lobby_sound) {
			LobbySound(0);
		}
		else if (!lobby_sound) {
			LobbySound(1);
		}
		GameSound(1);
		StateInitialization();

		// 배경화면
		pold = memDC.SelectObject(&background);
		pDC->StretchBlt(0, 0, 1024, 768, &memDC, 0, 0, 612, 408, SRCCOPY);
		memDC.SelectObject(pold);
		// 플레이 버튼
		pold = memDC.SelectObject(&play_bt);
		pDC->TransparentBlt(210, 340, 96 * 1.7, 96 * 1.7, &memDC, 0, 0, 96, 96, RGB(196, 219, 74));
		memDC.SelectObject(pold);
		// 옵션 버튼 출력
		pold = memDC.SelectObject(&option_bt);
		pDC->TransparentBlt(440, 340, 96 * 1.7, 96 * 1.7, &memDC, 0, 0, 96, 96, RGB(196,219,74));
		memDC.SelectObject(pold);
		// 닫기 버튼 출력
		pold = memDC.SelectObject(&cancle_bt);
		pDC->TransparentBlt(670, 340, 96 * 1.7, 96 * 1.7, &memDC, 0, 0, 96, 96, RGB(196, 219, 74));
		memDC.SelectObject(pold);

	}

	// 옵션 화면
	if (option_check) { // 옵션창이 열렸을때
		// 배경
		pold = memDC.SelectObject(&option_bg);	
		pDC->TransparentBlt(310, 60, 421, 629, &memDC, 0, 0, 421, 629, RGB(253, 247, 237)); // RGB(0,0,0) = 검은색
		memDC.SelectObject(pold);
		// 닫기 버튼
		pold = memDC.SelectObject(&option_cancle);
		pDC->TransparentBlt(475, 625, 99, 106, &memDC, 0, 0, 99, 106, RGB(253, 247, 237));
		memDC.SelectObject(pold);

		// 글씨들
		pold = memDC.SelectObject(&t_LobbySound); // 로비사운드
		pDC->TransparentBlt(320, 200, 280, 70, &memDC, 0, 0, 280, 70, RGB(155, 114, 54));
		memDC.SelectObject(pold);
		pold = memDC.SelectObject(&t_GameSound); // 게임사운드
		pDC->TransparentBlt(320, 300, 280, 70, &memDC, 0, 0, 280, 70, RGB(155, 114, 54));
		memDC.SelectObject(pold);
		pold = memDC.SelectObject(&t_GameClickSound); // 게임슈팅사운드
		pDC->TransparentBlt(320, 400, 280, 70, &memDC, 0, 0, 280, 70, RGB(155, 114, 54));
		memDC.SelectObject(pold);
		pold = memDC.SelectObject(&t_LobbyClickSound); // 로비클릭사운드
		pDC->TransparentBlt(320, 500, 280, 70, &memDC, 0, 0, 280, 70, RGB(155, 114, 54));
		memDC.SelectObject(pold);
		
		
		if (lobby_sound) { // 로비 배경음악이 켜져있을시
			pold = memDC.SelectObject(&option_lobby_on); // 로비사운드 On 버튼
			pDC->TransparentBlt(590, 200, 120, 60, &memDC, 0, 0, 201, 95, RGB(253, 247, 237));
			memDC.SelectObject(pold);
		}
		if (!lobby_sound) { // 로비 배경음악이 꺼져있을시
			pold = memDC.SelectObject(&option_lobby_off); // 로비사운드 OFF 버튼
			pDC->TransparentBlt(590, 200, 120, 60, &memDC, 0, 0, 201, 95, RGB(253, 247, 237));
			memDC.SelectObject(pold);
		}
		if (stage_sound) { // 게임 배경음악이 켜져있을시
			pold = memDC.SelectObject(&option_stage_on); // 로비사운드 On 버튼
			pDC->TransparentBlt(590, 300, 120, 60, &memDC, 0, 0, 201, 95, RGB(253, 247, 237));
			memDC.SelectObject(pold);
		}
		if (!stage_sound) { // 게임 배경음악이 꺼져있을시
			pold = memDC.SelectObject(&option_stage_off); // 로비사운드 OFF 버튼
			pDC->TransparentBlt(590, 300, 120, 60, &memDC, 0, 0, 201, 95, RGB(253, 247, 237));
			memDC.SelectObject(pold);
		}
		if (shoot_sound) { // 슈팅음악이 켜져있을시
			pold = memDC.SelectObject(&option_shoot_on); // 로비사운드 On 버튼
			pDC->TransparentBlt(590, 400, 120, 60, &memDC, 0, 0, 201, 95, RGB(253, 247, 237));
			memDC.SelectObject(pold);
		}
		if (!shoot_sound) { // 슈팅음악이 꺼져있을시
			pold = memDC.SelectObject(&option_stage_off); // 로비사운드 OFF 버튼
			pDC->TransparentBlt(590, 400, 120, 60, &memDC, 0, 0, 201, 95, RGB(253, 247, 237));
			memDC.SelectObject(pold);
		}
		if (click_sound) { // 클릭음악이 켜져있을시
			pold = memDC.SelectObject(&option_stage_on); // 로비사운드 On 버튼
			pDC->TransparentBlt(590, 500, 120, 60, &memDC, 0, 0, 201, 95, RGB(253, 247, 237));
			memDC.SelectObject(pold);
		}
		if (!click_sound) { // 클릭음악이 꺼져있을시
			pold = memDC.SelectObject(&option_stage_off); // 로비사운드 OFF 버튼
			pDC->TransparentBlt(590, 500, 120, 60, &memDC, 0, 0, 201, 95, RGB(253, 247, 237));
			memDC.SelectObject(pold);
		}
	}

	// 게임 시작 넘어가는 부분
	if (stage_start) {
		background.DeleteObject();
		play_bt.DeleteObject();
		option_bt.DeleteObject();
		cancle_bt.DeleteObject();
		// 배경화면
		if (lobby_check) {
			pold = memDC.SelectObject(&background);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}
		else if (stage1_start_check) {
			// 게임 1스테이지 배경화면
			pold = memDC.SelectObject(&stage1_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}
		else if (stage2_start_check) {
			// 게임 2스테이지 배경화면
			pold = memDC.SelectObject(&stage2_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}
		else if (stage3_start_check) {
			// 게임 3스테이지 배경화면
			pold = memDC.SelectObject(&stage3_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}
		else if (stage4_start_check) {
			// 게임 4스테이지 배경화면
			pold = memDC.SelectObject(&stage4_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}

		// 게임 준비 화면
		pold = memDC.SelectObject(&stage_ready);

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 200; // 0~255 불투명도
		bf.AlphaFormat = 0;

		pDC->AlphaBlend(0, 220, 1024, 350, &memDC, 0, 0, 1024, 350, bf); // 불투명도 배경 출력
		memDC.SelectObject(pold);

		pold = memDC.SelectObject(&stage_ready_text);
		pDC->TransparentBlt(0, 220, 1024, 350, &memDC, 0, 0, 1024,350, RGB(113,184,56)); // 텍스트 출력
		memDC.SelectObject(pold);

		Sleep(1000);
		
		if (lobby_start_check) {
			lobby_start_check = false;
			stage1_check = true;
		} else if (stage1_start_check) {
			stage1_start_check = false;
			stage1_check = true;
		}
		else if (stage2_start_check) {
			stage2_start_check = false;
			stage2_check = true;
		}
		else if (stage3_start_check) {
			stage3_start_check = false;
			stage3_check = true;
		}
		else if (stage4_start_check) {
			stage4_start_check = false;
			stage4_check = true;
		}
		for (int i = 0; i < 10; i++) {
			BridKindRandom(i);
			bird[i].l_xy.x = rand() % 200 - i * 100; // 스폰 x 좌표 랜덤
			if (i >= 5) {
				bird[i].r_xy.x = rand() % 200 + 1000 + (i -5) * 100; // 스폰 x 좌표 랜덤
			}
		}
		stage_start = false;
		Invalidate(false);

	}

	// 스테이지 클리어 화면
	if (stage_clear_check) {
		// 게임 준비 화면
		pold = memDC.SelectObject(&stage_ready);

		// 배경
		if (stage1_check) {
			// 게임 1스테이지 배경화면
			pold = memDC.SelectObject(&stage1_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}
		else if (stage2_check) {
			// 게임 2스테이지 배경화면
			pold = memDC.SelectObject(&stage2_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}
		else if (stage3_check) {
			// 게임 3스테이지 배경화면
			pold = memDC.SelectObject(&stage3_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}
		else if (stage4_check) {
			// 게임 4스테이지 배경화면
			pold = memDC.SelectObject(&stage4_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}


		// 총 총알수, 쏜 총알수, 잡은 새 마리수 변수값 -> Text로 변환
		allBulletText.Format(_T("X %d"), sallBullet);
		shootBulletTest.Format(_T("X %d"), sshootBullet);
		catchBirdText.Format(_T("잡은 새 : %d"), scatchBird);

		pDC->SetBkMode(TRANSPARENT); // TextOutW 박스 배경화면 투명으로 설정
		pDC->SetTextColor(RGB(0, 0, 0)); // 글씨 색 블랙으로 설정
		pDC->TextOutW(80, 710, allBulletText);		// 총 총알수 출력
		pDC->TextOutW(160, 710, shootBulletTest);	// 쏜 총알수 출력
		pDC->TextOutW(870, 710, catchBirdText);		// 잡은새 수 출력

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 200; // 0~255 불투명도
		bf.AlphaFormat = 0;

		pDC->AlphaBlend(0, 220, 1024, 350, &memDC, 0, 0, 1024, 350, bf); // 불투명도 배경 출력
		memDC.SelectObject(pold);

		pold = memDC.SelectObject(&stage_clear_text);
		pDC->TransparentBlt(0, 220, 1024, 350, &memDC, 0, 0, 1024, 350, RGB(113, 184, 57)); // 텍스트 출력
		memDC.SelectObject(pold);
		Invalidate();
		Sleep(3000); // 3초
		stage_clear_check = false;
		if (stage1_check) {
			stage1_check = false;
			stage2_check = true;
		}
		else if (stage2_check) {
			stage2_check = false;
			stage3_check = true;
		}
		else if (stage3_check) {
			stage3_check = false;
			stage4_check = true;
		}
		else if (stage4_check) {
			stage4_check = false;
			lobby_check = true;
		}
		for (int i = 0; i < 10; i++) {
			BridKindRandom(i);
			bird[i].l_xy.x = rand() % 200 - i * 100; // 스폰 x 좌표 랜덤
			if (i >= 5) {
				bird[i].r_xy.x = rand() % 200 + 1000 + (i - 5) * 100; // 스폰 x 좌표 랜덤
			}
		}
	}

	// 스테이지 실패 화면
	if (stage_failed_check) {
		GameSound(2);
		// GameOver Music
		PlaySound(gameOverMusic, AfxGetInstanceHandle(), SND_ASYNC);
		cursor_gameSet = false;			// 게임 에임 마우스 커서 Off
		cursor_lobbySet = true;			// 로비 기본 마우스 커서 On
		cursor_lobbyclickSet = true;	// 로비 클릭 마우스 커서 On
		
		// 배경
		if (stage1_failed_check) {
			// 게임 1스테이지 배경화면
			pold = memDC.SelectObject(&stage1_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}
		else if (stage2_failed_check) {
			// 게임 2스테이지 배경화면
			pold = memDC.SelectObject(&stage2_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}
		else if (stage3_failed_check) {
			// 게임 3스테이지 배경화면
			pold = memDC.SelectObject(&stage3_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}
		else if (stage4_failed_check) {
			// 게임 4스테이지 배경화면
			pold = memDC.SelectObject(&stage4_bg);
			pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pold);
		}


		// 총 총알수, 쏜 총알수, 잡은 새 마리수 변수값 -> Text로 변환
		allBulletText.Format(_T("X %d"), sallBullet);
		shootBulletTest.Format(_T("X %d"), sshootBullet);
		catchBirdText.Format(_T("잡은 새 : %d"), scatchBird);

		pDC->SetBkMode(TRANSPARENT); // TextOutW 박스 배경화면 투명으로 설정
		pDC->SetTextColor(RGB(0, 0, 0)); // 글씨 색 블랙으로 설정
		pDC->TextOutW(80, 710, allBulletText);		// 총 총알수 출력
		pDC->TextOutW(160, 710, shootBulletTest);	// 쏜 총알수 출력
		pDC->TextOutW(870, 710, catchBirdText);		// 잡은새 수 출력

		pold = memDC.SelectObject(&stage_failed_text);
		pDC->TransparentBlt(310, 60, 421, 629, &memDC, 0, 0, 421, 629, RGB(253, 247, 237)); // RGB(0,0,0) = 검은색
		memDC.SelectObject(pold);

		pold = memDC.SelectObject(&failed_mainmenu); // 로비사운드 On 버튼
		pDC->TransparentBlt(590, 210, 96, 96, &memDC, 0, 0, 96, 96, RGB(157, 69, 70));
		memDC.SelectObject(pold);

		pold = memDC.SelectObject(&failed_retry); // 로비사운드 OFF 버튼
		pDC->TransparentBlt(590, 350, 96, 96, &memDC, 0, 0, 96, 96, RGB(157, 69, 70));
		memDC.SelectObject(pold);

		pold = memDC.SelectObject(&failed_cancle); // 로비사운드 On 버튼
		pDC->TransparentBlt(590, 490, 96, 96, &memDC, 0, 0, 96, 96, RGB(157, 69, 70));
		memDC.SelectObject(pold);
		
	}

	// 스테이지 1 진입 시작
	if (stage1_check) {
		LobbySound(1);				// 로비 배경음악 끄기

		cursor_lobbySet = false;		// 로비 기본 마우스 커서 Off
		cursor_lobbyclickSet = false;	// 로비 클릭 마우스 커서 Off
		cursor_gameSet = true;			// 게임 에임 마우스 커서 On

		if (stage_sound) {				// 게임 내 음악 On 일땐,
			GameSound(0);			// 게임 배경음악 켜기
		}
		else {							// 게임 내 음악 Off 일땐,
			GameSound(1);			// 게임 배경음악 끄기
		}

		// 게임 1스테이지 배경화면
		pold = memDC.SelectObject(&stage1_bg);
		pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pold);

		// b_num = x || x의 값에 따라 새 10마리(0~9) 순서대로 int값 초기화
		// RedrawBird(); 을 쓰는 이유 : Timer 시작전 그림을 다시 그려냄으로써, 첫 이미지 XOR 처리로 인해 이미지 사라짐을 유도
		// SetTimer(b_num, 40, NULL); : b_num(int값)에 맞게 Timer 시작
		b_num = 0;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);
		b_num = 1;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);
		b_num = 2;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);
		b_num = 3;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);
		b_num = 4;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);
		b_num = 5;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);
		b_num = 6;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);
		b_num = 7;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);
		b_num = 8;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);
		b_num = 9;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		// 게임 진행 처리
		if (allBullet == 0 || catchBird == 10) {				// 쏘지 않은 총알이 0발 일때
			for (int i = 0; i < 10; i++) {	// bird Timer 돌린 모든 것들 KillTimer 설정
				KillTimer(i);
			}
			if (catchBird >= 7) {			// 잡은 새가 7마리 이상일때,
				stage_clear_check = true;	// 스테이지 클리어 화면으로 이동
			}
			else {							// 새를 7마리 미만으로 잡을때
				stage1_check = false;		// 스테이지 1은 닫기
				stage1_failed_check = true;	// 스테이지 실패 화면으로 이동
				stage_failed_check = true;	// 실패화면으로 넘어가기 위한 상태 체크
			}
			Invalidate();					// 이걸 해줘야 총알, 잡은 새 Text들이 사라지면서 화면 재출력
			StateInitialization();			// 초기화 함수
		}
	}

	// 스테이지 2 진입 시작
	if (stage2_check) {
		cursor_lobbySet = false;
		cursor_lobbyclickSet = false;
		cursor_gameSet = true;
		if (stage_sound) {
			GameSound(0);
		}
		else if (!stage_sound) {
			GameSound(1);
		}
		// 게임 배경화면
		pold = memDC.SelectObject(&stage2_bg);
		pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pold);

		// b_num = x || x의 값에 따라 새 10마리(0~9) 순서대로 int값 초기화
		// RedrawBird(); 을 쓰는 이유 : Timer 시작전 그림을 다시 그려냄으로써, 첫 이미지 XOR 처리로 인해 이미지 사라짐을 유도
		// SetTimer(b_num, 40, NULL); : b_num(int값)에 맞게 Timer 시작
		b_num = 0;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 1;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 2;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 3;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 4;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 5;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 6;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 7;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 8;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 9;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		// 게임 진행 처리
		if (allBullet == 0) { // 가진 총알을 전부 소비 했을때
			for (int i = 0; i < 10; i++) { // 새 Timer 전부 삭제
				KillTimer(i);
			}
			if (catchBird >= 7) { // 잡은 새가 7마리 이상 일때
				stage_clear_check = true;
				Invalidate(); // 이걸 해줘야 전달받은 Text값들이 사라지면서 화면 재출력
				StateInitialization();	// 초기화 함수
			}
			else { // 새를 7마리 미만으로 잡을때
				stage2_check = false;
				stage2_failed_check = true;
				stage_failed_check = true;		// 실패화면으로 넘어가기
				Invalidate(); // 이걸 해줘야 전달받은 Text값들이 사라지면서 화면 재출력
				StateInitialization();	// 초기화 함수
			}
		}
	}

	// 스테이지 3 진입 시작
	if (stage3_check) {
		cursor_lobbySet = false;
		cursor_lobbyclickSet = false;
		cursor_gameSet = true;
		if (stage_sound) {
			GameSound(0);
		}
		else if (!stage_sound) {
			GameSound(1);
		}
		// 게임 배경화면
		pold = memDC.SelectObject(&stage3_bg);
		pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pold);

		// b_num = x || x의 값에 따라 새 10마리(0~9) 순서대로 int값 초기화
		// RedrawBird(); 을 쓰는 이유 : Timer 시작전 그림을 다시 그려냄으로써, 첫 이미지 XOR 처리로 인해 이미지 사라짐을 유도
		// SetTimer(b_num, 40, NULL); : b_num(int값)에 맞게 Timer 시작
		b_num = 0;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 1;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 2;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 3;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 4;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 5;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 6;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 7;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 8;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 9;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		// 게임 진행 처리
		if (allBullet == 0) { // 가진 총알을 전부 소비 했을때
			for (int i = 0; i < 10; i++) { // 새 Timer 전부 삭제
				KillTimer(i);
			}
			if (catchBird >= 7) { // 잡은 새가 7마리 이상 일때
				stage_clear_check = true;
				Invalidate(); // 이걸 해줘야 전달받은 Text값들이 사라지면서 화면 재출력
				StateInitialization();	// 초기화 함수
			}
			else { // 새를 7마리 미만으로 잡을때
				stage3_check = false;
				stage3_failed_check = true;
				stage_failed_check = true;		// 실패화면으로 넘어가기
				Invalidate(); // 이걸 해줘야 전달받은 Text값들이 사라지면서 화면 재출력
				StateInitialization();	// 초기화 함수
			}
		}
	}

	// 스테이지 4 진입 시작
	if (stage4_check) {
		cursor_lobbySet = false;
		cursor_lobbyclickSet = false;
		cursor_gameSet = true;
		if (stage_sound) {
			GameSound(0);
		}
		else if (!stage_sound) {
			GameSound(1);
		}
		// 게임 배경화면
		pold = memDC.SelectObject(&stage4_bg);
		pDC->BitBlt(0, 0, 1024, 768, &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pold);

		// b_num = x || x의 값에 따라 새 10마리(0~9) 순서대로 int값 초기화
		// RedrawBird(); 을 쓰는 이유 : Timer 시작전 그림을 다시 그려냄으로써, 첫 이미지 XOR 처리로 인해 이미지 사라짐을 유도
		// SetTimer(b_num, 40, NULL); : b_num(int값)에 맞게 Timer 시작
		b_num = 0;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 1;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 2;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 3;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 4;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 5;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 6;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 7;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 8;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		b_num = 9;
		ReDrawBird(b_num);
		SetTimer(b_num, 40, NULL);

		// 게임 진행 처리
		if (allBullet == 0) { // 가진 총알을 전부 소비 했을때
			for (int i = 0; i < 10; i++) { // 새 Timer 전부 삭제
				KillTimer(i);
			}
			if (catchBird >= 7) { // 잡은 새가 7마리 이상 일때
				stage_clear_check = true;
				Invalidate(); // 이걸 해줘야 전달받은 Text값들이 사라지면서 화면 재출력
				StateInitialization();	// 초기화 함수
			}
			else { // 새를 7마리 미만으로 잡을때
				stage4_check = false;
				stage4_failed_check = true;
				stage_failed_check = true;		// 실패화면으로 넘어가기
				Invalidate(); // 이걸 해줘야 전달받은 Text값들이 사라지면서 화면 재출력
				StateInitialization();	// 초기화 함수
			}
		}
	}

}

// CBirdShootingGameView 인쇄
BOOL CBirdShootingGameView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}
void CBirdShootingGameView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}
void CBirdShootingGameView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}
#ifdef _DEBUG
void CBirdShootingGameView::AssertValid() const
{
	CView::AssertValid();
}

void CBirdShootingGameView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBirdShootingGameDoc* CBirdShootingGameView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBirdShootingGameDoc)));
	return (CBirdShootingGameDoc*)m_pDocument;
}
#endif //_DEBUG

// 마우스 좌클릭시
void CBirdShootingGameView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CString clickMusic = _T("..\\BirdShootingGame\\res\\Sound\\Click\\Button_Click.wav");
	CString shootMusic = _T("..\\BirdShootingGame\\res\\Sound\\Shoot\\Shoot.wav");
	// 로비화면 일시
	if (lobby_check) {
		cursor_lobbySet = false; // 마우스 커서 설정(로비 기본)
		cursor_lobbyclickSet = true; // 마우스 커서 설정(로비 클릭시)
		if (click_sound) { // 클릭 사운드 On 일때만
			PlaySound(clickMusic, AfxGetInstanceHandle(), SND_ASYNC);
		}
		
		double play_x, play_y, play_xx, play_yy; // 플레이 버튼
		play_x = 210.0;
		play_y = 340.0;
		play_xx = play_x + (96 * 1.7);
		play_yy = play_y + (96 * 1.7);

		double option_x, option_y, option_xx, option_yy; // 옵션 버튼
		option_x = 440.0;
		option_y = 340.0;
		option_xx = option_x + (96 * 1.7);
		option_yy = option_y + (96 * 1.7);

		double cancle_x, cancle_y, cancle_xx, cancle_yy; // 닫기 버튼
		cancle_x = 670.0;
		cancle_y = 340.0;
		cancle_xx = cancle_x + (96 * 1.7); 
		cancle_yy = cancle_y + (96 * 1.7); 

		double option_cancle_x, option_cancle_xx, option_cancle_y, option_cancle_yy; // (옵션창)닫기 버튼
		option_cancle_x = 475;
		option_cancle_y = 650;
		option_cancle_xx = option_cancle_x + 96;
		option_cancle_yy = option_cancle_y + 96;

		double option_lobby_x, option_lobby_xx, option_lobby_y, option_lobby_yy; // 로비 배경음악 On/Off
		option_lobby_x = 590;
		option_lobby_y = 200;
		option_lobby_xx = option_lobby_x + 120;
		option_lobby_yy = option_lobby_y + 60;

		double option_stage_x, option_stage_xx, option_stage_y, option_stage_yy; // 게임 배경음악 On/Off
		option_stage_x = 590;
		option_stage_y = 300;
		option_stage_xx = option_stage_x + 120;
		option_stage_yy = option_stage_y + 60;

		double option_shoot_x, option_shoot_xx, option_shoot_y, option_shoot_yy; // 슈팅 사운드 On/Off
		option_shoot_x = 590;
		option_shoot_y = 400;
		option_shoot_xx = option_shoot_x + 120;
		option_shoot_yy = option_shoot_y + 60;

		double option_click_x, option_click_xx, option_click_y, option_click_yy; // 클릭 사운드 On/Off
		option_click_x = 590;
		option_click_y = 500;
		option_click_xx = option_click_x + 120;
		option_click_yy = option_click_y + 60;

		// 옵션 창 열릴시
		if (option_check) {
			if (point.x >= option_cancle_x && point.x <= option_cancle_xx && point.y >= option_cancle_y && point.y <= option_cancle_yy ) {// 닫기 버튼 클릭시
				option_check = false;
				Invalidate(true);
			}

			if (lobby_sound) { // 로비 사운드 On 일시
				if (point.x >= option_lobby_x && point.x <= option_lobby_xx && point.y >= option_lobby_y && point.y <= option_lobby_yy) {// 로비 사운드 버튼 클릭시
					lobby_sound = false;
					Invalidate(false);
				}
			}
			else { // 로비 사운드 Off 일시
				if (point.x >= option_lobby_x && point.x <= option_lobby_xx && point.y >= option_lobby_y && point.y <= option_lobby_yy) {// 로비 사운드 버튼 클릭시
					lobby_sound = true;
					Invalidate(false);
				}
			}

			if (stage_sound) { // 게임 배경 사운드 On 일시
				if (point.x >= option_stage_x && point.x <= option_stage_xx && point.y >= option_stage_y && point.y <= option_stage_yy) {// 게임 배경 사운드 버튼 클릭시
					stage_sound = false;
					Invalidate(false);
				}
			}
			else { //  게임 배경 사운드 Off 일시
				if (point.x >= option_stage_x && point.x <= option_stage_xx && point.y >= option_stage_y && point.y <= option_stage_yy) {// 게임 배경 사운드 버튼 클릭시
					stage_sound = true;
					Invalidate(false);
				}
			}

			if (shoot_sound) { // 총 사격 사운드 On 일시
				if (point.x >= option_shoot_x && point.x <= option_shoot_xx && point.y >= option_shoot_y && point.y <= option_shoot_yy) {// 총 사격 사운드 버튼 클릭시
					shoot_sound = false;
					Invalidate(false);
				}
			}
			else { // 총 사격 사운드 Off 일시
				if (point.x >= option_shoot_x && point.x <= option_shoot_xx && point.y >= option_shoot_y && point.y <= option_shoot_yy) {// 총 사격 사운드 버튼 클릭시
					shoot_sound = true;
					Invalidate(false);
				}
			}
			if (click_sound) { // 클릭 사운드 On 일시
				if (point.x >= option_click_x && point.x <= option_click_xx && point.y >= option_click_y && point.y <= option_click_yy) {// 총 사격 사운드 버튼 클릭시
					click_sound = false;
					Invalidate(false);
				}
			}
			else { // 클릭 사운드 Off 일시
				if (point.x >= option_click_x && point.x <= option_click_xx && point.y >= option_click_y && point.y <= option_click_yy) {// 총 사격 사운드 버튼 클릭시
					click_sound = true;
					Invalidate(false);
				}
			}
		}

		// 옵션창 닫혀있을시
		else {
			if (point.x >= play_x && point.x <= play_xx && point.y >= play_y && point.y <= play_yy) {		// 플레이 버튼 클릭시
				lobby_start_check = true;
				stage_start = true;	// 스테이지 준비으로 넘어감
				lobby_check = false;	// 로비상태는 꺼주기
				Invalidate(false);			// 화면 재출력
			}
			if (point.x >= option_x && point.x <= option_xx && point.y >= option_y && point.y <= option_yy) {// 옵션 버튼 클릭시
				option_check = true;	// 옵션 창 상태 열기
				Invalidate(false);		// 로비화면 있는 상태에서 그 위에 출력
			}
			if (point.x >= cancle_x && point.x <= cancle_xx && point.y >= cancle_y && point.y <= cancle_yy) {// 닫기 버튼 클릭시
				AfxGetMainWnd()->PostMessage(WM_QUIT);
			}
		}
	}

	// 스테이지 실패 화면 일시
	if (stage_failed_check) {
		cursor_lobbySet = false;		// 로비 마우스 클릭 커서 화면 전환(기본)
		cursor_lobbyclickSet = true;	// 로비 마우스 클릭 커서 화면 전환(클릭시)
		if (click_sound) {				// 클릭 사운드 On 일때,
			PlaySound(clickMusic, AfxGetInstanceHandle(), SND_ASYNC);
		}

		double mainmenu_x, mainmenu_y, mainmenu_xx, mainmenu_yy; // 로비로 가는 버튼
		mainmenu_x = 590.0;
		mainmenu_y = 210.0;
		mainmenu_xx = mainmenu_x + 96;
		mainmenu_yy = mainmenu_y + 96;

		double retry_x, retry_y, retry_xx, retry_yy; // 재시작 버튼
		retry_x = 590.0;
		retry_y = 350.0;
		retry_xx = retry_x + 96;
		retry_yy = retry_y + 96;

		double cancle_x, cancle_y, cancle_xx, cancle_yy; // 게임 끄기 버튼
		cancle_x = 590.0;
		cancle_y = 490.0;
		cancle_xx = cancle_x + 96;
		cancle_yy = cancle_y + 96;

		if (point.x >= mainmenu_x && point.x <= mainmenu_xx && point.y >= mainmenu_y && point.y <= mainmenu_yy) {		// 메인메뉴 버튼 클릭시
			stage_failed_check = false;		// 스테이지 준비으로 넘어감
			lobby_check = true;				// 로비상태는 꺼주기
			Invalidate(true);			// 화면 재출력
		}
		if (point.x >= retry_x && point.x <= retry_xx && point.y >= retry_y && point.y <= retry_yy) {// 재시작 버튼 클릭시
			stage_failed_check = false;
			if (stage1_failed_check) {		// 1스테이지 실패시
				stage_start = true;
				stage1_start_check = true;
				stage1_failed_check = false;
			}
			else if (stage2_failed_check) {	// 2스테이지 실패시
				stage_start = true;
				stage2_start_check = true;
				stage2_failed_check = false;
			}
			else if (stage3_failed_check) {	// 3스테이지 실패시
				stage_start = true;
				stage3_start_check = true;
				stage3_failed_check = false;
			}
			else if (stage4_failed_check) {	// 4스테이지 실패시
				stage_start = true;
				stage4_start_check = true;
				stage4_failed_check = false;
			}
			Invalidate(true);			// 화면 전부 재출력
		}
		if (point.x >= cancle_x && point.x <= cancle_xx && point.y >= cancle_y && point.y <= cancle_yy) {// 게임 끄기 버튼
			AfxGetMainWnd()->PostMessage(WM_QUIT); // 프로그램 끄기
		}
	}
	
	//스테이지 1 게임 시작시
	if (stage1_check) {
		if (shoot_sound) {
			PlaySound(shootMusic, AfxGetInstanceHandle(), SND_ASYNC);
		}
		b_num = 0;
		if (point.x >= 0 && point.x <= 1024 && point.y >= 0 && point.y <= 768) { // 게임 화면 안만 클릭 되게 제한 걸기 ( 1024x768 )
			if (allBullet <= 0) { // 총 총알수가 0발이 되는 순간
				allBullet = 0;    // 총 총알수 0발 고정
				shootBullet = 10; // 쏜 총알수 10발 고정
			}
			else { // 총 총알수가 1발 이상일 때
				allBullet -= 1;	// 총 총알수 -1
				shootBullet += 1;	// 쏜 총알수 +1

				// 왼쪽에서 스폰되는 새
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][1]) {// 하얀새 좌표 설정
					ReDrawBird(b_num); // 클릭하게 될 마지막 이미지를 한번더 출력함으로써, 이미지 XOR처리로 삭제
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0; // 크기를 0으로 줄임으로써, 이미지 사라진 표현
					catchBird += 1; // 잡은 새 +1
				}
				b_num = 1;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 2;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 3;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 4;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}

				// 오른쪽에서 스폰되는 새
				b_num = 5;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 6;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 7;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 8;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 9;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
			}
			sallBullet = allBullet;
			sshootBullet = shootBullet;
			scatchBird = catchBird;
			Invalidate(false); // 이 처리로 인해 allBulletText, shootBulletText, catchBirdText 3개가 재출력
		}
	}

	//스테이지 2 게임 시작시
	if (stage2_check) {
		if (shoot_sound) {
			PlaySound(shootMusic, AfxGetInstanceHandle(), SND_ASYNC);
		}
		b_num = 0;
		if (point.x >= 0 && point.x <= 1024 && point.y >= 0 && point.y <= 768) { // 게임 화면 안만 클릭 되게 제한 걸기 ( 1024x768 )
			if (allBullet <= 0) { // 총 총알수가 0발이 되는 순간
				allBullet = 0;    // 총 총알수 0발 고정
				shootBullet = 10; // 쏜 총알수 10발 고정
			}
			else { // 총 총알수가 1발 이상일 때
				allBullet -= 1;	// 총 총알수 -1
				shootBullet += 1;	// 쏜 총알수 +1

				// 왼쪽에서 스폰되는 새
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {// 하얀새 좌표 설정
					ReDrawBird(b_num); // 클릭하게 될 마지막 이미지를 한번더 출력함으로써, 이미지 XOR처리로 삭제
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0; // 크기를 0으로 줄임으로써, 이미지 사라진 표현
					catchBird += 1; // 잡은 새 +1
				}
				b_num = 1;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 2;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 3;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 4;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}

				// 오른쪽에서 스폰되는 새
				b_num = 5;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 6;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 7;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 8;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 9;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
			}
			sallBullet = allBullet;
			sshootBullet = shootBullet;
			scatchBird = catchBird;
			Invalidate(false); // 이 처리로 인해 allBulletText, shootBulletText, catchBirdText 3개가 재출력
		}
	}

	//스테이지 3 게임 시작시
	if (stage3_check) {
		if (shoot_sound) {
			PlaySound(shootMusic, AfxGetInstanceHandle(), SND_ASYNC);
		}
		b_num = 0;
		if (point.x >= 0 && point.x <= 1024 && point.y >= 0 && point.y <= 768) { // 게임 화면 안만 클릭 되게 제한 걸기 ( 1024x768 )
			if (allBullet <= 0) { // 총 총알수가 0발이 되는 순간
				allBullet = 0;    // 총 총알수 0발 고정
				shootBullet = 10; // 쏜 총알수 10발 고정
			}
			else { // 총 총알수가 1발 이상일 때
				allBullet -= 1;	// 총 총알수 -1
				shootBullet += 1;	// 쏜 총알수 +1

				// 왼쪽에서 스폰되는 새
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {// 하얀새 좌표 설정
					ReDrawBird(b_num); // 클릭하게 될 마지막 이미지를 한번더 출력함으로써, 이미지 XOR처리로 삭제
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0; // 크기를 0으로 줄임으로써, 이미지 사라진 표현
					catchBird += 1; // 잡은 새 +1
				}
				b_num = 1;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 2;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 3;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 4;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}

				// 오른쪽에서 스폰되는 새
				b_num = 5;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 6;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 7;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 8;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 9;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
			}
			sallBullet = allBullet;
			sshootBullet = shootBullet;
			scatchBird = catchBird;
			Invalidate(false); // 이 처리로 인해 allBulletText, shootBulletText, catchBirdText 3개가 재출력
		}
	}

	//스테이지 4 게임 시작시
	if (stage4_check) {
		if (shoot_sound) {
			PlaySound(shootMusic, AfxGetInstanceHandle(), SND_ASYNC);
		}
		b_num = 0;
		if (point.x >= 0 && point.x <= 1024 && point.y >= 0 && point.y <= 768) { // 게임 화면 안만 클릭 되게 제한 걸기 ( 1024x768 )
			if (allBullet <= 0) { // 총 총알수가 0발이 되는 순간
				allBullet = 0;    // 총 총알수 0발 고정
				shootBullet = 10; // 쏜 총알수 10발 고정
			}
			else { // 총 총알수가 1발 이상일 때
				allBullet -= 1;	// 총 총알수 -1
				shootBullet += 1;	// 쏜 총알수 +1

				// 왼쪽에서 스폰되는 새
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {// 하얀새 좌표 설정
					ReDrawBird(b_num); // 클릭하게 될 마지막 이미지를 한번더 출력함으로써, 이미지 XOR처리로 삭제
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0; // 크기를 0으로 줄임으로써, 이미지 사라진 표현
					catchBird += 1; // 잡은 새 +1
				}
				b_num = 1;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 2;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 3;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 4;
				if (point.x >= bird[b_num].l_xy.x && point.x <= bird[b_num].l_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].l_xy.y && point.y <= bird[b_num].l_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}

				// 오른쪽에서 스폰되는 새
				b_num = 5;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 6;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 7;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 8;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
				b_num = 9;
				if (point.x >= bird[b_num].r_xy.x && point.x <= bird[b_num].r_xy.x + bird[b_num].size[bird[b_num].kind][0] && point.y >= bird[b_num].r_xy.y && point.y <= bird[b_num].r_xy.y + bird[b_num].size[bird[b_num].kind][0]) {
					ReDrawBird(b_num);
					bird[b_num].size[bird[b_num].kind][0] = 0, bird[b_num].size[bird[b_num].kind][0] = 0;
					catchBird += 1;
				}
			}
			sallBullet = allBullet;
			sshootBullet = shootBullet;
			scatchBird = catchBird;
			Invalidate(false); // 이 처리로 인해 allBulletText, shootBulletText, catchBirdText 3개가 재출력
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

void CBirdShootingGameView::OnTimer(UINT_PTR nIDEvent)
{
	CDC* pDC = GetDC();

	CBitmap* pold;
	CBitmap l_WhiteBird, r_WhiteBird;
	CBitmap l_BlueBird, r_BlueBird;
	CBitmap l_BrownBird, r_BrownBird;
	CBitmap l_GreenBird, r_GreenBird;
	CBitmap l_EgleBird, r_EgleBird;
	CBitmap l_RedBird, r_RedBird;

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	
	l_WhiteBird.LoadBitmapW(IDB_B_WHITE_L); // 왼쪽에서 스폰되는 하얀 새
	r_WhiteBird.LoadBitmapW(IDB_B_WHITE_R); // 오른쪽에서 스폰되는 하얀 새
	l_BlueBird.LoadBitmapW(IDB_B_BLUE_L); // 왼쪽에서 스폰되는 파란 새
	r_BlueBird.LoadBitmapW(IDB_B_BLUE_R); // 오른쪽에서 스폰되는 파란 새
	l_BrownBird.LoadBitmapW(IDB_B_BROWN_L); // 왼쪽에서 스폰되는 파란 새
	r_BrownBird.LoadBitmapW(IDB_B_BROWN_R); // 오른쪽에서 스폰되는 파란 새
	l_GreenBird.LoadBitmapW(IDB_B_GREEN_L); // 왼쪽에서 스폰되는 파란 새
	r_GreenBird.LoadBitmapW(IDB_B_GREEN_R); // 오른쪽에서 스폰되는 파란 새
	l_EgleBird.LoadBitmapW(IDB_B_EGLE_L); // 왼쪽에서 스폰되는 파란 새
	r_EgleBird.LoadBitmapW(IDB_B_EGLE_R); // 오른쪽에서 스폰되는 파란 새
	l_RedBird.LoadBitmapW(IDB_B_RED_L); // 왼쪽에서 스폰되는 파란 새
	r_RedBird.LoadBitmapW(IDB_B_RED_R); // 오른쪽에서 스폰되는 파란 새

	bool predraw; // 이전 이미지 출력이 ( 있는지, 없는지 ) 상태 체크 변수
	POINT predrawXY = { 0, 0 }; // 이전 출력 이미지 좌표값 임시 저장 변수

	// 총 총알수, 쏜 총알수, 잡은 새 마리수 변수값 -> Text로 변환
	allBulletText.Format(_T("X %d"), allBullet);
	shootBulletTest.Format(_T("X %d"), shootBullet);
	catchBirdText.Format(_T("잡은 새 : %d"), catchBird);

	pDC->SetBkMode(TRANSPARENT); // TextOutW 박스 배경화면 투명으로 설정
	pDC->SetTextColor(RGB(0, 0, 0)); // 글씨 색 블랙으로 설정
	pDC->TextOutW(80, 710, allBulletText);		// 총 총알수 출력
	pDC->TextOutW(160, 710, shootBulletTest);	// 쏜 총알수 출력
	pDC->TextOutW(870, 710, catchBirdText);		// 잡은새 수 출력
	

	// 왼쪽 새
	if (nIDEvent == 0) {
		int b_num = nIDEvent;
		predraw = false; // 이전 그림이 없다는 판단
		switch (bird[b_num].kind) {
		case 0:
			pold = memDC.SelectObject(&l_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&l_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&l_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&l_GreenBird);
			break;
		}
		
		bird[b_num].LorR = 0; // 0의 값이면 왼쪽 스폰 새
		pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		predrawXY.x = bird[b_num].l_xy.x; // 첫 그린 그림의 x값 저장
		predrawXY.y = bird[b_num].l_xy.y; // 첫 그린 그림의 y값 저장
		predraw = true; // 바로 위 출력으로 인해 이전 그림이 생성
		if (predraw) {	// 이전 그림이 있다면,
			bird[b_num].l_xy.x = predrawXY.x; // 저장된 x값 객체 x값에 저장
			bird[b_num].l_xy.y = predrawXY.y; // 저장된 y값 객체 y값에 저장
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1],  SRCINVERT);
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1],  SRCINVERT);
			
			// 새가 움직이는 방향 및 속도 조절
			bird[b_num].l_xy.x += 10*bird[b_num].speed[bird[b_num].kind];
			if (bird[b_num].l_xy.y >= 708) {
				bird[b_num].l_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird[b_num].l_xy.y <= -20) {
				bird[b_num].l_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird_time >= rand() % 1 + 1.5 && bird_time <= 3) {
				bird[b_num].l_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else {
				bird[b_num].l_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			
			// 가장 마지막 움직인새 이미지 출력
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1],  SRCINVERT);
		}
		if (bird[b_num].l_xy.x >= 924) { // 만약 왼쪽의 새가 오른쪽 끝에 왔을경우 ReSpawn
			ReDrawBird(b_num); // 자동적으로 이미지 지워주고
			bird[b_num].l_xy.x = rand() % 100 - 150, bird[b_num].l_xy.y = rand() % 600; // x좌표는 왼쪽끝 설정, y좌표는 랜덤 재설정
			BridKindRandom(b_num);
			ReDrawBird(b_num); // 첫 소환된 이미지를 XOR로 없애기 위한 처리
		}
	}
	if (nIDEvent == 1) {
		int b_num = nIDEvent;
		predraw = false; // 이전 그림이 없다는 판단
		switch (bird[b_num].kind) {
		case 0:
			pold = memDC.SelectObject(&l_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&l_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&l_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&l_GreenBird);
			break;
		}
		
		bird[b_num].LorR = 0; // 0의 값이면 왼쪽 스폰 새
		pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		predrawXY.x = bird[b_num].l_xy.x; // 첫 그린 그림의 x값 저장
		predrawXY.y = bird[b_num].l_xy.y; // 첫 그린 그림의 y값 저장
		predraw = true; // 바로 위 출력으로 인해 이전 그림이 생성
		if (predraw) {	// 이전 그림이 있다면,
			bird[b_num].l_xy.x = predrawXY.x; // 저장된 x값 객체 x값에 저장
			bird[b_num].l_xy.y = predrawXY.y; // 저장된 y값 객체 y값에 저장
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);

			// 새가 움직이는 방향 및 속도 조절
			bird[b_num].l_xy.x += 10 * bird[b_num].speed[bird[b_num].kind];
			if (bird[b_num].l_xy.y >= 708) {
				bird[b_num].l_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird[b_num].l_xy.y <= -20) {
				bird[b_num].l_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird_time >= rand() % 1 + 1.8 && bird_time <= 3) {
				bird[b_num].l_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else {
				bird[b_num].l_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}

			// 가장 마지막 움직인새 이미지 출력
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		}
		if (bird[b_num].l_xy.x >= 924) { // 만약 왼쪽의 새가 오른쪽 끝에 왔을경우 ReSpawn
			ReDrawBird(b_num); // 자동적으로 이미지 지워주고
			bird[b_num].l_xy.x = rand() %100-350, bird[b_num].l_xy.y = rand() % 600; // x좌표는 왼쪽끝 설정, y좌표는 랜덤 재설정
			BridKindRandom(b_num);
			ReDrawBird(b_num); // 첫 소환된 이미지를 XOR로 없애기 위한 처리
		}
	}
	if (nIDEvent == 2) {
		int b_num = nIDEvent;
		predraw = false; // 이전 그림이 없다는 판단
		switch (bird[b_num].kind) {
		case 0:
			pold = memDC.SelectObject(&l_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&l_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&l_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&l_GreenBird);
			break;
		}
		bird[b_num].LorR = 0; // 0의 값이면 왼쪽 스폰 새
		pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		predrawXY.x = bird[b_num].l_xy.x; // 첫 그린 그림의 x값 저장
		predrawXY.y = bird[b_num].l_xy.y; // 첫 그린 그림의 y값 저장
		predraw = true; // 바로 위 출력으로 인해 이전 그림이 생성
		if (predraw) {	// 이전 그림이 있다면,
			bird[b_num].l_xy.x = predrawXY.x; // 저장된 x값 객체 x값에 저장
			bird[b_num].l_xy.y = predrawXY.y; // 저장된 y값 객체 y값에 저장
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);

			// 새가 움직이는 방향 및 속도 조절
			bird[b_num].l_xy.x += 10 * bird[b_num].speed[bird[b_num].kind];
			if (bird[b_num].l_xy.y >= 708) {
				bird[b_num].l_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird[b_num].l_xy.y <= -20) {
				bird[b_num].l_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird_time >= rand() % 1 + 0.7 && bird_time <= 3) {
				bird[b_num].l_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else {
				bird[b_num].l_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}

			// 가장 마지막 움직인새 이미지 출력
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		}
		if (bird[b_num].l_xy.x >= 924) { // 만약 왼쪽의 새가 오른쪽 끝에 왔을경우 ReSpawn
			ReDrawBird(b_num); // 자동적으로 이미지 지워주고
			bird[b_num].l_xy.x = rand() % 100 - 200, bird[b_num].l_xy.y = rand() % 600; // x좌표는 왼쪽끝 설정, y좌표는 랜덤 재설정
			BridKindRandom(b_num);
			ReDrawBird(b_num); // 첫 소환된 이미지를 XOR로 없애기 위한 처리
		}
	}
	if (nIDEvent == 3) {
		int b_num = nIDEvent;
		predraw = false; // 이전 그림이 없다는 판단
		switch (bird[b_num].kind) {
		case 0:
			pold = memDC.SelectObject(&l_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&l_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&l_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&l_GreenBird);
			break;
		}
		bird[b_num].LorR = 0; // 0의 값이면 왼쪽 스폰 새
		pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		predrawXY.x = bird[b_num].l_xy.x; // 첫 그린 그림의 x값 저장
		predrawXY.y = bird[b_num].l_xy.y; // 첫 그린 그림의 y값 저장
		predraw = true; // 바로 위 출력으로 인해 이전 그림이 생성
		if (predraw) {	// 이전 그림이 있다면,
			bird[b_num].l_xy.x = predrawXY.x; // 저장된 x값 객체 x값에 저장
			bird[b_num].l_xy.y = predrawXY.y; // 저장된 y값 객체 y값에 저장
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);

			// 새가 움직이는 방향 및 속도 조절
			bird[b_num].l_xy.x += 10 * bird[b_num].speed[bird[b_num].kind];
			if (bird[b_num].l_xy.y >= 708) {
				bird[b_num].l_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird[b_num].l_xy.y <= -20) {
				bird[b_num].l_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird_time >= rand() % 1 + 1.0 && bird_time <= 3) {
				bird[b_num].l_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else {
				bird[b_num].l_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}

			// 가장 마지막 움직인새 이미지 출력
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		}
		if (bird[b_num].l_xy.x >= 924) { // 만약 왼쪽의 새가 오른쪽 끝에 왔을경우 ReSpawn
			ReDrawBird(b_num); // 자동적으로 이미지 지워주고
			bird[b_num].l_xy.x = rand() % 100 - 100, bird[b_num].l_xy.y = rand() % 600; // x좌표는 왼쪽끝 설정, y좌표는 랜덤 재설정
			BridKindRandom(b_num);
			ReDrawBird(b_num); // 첫 소환된 이미지를 XOR로 없애기 위한 처리
		}
	}
	if (nIDEvent == 4) {
		int b_num = nIDEvent;
		predraw = false; // 이전 그림이 없다는 판단
		switch (bird[b_num].kind) {
		case 0:
			pold = memDC.SelectObject(&l_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&l_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&l_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&l_GreenBird);
			break;
		}
		bird[b_num].LorR = 0; // 0의 값이면 왼쪽 스폰 새
		pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		predrawXY.x = bird[b_num].l_xy.x; // 첫 그린 그림의 x값 저장
		predrawXY.y = bird[b_num].l_xy.y; // 첫 그린 그림의 y값 저장
		predraw = true; // 바로 위 출력으로 인해 이전 그림이 생성
		if (predraw) {	// 이전 그림이 있다면,
			bird[b_num].l_xy.x = predrawXY.x; // 저장된 x값 객체 x값에 저장
			bird[b_num].l_xy.y = predrawXY.y; // 저장된 y값 객체 y값에 저장
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);

			// 새가 움직이는 방향 및 속도 조절
			bird[b_num].l_xy.x += 10 * bird[b_num].speed[bird[b_num].kind];
			if (bird[b_num].l_xy.y >= 708) {
				bird[b_num].l_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird[b_num].l_xy.y <= -20) {
				bird[b_num].l_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird_time >= rand() % 1 + 2.1 && bird_time <= 3) {
				bird[b_num].l_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else {
				bird[b_num].l_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}

			// 가장 마지막 움직인새 이미지 출력
			pDC->StretchBlt(bird[b_num].l_xy.x, bird[b_num].l_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		}
		if (bird[b_num].l_xy.x >= 924) { // 만약 왼쪽의 새가 오른쪽 끝에 왔을경우 ReSpawn
			ReDrawBird(b_num); // 자동적으로 이미지 지워주고
			bird[b_num].l_xy.x = rand() % 100-300, bird[b_num].l_xy.y = rand() % 600; // x좌표는 왼쪽끝 설정, y좌표는 랜덤 재설정
			BridKindRandom(b_num);
			ReDrawBird(b_num); // 첫 소환된 이미지를 XOR로 없애기 위한 처리
		}
	}

	// 오른쪽 새
	if (nIDEvent == 5) {
		int b_num = nIDEvent;
		predraw = false; // 이전 그림이 없다는 판단
		switch (bird[b_num].kind) {
		case 0:
			pold = memDC.SelectObject(&r_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&r_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&r_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&r_GreenBird);
			break;
		}
		bird[b_num].LorR = 1;
		pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1],  SRCINVERT);
		predrawXY.x = bird[b_num].r_xy.x;
		predrawXY.y = bird[b_num].r_xy.y;
		predraw = true;
		if (predraw) {
			bird[b_num].r_xy.x = predrawXY.x;
			bird[b_num].r_xy.y = predrawXY.y;
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1],  SRCINVERT);
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1],  SRCINVERT);
			
			bird[b_num].r_xy.x -= 10 * bird[b_num].speed[bird[b_num].kind];
			if (bird[b_num].r_xy.y >= 708) {
				bird[b_num].r_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird[b_num].r_xy.y <= -20) {
				bird[b_num].r_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird_time >= rand() % 2 + 1.5 && bird_time <= 3) {
				bird[b_num].r_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];;
			}
			else {
				bird[b_num].r_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];;
			}
			
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1],  SRCINVERT);
		}
		if (bird[b_num].r_xy.x <= 0) {
			ReDrawBird(b_num);
			bird[b_num].r_xy.x = rand() % 200 + 1000, bird[b_num].r_xy.y = rand() % 600;
			BridKindRandom(b_num);
			ReDrawBird(b_num);
		}
	}
	if (nIDEvent == 6) {
		int b_num = nIDEvent;
		predraw = false; // 이전 그림이 없다는 판단
		switch (bird[b_num].kind) {
		case 0:
			pold = memDC.SelectObject(&r_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&r_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&r_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&r_GreenBird);
			break;
		}
		bird[b_num].LorR = 1;
		pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		predrawXY.x = bird[b_num].r_xy.x;
		predrawXY.y = bird[b_num].r_xy.y;
		predraw = true;
		if (predraw) {
			bird[b_num].r_xy.x = predrawXY.x;
			bird[b_num].r_xy.y = predrawXY.y;
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);


			bird[b_num].r_xy.x -= 10 * bird[b_num].speed[bird[b_num].kind];
			if (bird[b_num].r_xy.y >= 708) {
				bird[b_num].r_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird[b_num].r_xy.y <= -20) {
				bird[b_num].r_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird_time >= rand() % 2 + 1.8 && bird_time <= 3) {
				bird[b_num].r_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];;
			}
			else {
				bird[b_num].r_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];;
			}

			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		}
		if (bird[b_num].r_xy.x <= 0) {
			ReDrawBird(b_num);
			bird[b_num].r_xy.x = rand() % 200 + 1100, bird[b_num].r_xy.y = rand() % 600;
			BridKindRandom(b_num);
			ReDrawBird(b_num);
		}
	}

	if (nIDEvent == 7) {
		int b_num = nIDEvent;
		predraw = false; // 이전 그림이 없다는 판단
		switch (bird[b_num].kind) {
		case 0:
			pold = memDC.SelectObject(&r_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&r_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&r_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&r_GreenBird);
			break;
		}
		bird[b_num].LorR = 1;
		pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		predrawXY.x = bird[b_num].r_xy.x;
		predrawXY.y = bird[b_num].r_xy.y;
		predraw = true;
		if (predraw) {
			bird[b_num].r_xy.x = predrawXY.x;
			bird[b_num].r_xy.y = predrawXY.y;
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);

			bird[b_num].r_xy.x -= 10 * bird[b_num].speed[bird[b_num].kind];
			if (bird[b_num].r_xy.y >= 708) {
				bird[b_num].r_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird[b_num].r_xy.y <= -20) {
				bird[b_num].r_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird_time >= rand() % 2 + 1.0 && bird_time <= 3) {
				bird[b_num].r_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];;
			}
			else {
				bird[b_num].r_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];;
			}

			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		}
		if (bird[b_num].r_xy.x <= 0) {
			ReDrawBird(b_num);
			bird[b_num].r_xy.x = rand() % 200 + 1300, bird[b_num].r_xy.y = rand() % 600;
			BridKindRandom(b_num);
			ReDrawBird(b_num);
		}
	}
	if (nIDEvent == 8) {
		int b_num = nIDEvent;
		predraw = false; // 이전 그림이 없다는 판단
		switch (bird[b_num].kind) {
		case 0:
			pold = memDC.SelectObject(&r_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&r_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&r_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&r_GreenBird);
			break;
		}
		bird[b_num].LorR = 1;
		pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		predrawXY.x = bird[b_num].r_xy.x;
		predrawXY.y = bird[b_num].r_xy.y;
		predraw = true;
		if (predraw) {
			bird[b_num].r_xy.x = predrawXY.x;
			bird[b_num].r_xy.y = predrawXY.y;
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);

			bird[b_num].r_xy.x -= 10 * bird[b_num].speed[bird[b_num].kind]; if (bird[b_num].r_xy.y >= 708) {
				bird[b_num].r_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird[b_num].r_xy.y <= -20) {
				bird[b_num].r_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird_time >= rand() % 2 + 1.2 && bird_time <= 3) {
				bird[b_num].r_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];;
			}
			else {
				bird[b_num].r_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];;
			}

			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		}
		if (bird[b_num].r_xy.x <= 0) {
			ReDrawBird(b_num);
			bird[b_num].r_xy.x = rand() % 200 + 1200, bird[b_num].r_xy.y = rand() % 600;
			BridKindRandom(b_num);
			ReDrawBird(b_num);
		}
	}
	if (nIDEvent == 9) {
		int b_num = nIDEvent;
		predraw = false; // 이전 그림이 없다는 판단
		switch (bird[b_num].kind) {
		case 0:
			pold = memDC.SelectObject(&r_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&r_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&r_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&r_GreenBird);
			break;
		}
		bird[b_num].LorR = 1;
		pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		predrawXY.x = bird[b_num].r_xy.x;
		predrawXY.y = bird[b_num].r_xy.y;
		predraw = true;
		if (predraw) {
			bird[b_num].r_xy.x = predrawXY.x;
			bird[b_num].r_xy.y = predrawXY.y;
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);

			bird[b_num].r_xy.x -= 10 * bird[b_num].speed[bird[b_num].kind];
			if (bird[b_num].r_xy.y >= 708) {
				bird[b_num].r_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird[b_num].r_xy.y <= -20) {
				bird[b_num].r_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];
			}
			else if (bird_time >= rand() % 1 + 1.3 && bird_time <= 3) {
				bird[b_num].r_xy.y -= rand() % 20 * bird[b_num].speed[bird[b_num].kind];;
			}
			else {
				bird[b_num].r_xy.y += rand() % 20 * bird[b_num].speed[bird[b_num].kind];;
			}

			pDC->StretchBlt(bird[b_num].r_xy.x, bird[b_num].r_xy.y, bird[b_num].size[bird[b_num].kind][0], bird[b_num].size[bird[b_num].kind][1], &memDC, 0, 0, bird[b_num].originalSize[bird[b_num].kind][0], bird[b_num].originalSize[bird[b_num].kind][1], SRCINVERT);
		}
		if (bird[b_num].r_xy.x <= 0) {
			ReDrawBird(b_num);
			bird[b_num].r_xy.x = rand() % 200 + 1150, bird[b_num].r_xy.y = rand() % 600;
			BridKindRandom(b_num);
			ReDrawBird(b_num);
		}
	}

	bird_time += 0.01; 
	if (bird_time >= 3) {
		bird_time = 0;
	}
	ReleaseDC(pDC); // 메모리 사용량 늘어나는거 방지
	
	return CView::OnTimer(nIDEvent);
}

void CBirdShootingGameView::ReDrawBird(int bird_code) {

	CDC* pDC = GetDC();
	CBitmap* pold;
	CBitmap l_WhiteBird, r_WhiteBird;
	CBitmap l_BlueBird, r_BlueBird;
	CBitmap l_BrownBird, r_BrownBird;
	CBitmap l_GreenBird, r_GreenBird;
	CBitmap l_EgleBird, r_EgleBird;
	CBitmap l_RedBird, r_RedBird;

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	l_WhiteBird.LoadBitmapW(IDB_B_WHITE_L); // 왼쪽에서 스폰되는 하얀 새
	r_WhiteBird.LoadBitmapW(IDB_B_WHITE_R); // 오른쪽에서 스폰되는 하얀 새
	l_BlueBird.LoadBitmapW(IDB_B_BLUE_L); // 왼쪽에서 스폰되는 파란 새
	r_BlueBird.LoadBitmapW(IDB_B_BLUE_R); // 오른쪽에서 스폰되는 파란 새
	l_BrownBird.LoadBitmapW(IDB_B_BROWN_L); // 왼쪽에서 스폰되는 파란 새
	r_BrownBird.LoadBitmapW(IDB_B_BROWN_R); // 오른쪽에서 스폰되는 파란 새
	l_GreenBird.LoadBitmapW(IDB_B_GREEN_L); // 왼쪽에서 스폰되는 파란 새
	r_GreenBird.LoadBitmapW(IDB_B_GREEN_R); // 오른쪽에서 스폰되는 파란 새
	l_EgleBird.LoadBitmapW(IDB_B_EGLE_L); // 왼쪽에서 스폰되는 파란 새
	r_EgleBird.LoadBitmapW(IDB_B_EGLE_R); // 오른쪽에서 스폰되는 파란 새
	l_RedBird.LoadBitmapW(IDB_B_RED_L); // 왼쪽에서 스폰되는 파란 새
	r_RedBird.LoadBitmapW(IDB_B_RED_R); // 오른쪽에서 스폰되는 파란 새

	switch(bird_code) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		switch (bird[bird_code].kind) {
		case 0:
			pold = memDC.SelectObject(&l_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&l_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&l_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&l_GreenBird);
			break;
		}
		pDC->StretchBlt(bird[bird_code].l_xy.x, bird[bird_code].l_xy.y, bird[bird_code].size[bird[bird_code].kind][0], bird[bird_code].size[bird[bird_code].kind][1], &memDC, 0, 0, bird[bird_code].originalSize[bird[bird_code].kind][0], bird[bird_code].originalSize[bird[bird_code].kind][1],  SRCINVERT);
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		switch (bird[bird_code].kind) {
		case 0:
			pold = memDC.SelectObject(&r_WhiteBird);
			break;
		case 1:
			pold = memDC.SelectObject(&r_BlueBird);
			break;
		case 2:
			pold = memDC.SelectObject(&r_BrownBird);
			break;
		case 3:
			pold = memDC.SelectObject(&r_GreenBird);
			break;
		}
		pDC->StretchBlt(bird[bird_code].r_xy.x, bird[bird_code].r_xy.y, bird[bird_code].size[bird[bird_code].kind][0], bird[bird_code].size[bird[bird_code].kind][1], &memDC, 0, 0, bird[bird_code].originalSize[bird[bird_code].kind][0], bird[bird_code].originalSize[bird[bird_code].kind][1],  SRCINVERT);
		break;
	}

	ReleaseDC(pDC); // 메모리 사용량 늘어나는거 방지
}

void CBirdShootingGameView::StateInitialization() {
	for (b_num = 0; b_num < 10; b_num++) { // 새 10마리(0~9) 모두 사이즈를 되돌려주고, 소환되는 좌표값을 재지정
		switch(bird[b_num].kind) {
		case 0:
		case 1:
			bird[b_num].size[bird[b_num].kind][0] = 104, bird[b_num].size[bird[b_num].kind][0] = 80;	// 사이즈 원상복구
			break;
		case 2:
			bird[b_num].size[bird[b_num].kind][0] = 80, bird[b_num].size[bird[b_num].kind][0] = 61;	// 사이즈 원상복구
			break;
		case 3:
			bird[b_num].size[bird[b_num].kind][0] = 50, bird[b_num].size[bird[b_num].kind][0] = 38;	// 사이즈 원상복구
			break;
		}
		if (!bird[b_num].LorR) { // bird[b_num].LorR의 값이 0일땐, 왼쪽에서 소환되는 새
			bird[b_num].l_xy.x = 0, bird[b_num].l_xy.y = rand() % 600; // 소환되는 좌표값 재지정
		}
		else if (bird[b_num].LorR) { // bird[b_num].LorR의 값이 1일땐, 오른쪽에서 소환되는 새
			bird[b_num].r_xy.x = 924, bird[b_num].r_xy.y = rand() % 600; // 소환되는 좌표값 재지정
		}
	}
	allBullet = 10;		// 총 총알수 초기화
	shootBullet = 0;	// 쏜 총알수 초기화
	catchBird = 0;		// 잡은 새 마릿수 초기화
}

BOOL CBirdShootingGameView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (cursor_lobbySet) {
		::SetCursor(lobbyCursor);
		return TRUE;
	}
	if (cursor_lobbyclickSet) {
		::SetCursor(lobbyClickCursor);
		cursor_lobbySet = true;
		cursor_lobbyclickSet = false;
		return TRUE;
	}
	if (cursor_gameSet) {
		::SetCursor(gameCursor);
		return TRUE;
	}

	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CBirdShootingGameView::BridKindRandom(int bird_code) {
	if (stage1_check) {
		bird[bird_code].kind = rand() % 1;
	}
	else if (stage2_check) {
		bird[bird_code].kind = rand() % 2;
	}
	else if (stage3_check) {
		bird[bird_code].kind = rand() % 3;
	}
	else if (stage4_check) {
		bird[bird_code].kind = rand() % 4+1;
	}
}
