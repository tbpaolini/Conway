#include <SDL2\SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Window dimensions
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 640

// Game of life's board parameters
#define CELL_SIZE 4
const int CELLS_HORIZONTAL = (WINDOW_WIDTH / CELL_SIZE);
const int CELLS_VERTICAL = (WINDOW_HEIGHT / CELL_SIZE);

// Representation of the cell's states
#define ALIVE (char)1
#define DEAD (char)0

// Coordinates
int X, Y;            // Board (2D array)
int x_win, y_win;    // Window

char cell;              // Current value of a cell
char sum;               // Number of 'alive' adjascent cells
int X1, X2, Y1, Y2;     // Coordinates of the adjascent cells

// Create a 2D array for the board
char* create_board()
{
    // Allocate memory for the board
    char *board = malloc(sizeof(char) * CELLS_HORIZONTAL * CELLS_VERTICAL);
    return board;
}

// Get the board index from a (x,y) coordinate
int coord(int x, int y)
{
    return (y * CELLS_HORIZONTAL) + x;
}

void draw_cell(int x, int y, SDL_Renderer* renderer)
{
    SDL_Rect cell = {x, y, CELL_SIZE, CELL_SIZE};    // Square shape
    SDL_RenderFillRect(renderer, &cell);
}

// Timing
int usleep();         // Prevents the compiler to warn that the function has an implicit declaration
Uint64 start, total;  // Start and end of the timing
Uint64 frequency;     // Ticks per unit of time
Uint64 sleep_time;    // Time (in microseconds) to wait before the next frame

// Frame rate
#define FPS 15
Uint64 FRAME = (Uint64)1000000 / FPS;   // Duration in microseconds

// Entry point
int WinMain(int argc, char* argv[])
{
    // Game of life's board
    const int cells_amount = CELLS_VERTICAL * CELLS_HORIZONTAL;
    char* board = create_board();   // Main board
    char* temp = create_board();    // Temporary board to store the changes

    // Populate the board randomly
    srand(time(0));
    
    for (int i = 0; i < cells_amount; i++)
    {
        board[i] = (char)(rand() % 2);
    }
    
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Open the window
    SDL_Window* window = SDL_CreateWindow(
        "Conway's Game of Life",  // Title
        SDL_WINDOWPOS_UNDEFINED,  // X position
        SDL_WINDOWPOS_UNDEFINED,  // Y position
        WINDOW_WIDTH,             // Width
        WINDOW_HEIGHT,            // HEIGHT
        SDL_WINDOW_SHOWN          // Status of the window
    );

    // Get the window surface
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    // Renderer for drawing to the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // Event handler
    SDL_Event event;

    frequency = SDL_GetPerformanceFrequency();

    // Main loop (draw and update the board)
    while (1)
    {
        // Begin timing
        start =  SDL_GetPerformanceCounter();

        // Clear the screen with white
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // Handle the events
        while (SDL_PollEvent(&event))
        {
            // Window is closed
            if (event.type == SDL_QUIT)
            {
                goto exit;  // Quirky but effective :-)
            }
        }
        
        for (int i = 0; i < cells_amount; i++)
        {
            cell = board[i];
            
            // Board coordinates
            X = i % CELLS_HORIZONTAL;
            Y = i / CELLS_HORIZONTAL;
            
            // Window coordinates
            x_win = X * CELL_SIZE;
            y_win = Y * CELL_SIZE;

            // Adjascent cells
            // Their coordinates wrap around the edges
            X1 = X == 0 ? CELLS_HORIZONTAL-1 : (X - 1);
            Y1 = Y == 0 ? CELLS_VERTICAL-1   : (Y - 1);
            X2 = X == CELLS_HORIZONTAL-1 ? 0 : (X + 1);
            Y2 = Y == CELLS_VERTICAL-1 ? 0   : (Y + 1);

            // Amount of adjascent cells
            sum = (
                board[coord(X1, Y1)] +
                board[coord(X1, Y2)] +
                board[coord(X2, Y1)] +
                board[coord(X2, Y2)] +
                board[coord(X, Y1)]  +
                board[coord(X, Y2)]  +
                board[coord(X1, Y)]  +
                board[coord(X2, Y)]
            );

            // Set cell color to black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            // The cell is 'alive'
            if (cell == ALIVE)
            {
                // There are 2 or 3 'alive' neighbors
                if ((sum == (char)2) || (sum == (char)3))
                {
                    temp[i] = ALIVE;
                    draw_cell(x_win, y_win, renderer);
                }

                // There are less than 2 or more than 3 'alive' neighbors
                else
                {
                    temp[i] = DEAD;
                }
            }
            
            // The cell is 'dead'
            else
            {
                // There are exactly 3 'alive' neighbors
                if (sum == (char)3)
                {
                    temp[i] = ALIVE;
                    draw_cell(x_win, y_win, renderer);
                }
                
                // There are any amount other than 3 of 'alive' neighbors
                else
                {
                    temp[i] = DEAD;
                }
            }
        }

        // Update the screen
        SDL_RenderPresent(renderer);

        // Copy the cells from the temporary array to the board array
        for (int i = 0; i < cells_amount; i++)
        {
            board[i] = temp[i];
        }

        // Sleep for the remaining of the frame
        total = ((SDL_GetPerformanceCounter() - start) / frequency) * (Uint64)1000000;
        sleep_time = total < FRAME ? FRAME - total : 0;  // Do not sleep if the total time is longer than the frame
        usleep(sleep_time);
    }
    
    // When the user is closing the program
    exit:
        // Garbage collection
        free(board);
        free(temp);

        // Exit from SDL subsystems
        SDL_Quit();

        return 0;
}