//ヘッダファイル読み込み
#include "game.h"		//ゲーム全体のヘッダファイル
#include "keyboard.h"	//キーボードの処理
#include "FPS.h"		//FPSの処理

#include "mouse.h"		//マウスの処理
#include "shape.h"		//図形の処理

#include <math.h>		//数学

//マクロ定義
#define TAMA_DIV_MAX	4	//弾の画像の最大数
#define TAMA_MAX		100	//弾の総数
#define TEKI_KIND		1	//敵の種類
#define TEKI_MAX		30	//敵の数

//構造体の定義

//画像の構造体
struct IMAGE
{
	int handle = -1;	//画像のハンドル(管理番号)
	char path[255];		//画像の場所(パス)

	int x;				//X位置
	int y;				//Y位置
	int width;			//幅
	int height;			//高さ

	BOOL IsDraw = FALSE;//画像が描画できる？
};

//キャラクタの構造体
struct CHARACTOR
{
	IMAGE img;			//画像構造体
	int speed = 1;		//移動速度	
	RECT coll;			//当たり判定の領域(四角)
};

//動画の構造体
struct MOVIE
{
	int handle = -1;	//動画のハンドル
	char path[255];		//動画のパス

	int x;				//X位置
	int y;				//Y位置
	int width;			//幅
	int height;			//高さ

	int Volume = 255;	//ボリューム(最小)0〜255(最大)　
};

//音楽の構造体
struct AUDIO
{
	int handle = -1;	//音楽のハンドル
	char path[255];		//音楽のパス

	int Volume = -1;	//ボリューム（MIN 0 〜 255 MAX）
	int playType = -1;	//BGM or SE
};

//弾の構造体
struct TAMA
{
	int handle[TAMA_DIV_MAX];	//画僧のハンドル
	char path[255];	//画像のパス

	int DivTate;	//分割数（縦）
	int DivYoko;	//分割数（横）
	int DivMAX;		//分割総数

	int AnimeCnt = 0;		//アニメーションカウンタ
	int AnimeCntMAX = 0;	//アニメーションカウンタMAX

	int NowIndex = 0;		//現在位の画像の要素数

	int startX;				 //Xの最初の位置
	int startY;				 //Yの最初の位置

	float radius;
	float degree;

	int x;					//X位置
	int y;					//Y位置
	int width;				//幅
	int height;				//高さ

	int Speed;				//速度

	RECT coll;				//当たり判定（矩形）

	BOOL IsDraw = FALSE;	//描画できる？
};

//グローバル変数
//シーンを管理する変数
GAME_SCENE GameScene;		//現在のゲームのシーン
GAME_SCENE OldGameScene;	//前回のゲームのシーン
GAME_SCENE NextGameScene;	//次のゲームのシーン

//画面の切り替え
BOOL IsFadeOut = FALSE;		//フェードアウト
BOOL IsFadeIn = FALSE;		//フェードイン

int fadeTimeMill = 2000;					//切り替えミリ秒
int fadeTimeMax = fadeTimeMill / 1000 * 60;	//ミリ秒をフレーム秒に変換

//フェードアウト
int fadeOutCntInit = 0;				//初期値
int fadeOutCnt = fadeOutCntInit;	//フェードアウトのカウンタ
int fadeOutCntMax = fadeTimeMax;	//フェードアウトのカウンタMAX

//フェードイン
int fadeInCntInit = fadeTimeMax;	//初期値
int fadeInCnt = fadeInCntInit;		//フェードアウトのカウンタ
int fadeInCntMax = fadeTimeMax;		//フェードアウトのカウンタMAX

//弾の構造体 
struct TAMA tama_moto;	//元	
struct TAMA tama[TAMA_MAX];	//実際に使う

//弾の発射カウンタ
int tamaShotCnt = 0;
int tamaShotCntMAX = 5;

//プレイヤー
CHARACTOR player;

//背景画像
IMAGE back[3];	//背景は2つの画像
IMAGE Back[6];	//オープニングとエンディング画像用

//音楽
AUDIO backmusic[3];	//バックグラウンドミュージック

//敵データ（元）
CHARACTOR teki_moto[TEKI_KIND];

//実際の敵のデータ
CHARACTOR teki[TEKI_MAX];

//敵データのパス
char tekiPath[TEKI_KIND][255] =
{
	{".\\img\\Enemy.png"},
	/*{".\\img\\teki_gray.png"},
	{".\\img\\teki_green.png"},
	{".\\img\\teki_mizu.png"},
	{".\\img\\teki_purple.png"},
	{".\\img\\teki_red.png"},
	{".\\img\\teki_red_big.png"},
	{".\\img\\teki_yellow.png"}*/
};

//敵が出てくるカウント
int TekiAddCnt = 0;
int TekiAddCntMax = 30;		//60FPSで1回・・・1秒に1回

//ゲームスコア
int Score = 0;

//プロトタイプ宣言
VOID Title(VOID);		//タイトル画面
VOID TitleProc(VOID);	//タイトル画面(処理)
VOID TitleDraw(VOID);	//タイトル画面(描画)

VOID Title2(VOID);		//タイトル画面
VOID TitleProc2(VOID);	//タイトル画面(処理)
VOID TitleDraw2(VOID);	//タイトル画面(描画)

VOID Play(VOID);		//プレイ画面
VOID PlayProc(VOID);	//プレイ画面(処理)
VOID PlayDraw(VOID);	//プレイ画面(描画)

VOID End(VOID);			//エンド画面
VOID EndProc(VOID);		//エンド画面(処理)
VOID EndDraw(VOID);		//エンド画面(描画)

VOID Change(VOID);		//切り替え画面
VOID ChangeProc(VOID);	//切り替え画面(処理)
VOID ChangeDraw(VOID);	//切り替え画面(描画)

VOID ChangeScene(GAME_SCENE scene);	//シーン切り替え

VOID CollUpdatePlayer(CHARACTOR* chara);	//当たり判定の領域を更新
VOID CollUpdate(CHARACTOR* chara);			//当たり判定
VOID CollUpdateTama(TAMA* tama);			//弾の当たり判定の更新
VOID CollUpdateTeki(CHARACTOR* chara);		//敵の当たり判定

BOOL OnCollRect(RECT a, RECT b);			//矩形と矩形の当たり判定

BOOL GameLoad(VOID);	//ゲームのデータを読み込み

BOOL LoadImageMem(IMAGE* image, const char* path);										//ゲームの画像を読み込み
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType);				//ゲームの音楽を読み込み
BOOL LoadImageDivMem(int* handle, const char* path, int bunkatuYoko, int bunkatuTate);	//ゲームの画像の分割読み込み

VOID GameInit(VOID);						//ゲームのデータの初期化

VOID DrawTama(TAMA* tama);					//弾の描画
VOID ShotTama(TAMA* tama, float deg);		//弾の発射

// プログラムは WinMain から始まります
//Windowsのプログラミング方法 = (WinAPI)で動いている！
//DxLibは、DirectXという、ゲームプログラミングをかんたんに使える仕組み
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);				//Log.txtを出力しない
	ChangeWindowMode(TRUE);								//ウィンドウモードに設定
	SetMainWindowText(GAME_TITLE);						//ウィンドウのタイトルの文字
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//ウィンドウの解像度を設定
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);				//ウィンドウの大きさを設定
	SetBackgroundColor(255, 255, 255);					//デフォルトの背景の色
	SetWindowIconID(GAME_ICON_ID);						//アイコンファイルを読込
	SetWindowStyleMode(GAME_WINDOW_BAR);				//ウィンドウバーの状態
	SetWaitVSyncFlag(TRUE);								//ディスプレイの垂直同期を有効にする
	SetAlwaysRunFlag(TRUE);								//ウィンドウをずっとアクティブにする

	//ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1)
	{
		// エラーが起きたら直ちに終了
		return -1;
	}

	//ダブルバッファリング有効化
	SetDrawScreen(DX_SCREEN_BACK);

	//最初のシーンは、タイトル画面から
	GameScene = GAME_SCENE_TITLE;

	//ゲーム全体の初期化

	//ゲーム読み込み
	if (!GameLoad())
	{
		//データの読み込みに失敗したとき
		DxLib_End();	//DxLib終了
		return -1;		//異常終了
	}

	//ゲームの初期化
	GameInit();

	//無限ループ
	while (1)
	{
		if (ProcessMessage() != 0) { break; }	//メッセージを受け取り続ける
		if (ClearDrawScreen() != 0) { break; }	//画面を消去する

		//キーボード入力の更新
		AllKeyUpdate();

		//マウスの入力の更新
		MouseUpdate();

		//FPS値の更新
		FPSUpdate();

		//ESCキーで強制終了
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) { break; }

		//以前のシーンを取得
		if (GameScene != GAME_SCENE_CHANGE)
		{
			OldGameScene = GameScene;
		}

		//シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:
			Title();			//タイトル画面
			break;
		case GAME_SCENE_TITLE2:
			Title2();			//タイトル画面
			break;
		case GAME_SCENE_PLAY:
			Play();				//プレイ画面
			break;
		case GAME_SCENE_END:
			End();				//エンド画面
			break;
		case GAME_SCENE_CHANGE:
			Change();			//切り替え画面
			break;
		default:
			break;
		}

		//シーンを切り替える
		if (OldGameScene != GameScene)
		{
			//現在のシーンが切り替え画面でないとき
			if (GameScene != GAME_SCENE_CHANGE)
			{
				NextGameScene = GameScene;		//次のシーンを保存
				GameScene = GAME_SCENE_CHANGE;	//画面切り替えシーンに変える
			}
		}

		//FPS値を描画
		FPSDraw();

		//FPS値を待つ
		FPSWait();

		ScreenFlip();	//ダブルバッファリングした画面を描画
	}

	//読み込んだ画像を解放
	for (int i = 0; i < TAMA_DIV_MAX; i++) {DeleteGraph(tama_moto.handle[i]);}

	//プレイヤー解放
	DeleteGraph(player.img.handle);

	//背景画像を解放
	DeleteGraph(back[0].handle);
	DeleteGraph(back[1].handle);
	DeleteGraph(Back[0].handle);
	DeleteGraph(Back[1].handle);
	DeleteGraph(Back[2].handle);
	DeleteGraph(Back[3].handle);
	DeleteGraph(Back[4].handle);
	DeleteGraph(Back[5].handle);

	//敵の画像を解放
	for (int i = 0; i < TEKI_KIND; i++)
	{
	DeleteGraph(teki_moto[i].img.handle);
	}

	//音楽解放
	DeleteSoundMem(backmusic[0].handle);
	DeleteSoundMem(backmusic[1].handle);
	DeleteSoundMem(backmusic[2].handle);

	//ＤＸライブラリ使用の終了処理
	DxLib_End();

	return 0;	// ソフトの終了 
}

/// <summary>
/// ゲームのデータを読み込み
/// </summary>
/// <returns>読み込めたらTRUE / 読み込めなかったらFALSE</returns>
BOOL GameLoad(VOID)
{
	//弾の分割数を設定
	tama_moto.DivYoko = 4;
	tama_moto.DivTate = 1;

	//
	strcpyDx(tama_moto.path, ".\\img\\dia_blue.png");
	
	//画像を分割して読み込み
	if (LoadImageDivMem(&tama_moto.handle[0],tama_moto.path,tama_moto.DivYoko, tama_moto.DivTate) == FALSE) {return FALSE;}

	//幅と高さを取得
	GetGraphSize(tama_moto.handle[0], &tama_moto.width, &tama_moto.height);
	
	//位置を設定
	tama_moto.x = GAME_WIDTH / 2 - tama_moto.width / 2;		//中央揃え
	tama_moto.y = GAME_HEIGHT  - tama_moto.height;	//画面下

	//速度
	tama_moto.Speed = 1;		

	//アニメを買える速度
	tama_moto.AnimeCntMAX = 10;

	//当たり判定の更新
	CollUpdateTama(&tama_moto);

	//画像を表示しない
	tama_moto.IsDraw = FALSE;

	//すべてのたまに情報をコピー
	for (int i = 0; i < TAMA_MAX; i++)
	{
		tama[i] = tama_moto;
	}

	//プレイヤーの画像を読み込み
	if (LoadImageMem(&player.img, ".\\img\\player.png") == FALSE) { return FALSE;}
	player.img.x = GAME_WIDTH / 2 - player.img.width;
	player.img.y = GAME_HEIGHT / 2 - player.img.width;
	CollUpdatePlayer(&player);	//当たり判定
	player.img.IsDraw = TRUE;	//描画する

	//背景の画像を読み込み?@
	if (LoadImageMem(&back[0], ".\\img\\Playback.png") == FALSE) { return FALSE; }
	back[0].x = back[0].width;
	back[0].y = 0;
	back[0].IsDraw = TRUE;	//描画する

	//背景の画像を読み込み?A
	if (LoadImageMem(&back[1], ".\\img\\Playback2.png") == FALSE) { return FALSE; }
	back[1].x = 0;
	back[1].y = 0;
	back[1].IsDraw = TRUE;	//描画する

	//背景の画像を読み込み?B
	if (LoadImageMem(&Back[0], ".\\img\\opening.png") == FALSE) { return FALSE; }
	Back[0].x = 0;
	Back[0].y = 0;
	Back[0].IsDraw = TRUE;	//描画する

	//背景の画像を読み込み?C
	if (LoadImageMem(&Back[1], ".\\img\\Title.png") == FALSE) { return FALSE; }
	Back[1].x = 220;
	Back[1].y = 170;
	Back[1].IsDraw = TRUE;	//描画する

		//背景の画像を読み込み?D
	if (LoadImageMem(&Back[4], ".\\img\\PushEnter.png") == FALSE) { return FALSE; }
	Back[4].x = 260;
	Back[4].y = 350;
	Back[4].IsDraw = TRUE;	//描画する

			//背景の画像を読み込み?D
	if (LoadImageMem(&Back[5], ".\\img\\setumei.png") == FALSE) { return FALSE; }
	Back[5].x = 0;
	Back[5].y = 0;
	Back[5].IsDraw = TRUE;	//描画する

	//背景の画像を読み込み?C
	if (LoadImageMem(&Back[2], ".\\img\\Ending.png") == FALSE) { return FALSE; }
	Back[2].x = 0;
	Back[2].y = 0;
	Back[2].IsDraw = TRUE;	//描画する

	//背景の画像を読み込み?D
	if (LoadImageMem(&Back[3], ".\\img\\GAMECLERA.png") == FALSE) { return FALSE; }
	Back[3].x = 210;
	Back[3].y = 150;
	Back[3].IsDraw = TRUE;	//描画する

		//敵の画像を読み込み
	for (int i = 0; i < TEKI_KIND; i++)
	{
		if (LoadImageMem(&teki_moto[0].img, tekiPath[i]) == FALSE) { return FALSE; }
		teki_moto[0].img.x = -teki_moto[i].img.width;
		teki_moto[0].img.y = GAME_HEIGHT / 2 - teki_moto[0].img.height;
		CollUpdatePlayer(&teki_moto[i]);	//当たり判定
		teki_moto[0].img.IsDraw = FALSE;	//描画しません
	}

	//音楽読み込み
	if (!LoadAudio(&backmusic[0], ".\\audio\\TitleBGM.m4a", 50, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&backmusic[1], ".\\audio\\PlayBGM.wav", 50, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&backmusic[2], ".\\audio\\EndingBGM.mp3", 50, DX_PLAYTYPE_LOOP)) { return FALSE; }

	return TRUE;	//全て読み込みた！ 
}

/// <summary>
/// 画像を分割にしてメモリに読み込み
/// </summary>
/// <param name="handle">ハンドル配列の先頭アドレス</param>
/// <param name="path">画像のパス</param>
/// <param name="bunkatuYoko">分割するときの横の数</param>
/// <param name="bunkatuTate">分割するときの縦の数</param>
/// <returns></returns>
BOOL LoadImageDivMem(int* handle, const char* path, int bunkatuYoko, int bunkatuTate)
{
	//弾の読み込み
	int IsTamaLoad = -1;	//画像が読み込みたか？

	//一時的に画像のハンドルを用意する
	int TamaHandle = LoadGraph(path);

	//読み込みエラー
	if (TamaHandle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//ウィンドウハンドル
			path,				//本文
			"画像読み込みエラー",	//タイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}

	//画像の幅と高さを取得
	int TamaWidth = -1;		//幅
	int TamaHeight = -1;	//高さ
	GetGraphSize(TamaHandle, &TamaWidth, &TamaHeight);

	//分割して読み込み
	IsTamaLoad = LoadDivGraph(
		path,								//画像のパス
		TAMA_DIV_MAX,						//分割総数
		bunkatuYoko, bunkatuTate,			//横の分割,縦の分割
		TamaWidth / bunkatuYoko, TamaHeight / bunkatuTate,	//画像1つ分の幅,高さ
		handle								//連続で管理する配列の先頭アドレス
	);

	//分割エラー
	if (IsTamaLoad == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//ウィンドウハンドル
			path,				//本文
			"画像分割エラー",		//タイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}

	//一時的に読み込んだハンドルを解放
	DeleteGraph(TamaHandle);

	return TRUE;
}

/// <summary>
/// 画像をメモリに読み込み
/// </summary>
/// <param name="image">画像構造体のアドレス</param>
/// <param name="path">画像のパス</param>
/// <returns></returns>
BOOL LoadImageMem(IMAGE* image, const char* path)
{
	//ゴールの画像を読み込み
	strcpyDx(image->path, path);	//パスのコピー
	image->handle = LoadGraph(image->path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (image->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			image->path,			//メッセージ本文
			"画像読み込みエラー！",		//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}

	//画像の幅と高さを取得
	GetGraphSize(image->handle, &image->width, &image->height);

	//読み込めた
	return TRUE;
}

/// <summary>
/// 音楽をメモリに読み込み
/// </summary>
/// <param name="audio">Audio構造体変数のアドレス</param>
/// <param name="path">Audioの音楽パス</param>
/// <param name="volume">ボリューム</param>
/// <param name="playType">DX_PLAYTYPE_LOOP or DX_PLAYTYPE_BACK</param>
/// <returns></returns>
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType)
{
	//音楽の読み込み
	strcpyDx(audio->path, path);					//パスのコピー
	audio->handle = LoadSoundMem(audio->path);		//音楽の読み込み

	//音楽が読み込めなかったときは、エラー(-1)が入る
	if (audio->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			audio->path,			//メッセージ本文
			"音楽読み込みエラー！",		//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}

	//その他の設定
	audio->Volume = volume;
	audio->playType = playType;

	return TRUE;
}

/// <summary>
/// ゲームデータを初期化
/// </summary>
/// <param name=""></param>
VOID GameInit(VOID)
{
	//ゲームスコアを初期化
	Score = 0;

	//プレイヤーの初期化
	player.img.x = GAME_WIDTH / 2 - player.img.width;
	player.img.y = GAME_HEIGHT / 2 - player.img.width;
	CollUpdatePlayer(&player);	//当たり判定
	player.img.IsDraw = TRUE;	//描画する

	//背景画像を設定?@
	back[0].x = back[0].width;
	back[0].y = 0;
	back[0].IsDraw = TRUE;	//描画する

	//背景画像を設定?A
	back[1].x = 0;
	back[1].y = 0;
	back[1].IsDraw = TRUE;	//描画する

	//敵の初期化
	for (int i = 0; i < TEKI_KIND; i++)
	{
		teki_moto[0].img.x = GAME_WIDTH / 2 - teki_moto[0].img.width;
		teki_moto[0].img.y = -teki_moto[i].img.width;
		CollUpdatePlayer(&teki_moto[i]);	//当たり判定
		teki_moto[0].img.IsDraw = FALSE;	//描画しません
	}
}

/// <summary>
/// シーンを切り替える関数
/// </summary>
/// <param name="scene">シーン</param>
VOID ChangeScene(GAME_SCENE scene)
{
	GameScene = scene;	//シーンを切り替え
	IsFadeIn = FALSE;	//フェードインしない
	IsFadeOut = TRUE;	//フェードアウトする

	return;
}

/// <summary>
/// タイトル画面
/// </summary>
VOID Title(VOID)
{
	TitleProc();	//処理
	TitleDraw();	//描画

	return;
}

/// <summary>
/// タイトル画面の処理
/// </summary>
VOID TitleProc(VOID)
{
	//音楽を流す処理
	if (CheckSoundMem(backmusic[0].handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(backmusic[0].handle, backmusic[0].playType);
	}



	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーン切り替え
		//次のシーンの初期化をここで行うと楽

		//音楽を止める
		StopSoundMem(backmusic[0].handle);

		//ゲームの初期化
		GameInit();

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_TITLE2);

		return;
	}

	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	DrawGraph(Back[0].x, Back[0].y, Back[0].handle, TRUE);
	DrawGraph(Back[1].x, Back[1].y, Back[1].handle, TRUE);
	DrawGraph(Back[4].x, Back[4].y, Back[4].handle, TRUE);

	//DrawString(0, 0, "タイトル画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 弾の描画
/// </summary>
/// <param name=""></param>
VOID DrawTama(TAMA* tama) 
{

	//	弾の描画ができるとき
	if(tama->IsDraw == TRUE)
	{
		//弾の描画
		DrawGraph(tama->x, tama->y, tama->handle[tama->NowIndex], TRUE);

		//画像を変えるタイミング
		if (tama->AnimeCnt  < tama->AnimeCntMAX)
		{
			tama->AnimeCnt++;
		}
		else {

			//弾の添字が弾の分割数の最大よりも下の時
			if (tama->NowIndex < TAMA_DIV_MAX - 1)
			{
				tama->NowIndex++;	//次の画像へ
			}
			else
			{
				tama->NowIndex = 0;	//最初に戻す
			}
			tama->AnimeCnt = 0;
		}
	}
}

/// <summary>
/// 説明画面
/// </summary>
VOID Title2(VOID)
{
	TitleProc2();	//処理
	TitleDraw2();	//描画

	return;
}

/// <summary>
/// 説明画面の処理
/// </summary>
VOID TitleProc2(VOID)
{
	//音楽を流す処理
	/*if (CheckSoundMem(backmusic[0].handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(backmusic[0].handle, backmusic[0].playType);
	}*/



	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーン切り替え
		//次のシーンの初期化をここで行うと楽

		//音楽を止める
		StopSoundMem(backmusic[0].handle);

		//ゲームの初期化
		GameInit();

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);

		return;
	}

	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw2(VOID)
{
	DrawGraph(Back[5].x, Back[5].y, Back[5].handle, TRUE);//数字変えて
	DrawGraph(Back[4].x, Back[4].y, Back[4].handle, TRUE);

	//DrawString(0, 0, "タイトル画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// プレイ画面
/// </summary>
VOID Play(VOID)
{
	PlayProc();	//処理
	PlayDraw();	//描画

	return;
}

/// <summary>
/// プレイ画面の処理
/// </summary>
VOID PlayProc(VOID)
{
	//音楽を流す処理
	if (CheckSoundMem(backmusic[1].handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(backmusic[1].handle, backmusic[1].playType);
	}
	if (Score >= 5000)
	{
		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_END);

		//音楽を止める
		StopSoundMem(backmusic[1].handle);

		//マウスを描画する
		SetMouseDispFlag(TRUE);

		return;
	}
	/*
	//敵と自分が当たったら終了
	else if (OnCollRect(player.coll = teki->coll) ==TRUE)
	{

	}
	*/
	/*プレイヤーを操作する
	if (KeyDown(KEY_INPUT_A) == TRUE)
	{
		if (player.img.x - player.speed >= 0)
		{
			player.img.x -= player.speed+5;
		}
	}
	if (KeyDown(KEY_INPUT_D) == TRUE)
	{
		if (player.img.x +player.img.width +  player.speed <= GAME_WIDTH)
		{
			player.img.x += player.speed+5;
		}
	}
	if (KeyDown(KEY_INPUT_W) == TRUE)
	{
		if (player.img.y - player.speed >= 0)
		{
			player.img.y -= player.speed+5;
		}
	}
	if (KeyDown(KEY_INPUT_S) == TRUE)
	{
		if (player.img.y + player.img.height +  player.speed <= GAME_HEIGHT)
		{
			player.img.y += player.speed+5;
		}
	}
	*/

	//マウスの位置にプレイヤーを置く
	player.img.x = mouse.Point.x - player.img.width / 2;
	player.img.y = mouse.Point.y - player.img.height / 2;

	//プレイヤーの当たり判定の更新
	CollUpdatePlayer(&player);

	//スペースキーを押しているとき
	//if (KeyDown(KEY_INPUT_SPACE) == TRUE)

	//マウスの左ボタンを押しているとき
	if(MouseDown(MOUSE_INPUT_LEFT) == TRUE)
	{
		if (tamaShotCnt == 0)
		{
			//弾を発射する
			for (int i = 0; i < TAMA_MAX; i++)
			{
				if(tama[i].IsDraw == FALSE)
				{
					ShotTama(&tama[i], 270);
					
					//弾を１発出したらループを抜ける
					break;
				}
					
			}
			
			//弾を発射する
			for (int i = 0; i < TAMA_MAX; i++)
			{
				if(tama[i].IsDraw == FALSE)
				{
					ShotTama(&tama[i], 240);
					
					//弾を１発出したらループを抜ける
					break;
				}
					
			}
			/*弾を発射する
			for (int i = 0; i < TAMA_MAX; i++)
			{
				if(tama[i].IsDraw == FALSE)
				{
					ShotTama(&tama[i], 210);
					
					//弾を１発出したらループを抜ける
					break;
				}
					
			}
			//弾を発射する
			for (int i = 0; i < TAMA_MAX; i++)
			{
				if(tama[i].IsDraw == FALSE)
				{
					ShotTama(&tama[i], 300);
					
					//弾を１発出したらループを抜ける
					break;
				}
					
			}
			弾を発射する
			for (int i = 0; i < TAMA_MAX; i++)
			{
				if(tama[i].IsDraw == FALSE)
				{
					ShotTama(&tama[i], 330);
					
					//弾を１発出したらループを抜ける
					break;
				}
					
			}
			*/
		}
		
		//弾の発射待ち
		if (tamaShotCnt < tamaShotCntMAX)
		{
			tamaShotCnt++;
		}
		else
		{
			tamaShotCnt = 0;
		}


	}

	//弾を飛ばす
	for (int i = 0; i < TAMA_MAX; i++)
	{
		//描画されているとき
		if (tama[i].IsDraw == TRUE)
		{
			//tama[i].x
			//tama[i].y -= tama[i].Speed;

			//弾の位置を修正
			//tama[i].x;

			//弾を横に飛ばす
			tama[i].x = tama[i].startX + cos(tama[i].degree * DX_PI/ 180.0f) * tama[i].radius;
			tama[i].y = tama[i].startY + sin(tama[i].radius* DX_PI/ 180.0f) * tama[i].degree;

			//渦巻になる
			//tama[i].degree++;

			//半径を足す
			tama[i].radius += tama[i].Speed;

			//当たり判定の更新
			CollUpdateTama(&tama[i]);

			//画面外に出たら、描画しない
			if (tama[i].y + tama[i].height < 0 ||	 //画面外（上）
				tama[i].y > GAME_HEIGHT ||			 //画面外（下）
				tama[i].x + tama[i].width < 0 ||	 //画面外（左）
				tama[i].x > GAME_WIDTH)				 //画面外（右）
			{
				tama[i].IsDraw = FALSE;
			}

		}
	}

	if (TekiAddCnt < TekiAddCntMax)
	{
		TekiAddCnt++;
	}
	else
	{
		TekiAddCnt = 0;

		//敵を生成
		for (int i = 1; i < TEKI_MAX; i++)
		{
			if (teki[i].img.IsDraw == FALSE)
			{
				int Bunkatu = 8;	//画面の横分割

				if (Score < 10000)
				{
					teki[i] = teki_moto[0];
				}
				/*else if (Score <= 20000)
				{
					teki[i] = teki_moto[1];
				}
				else
				{
					teki[i] = teki_moto[GetRand(TEKI_KIND - 1)];
				}*/

				teki[i].img.x = GAME_WIDTH + 1;
				teki[i].img.y = GetRand(Bunkatu - 1) * GAME_HEIGHT / Bunkatu;

				teki[i].img.IsDraw = TRUE;	//描画する
				break;
			}
		}
	}

	//敵の処理
	for (int i = 0; i < TEKI_MAX; i++)
	{
		if (teki[i].img.IsDraw == TRUE)
		{
			teki[i].img.x -= 8;	//とりあえず左へ移動
			
			//敵の当たり判定の更新
			CollUpdateTeki(&teki[i]);

			//敵が左まで行ったら表示しない
			if (teki[i].img.x > GAME_WIDTH)
			{
				teki[i].img.IsDraw = FALSE;
			}

			//敵と自分の弾が当たった時
			for (int cnt = 0; cnt < TAMA_MAX; cnt++)
			{
				//描画されているとき
				if (tama[cnt].IsDraw == TRUE)
				{
					//当たり判定
					if (OnCollRect(teki[i].coll, tama[cnt].coll) == TRUE)
					{
						tama[cnt].IsDraw = FALSE;					 //弾の描画をしない
						teki[i].img.IsDraw = FALSE;					 //敵の描画をしない

						Score += 100;
					}

				}
			}
		}

	}

	return;
}

/// <summary>
/// 弾を飛ばす
/// </summary>
/// <param name="tama"></param>
VOID ShotTama(TAMA* tama, float deg)
{
	//弾を発射する
	tama->IsDraw = TRUE;

	//弾の位置を決める
	tama-> startX = player.img.x + (player.img.width  + tama->width / 2) + 4;
	tama->startY = player.img.y;

	//弾の角度
	tama->radius = deg;

	//弾の速度を変える
	tama->Speed = 6;

	//弾の半径
	tama->radius = 0.0f;

	//弾の当たり判定の更新
	CollUpdateTama(tama);
	
}

/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	
	for (int i = 0; i < 2; i++)
	{
		//描画
		DrawGraph(back[i].x, back[i].y, back[i].handle, TRUE);

		//DrawFormatString(0, GAME_HEIGHT - 80, GetColor(255, 25, 25), "back1[x:%4d/y:%4d] back2[x:%4d/y:%4d]", back[0].x, back[0].y, back[1].x, back[1].y);
		//画像が左まで行ったとき
		if (back[i].x < 0 -GAME_WIDTH)
		{
			back[i].x = back[i].width - 1;	
		}

		//画像を左に動かす
		back[i].x--;
	}

	//敵の描画
	for (int i = 0; i < TEKI_MAX; i++)
	{
		if (teki[i].img.IsDraw == TRUE)
		{
			DrawGraph(teki[i].img.x, teki[i].img.y, teki[i].img.handle, TRUE);
		}

		
		//当たり判定
		if (GAME_DEBUG == TRUE)
		{
			DrawBox(
				teki[i].coll.left, teki[i].coll.top, teki[i].coll.right, teki[i].coll.bottom,
				GetColor(0, 0, 255), FALSE);
		}
	}

	//プレイヤーの描画
	if (player.img.IsDraw == TRUE)
	{

		DrawGraph(player.img.x, player.img.y, player.img.handle, TRUE);

		//当たり判定
		if (GAME_DEBUG == TRUE)
		{
			DrawBox(
				player.coll.left, player.coll.top, player.coll.right, player.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}

	}
	// 
	//弾を飛ばす
	for (int i = 0; i < TAMA_MAX; i++)
	{
		//描画されているとき
		if (tama[i].IsDraw == TRUE)
		{
			DrawTama(&tama[i]);
			//当たり判定


			if (GAME_DEBUG == TRUE)
			{
				DrawBox(
					tama[i].coll.left, tama[i].coll.top, tama[i].coll.right, tama[i].coll.bottom,
					GetColor(255, 0, 0), FALSE);
			}
		}
	}

	//スコアの描画
	int old = GetFontSize();
	SetFontSize(40);
	DrawFormatString(0, 15, GetColor(255, 20, 25), "SCORE:%05d", Score);
	SetFontSize(old);

	//マウスの位置を描画
	MouseDraw();

	//DrawString(0, 0, "プレイ画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// エンド画面
/// </summary>
VOID End(VOID)
{
	EndProc();	//処理
	EndDraw();	//描画

	return;
}

/// <summary>
/// エンド画面の処理
/// </summary>
VOID EndProc(VOID)
{

	//音楽を流す処理
	if (CheckSoundMem(backmusic[2].handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(backmusic[2].handle, backmusic[2].playType);
	}

	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{

		//音楽を止める
		StopSoundMem(backmusic[2].handle);

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}

	return;
}

/// <summary>
/// エンド画面の描画
/// </summary>
VOID EndDraw(VOID)
{
	DrawGraph(Back[2].x, Back[2].y, Back[2].handle, TRUE);
	DrawGraph(Back[3].x, Back[3].y, Back[3].handle, TRUE);

	//DrawString(0, 0, "エンド画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 切り替え画面
/// </summary>
VOID Change(VOID)
{
	ChangeProc();	//処理
	ChangeDraw();	//描画

	return;
}

/// <summary>
/// 切り替え画面の処理
/// </summary>
VOID ChangeProc(VOID)
{

	//フェードイン
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMax)
		{
			fadeInCnt--;	//カウンタを減らす
		}
		else
		{
			//フェードイン処理が終わった

			fadeInCnt = fadeInCntInit;	//カウンタ初期化
			IsFadeIn = FALSE;			//フェードイン処理終了
		}
	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMax)
		{
			fadeOutCnt++;	//カウンタを増やす
		}
		else
		{
			//フェードアウト処理が終わった

			fadeOutCnt = fadeOutCntInit;	//カウンタ初期化
			IsFadeOut = FALSE;				//フェードアウト処理終了
		}
	}

	//切り替え処理終了
	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//フェードインしていない、フェードアウトもしていないとき
		GameScene = NextGameScene;	//次のシーンに切り替え
		OldGameScene = GameScene;	//以前のゲームシーン更新
	}


	return;
}

/// <summary>
/// 切り替え画面の描画
/// </summary>
/// 

VOID ChangeDraw(VOID)
{
	//以前のシーンを描画
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();	//タイトル画面の描画
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();		//プレイ画面の描画
		break;
	case GAME_SCENE_END:
		EndDraw();		//エンド画面の描画
		break;
	default:
		break;
	}

	//フェードイン
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntMax) * 255);
	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMax) * 255);
	}

	//四角を描画
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);

	//半透明終了
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//DrawString(0, 0, "切り替え画面", GetColor(0, 0, 0));
	return;
}


/// <summary>
/// 当たり判定の領域更新（プレイヤー）
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdatePlayer(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x + 10;					//当たり判定を微調整
	chara->coll.top = chara->img.y + 40;						//当たり判定を微調整

	chara->coll.right = chara->img.x + chara->img.width -10;		//当たり判定を微調整
	chara->coll.bottom = chara->img.y + chara->img.height -10;	//当たり判定を微調整

	return;
}

/// <summary>
/// 当たり判定の領域更新（敵）
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdateTeki(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;					//当たり判定を微調整
	chara->coll.top = chara->img.y;						//当たり判定を微調整

	chara->coll.right = chara->img.x + chara->img.width;		//当たり判定を微調整
	chara->coll.bottom = chara->img.y + chara->img.height;	//当たり判定を微調整

	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdate(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;
	chara->coll.top = chara->img.y;

	chara->coll.right = chara->img.x + chara->img.width;
	chara->coll.bottom = chara->img.y + chara->img.height;

	return;
}

/// <summary>
/// 当たり判定の領域更新(弾)
/// </summary>
/// <param name="tama">弾の構造体</param>
VOID CollUpdateTama(TAMA* tama)
{
	tama->coll.left = tama->x;
	tama->coll.top = tama->y;

	tama->coll.right = tama->x + tama->width;
	tama->coll.bottom = tama->y + tama->height;

	return;
}

/// <summary>
/// 矩形と矩形の当たり判定
/// </summary>
/// <param name="a">矩形A</param>
/// <param name="b">矩形B</param>
/// <returns>あたったらTRUE/あたらないならFALSE</returns>
BOOL OnCollRect(RECT a, RECT b)
{
	if (
		a.left < b.right &&		//　矩形Aの左辺X座標 < 矩形Bの右辺X座標　かつ
		a.right > b.left &&		//　矩形Aの右辺X座標 > 矩形Bの左辺X座標　かつ
		a.top  < b.bottom &&	//　矩形Aの上辺Y座標 < 矩形Bの下辺Y座標　かつ
		a.bottom > b.top		//　矩形Aの下辺Y座標 > 矩形Bの上辺Y座標
		)
	{
		//あたっているとき
		return TRUE;
	}
	else
	{
		//あたっていないとき
		return FALSE;
	}
}