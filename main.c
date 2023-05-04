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

    for (uint8_t val = 1; val <= 9; ++val)
        if (board_cell_valid(board, idx, val))
        {
            SET_CELL(board, idx, val);

            if (board_backtrack(board, board_next_empty(board, idx+1)))
                return 1;

            SET_CELL(board, idx, 0);
        }

    return 0;
}

void
board_fill(uint8_t board[(BOARDLEN + 1) / 2], uint8_t const input[BOARDLEN])
{
    for (uint8_t i = 0; i < BOARDLEN; ++i)
        SET_CELL(board, i, input[i]);
}

int main(int argc, char **argv)
{
    uint8_t board[(BOARDLEN + 1) / 2] = {0};

    // I spent 3 days trying to automate it and then 3 minutes hardcoding the boards.
    uint8_t const boards[10][BOARDLEN] = {
        {0,3,4,0,0,0,6,0,0,
         0,0,2,6,0,0,0,8,0,
         0,6,8,3,0,0,0,0,7,
         0,0,3,0,0,1,0,0,5,
         0,5,9,0,6,0,0,7,2,
         0,0,0,5,2,0,0,4,6,
         2,0,5,9,0,6,0,0,0,
         0,0,0,4,0,8,0,5,0,
         0,0,0,0,0,7,0,0,4},

        {0,0,0,5,0,4,0,0,0,
         0,0,0,0,8,9,0,2,0,
         7,8,5,0,0,0,0,0,0,
         0,0,2,3,4,6,0,0,8,
         0,4,0,2,9,0,0,5,0,
         8,6,0,0,0,0,0,0,0,
         0,3,0,0,0,7,0,4,2,
         4,0,0,0,6,0,8,0,5,
         6,0,8,0,0,5,0,3,0,},

        {0,4,0,0,0,0,3,0,0,
         0,3,6,0,7,2,0,0,0,
         0,7,8,0,6,0,4,0,0,
         0,8,3,0,0,0,6,0,0,
         0,0,0,3,0,0,0,0,5,
         0,2,5,0,0,8,0,7,0,
         3,0,0,0,0,4,8,0,0,
         0,0,0,0,0,0,0,2,4,
         7,6,4,0,8,9,5,3,0},

        {0,0,0,0,7,4,9,0,0,
         0,0,0,0,0,0,0,4,6,
         0,7,9,0,0,0,3,0,0,
         6,0,0,7,2,8,0,0,9,
         9,8,0,5,0,3,0,0,0,
         0,3,7,9,4,0,0,5,0,
         2,0,0,0,0,0,0,0,0,
         0,0,8,0,3,0,0,6,0,
         0,6,0,4,9,0,0,2,3},

        {0,0,0,0,0,0,0,0,8,
         0,9,8,0,7,0,6,0,0,
         2,0,0,0,0,0,7,5,9,
         0,0,0,0,8,0,9,0,0,
         0,0,6,0,0,0,0,4,1,
         9,0,7,0,0,3,0,0,5,
         1,6,9,0,0,0,4,0,0,
         0,0,3,4,0,1,0,9,0,
         7,5,0,8,0,9,0,3,0},

        {0,6,0,2,0,0,0,8,0,
         0,8,0,6,3,4,5,0,1,
         0,0,0,8,0,0,0,6,0,
         0,0,0,0,0,5,6,0,8,
         2,4,0,3,0,0,0,5,0,
         0,0,0,9,6,7,0,0,2,
         3,0,7,0,0,0,4,0,0,
         0,2,5,0,4,3,0,0,7,
         8,0,0,0,0,0,0,0,0},

        {0,8,0,6,9,3,0,0,0,
         0,2,0,5,0,0,0,0,4,
         0,7,0,0,8,4,3,5,0,
         0,0,0,0,7,0,8,0,0,
         0,3,7,8,2,0,0,0,0,
         0,0,4,0,0,0,0,0,0,
         0,0,3,0,0,0,0,0,7,
         0,4,0,0,0,2,0,0,6,
         0,5,2,7,6,0,0,0,0},

        {6,0,7,0,0,5,0,1,4,
         5,3,9,1,0,4,2,7,0,
         0,2,0,0,8,7,9,6,0,
         9,1,8,0,0,6,0,0,7,
         0,0,0,7,0,1,0,0,0,
         0,0,0,0,0,0,0,9,0,
         0,7,1,5,0,0,0,8,0,
         2,9,0,4,0,8,0,0,0,
         8,0,0,6,0,2,0,4,9},

        {0,2,6,0,3,0,4,7,0,
         0,0,0,2,0,0,5,1,0,
         7,0,0,9,0,0,0,2,0,
         5,0,9,0,0,0,0,0,0,
         0,0,0,0,5,0,0,0,0,
         0,0,0,0,0,0,3,0,7,
         0,8,0,0,0,9,0,0,1,
         0,3,4,0,0,6,0,0,0,
         0,9,5,0,7,0,6,4,0},

        {0,0,7,0,0,1,0,0,4,
         1,3,0,0,5,0,8,6,0,
         0,0,5,0,0,4,0,0,0,
         6,0,8,0,0,0,0,7,0,
         0,0,0,0,7,0,0,0,0,
         0,5,0,0,0,0,9,0,3,
         0,0,0,9,0,0,1,0,0,
         0,7,2,0,8,0,0,9,5,
         5,0,0,7,0,0,4,0,0}
    };

    for (int i = 0; i < 10; ++i)
    {
        board_fill(board, boards[i]);

        if (board_backtrack(board, board_next_empty(board, 0)))
            for (uint8_t i = 0; i < BOARDLEN; ++i)
            {
                printf("%d ", GET_CELL(board, i));
                if (i % 9 == 8)
                    puts("");

            }

        else
            puts("No solution found.");

        puts("");
    }

    return 0;
}
