/**
 * By: Michajlo Matijkiw
 * Simple snake game
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

#include "list.h"

#define BOARD_WIDTH 30
#define BOARD_HEIGHT 20
#define SNAKE_START_LENGTH 5

#define DIR_UP 'A'
#define DIR_DOWN 'B'
#define DIR_RIGHT 'C'
#define DIR_LEFT 'D'

#define CLS()   printf("\033[2J");
#define HOME()  printf("\033[H");
#define GOTO(x,y)   printf("\033[%d;%dH",(y),(x))

#ifdef __DEBUG__
    #define DEBUG(m) printf("%s",(m))
#else
    #define DEBUG(m) 
#endif

typedef struct {
    int x, y;
} snake_part;

LIST snake;
char snake_direction;
char next_snake_direction;
int score;
int game_over;

int got_apple;
int apple_x;
int apple_y;

void make_snake() {
    int i, x, y;
    list_init(&snake);
    
    x = BOARD_WIDTH/2;
    y = BOARD_HEIGHT/2;

    for (i=0; i<SNAKE_START_LENGTH; i++) {
        snake_part *part = (snake_part *) malloc(sizeof(snake_part));
        part->x = x;
        part->y = y;
        list_prepend(&snake, part);
        
        x--;
    }
}

void *input_thread(void *dummy) {
    while(!game_over) {
        char c = getchar();
        if ((c == DIR_UP && snake_direction != DIR_DOWN) ||
            (c == DIR_DOWN && snake_direction != DIR_UP) ||
            (c == DIR_LEFT && snake_direction != DIR_RIGHT) ||
            (c == DIR_RIGHT && snake_direction != DIR_LEFT)) {
            next_snake_direction = c;
        }
    }
    return NULL;
}

void end_game() {
    GOTO(0, BOARD_HEIGHT+3);
    printf("Game over, press any key to exit...\n");
    fflush(stdout);
    game_over = 1;
    pthread_exit(NULL);
}

int is_snake_at(int x, int y) {
    NODE *comp = snake.firstNode;
    for (; comp; comp=comp->next) {
        snake_part *part = (snake_part *)comp->data;
        if (part->x == x || part->y == y) {
            return 1;
        }
    }
    return 0;
}

void put_apple() {
    do {
        apple_x = (rand()%(BOARD_WIDTH-1)) + 1;
        apple_y = (rand()%(BOARD_HEIGHT-1)) + 1;
    } while (is_snake_at(apple_x, apple_y));
}

void collision_check(void *spart) {
    snake_part *me = (snake_part *)spart;
    NODE *node = snake.firstNode;
    for ( ; node; node = node->next ) {
        snake_part *comp = (snake_part *)node->data;
        if (comp == me) {
            continue;
        }
        if (comp->x == me->x && comp->y == me->y) {
            end_game();
        }
    }
}

void draw_board() {
    int i;
    HOME();
    for (i=0;i<BOARD_WIDTH+1;i++) {
        printf("X");
    }

    GOTO(0, BOARD_HEIGHT+1);
    for (i=0;i<BOARD_WIDTH+1;i++) {
        printf("X");
    }

    for (i=1;i<BOARD_HEIGHT+1;i++) {
        GOTO(0, i);
        printf("X");
        GOTO(BOARD_WIDTH+1, i);
        printf("X");
    }
}

void draw_part(void *spart) {
    snake_part *part = (snake_part *)spart;
    GOTO(part->x+1, part->y+1);
    printf("#");
}

void *snake_thread(void *dummy) {
    while(1) {
        // first advance snake parts, delete tail, add head
        if (!got_apple) {
            snake_part *tail = (snake_part *)list_front(&snake);
            free(tail);
        }

        got_apple = 0;

        snake_direction = next_snake_direction;

        snake_part *head = (snake_part *)snake.lastNode->data;
        snake_part *new_head = (snake_part *)malloc(sizeof(snake_part));
        switch (snake_direction) {
            case DIR_UP:
                new_head->x = head->x;
                new_head->y = head->y-1;
                break;
            case DIR_DOWN:
                new_head->x = head->x;
                new_head->y = head->y+1;
                break;
            case DIR_LEFT:
                new_head->x = head->x-1;
                new_head->y = head->y;
                break;
            case DIR_RIGHT:
                new_head->x = head->x+1;
                new_head->y = head->y;
                break;
        }
        list_append(&snake, new_head);
        head = new_head;
        
        // check new head doesnt hit any walls
        if (head->x < 1 || head->x >= BOARD_WIDTH || head->y < 1 || head->y >= BOARD_HEIGHT) {
            end_game();
        }
       
        // check if we hit ourself...
        list_map(&snake, collision_check);

        // apple logic
        if (head->x == apple_x && head->y == apple_y) {
            got_apple = 1;
            score++;
            put_apple();
        }

        // then draw
        CLS();
        draw_board();
        list_map(&snake, draw_part);
        GOTO(apple_x+1, apple_y+1);
        printf("0");
        GOTO(0, BOARD_HEIGHT+2);
        printf("Score: %d", score);
        HOME();
        fflush(stdout);

        usleep(100000);
    }
}

int main() {
    struct termios old_settings;
    struct termios new_settings;
    pthread_t s_thread, i_thread;

    srand(time(NULL));

    tcgetattr(0, &old_settings);

    new_settings = old_settings;

    new_settings.c_lflag &= (~ICANON);
    new_settings.c_lflag &= (~ECHO);
    new_settings.c_cc[VMIN] = 1;
 
    tcsetattr(0, TCSANOW, &new_settings);   

    score = 0;
    game_over = 0;
    snake_direction = DIR_RIGHT;
    next_snake_direction = DIR_RIGHT;
    make_snake();
    put_apple();

    pthread_create(&s_thread, NULL, snake_thread, NULL);
    pthread_create(&i_thread, NULL, input_thread, NULL);

    pthread_join(i_thread, NULL);
    pthread_join(s_thread, NULL);

    tcsetattr(0, TCSANOW, &old_settings);

    return 0;
}
