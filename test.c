#include <stdio.h>
#include <stdint.h>

#define BOARDLEN 81
#define BOARDS 10

#define COORD_1TOX(offset, width) \
    ( (offset) % (width) )

#define COORD_1TOY(offset, width) \
    ( (offset) / (width) )

#define CELL_2DIDX(row,col) \
    ( (row) * (9) + (col) )

#define GET_CELL(board, index) \
    ( ( (board) [ (index) / 2 ] >> ( ( (index) % 2 ) * 4) ) & 0x0F )

#define SET_CELL(board, index, value) \
    ( (board) [ (index) / 2 ] = ( (board) [ (index) / 2 ] & ~(0x0F << ( ( (index) % 2 ) * 4) ) ) | ( (value) << ( ( (index) % 2 ) * 4) ) )

static void
read_sudoku_boards(FILE *file, uint8_t boards[BOARDS][BOARDLEN * BOARDLEN])
{
    char buffer[10];
    int board_index = 0, row_index = 0;

    while (fread(buffer, 1, 10, file) > 0) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            boards[board_index][row_index * BOARD_SIZE + i] = atoi(&buffer[i]);
        }
        row_index++;
        if (row_index == BOARD_SIZE) {
            board_index++;
            row_index = 0;
        }
    }
}

static inline const uint8_t
board_next_empty(uint8_t board[(BOARDLEN + 1) / 2], uint8_t idx)
{
    for (uint8_t i = idx; i < BOARDLEN; ++i)
        if (GET_CELL(board, i) == 0)
            return i;
    
    return BOARDLEN;
}

static inline int
board_cell_valid(uint8_t board[(BOARDLEN + 1) / 2], uint8_t idx, uint8_t value)
{
    uint8_t row = COORD_1TOY(idx, 9);
    uint8_t col = COORD_1TOX(idx, 9);
    uint8_t unit_row = row / 3 * 3;
    uint8_t unit_col = col / 3 * 3;

    for (uint8_t i = 0; i < 9; ++i)
        if (GET_CELL(board, CELL_2DIDX(row, i)) == value ||
            GET_CELL(board, CELL_2DIDX(i, col)) == value ||
            GET_CELL(board, CELL_2DIDX(unit_row + i / 3, unit_col + i % 3)) == value)
            return 0;

    return 1;
}

static int
board_backtrack(uint8_t board[(BOARDLEN + 1) / 2], uint8_t idx)
{
    if (idx == BOARDLEN)
        return 1;

    if (GET_CELL(board, idx) != 0)
        return board_backtrack(board, board_next_empty(board, idx+1));

    for (uint8_t i = 1; i <= 9; ++i)
        if (board_cell_valid(board, idx, i))
        {
            SET_CELL(board, idx, i);

            if (board_backtrack(board, board_next_empty(board, idx+1)))
                return 1;

            SET_CELL(board, idx, 0);
        }

    return 0;
}

static void
board_fill(uint8_t board[(BOARDLEN + 1) / 2], uint8_t const input[BOARDLEN])
{
    for (uint8_t i = 0; i < BOARDLEN; ++i)
        SET_CELL(board, i, input[i]);
}

int
main(int argc, char **argv)
{
    uint8_t board[(BOARDLEN + 1) / 2] = {0};

    // random puzzle i found
    uint8_t const initial_board[BOARDLEN] = {
        5, 3, 0, 0, 7, 0, 0, 0, 0,
        6, 0, 0, 1, 9, 5, 0, 0, 0,
        0, 9, 8, 0, 0, 0, 0, 6, 0,
        8, 0, 0, 0, 6, 0, 0, 0, 3,
        4, 0, 0, 8, 0, 3, 0, 0, 1,
        7, 0, 0, 0, 2, 0, 0, 0, 6,
        0, 6, 0, 0, 0, 0, 2, 8, 0,
        0, 0, 0, 4, 1, 9, 0, 0, 5,
        0, 0, 0, 0, 8, 0, 0, 7, 9
    };

    board_fill(board, initial_board);

    if (board_backtrack(board, board_next_empty(board, 0)))
        for (uint8_t i = 0; i < BOARDLEN; ++i)
        {
            printf("%d ", GET_CELL(board, i));
            if (i % 9 == 8)
                puts("");
        }

    else
        puts("No solution found.");

    return 0;
}
