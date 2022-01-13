#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Dimensions of the board
#define WIDTH (size_t)119
#define HEIGHT (size_t)29

// Representation of the cells as integers and text
#define ALIVE (char)1
#define DEAD (char)0
#define T_ALIVE 'o'
#define T_DEAD ' '

// Arrays to store the board
char board[WIDTH][HEIGHT];              // The board of cells (0 | 1 - 'dead' | 'alive')
char temp[WIDTH][HEIGHT];               // Copy of the board to store the results
char output[(WIDTH+1) * HEIGHT + 1];    // The board in text format to be shown on the terminal

char cell;              // Current value of a cell
char sum;               // Number of 'alive' adjascent cells
size_t x1, x2, y1, y2;  // Coordinates of the adjascent cells
size_t text_pos;        // Current position in the text array

int main()
{
    // Populate the board randomly
    srand(time(0));
    text_pos = 0;
    for (size_t y = 0; y < HEIGHT; y++)
    {
        for (size_t x = 0; x < WIDTH; x++)
        {
            board[x][y] = (char)(rand() % 2);
            output[text_pos++] = board[x][y] ? T_ALIVE : T_DEAD;
        }
        output[text_pos++] = '\n';
    }

    // Add a NULL terminator to the end of text array
    output[text_pos] = '\0';

    // Draw the starting board
    printf_s(output);

    while (1)
    {
        // Return to the begining of the text array
        text_pos = 0;
        
        // Loop through the board
        for (size_t y = 0; y < HEIGHT; y++)
        {
            for (size_t x = 0; x < WIDTH; x++)
            {
                // Current cell
                cell = board[x][y];

                // The adjascent coordinates wrap around the edges
                x1 = x == 0 ? WIDTH-1  : (x - 1);
                y1 = y == 0 ? HEIGHT-1 : (y - 1);
                x2 = x == WIDTH-1  ? 0 : (x + 1);
                y2 = y == HEIGHT-1 ? 0 : (y + 1);

                // Howe many adjascent cells are 'alive'
                sum = (
                    board[x1][y1] + board[x1][y2] + board[x2][y1] + board[x2][y2]
                    + board[x][y1] + board[x][y2] + board[x1][y] + board[x2][y]
                );

                // The cell is 'alive'
                if (cell == ALIVE)
                {
                    // There are 2 or 3 'alive' neighbors
                    if ((sum == (char)2) || (sum == (char)3))
                    {
                        temp[x][y] = ALIVE;
                        output[text_pos++] = T_ALIVE;
                    }

                    // There are less than 2 or more than 3 'alive' neighbors
                    else
                    {
                        temp[x][y] = DEAD;
                        output[text_pos++] = T_DEAD;
                    }
                }
                
                // The cell is 'dead'
                if (cell == DEAD)
                {
                    // There are exactly 3 'alive' neighbors
                    if (sum == (char)3)
                    {
                        temp[x][y] = ALIVE;
                        output[text_pos++] = T_ALIVE;
                    }
                    
                    // There are any amount other than 3 of 'alive' neighbors
                    else
                    {
                        temp[x][y] = DEAD;
                        output[text_pos++] = T_DEAD;
                    }
                }
            }

            // Add a line break at the end of the row
            output[text_pos++] = '\n';
        }
        
        // Copy the cells from the temporary array to the board array
        memcpy(board, temp, sizeof(board));

        // Draw the current board
        printf_s(output);
        usleep(66667);
    }
    
    return 0;
}