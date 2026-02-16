// ========================================================================
//    Name : show score (score.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/08
// ========================================================================
#include "Score.h"
#include "direct3d.h"
#include "Texture.h"
#include "sprite.h"
#include <algorithm>

static Score g_Scores[SCORE_MAX];
static float g_ScoresX[SCORE_MAX] = {1600.0f};
static float g_ScoresY[SCORE_MAX] = {220.0f};
static int g_ScoresDigit[SCORE_MAX] = {5};

static constexpr int SCORE_DOT_WIDTH = 35;
static constexpr int SCORE_FONT_WIDTH = 39;
static constexpr int SCORE_FONT_HEIGHT = 51;

static int g_ScoreAddSpeed = 100;

void Score_Initialize()
{

	int cnt = 0;
	for (Score& s : g_Scores) {
		s.score = 0;
		s.viewScore = 0;
		s.counterStop = 1;
		s.x = g_ScoresX[cnt];
		s.y = g_ScoresY[cnt];
		s.digit = g_ScoresDigit[cnt];
		s.alpha = 1.0f;
		s.texId = Texture_Load(L"resource/texture/scoreText2.png", TRUE);

		for (int i = 0; i < s.digit; i++) {
			s.counterStop *= 10;
		}
		s.counterStop--;
		cnt++;
	}
}

void Score_Finalize()
{

}

void Score_Update()
{
	for (Score& s : g_Scores) {
		s.viewScore = std::min(s.viewScore + g_ScoreAddSpeed, s.score);
	}
}

void Score_Draw()
{
    Score& s = g_Scores[0];

    const float margin = 20.0f;

    const int digi_w = SCORE_FONT_WIDTH;      // 39
    const int digi_h = SCORE_FONT_HEIGHT;     // 51

    // あなたのフォーマット: digit=5 -> 000.00
    const int intDigits = s.digit - 2; // 3
    const int fracDigits = 2;

    // "." は 10番目(0-9の次)
    const int DOT_SRC_X = digi_w * 10;        // 390
    const int DOT_SRC_W = 417 - DOT_SRC_X;     // 27  (画像が417px幅なので)
    const int DOT_DRAW_W = SCORE_DOT_WIDTH;    // 見た目の幅(16でOK)

    // 右上配置（描画幅で計算）
    int totalWidth = intDigits * digi_w + DOT_DRAW_W + fracDigits * digi_w;
    s.x = (float)Direct3D_GetBackBufferWidth() - margin - totalWidth;
    s.y = margin;

    unsigned int value = std::min(s.viewScore, s.counterStop);

    // value を 2桁小数として扱う：12345 -> 123.45
    unsigned int intPart = value / 100;
    unsigned int fracPart = value % 100;

    float x = s.x;

    // -------- 整数部（0埋め） --------
    for (int i = 0; i < intDigits; i++) {
        int n = intPart % 10;
        intPart /= 10;

        Sprite_Draw(s.texId,
            x + (intDigits - 1 - i) * digi_w, s.y,
            digi_w * n, 0,
            digi_w, digi_h,
            { 1,1,1, s.alpha });
    }
    x += intDigits * digi_w;

    // -------- "." --------
    Sprite_Draw(s.texId,
        x, s.y,
        DOT_SRC_X, 0,
        DOT_SRC_W, digi_h,     // ← ソースは27px
        { 1,1,1, s.alpha });

    x += DOT_DRAW_W;            // ← 次の桁へ（見た目の幅16px）

    // -------- 小数部（2桁） --------
    int tens = (int)(fracPart / 10);
    int ones = (int)(fracPart % 10);

    Sprite_Draw(s.texId,
        x, s.y,
        digi_w * tens, 0,
        digi_w, digi_h,
        { 1,1,1, s.alpha });

    Sprite_Draw(s.texId,
        x + digi_w, s.y,
        digi_w * ones, 0,
        digi_w, digi_h,
        { 1,1,1, s.alpha });
}

void Score_AddScore(int cnt, int score)
{
	g_Scores[cnt].viewScore = g_Scores[cnt].score;
	g_Scores[cnt].score += score;
}

void Score_Reset()
{
	for (Score& s : g_Scores) {
		s.score = 0;
	}
}

void Score_ShowScore(int cnt) {
	g_Scores[cnt].alpha = 1.0f;
}

void Score_ChangeAddSpeed(int speed) {
	g_ScoreAddSpeed = speed;
}