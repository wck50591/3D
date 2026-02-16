// ========================================================================
//    Name : show score (score.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/08
// ========================================================================
#ifndef SCORE_H
#define SCORE_H

static constexpr int SCORE_MAX = 1;

struct Score {
	unsigned int score, viewScore, counterStop;
	float x, y;
	int digit;
	float alpha;
	int texId;
};

void Score_Initialize();
void Score_Finalize();
void Score_Update();
void Score_Draw();

void Score_AddScore(int cnt, int score);
void Score_Reset();
void Score_ShowScore(int cnt);
void Score_ChangeAddSpeed(int speed);

#endif // !SCORE_H
