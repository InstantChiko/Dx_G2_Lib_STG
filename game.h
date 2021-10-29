#pragma once


//ゲーム全体のヘッダファイル

//ヘッダーファイルの読み込み
#include "DxLib.h"	//DxLibを使う時は必要

//マクロ定義
#define GAME_TITLE	"ゲームタイトル"	//ゲームタイトル（ゲームタイトル）
#define GAME_WIDTH	960					//ゲーム画面の幅（ウィドス）
#define GAME_HEIGHT	540					//ゲーム画面の高さ（ハイト）
#define GAME_COLOR	32					//ゲームの色域

#define GAME_ICON_ID	333				//ゲームのICONのID

#define GAME_WINDOW_BAR 0				//ウィンドウバーの種類

#define GAME_DEBUG FALSE					//デバッグモード

//列挙型
enum GAME_SCENE
{
	GAME_SCENE_TITLE,
	GAME_SCENE_TITLE2,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
	GAME_SCENE_CHANGE
};//ゲームのシーン