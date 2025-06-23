#include <stdio.h>
#include <stdlib.h>

char board[3][3];

void init_board() {
    char c = '1';
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            board[i][j] = c++;
}

void print_board() {
    system("clear");
    printf("Tic-Tac-Toe\n\n");
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            printf(" %c ", board[i][j]);
            if (j<2) printf("|");
        }
        printf("\n");
        if (i<2) printf("---+---+---\n");
    }
    printf("\n");
}

int check_winner() {
    for (int i=0; i<3; i++) {
        if (board[i][0]==board[i][1] && board[i][1]==board[i][2]) return 1;
        if (board[0][i]==board[1][i] && board[1][i]==board[2][i]) return 1;
    }
    if (board[0][0]==board[1][1] && board[1][1]==board[2][2]) return 1;
    if (board[0][2]==board[1][1] && board[1][1]==board[2][0]) return 1;
    return 0;
}

void play_game() {
    init_board();
    char player = 'X';
    int move, row, col;
    for (int turn=0; turn<9; turn++) {
        print_board();
        printf("Turno del jugador %c. Ingresa número de casilla: ", player);
        scanf("%d", &move);
        move--;
        row = move / 3;
        col = move % 3;
        if (board[row][col]=='X' || board[row][col]=='O') {
            printf("Casilla ocupada, intenta otra.\n");
            turn--;
            continue;
        }
        board[row][col] = player;
        if (check_winner()) {
            print_board();
            printf("¡Ganó el jugador %c!\n", player);
            return;
        }
        player = (player == 'X') ? 'O' : 'X';
    }
    print_board();
    printf("Empate.\n");
}
