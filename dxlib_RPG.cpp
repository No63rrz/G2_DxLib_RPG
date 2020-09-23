//########## �w�b�_�[�t�@�C���ǂݍ��� ##########
#include "DxLib.h"
#include "resource.h"

#define GAME_WIDTH			800	//��ʂ̉��̑傫��
#define GAME_HEIGHT			600	//��ʂ̏c�̑傫��
#define GAME_COLOR			32	//��ʂ̃J���[�r�b�g

#define GAME_WINDOW_BAR		0	//�^�C�g���o�[�̓f�t�H���g�ɂ���
#define GAME_WINDOW_NAME	"GAME TITLE"	//�E�B���h�E�̃^�C�g��

#define GAME_FPS 60 //FPS�̐��l

//�}�E�X�̃{�^��
#define MOUSE_BUTTON_CODE 129 //0x0000�`0x0080�܂�

//�L�[�{�[�h�̎��
#define KEY_CODE_KIND 256 //256���

//�p�X�̒���
#define PATH_MAX 255 //255�����܂�
#define NAME_MAX 255 //255�����܂�

//�t�H���g
#define FONT_TANU_PATH TEXT(".\\FONT\\TanukiMagic.ttf")
#define FONT_TANU_NAME TEXT("���ʂ������}�W�b�N")

//�G���[���b�Z�[�W
#define FONT_INSTALL_ERR_TITLE TEXT("�t�H���g�C���X�g�[���G���[")
#define FONT_CREATE_ERR_TITLE TEXT("�t�H���g�쐬�G���[")

#define IMAGE_LOAD_ERR_TITLE TEXT("�摜�ǂݍ��݃G���[")

#define MUSIC_LOAD_ERR_TITLE TEXT("���y�ǂݍ��݃G���[")

//����{�^�����������Ƃ�
#define MSG_CLOSE_TITLE TEXT("�I�����b�Z�[�W")
#define MSG_CLOSE_CAPTION TEXT("�Q�[�����I�����܂����H")

//MOVIE�t�H���_�ƁAmp4�t�@�C�����A�ǉ����ĉ�����
//#define MOVIE_PATH			".\\MOVIE\\neko.mp4"	//����̃p�X
//
//int handle = -1;	//����̃n���h��

enum GAME_SCENE {
	GAME_SCENE_START,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
};

//int�^��POINT�\����
typedef struct STRUCT_I_POINT
{
	int x = -1;
	int y = -1;
}iPOINT;

//�}�E�X�\����
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

//FPS�֘A
int StartTimeFps;
int CountFps;
float CalcFps;
int SampleNumFps = GAME_FPS;

//�L�[�{�[�h���͎擾
char AllKeyState[KEY_CODE_KIND] = { '\0' };
char OldAllKeyState[KEY_CODE_KIND] = { '\0' };

//�}�E�X�̍��W���擾
MOUSE mouse;

//�Q�[���֘A
int GameScene; //�Q�[���V�[�����Ǘ�

//�v���C���[�֘A
CHARA player; //�Q�[���̃L����

//########## �v���g�^�C�v�錾 ##########
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

//########## �v���O�����ōŏ��Ɏ��s�����֐� ##########
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);				//log.txt���o�͂��Ȃ�
	ChangeWindowMode(TRUE);								//�E�B���h�E���[�h�ɐݒ�
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//�w��̐��l�ŃE�B���h�E��\������
	SetWindowStyleMode(GAME_WINDOW_BAR);				//�^�C�g���o�[�̓f�t�H���g�ɂ���
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));			//�E�B���h�E�̃^�C�g���̕���
	SetAlwaysRunFlag(TRUE);								//��A�N�e�B�u�ł����s����
	SetWindowIconID(IDI_ICON1);
	SetWindowUserCloseEnableFlag(FALSE);


	if (DxLib_Init() == -1) { return -1; }	//�c�w���C�u��������������

	//�摜��ǂݍ���
	if (MY_LOAD_IMAGE() == FALSE) { return -1; }
	//���y��ǂݍ���
	if (MY_LOAD_MUSIC() == FALSE) { return -1; }


	//�t�H���g���ꎞ�I�ɃC���X�g�[��
	if (MY_FONT_INSTALL_ONCE() == FALSE) { return -1; }
	//�t�H���g�n���h�����쐬
	if (MY_FONT_CREATE() == FALSE) { return -1; }

	//����̓ǂݍ���
	//handle = LoadGraph(MOVIE_PATH);

	GameScene = GAME_SCENE_START; //�Q�[���V�[���̓X�^�[�g��ʂ���
	SetDrawScreen(DX_SCREEN_BACK); //Draw�n�֐��͗���ʂɕ`��

	//�������[�v
	while (TRUE)
	{
		if (ProcessMessage() != 0) { break; }	//���b�Z�[�W�����̌��ʂ��G���[�̂Ƃ��A�����I��
		if (ClearDrawScreen() != 0) { break; }	//��ʂ������ł��Ȃ������Ƃ��A�����I��

		//��ʂ̕���{�^���������ꂽ��
		if (GetWindowUserCloseFlag(TRUE))
		{
			//�I���_�C�A���O��\��
			int Ret = MessageBox(GetMainWindowHandle(), MSG_CLOSE_CAPTION, MSG_CLOSE_TITLE, MB_YESNO | MB_ICONASTERISK);
			if (Ret == IDYES) { break; } //YES�Ȃ�Q�[���I��
		}
		MY_ALL_KEYDOWN_UPDATE(); //�����Ă���L�[��Ԃ��擾

		MY_MOUSE_UPDATE(); //�}�E�X�̏�Ԃ��擾

		MY_FPS_UPDATE(); //fps�̏����i�X�V�j

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
		//	SeekMovieToGraph(handle, 0);	//����̍Đ��o�[���ŏ�����ɂ���
		//	PlayMovieToGraph(handle);		//������Đ���Ԃɂ���
		//}

		////�^�C�g������`��
		////DrawGraph(0, 0, handle, FALSE);

		////���悪�؂�Ȃ��悤�ɂ������I
		//DrawExtendGraph(0, 0, GAME_WIDTH, GAME_HEIGHT, handle, FALSE);

		//DrawString(0, 0, "������Đ����Ă��܂��E�E�E", GetColor(255, 255, 255));
		//DrawString(0, 20, "�L�̓���", GetColor(255, 255, 255)); //����̐���
		//ScreenFlip();		//���j�^�̃��t���b�V�����[�g�̑����ŗ���ʂ��ĕ`��
	}

	MY_FONT_DERETE();
	MY_FONT_UNINSTALL_ONCE();
	MY_DELETE_IMAGE();
	MY_DELETE_MUSIC();

	DxLib_End();	//�c�w���C�u�����g�p�̏I������

	//DeleteGraph(handle);	//����̍폜

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
		MY_PLAY_INIT(); //�Q�[��������

		GameScene = GAME_SCENE_PLAY; //�v���C��ʂɑJ��

		return;
	}
	return;
}

VOID MY_START_DRAW(VOID)
{
	DrawString(0, 0, "�X�^�[�g��ʁi�G���^�[�L�[�������Ă��������j", GetColor(255, 255, 255));

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
		GameScene = GAME_SCENE_END; //�v���C��ʂɑJ��

		return;
	}
	return;
}

VOID MY_PLAY_DRAW(VOID)
{
	DrawString(0, 0, "�v���C���(�X�y�[�X�L�[�������Ă��������B)", GetColor(255, 255, 255));

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
		GameScene = GAME_SCENE_START; //�v���C��ʂɑJ��

		return;
	}
	return;
}

VOID MY_END_DRAW(VOID)
{
	DrawString(0, 0, "�G���h���(�G�X�P�[�v�L�[�������Ă��������B)", GetColor(255, 255, 255));

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