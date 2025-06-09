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

/*터미널 모드 설정*/
struct termios original_termios;

void set_input_mode() {
	struct termios new_termios;
	tcgetattr(STDIN_FILENO, &original_termios);
	new_termios = original_termios;
	new_termios.c_lflag &= ~(ICANON | ECHO);  // 비정규 모드 + 에코 제거
	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void reset_input_mode() {
	tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

// 키 입력 여부 확인
int key_pressed() {
	struct timeval tv = {0L, 0L};
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	return select(1, &fds, NULL, NULL, &tv);
}

// 키 하나 읽기
char read_key() {
	char ch;
	if (read(STDIN_FILENO, &ch, 1) > 0)
		return ch;
	return 0;
}


int display_menu(void); /* 메뉴 표시*/


/* 메뉴 표시*/
int display_menu(void)
{

	int menu = 0;
    reset_input_mode();
    int ch = 0;

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
		ch = getchar();
		if (ch >= '1' && ch <= '4') {
			break;
		}
	}

	set_input_mode();  // 다시 비정규 모드
	return ch - '0';
}

//충돌검사
int check_collision(char block[4][4], int x, int y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[i][j] != 0) {
                int py = y + i;
                int px = x + j;

                // 범위 검사 먼저
                if (py < 0 || py >= 20 || px < 0 || px >= 10)
                    return 1;

                // 💥 0이 아니면 충돌!
                if (tetris_table[py][px] != 0)
                    return 1;
            }
        }
    }
    return 0;
}

//게임오버버 
void game_over() {
    system("clear");

    // 이름 입력
    printf("\n\n\t\t\t  ===== GAME OVER =====\n\n");
    printf("\t\t\t   최종 점수: %ld\n", point);

    printf("\t\t\t   이름을 입력하세요: ");
    scanf("%s", temp_result.name);

    // 점수 저장
    temp_result.point = point;

    // 시간 저장
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    temp_result.year = t->tm_year + 1900;
    temp_result.month = t->tm_mon + 1;
    temp_result.day = t->tm_mday;
    temp_result.hour = t->tm_hour;
    temp_result.min = t->tm_min;

    // 순위는 나중에 정렬할 때 계산 (현재는 0으로 초기화)
    temp_result.rank = 0;

    printf("\n\t\t\t   결과가 저장되었습니다.\n");

	//테스트용
	printf("\n이름: %s, 점수: %ld, 날짜: %04d-%02d-%02d %02d:%02d\n",
    temp_result.name,
    temp_result.point,
    temp_result.year,
    temp_result.month,
    temp_result.day,
    temp_result.hour,
    temp_result.min);

    printf("\t\t\t   아무 키나 눌러 메뉴로...\n");
    getchar(); // 남은 개행 제거
    getchar(); // 입력 대기
}


//블록 삭제
void clear_lines() {
    int lines_cleared = 0;

    for (int i = 18; i >= 1; i--) {
        int full = 1;
        for (int j = 1; j <= 8; j++) {
            if (tetris_table[i][j] == 0) {
                full = 0;
                break;
            }
        }

        if (full) {
            // 한 줄 삭제
            for (int k = i; k >= 1; k--) {
                for (int j = 1; j <= 8; j++) {
                    tetris_table[k][j] = tetris_table[k - 1][j];
                }
            }

            // 맨 윗 줄 초기화
            for (int j = 1; j <= 8; j++) {
                tetris_table[0][j] = 0;
            }

            lines_cleared++;  // 삭제한 줄 수 누적
            i++; // 현재 줄 다시 확인
        }
    }

    // 점수 추가 (보너스 포함)
    switch (lines_cleared) {
        case 1: point += 100; break;
        case 2: point += 300; break;
        case 3: point += 500; break;
        case 4: point += 800; break;
        default: break;
    }
}




/*게임 실행 화면*/
int game_start(void){
    
    srand(time(NULL));
	while(1){
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

	x = 4;
	y = 1;
	block_state = 0;

	// 블럭 생성 직후 충돌 → 게임 오버
if (check_collision(block[block_state], x, y)) {
    game_over();
    return 0;
}


	while (1) {

        if (key_pressed()) {
	        char ch = read_key();
	        if (ch == 'i' || ch == 'I') {
		        int next_rotation = (block_state + 1) % 4;
		        if (!check_collision(block[next_rotation], x, y)) {
			        block_state = next_rotation;
		        }
	        }
	    else if (ch == 'j' || ch == 'J') {
		    if (!check_collision(block[block_state], x - 1, y)) {
			    x--;
		    }
	    }
	    else if (ch == 'l' || ch == 'L') {
		    if (!check_collision(block[block_state], x + 1, y)) {
			    x++;
		    }
	    }
	    else if (ch == 'k' || ch == 'K') {
		    if (!check_collision(block[block_state], x, y + 1)) {
			    y++;
		    }
	    }
	    else if (ch == 'a' || ch == 'A') {
		    while (!check_collision(block[block_state], x, y + 1)) {
			    y++;
		    }
	    }
	    else if (ch == 'p' || ch == 'P') {
		    printf("게임을 종료합니다.\n");
		    exit(0);
	    }
    }

		system("clear");

		// 블럭 + 배경 출력
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 10; j++) {
				if (i >= y && i < y + 4 && j >= x && j < x + 4 && block[block_state][i - y][j - x] != 0) {
					printf("%d", block[block_state][i - y][j - x]);
				} 
                else {
					printf("%d", tetris_table[i][j]);
				}
			}
			printf("\n");
		}

		usleep(500000); // 0.5초마다 낙하

		// 아래로 한 칸 내릴 수 있는지 확인
		if (check_collision(block[block_state], x, y + 1)) {
    // 💥 충돌했으면 지금 위치에 고정하고 break!
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[block_state][i][j] != 0) {
                int py = y + i;
                int px = x + j;
                if (py >= 0 && py < 20 && px >= 0 && px < 10) {
                    tetris_table[py][px] = block[block_state][i][j];
                }
            }
        }
    }

	clear_lines();
    break;
}

y++; // 충돌 안 했을 때만 한 칸 내림
	}
	}
	
	return 0;
}




/// 테트리스 게임 메인 함수
/// 메뉴를 표시하고 사용자의 선택에 따라 게임을 시작하거나 결과를 검색하거나 종료합니다.
/// @param  
/// @return 
int main(void)
{
    set_input_mode(); // 터미널 설정

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

    reset_input_mode(); // 터미널 설정정
	return 0;
}