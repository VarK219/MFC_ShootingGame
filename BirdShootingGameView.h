#pragma once
#pragma comment(lib, "winmm.lib")

class CBirdShootingGameView : public CView
{
protected: // serialization에서만 만들어집니다.
	CBirdShootingGameView() noexcept;
	DECLARE_DYNCREATE(CBirdShootingGameView)

// 특성입니다.
public:
	CBirdShootingGameDoc* GetDocument() const;

// 작업입니다.
public:
	void LobbySound(int state); // 로비 배경음악 사운드
	void GameSound(int state); // 게임 배경음악 사운드

	HCURSOR lobbyCursor; // 로비 커서 이미지 등록
	HCURSOR lobbyClickCursor; // 로비 클릭 커서 이미지 등록
	HCURSOR gameCursor; // 게임 커서 이미지 등록
	bool cursor_lobbySet = true; // 로비 커서 상태 체크
	bool cursor_lobbyclickSet = true; // 로비 클릭 커서 상태 체크
	bool cursor_gameSet = true; // 게임 커서 상태 체크

	void StateInitialization(); // 변수의 값들 초기화 하는 함수 ( 새의 크기, 새의 스폰 좌푯값, 총 총알수, 쏜 총알수, 잡은새 마리수 )
	void ReDrawBird(int bird_code); // Bird 재출력 함수
	int b_num = 0; // Bird 번호 (0 번 ~ 9번)
	double bird_time = 0; // 새 날아가는 타이밍 시간값
	void BridKindRandom(int bird_code); //
	int b_kind; // 새의 종류 랜덤값으로 가져갈 값

	int allBullet = 10;		// 총 총알수 ( 10발 초기화 )
	int shootBullet = 0;	// 쏜 총알수 ( 0발 초기화 )
	int catchBird = 0;		// 잡은새 수 ( 0마리 초기화 ) 
	int sallBullet = 10;    // 총 총알수 저장
	int sshootBullet = 0;   // 쏜 총알수 저장
	int scatchBird = 0;     // 잡은새 수 저장
	CString allBulletText;	// 총 총알수 -> Text 변환 변수
	CString shootBulletTest;// 쏜 총알수 -> Text 변환 변수
	CString catchBirdText;	// 잡은새 수 -> Text 변환 변수

	// 화면 상태 체크
	bool lobby_check = true;	// 로비 On/Off
	bool option_check = false;	// 설정창 On/Off
	bool stage_start = false;	// 게임 시작 넘어가는 On/Off
	bool stage_clear_check = false;	// 스테이지 클리어 On/Off
	bool stage_failed_check = false; // 스테이지 실패 On/Off
	bool stage1_check = false;	// 스테이지 1 On/Off
	bool stage2_check = false;	// 스테이지 2 On/Off
	bool stage3_check = false;	// 스테이지 3 On/Off
	bool stage4_check = false;	// 스테이지 4 On/Off

	bool stage1_failed_check = false; // 스테이지 1 실패시 상태체크
	bool stage2_failed_check = false; // 스테이지 2 실패시 상태체크
	bool stage3_failed_check = false; // 스테이지 3 실패시 상태체크
	bool stage4_failed_check = false; // 스테이지 4 실패시 상태체크

	bool lobby_start_check = false; // 로비에서 시작할시 상태
	bool stage1_start_check = false; // 스테이지 1에서 재시작할시 상태
	bool stage2_start_check = false; // 스테이지 2에서 재시작할시 상태
	bool stage3_start_check = false; // 스테이지 3에서 재시작할시 상태
	bool stage4_start_check = false; // 스테이지 4에서 재시작할시 상태
	

	// 사운드 On/OFF
	bool lobby_sound = true;	// 로비 사운드
	bool stage_sound = true;	// 스테이지 사운드
	bool shoot_sound = true;	// 슈팅 사운드
	bool score_sound = false;	// 점수 사운드
	bool click_sound = true;	// 클릭 사운드

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CBirdShootingGameView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

#ifndef _DEBUG  // BirdShootingGameView.cpp의 디버그 버전
inline CBirdShootingGameDoc* CBirdShootingGameView::GetDocument() const
   { return reinterpret_cast<CBirdShootingGameDoc*>(m_pDocument); }
#endif

