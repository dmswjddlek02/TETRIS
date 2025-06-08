#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>

/* 타이머  */
#define CCHAR 0
#ifdef CTIME
#undef CTIME
#endif
#define CTIME 1

/* 왼쪽, 오른쪽, 아래, 회전  */
#define LEFT 0
#define RIGHT 1
#define DOWN 2
#define ROTATE 3


/* 블록 모양 */
#define I_BLOCK 1
#define	T_BLOCK 2
#define S_BLOCK 3
#define Z_BLOCK 4
#define L_BLOCK 5
#define J_BLOCK 6
#define O_BLOCK 7

#define TEST_BLOCK 9 // 테스트 블록

/* 게임 시작, 게임 종료 */
#define GAME_START 0
#define GAME_END 1

/*

 * 블록 모양(I, T, S, Z, L, J, O) 
 * 4*4 배열의 2차원 배열
 * 모든 블록의 모양을 표시
 *
 * 블록의 모양을 표시
 * 왼쪽, 오른쪽, 아래, 회전 
 * 4*4 배열의 2차원 배열
 * 모든 블록의 모양을 표시
 *
 * 4*4*4 배열의 3차원 배열
 */


char i_block[4][4][4] = {
    {  // 첫 번째 회전 상태
        {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },
    {  // 두 번째 회전 상태
        {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1} },
    {  // 세 번째 회전 상태
        {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1} },
    {  // 네 번째 회전 상태
        {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0} }
};

char t_block[4][4][4] =
	{
			{{2, 0, 0, 0},   {2, 2, 0, 0},   {2, 0, 0, 0},   {0, 0, 0, 0}},
			{{2, 2, 2, 0},   {0, 2, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 2, 0},   {0, 2, 2, 0},   {0, 0, 2, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {0, 2, 0, 0},   {2, 2, 2, 0},   {0, 0, 0, 0}}
	};


char s_block[4][4][4] =
	{
			{{3, 0, 0, 0},   {3, 3, 0, 0},   {0, 3, 0, 0},   {0, 0, 0, 0}},
			{{0, 3, 3, 0},   {3, 3, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 3, 0, 0},   {0, 3, 3, 0},   {0, 0, 3, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {0, 3, 3, 0},   {3, 3, 0, 0},   {0, 0, 0, 0}}

	};

char z_block[4][4][4] =
	{
			{{0, 4, 0, 0},   {4, 4, 0, 0},   {4, 0, 0, 0},   {0, 0, 0, 0}},
			{{4, 4, 0, 0},   {0, 4, 4, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 4, 0},   {0, 4, 4, 0},   {0, 4, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {4, 4, 0, 0},   {0, 4, 4, 0},   {0, 0, 0, 0}}

	};

char l_block[4][4][4] =
	{
			{{5, 0, 0, 0},   {5, 0, 0, 0},   {5, 5, 0, 0},   {0, 0, 0, 0}},
			{{5, 5, 5, 0},   {5, 0, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 5, 5, 0},   {0, 0, 5, 0},   {0, 0, 5, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {0, 0, 5, 0},   {5, 5, 5, 0},   {0, 0, 0, 0}}

	};

char j_block[4][4][4] =
	{
			{{0, 6, 0, 0},   {0, 6, 0, 0},   {6, 6, 0, 0},   {0, 0, 0, 0}},
			{{6, 0, 0, 0},   {6, 6, 6, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{6, 6, 0, 0},   {6, 0, 0, 0},   {6, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {0, 0, 6, 0},   {6, 6, 6, 0},   {0, 0, 0, 0}}
	};


char o_block[4][4][4] =
	{
			{{7, 7, 0, 0},   {7, 7, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{7, 7, 0, 0},   {7, 7, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{7, 7, 0, 0},   {7, 7, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{7, 7, 0, 0},   {7, 7, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}}

	};


char test_block[1]={9}; //테스트 블록록

/* 테트리스 판을 2차원 배열로 표현
 * 2차원 배열의 2차원 배열
 * 모든 블록의 모양을 표시
 *
 * 20*8 배열
 * 모든 블록의 모양을 표시
 * 모든 블록의 모양을 표시*/

char tetris_table[20][10]={
	{8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 0, 0, 0, 0, 0, 0, 0, 0, 8},
	{8, 8, 8, 8, 8, 8, 8, 8, 8, 8}
};

/* 게임 종료 땨마다
 * 이름과 득점수와 
 * 날짜와 시간과 순위를 저장
 * */
static struct result
{
	char name[30];
	long point;
	int year;
	int month;
	int day;
	int hour;
	int min;
	int rank;
}temp_result;

int block_number = 0;  /*블록 번호*/
int next_block_number = 0; /*다음 블록 번호 */


int block_state = 0; /*블록 상태, 왼쪽, 오른쪽, 아래, 회전  */

int x = 4, y = 1; /*시작 블록의 위치*/

int game = GAME_END; /*게임 시작, 게임 종료*/
int best_point = 0; /* 최고 점수*/

long point = 0; /* 현재 점수*/


int display_menu(void); /* 메뉴 표시*/


/* 메뉴 표시*/
int display_menu(void)
{
	// 
	int menu = 0;

	while(1)
	{
		system("clear");
		printf("\n\n\t\t\t\tText Tetris");
		printf("\n\t\t\t============================");
		printf("\n\t\t\t\tGAME MENU\t\n");
		printf("\n\t\t\t============================");
		printf("\n\t\t\t   1) Game Start");
		printf("\n\t\t\t   2) Search history");
		printf("\n\t\t\t   3) Record Output");
		printf("\n\t\t\t   4) QUIT");
		printf("\n\t\t\t============================");
		printf("\n\t\t\t\t\t SELECT : ");
		scanf("%d",&menu);
		if(menu < 1 || menu > 4)
		{
			continue;
		}
		else
		{
			return menu;
		}
	}
	return 0;
}

int check_collision(char block[4][4], int x, int y) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (block[i][j] != 0 && tetris_table[y + i][x + j] != 0)
                return 1; // 충돌 발생
    return 0;
}


/*게임 실행 화면*/
int game_start(void){
    srand(time(NULL));
	int randum = rand() % 7 + 1;
    char (*block)[4][4];

    switch (randum) {
        case I_BLOCK: block = i_block; break;
        case T_BLOCK: block = t_block; break;
        case S_BLOCK: block = s_block; break;
        case Z_BLOCK: block = z_block; break;
        case L_BLOCK: block = l_block; break;
        case J_BLOCK: block = j_block; break;
        case O_BLOCK: block = o_block; break;
    }

	int rotation = 0;
	x = 4;
	y = 0;

	while (1) {
		system("clear");

		// 블럭 + 배경 출력
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 10; j++) {
				if (i >= y && i < y + 4 && j >= x && j < x + 4 && block[rotation][i - y][j - x] != 0) {
					printf("%d", block[rotation][i - y][j - x]);
				} 
                else {
					printf("%d", tetris_table[i][j]);
				}
			}
			printf("\n");
		}

		usleep(500000); // 0.5초마다 낙하

		// 아래로 한 칸 내릴 수 있는지 확인
		if (check_collision(block[rotation], x, y + 1)) {
            y++;
			// 블럭을 tetris_table에 고정
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					if (block[rotation][i][j] != 0)
						tetris_table[y + i][x + j] = block[rotation][i][j];
			break; // 다음 블럭으로
		}

		y++; // 한 칸 아래로 내림
	}

	return 0;
}




/// 테트리스 게임 메인 함수
/// 메뉴를 표시하고 사용자의 선택에 따라 게임을 시작하거나 결과를 검색하거나 종료합니다.
/// @param  
/// @return 
int main(void)
{
	int menu = 1;

	while(menu)
	{
		menu = display_menu();

		if(menu == 1)
		{
			game = GAME_START;
			menu = game_start();
		}
		else if(menu == 2)
		{
			//search_result();
		}
		else if(menu == 3)
		{
			//print_result();
		}
		else if(menu == 4)
		{
			exit(0);
		}
	}

	return 0;
}