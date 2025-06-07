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
#include <fcntl.h>

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
#define I_BLOCK 0
#define	T_BLOCK 1
#define S_BLOCK 2
#define Z_BLOCK 3
#define L_BLOCK 4
#define J_BLOCK 5
#define O_BLOCK 6

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
			{{1, 0, 0, 0},   {1, 1, 0, 0},   {1, 0, 0, 0},   {0, 0, 0, 0}},
			{{1, 1, 1, 0},   {0, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 1, 0},   {0, 1, 1, 0},   {0, 0, 1, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {0, 1, 0, 0},   {1, 1, 1, 0},   {0, 0, 0, 0}}
	};


char s_block[4][4][4] =
	{
			{{1, 0, 0, 0},   {1, 1, 0, 0},   {0, 1, 0, 0},   {0, 0, 0, 0}},
			{{0, 1, 1, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 1, 0, 0},   {0, 1, 1, 0},   {0, 0, 1, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {0, 1, 1, 0},   {1, 1, 0, 0},   {0, 0, 0, 0}}

	};

char z_block[4][4][4] =
	{
			{{0, 1, 0, 0},   {1, 1, 0, 0},   {1, 0, 0, 0},   {0, 0, 0, 0}},
			{{1, 1, 0, 0},   {0, 1, 1, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 1, 0},   {0, 1, 1, 0},   {0, 1, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {1, 1, 0, 0},   {0, 1, 1, 0},   {0, 0, 0, 0}}

	};

char l_block[4][4][4] =
	{
			{{1, 0, 0, 0},   {1, 0, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0}},
			{{1, 1, 1, 0},   {1, 0, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 1, 1, 0},   {0, 0, 1, 0},   {0, 0, 1, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {0, 0, 1, 0},   {1, 1, 1, 0},   {0, 0, 0, 0}}

	};

char j_block[4][4][4] =
	{
			{{0, 1, 0, 0},   {0, 1, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0}},
			{{1, 0, 0, 0},   {1, 1, 1, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{1, 1, 0, 0},   {1, 0, 0, 0},   {1, 0, 0, 0},   {0, 0, 0, 0}},
			{{0, 0, 0, 0},   {0, 0, 1, 0},   {1, 1, 1, 0},   {0, 0, 0, 0}}
	};


char o_block[4][4][4] =
	{
			{{1, 1, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{1, 1, 1, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{1, 1, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}},
			{{1, 1, 0, 0},   {1, 1, 0, 0},   {0, 0, 0, 0},   {0, 0, 0, 0}}

	};


/* 테트리스 판을 2차원 배열로 표현
 * 2차원 배열의 2차원 배열
 * 모든 블록의 모양을 표시
 *
 * 20*8 배열
 * 모든 블록의 모양을 표시
 * 모든 블록의 모양을 표시*/

char tetris_table[21][10];

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

int x = 3, y = 0; /*블록의 위치*/

int game = GAME_END; /*게임 시작, 게임 종료*/
int best_point = 0; /* 최고 점수*/

long point = 0; /* 현재 점수*/



int display_menu(void); /* 메뉴 표시*/

void draw_table(void);
char (*get_block_shape(int num))[4][4];
void draw_block(int block_num, int state, int x, int y);
int check_collision(int nx, int ny, int state, int block_num);
void fix_block(int block_num, int state, int x, int y);
void clear_lines(void);
int game_start(void);
int getch_noblock(void);
void handle_input(int key);

void print_result(void);
void recode_score(void);








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

/**
 * 전체 테트리스 맵을 그리는 함수
 * - 좌우 벽과 바닥 테두리 생성
 * - 현재 tetris_table 배열 값을 기반으로 출력
 */
void draw_table(void)
{
    int i, j;
    system("clear");
    for (i = 0; i < 21; i++) {
        for (j = 0; j < 10; j++) {
            if (i == 20) {
                printf("_");
            } else if (j == 0 || j == 9) {
                printf("|");
            } else if (tetris_table[i][j]) {
                printf("■");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

/**
 * 블록 유형에 맞는 4×4 배열 포인터를 반환
 */
char (*get_block_shape(int num))[4][4]
{
    switch (num) {
        case I_BLOCK: return i_block;
        case T_BLOCK: return t_block;
        case S_BLOCK: return s_block;
        case Z_BLOCK: return z_block;
        case L_BLOCK: return l_block;
        case J_BLOCK: return j_block;
        case O_BLOCK: return o_block;
        default:       return i_block;
    }
}

/**
 * 현재 블록을 그리는 함수
 * ANSI escape 코드를 사용하여 터미널 커서 위치 이동 후 '#' 출력
 */
void draw_block(int block_num, int state, int x, int y)
{
    char (*block)[4][4] = get_block_shape(block_num);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[state][i][j]) {
                // 터미널 커서 이동: 행 y+i+1, 열 x+j+1
                printf("\033[%d;%dH■", y + i + 1, x + j + 1);
            }
        }
    }
    fflush(stdout);
}

/**
 * 블록 충돌 검사 함수
 * 새로운 위치 (nx, ny) 에서 블록이 tetris_table 과 겹치면 1 리턴
 */
int check_collision(int nx, int ny, int state, int block_num)
{
    char (*block)[4][4] = get_block_shape(block_num);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[state][i][j]) {
                    int ty = ny + i;
                    int tx = nx + j;
                    // 경계 검사: 화면 밖이거나 바닥/벽과 충돌
                    if (ty < 0 || ty >= 21 || tx < 0 || tx >= 10 || tetris_table[ty][tx]) {
                        return 1;
                }
            }
        }
    }
    return 0;
}

/**
 * 블록 고정 함수
 * 현재 위치 (x, y) 에 블록을 tetris_table 에 기록
 */
void fix_block(int block_num, int state, int x, int y)
{
    char (*block)[4][4] = get_block_shape(block_num);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[state][i][j]) {
                tetris_table[y + i][x + j] = 1;
            }
        }
    }
}

/**
 * 라인 삭제 및 위로 당기기
 */
void clear_lines(void)
{
    for (int i = 0; i < 20; i++) {
        int full = 1;
        for (int j = 1; j < 9; j++) {
            if (!tetris_table[i][j]) {
                full = 0;
                break;
            }
        }
        if (full) {
            // 점수 추가: 한 라인 삭제 시 10점 부여
            point += 10;
            // 한 줄 삭제: 0 행에서 i-1 까지 아래로 이동
            for (int k = i; k > 0; k--) {
                for (int j = 1; j < 9; j++) {
                    tetris_table[k][j] = tetris_table[k-1][j];
                }
            }
            // 최상단 빈 칸 초기화
            for (int j = 1; j < 9; j++) {
                tetris_table[0][j] = 0;
            }
            // 삭제된 행 다시 검사
            i--;
        }
    }
}

/**
 * 검색 기능: 기록 파일에서 게임 결과를 읽어 출력
 */
void print_result(void)
{
    FILE *fp = fopen("record.dat", "rb");
    if (!fp) {
        printf("저장된 기록이 없습니다.\n");
        sleep(2);
        return;
    }
    struct result r;
    system("clear");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf("\n=== 기록 조회 ===\n");
    while (fread(&r, sizeof(r), 1, fp) == 1) {
        printf("이름: %s  점수: %ld  날짜: %04d-%02d-%02d %02d:%02d  순위: %d\n",
               r.name, r.point, r.year, r.month, r.day, r.hour, r.min, r.rank);
    }
    fclose(fp);
    printf("\n아무 키나 누르면 돌아갑니다...");
    getchar();
}

/**
 * 점수 기록 함수
 * - 기존 기록에서 랭크 계산
 * - 이름: "user" 고정
 * - 시간: 현재 로컬 시간
 * - point: 전역 변수 point 사용
 */
/**
 * 점수 기록 함수
 * - 기존 기록에서 랭크 계산
 * - 이름: "user" 고정
 * - 시간: 현재 로컬 시간
 * - point: 전역 변수 point 사용
 */
void recode_score(void)
{
    // 1) 기존 기록 읽어오기
    struct result *records = NULL;
    size_t count = 0, capacity = 0;
    struct result tmp;
    FILE *rf = fopen("record.dat", "rb");
    if (rf) {
        while (fread(&tmp, sizeof(tmp), 1, rf) == 1) {
            if (count == capacity) {
                capacity = capacity ? capacity * 2 : 4;
                records = realloc(records, capacity * sizeof(struct result));
            }
            records[count++] = tmp;
        }
        fclose(rf);
    }

    // 2) 새 기록 생성
    strncpy(temp_result.name, "user", sizeof(temp_result.name) - 1);
    temp_result.name[sizeof(temp_result.name) - 1] = '\0';
    temp_result.point = point;

    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    temp_result.year  = lt->tm_year + 1900;
    temp_result.month = lt->tm_mon + 1;
    temp_result.day   = lt->tm_mday;
    temp_result.hour  = lt->tm_hour;
    temp_result.min   = lt->tm_min;

    // 임시로 rank 0 설정 후 배열에 추가
    temp_result.rank = 0;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        records = realloc(records, capacity * sizeof(struct result));
    }
    records[count++] = temp_result;

    // 3) 점수 내림차순 정렬
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            if (records[j].point > records[i].point) {
                struct result swap = records[i];
                records[i] = records[j];
                records[j] = swap;
            }
        }
    }

    // 4) 랭크 재계산 (동점자 동일 랭크)
    for (size_t i = 0; i < count; i++) {
        if (i > 0 && records[i].point == records[i-1].point) {
            records[i].rank = records[i-1].rank;
        } else {
            records[i].rank = i + 1;
        }
    }

    // 5) 파일에 덮어쓰기
    FILE *wf = fopen("record.dat", "wb");
    if (wf) {
        fwrite(records, sizeof(struct result), count, wf);
        fclose(wf);
    }

    free(records);
}

/**
 * 비차단 입력 함수: 키가 눌리면 해당 문자 반환, 아니면 -1 반환
 */
#include <fcntl.h>
int getch_noblock(void)
{
    struct termios oldt, newt;
    int ch = -1;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) ch = c;
    fcntl(STDIN_FILENO, F_SETFL, flags);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

/**
 * 입력 처리 함수: 이동, 회전, 하드 드롭, 종료
 */
void handle_input(int key)
{
    switch (key) {
        case 'j': case 'J':
            if (!check_collision(x - 1, y, block_state, block_number)) x--;
            break;
        case 'l': case 'L':
            if (!check_collision(x + 1, y, block_state, block_number)) x++;
            break;
        case 'k': case 'K':
            if (!check_collision(x, y + 1, block_state, block_number)) y++;
            break;
        case 'i': case 'I': {
            int ns = (block_state + 1) % 4;
            if (!check_collision(x, y, ns, block_number)) block_state = ns;
            break;
        }
        case 'a': case 'A':
            while (!check_collision(x, y + 1, block_state, block_number)) {
                y++;
            }
            break;
        case 'p': case 'P':
            game = GAME_END;
            break;
        default:
            break;
    }
}

/// 게임 시작 핵심 로직 함수
int game_start(void)
{
    // 초기 게임 설정
    srand(time(NULL));
    block_number     = rand() % 7;
    next_block_number = rand() % 7;
    block_state      = 0;
    x                 = 3;
    y                 = 0;
    point             = 0;
    game              = GAME_START;
    // 커서 숨기기
    printf("\033[?25l");
    fflush(stdout);

    // 터미널 설정 저장 및 입력 에코/캐논 모드 비활성화
    struct termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw_termios = orig_termios;
    raw_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw_termios);

    // 테이블 초기화
    memset(tetris_table, 0, sizeof(tetris_table));
    // 테두리 칸 초기화 (바닥 및 좌우 벽)
    for (int i = 0; i < 21; i++) {
        tetris_table[i][0] = 1;
        tetris_table[i][9] = 1;
    }
    for (int j = 0; j < 10; j++) {
        tetris_table[20][j] = 1;
    }

    struct timeval last, current;
    gettimeofday(&last, NULL);
    const int drop_interval_ms = 500;

    // 게임 루프
    while (game == GAME_START) {
         // 1) 전체 맵 그리기
         draw_table();

        // 2) 현재 블록 그리기
        draw_block(block_number, block_state, x, y);

        // 3) 입력 비차단 처리 (추후 구현)
        int key = getch_noblock();
        if (key != -1) handle_input(key);

        // 자동 낙하
        gettimeofday(&current, NULL);
        int elapsed = (current.tv_sec - last.tv_sec) * 1000 + (current.tv_usec - last.tv_usec) / 1000;
        if (elapsed >= drop_interval_ms) {
            last = current;
            if (!check_collision(x,  y + 1, block_state, block_number)) {
                y++;
            } else {
                fix_block(block_number, block_state, x, y);
                clear_lines();
                block_number = next_block_number;
                next_block_number = rand() % 7;
                block_state = 0;
                x = 3; y = 0;
                if (check_collision(x, y, block_state, block_number)) {
                    game = GAME_END;
                }
            }
        }
        // 프레임 지연
        usleep(50000);
    }

    // 점수 기록
    recode_score();

    // 터미널 설정 복원
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    // 커서 다시 보이기
    printf("\033[?25h");
    fflush(stdout);
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
		else if (menu == 2) {
			//search_result();
		}
		else if(menu == 3)
		{
			print_result();
		}
		else if(menu == 4)
		{
			exit(0);
		}
	}

	return 0;
}
