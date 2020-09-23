//########## ヘッダーファイル読み込み ##########
#include "DxLib.h"
#include "resource.h"

#define GAME_WIDTH			800	//画面の横の大きさ
#define GAME_HEIGHT			600	//画面の縦の大きさ
#define GAME_COLOR			32	//画面のカラービット

#define GAME_WINDOW_BAR		0	//タイトルバーはデフォルトにする
#define GAME_WINDOW_NAME	"GAME TITLE"	//ウィンドウのタイトル

#define GAME_FPS 60 //FPSの数値

//マウスのボタン
#define MOUSE_BUTTON_CODE 129 //0x0000～0x0080まで

//キーボードの種類
#define KEY_CODE_KIND 256 //256種類

//パスの長さ
#define PATH_MAX 255 //255文字まで
#define NAME_MAX 255 //255文字まで

//フォント
#define FONT_TANU_PATH TEXT(".\\FONT\\TanukiMagic.ttf")
#define FONT_TANU_NAME TEXT("たぬき油性マジック")

//エラーメッセージ
#define FONT_INSTALL_ERR_TITLE TEXT("フォントインストールエラー")
#define FONT_CREATE_ERR_TITLE TEXT("フォント作成エラー")

#define IMAGE_LOAD_ERR_TITLE TEXT("画像読み込みエラー")

#define MUSIC_LOAD_ERR_TITLE TEXT("音楽読み込みエラー")

//閉じるボタンを押したとき
#define MSG_CLOSE_TITLE TEXT("終了メッセージ")
#define MSG_CLOSE_CAPTION TEXT("ゲームを終了しますか？")

//MOVIEフォルダと、mp4ファイルも、追加して下さい
//#define MOVIE_PATH			".\\MOVIE\\neko.mp4"	//動画のパス
//
//int handle = -1;	//動画のハンドル

enum GAME_SCENE {
	GAME_SCENE_START,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
};

//int型のPOINT構造体
typedef struct STRUCT_I_POINT
{
	int x = -1;
	int y = -1;
}iPOINT;

//マウス構造体
typedef struct STRUCT_MOUSE
{
	int InputValue = 0;
	int WheelValue = 0;
	iPOINT Point;
	iPOINT OldPoint;
	int OldButton[MOUSE_BUTTON_CODE] = { 0 };
	int Button[MOUSE_BUTTON_CODE] = { 0 };
}MOUSE;

typedef struct STRUCT_FONT
{
	char path[PATH_MAX];
	char name[NAME_MAX];
	int handle;
	int size;
	int bold;
	int type;
}FONT;

typedef struct STRUCT_IMAGE
{
	char path[PATH_MAX];
	int handle;
	int x;
	int y;
	int width;
	int height;

}IMAGE;

typedef struct STRUCT_MUSIC
{
	char path[PATH_MAX];
	int handle;
}MUSIC;

typedef struct STRUCT_CHARA
{
	IMAGE image;
	int speed;

	RECT coll;
	iPOINT collBeforePt;

}CHARA;

//FPS関連
int StartTimeFps;
int CountFps;
float CalcFps;
int SampleNumFps = GAME_FPS;

//キーボード入力取得
char AllKeyState[KEY_CODE_KIND] = { '\0' };
char OldAllKeyState[KEY_CODE_KIND] = { '\0' };

//マウスの座標を取得
MOUSE mouse;

//ゲーム関連
int GameScene; //ゲームシーンを管理

//プレイヤー関連
CHARA player; //ゲームのキャラ

//########## プロトタイプ宣言 ##########
VOID MY_FPS_UPDATE(VOID);
VOID MY_FPS_DRAW(VOID);
VOID MY_FPS_WAIT(VOID);

VOID MY_ALL_KEYDOWN_UPDATE(VOID);
BOOL MY_KEY_DOWN(int);
BOOL MY_KEY_UP(int);
BOOL MY_KEYDOWN_KEEP(int, int);

VOID MY_MOUSE_UPDATE(VOID);
BOOL MY_MOUSE_DOWN(int);
BOOL MY_MOUSE_UP(int);
BOOL MY_MOUSEDOWN_KEEP(int, int);

BOOL MY_FONT_INSTALL_ONCE(VOID);
VOID MY_FONT_UNINSTALL_ONCE(VOID);
BOOL MY_FONT_CREATE(VOID);
VOID MY_FONT_DERETE(VOID);

VOID MY_START(VOID);
VOID MY_START_PROC(VOID);
VOID MY_START_DRAW(VOID);

VOID MY_PLAY_INIT(VOID);
VOID MY_PLAY(VOID);
VOID MY_PLAY_PROC(VOID);
VOID MY_PLAY_DRAW(VOID);

VOID MY_END(VOID);
VOID MY_END_PROC(VOID);
VOID MY_END_DRAW(VOID);

BOOL MY_LOAD_IMAGE(VOID);
VOID MY_DELETE_IMAGE(VOID);

BOOL MY_LOAD_MUSIC(VOID);
VOID MY_DELETE_MUSIC(VOID);

//########## プログラムで最初に実行される関数 ##########
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);				//log.txtを出力しない
	ChangeWindowMode(TRUE);								//ウィンドウモードに設定
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//指定の数値でウィンドウを表示する
	SetWindowStyleMode(GAME_WINDOW_BAR);				//タイトルバーはデフォルトにする
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));			//ウィンドウのタイトルの文字
	SetAlwaysRunFlag(TRUE);								//非アクティブでも実行する
	SetWindowIconID(IDI_ICON1);
	SetWindowUserCloseEnableFlag(FALSE);


	if (DxLib_Init() == -1) { return -1; }	//ＤＸライブラリ初期化処理

	//画像を読み込む
	if (MY_LOAD_IMAGE() == FALSE) { return -1; }
	//音楽を読み込む
	if (MY_LOAD_MUSIC() == FALSE) { return -1; }


	//フォントを一時的にインストール
	if (MY_FONT_INSTALL_ONCE() == FALSE) { return -1; }
	//フォントハンドルを作成
	if (MY_FONT_CREATE() == FALSE) { return -1; }

	//動画の読み込み
	//handle = LoadGraph(MOVIE_PATH);

	GameScene = GAME_SCENE_START; //ゲームシーンはスタート画面から
	SetDrawScreen(DX_SCREEN_BACK); //Draw系関数は裏画面に描画

	//無限ループ
	while (TRUE)
	{
		if (ProcessMessage() != 0) { break; }	//メッセージ処理の結果がエラーのとき、強制終了
		if (ClearDrawScreen() != 0) { break; }	//画面を消去できなかったとき、強制終了

		//画面の閉じるボタンを押された時
		if (GetWindowUserCloseFlag(TRUE))
		{
			//終了ダイアログを表示
			int Ret = MessageBox(GetMainWindowHandle(), MSG_CLOSE_CAPTION, MSG_CLOSE_TITLE, MB_YESNO | MB_ICONASTERISK);
			if (Ret == IDYES) { break; } //YESならゲーム終了
		}
		MY_ALL_KEYDOWN_UPDATE(); //押しているキー状態を取得

		MY_MOUSE_UPDATE(); //マウスの状態を取得

		MY_FPS_UPDATE(); //fpsの処理（更新）

		switch (GameScene)
		{
		case GAME_SCENE_START:
			MY_START();
			break;
		case GAME_SCENE_PLAY:
			MY_PLAY();
			break;
		case GAME_SCENE_END:
			MY_END();
			break;
		}

		MY_FPS_DRAW();

		ScreenFlip();

		MY_FPS_WAIT();

		//if (GetMovieStateToGraph(handle) == 0)
		//{
		//	SeekMovieToGraph(handle, 0);	//動画の再生バーを最初からにする
		//	PlayMovieToGraph(handle);		//動画を再生状態にする
		//}

		////タイトル動画描画
		////DrawGraph(0, 0, handle, FALSE);

		////動画が切れないようにしたい！
		//DrawExtendGraph(0, 0, GAME_WIDTH, GAME_HEIGHT, handle, FALSE);

		//DrawString(0, 0, "動画を再生しています・・・", GetColor(255, 255, 255));
		//DrawString(0, 20, "猫の動画", GetColor(255, 255, 255)); //動画の説明
		//ScreenFlip();		//モニタのリフレッシュレートの速さで裏画面を再描画
	}

	MY_FONT_DERETE();
	MY_FONT_UNINSTALL_ONCE();
	MY_DELETE_IMAGE();
	MY_DELETE_MUSIC();

	DxLib_End();	//ＤＸライブラリ使用の終了処理

	//DeleteGraph(handle);	//動画の削除

	return 0;
}
VOID MY_FPS_UPDATE(VOID)
{
	if (CountFps == 0)
	{
		StartTimeFps = GetNowCount();
	}
	if (CountFps == SampleNumFps)
	{
		int now = GetNowCount();
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)SampleNumFps);
		CountFps = 0;
		StartTimeFps = now;
	}
	CountFps++;
	return;
}

VOID MY_FPS_DRAW(VOID)
{
	DrawFormatString(0, GAME_HEIGHT - 20, GetColor(255, 255, 255), "FPS:%.1f", CalcFps);
	return;
}

VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;
	int waitTime = CountFps * 1000 / GAME_FPS - resultTime;

	if (waitTime > 0)
	{
		WaitTimer(waitTime);
	}
	return;
}

VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	char TempKey[KEY_CODE_KIND];

	for (int i = 0; i < KEY_CODE_KIND; i++)
	{
		OldAllKeyState[i] = AllKeyState[i];
	}

	GetHitKeyStateAll(TempKey);
	for (int i = 0; i < KEY_CODE_KIND; i++)
	{
		if (TempKey[i] != 0)
		{
			AllKeyState[i]++;
		}
		else {
			AllKeyState[i] = 0;
		}
	}
	return;
}

BOOL MY_KEY_DOWN(int KEY_INPUT_)
{
	if (AllKeyState[KEY_INPUT_] != 0)
	{
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL MY_KEY_UP(int KEY_INPUT_)
{
	if (OldAllKeyState[KEY_INPUT_] >= 1
		&& AllKeyState[KEY_INPUT_] == 0)
	{
		return TRUE;
	}
	else {
		return FALSE;
	}

}

BOOL MY_KEYDOWN_KEEP(int KEY_INPUT_, int DownTime)
{
	int UpdateTime = DownTime * GAME_FPS;

	if (AllKeyState[KEY_INPUT_] > UpdateTime)
	{
		return TRUE;
	}
	else {
		return FALSE;
	}
}

VOID MY_MOUSE_UPDATE(VOID)
{
	mouse.OldPoint = mouse.Point;

	for (int i = 0; i < MOUSE_BUTTON_CODE; i++) { mouse.OldButton[i] = mouse.Button[i]; }

	GetMousePoint(&mouse.Point.x, &mouse.Point.y);

	mouse.InputValue = GetMouseInput();

	if ((mouse.InputValue & MOUSE_INPUT_LEFT) == MOUSE_INPUT_LEFT) { mouse.Button[MOUSE_INPUT_LEFT]++; }
	if ((mouse.InputValue & MOUSE_INPUT_LEFT) != MOUSE_INPUT_LEFT) { mouse.Button[MOUSE_INPUT_LEFT] = 0; }

	if ((mouse.InputValue & MOUSE_INPUT_MIDDLE) == MOUSE_INPUT_MIDDLE) { mouse.Button[MOUSE_INPUT_MIDDLE]++; }
	if ((mouse.InputValue & MOUSE_INPUT_MIDDLE) != MOUSE_INPUT_MIDDLE) { mouse.Button[MOUSE_INPUT_MIDDLE] = 0; }

	if ((mouse.InputValue & MOUSE_INPUT_RIGHT) == MOUSE_INPUT_RIGHT) { mouse.Button[MOUSE_INPUT_RIGHT]++; }
	if ((mouse.InputValue & MOUSE_INPUT_RIGHT) != MOUSE_INPUT_RIGHT) { mouse.Button[MOUSE_INPUT_RIGHT] = 0; }

	mouse.WheelValue = GetMouseWheelRotVol();

	return;
}

BOOL MY_MOUSE_DOWN(int MOUSE_INPUT_)
{
	if (mouse.Button[MOUSE_INPUT_] != 0)
	{
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL MY_MOUSE_UP(int MOUSE_INPUT_)
{
	if (mouse.OldButton[MOUSE_INPUT_] >= 1 &&
		mouse.Button[MOUSE_INPUT_] == 0)
	{
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL MY_MOUSEDOWN_KEEP(int MOUSE_INPUT_, int DownTime)
{
	int UpdateTime = DownTime * GAME_FPS;

	if (mouse.Button[MOUSE_INPUT_] > UpdateTime)
	{
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL MY_FONT_INSTALL_ONCE(VOID)
{
	if (AddFontResourceEx(FONT_TANU_PATH, FR_PRIVATE, NULL) == 0)
	{
		MessageBox(GetMainWindowHandle(), FONT_TANU_NAME, FONT_INSTALL_ERR_TITLE, MB_OK);
		return FALSE;
	}
	return TRUE;
}

VOID MY_FONT_UNINSTALL_ONCE(VOID)
{
	RemoveFontResourceEx(FONT_TANU_PATH, FR_PRIVATE, NULL);
	return;
}

BOOL MY_FONT_CREATE(VOID)
{
	//strcpy_s(FontTanu32.path, sizeof(FontTanu32.path), FONT_TANU_PATH);
	//strcpy_s(FontTanu32.name, sizeof(FontTanu32.name), FONT_TANU_NAME);
	//FontTanu32.handle = -1;
	//FontTanu32.size = 60;
	//FontTanu32.bold = 1;
	//FontTanu32.type = DX_FONTTYPE_ANTIALIASING_EDGE;

	//FontTanu32.handle = CreateFontToHandle(FontTanu32.name, FontTanu32.size, FontTanu32.bold, FontTanu32.type);
	//if (FontTanu32.handle == 1)
	//{
	//	MessageBox(GetMainWindowHandle(), FONT_TANU_NAME, FONT_CREATE_ERR_TITLE, MB_OK);
	//	return FALSE;

	//}
	return TRUE;

}

VOID MY_FONT_DERETE(VOID)
{
	return;
}

VOID MY_START(VOID)
{
	MY_START_PROC();
	MY_START_DRAW();

	return;
}

VOID MY_START_PROC(VOID)
{
	if (MY_KEY_DOWN(KEY_INPUT_RETURN) == TRUE)
	{
		MY_PLAY_INIT(); //ゲーム初期化

		GameScene = GAME_SCENE_PLAY; //プレイ画面に遷移

		return;
	}
	return;
}

VOID MY_START_DRAW(VOID)
{
	DrawString(0, 0, "スタート画面（エンターキーを押してください）", GetColor(255, 255, 255));

	return;
}

VOID MY_PLAY_INIT(VOID)
{
	return;
}

VOID MY_PLAY(VOID)
{
	MY_PLAY_PROC();
	MY_PLAY_DRAW();

	return;
}

VOID MY_PLAY_PROC(VOID)
{
	if (MY_KEY_DOWN(KEY_INPUT_SPACE) == TRUE)
	{
		GameScene = GAME_SCENE_END; //プレイ画面に遷移

		return;
	}
	return;
}

VOID MY_PLAY_DRAW(VOID)
{
	DrawString(0, 0, "プレイ画面(スペースキーを押してください。)", GetColor(255, 255, 255));

	return;
}
VOID MY_END(VOID)
{
	MY_END_PROC();
	MY_END_DRAW();

	return;
}

VOID MY_END_PROC(VOID)
{
	if (MY_KEY_DOWN(KEY_INPUT_ESCAPE) == TRUE)
	{
		GameScene = GAME_SCENE_START; //プレイ画面に遷移

		return;
	}
	return;
}

VOID MY_END_DRAW(VOID)
{
	DrawString(0, 0, "エンド画面(エスケープキーを押してください。)", GetColor(255, 255, 255));

	return;
}

BOOL MY_LOAD_IMAGE(VOID)
{
	return TRUE;
}

VOID MY_DELETE_IMAGE(VOID)
{
	return;
}

BOOL(MY_LOAD_MUSIC)(VOID)
{
	return TRUE;
}

VOID MY_DELETE_MUSIC(VOID)
{
	return;
}