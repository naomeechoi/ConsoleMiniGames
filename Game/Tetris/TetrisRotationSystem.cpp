#include "TetrisRotationSystem.h"

// JLSTZ 전용 정교한 킥 테이블 (SRS 표준 근거)
// [from_rotation][test_index][x, y]
const int COMMON_KICK[5][2] = {
    { 0,  0}, // 1순위: 제자리
    {-1,  0}, // 2순위: 왼쪽 한 칸 (우측 벽 충돌 시)
    { 1,  0}, // 3순위: 오른쪽 한 칸 (좌측 벽 충돌 시)
    { 0, -1}, // 4순위: 위쪽 한 칸 (바닥 충돌 시)
    { 0,  1}  // 5순위: 아래쪽 한 칸
};

const int I_KICK_SIMPLE[9][2] = {
    { 0,  0},
    {-1,  0}, { 1,  0}, // 1칸씩
    {-2,  0}, { 2,  0}, // 2칸씩 (I 미노 핵심)
    { 0, -1}, { 0, -2}, // 위로 (바닥 탈출)
    {-1, -1}, { 1, -1}  // 대각선 위
};

const int offsetsForRotation = 5;