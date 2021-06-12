#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>

char Cell[9] = "⬜";
char unCell[2] = "  ";      //one cell's place is tow rows and one cols

void CreatGrid(int **grid,int cols,int rows)
{
    /*Random start*/
    srand((unsigned)time(NULL));
    for (int i = 0;i < cols;i++)
    {
        for (int j = 0;j < rows;j++)
        {
            grid[i][j] = rand() % 2;
        }
    }
    /*Binker*/
    //grid[2][2]=grid[2][3]=grid[2][4]=1;
    /*Toad*/
    /*         grid[6][2]=grid[6][3]=grid[6][4]=1;
    grid[7][1]=grid[7][2]=grid[7][3]=1;*/
    /*Glider*/
    /*           grid[10][21]=1;
                              grid[11][22]=1;
    grid[12][20]=grid[12][21]=grid[12][22]=1;*/
    /*Gosper glider gun*/
    /*grid[ 6][26]=1;
                                                                                                                                        grid[ 7][24]=1;     grid[ 7][26]=1;
                                                grid[ 8][14]=grid[ 8][15]=1;                                        grid[ 8][22]=grid[ 8][23]=1;                                        grid[ 8][36]=grid[ 8][37]=1;
                                            grid[ 9][13] = 1;                 grid[ 9][17]=1;                       grid[ 9][22]=grid[ 9][23]=1;                                        grid[ 9][36]=grid[ 9][37]=1;
    grid[10][2]=grid[10][3]=1;      grid[10][12]=1;                               grid[10][18]=1;                   grid[10][22]=grid[10][23]=1;
    grid[11][2]=grid[11][3]=1;      grid[11][12]=1;            grid[11][16]=1;    grid[11][18]=1;grid[11][19]=1;                        grid[11][24]=1;     grid[11][26]=1;
                                    grid[12][12]=1;                               grid[12][18]=1;                                                           grid[12][26]=1;
                                            grid[13][13]=1;                   grid[13][17]=1;
                                                grid[14][14]=grid[14][15]=1;*/
}

void InitGrid(int **grid,int cols,int rows)
{
    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            grid[i][j] = 0;
        }
    }
}

void DispGrid(int **grid,int cols,int rows)
{
    for (int i = 0;i < cols;i++)
    {
        for (int j = 0;j < rows;j++)
        {
            printf("%s",grid[i][j] ? Cell : unCell);
        }
        printf("\n");
    }
}

void SwapGrid(int **destGrid,int cols,int rows,int **sourceGrid)
{
    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            destGrid[i][j] = sourceGrid[i][j];
        }
    }
}

int CountNeighbors(int **sourceGrid,int x,int y,int cols,int rows)
{
    int sum = 0,col,row;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1;j < 2; j++)
        {
            col = (x + i + cols) % cols;
            row = (y + j + rows) % rows;
            sum += sourceGrid[col][row];
        }
    }
    sum -= sourceGrid[x][y];
    return sum;
}

void ComputeGrid(int **destGrid,int cols,int rows,int **sourceGrid)
{
    int lifeStatu,neighbors;
    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            lifeStatu = sourceGrid[i][j];
            neighbors = CountNeighbors(sourceGrid,i,j,cols,rows);
            if (lifeStatu == 0 && neighbors == 3)
            {
                destGrid[i][j] = 1;
            } else if (lifeStatu == 1 && (neighbors < 2 || neighbors > 3)){
                destGrid[i][j] = 0;
            }else{
                destGrid[i][j] = lifeStatu;
            }       //Game rules
        }
    }
    SwapGrid(sourceGrid,cols,rows,destGrid);
}

int CountCells(int **grid,int cols,int rows)
{
    int sum = 0;
    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            sum += grid[i][j];
        }
    }
    return sum;
}

void FreeGrid(int **oldGrid,int **newGrid,int cols,int rows)
{
    for (int i = 0;i < cols;i++)
    {
        free(oldGrid[i]);
        free(newGrid[i]);
    }
    free(oldGrid);
    free(newGrid);
}

void GetWindows(int *cols,int *rows)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *rows = (csbi.srWindow.Right - csbi.srWindow.Left + 1)/2;
    *cols = csbi.srWindow.Bottom - csbi.srWindow.Top ;
}

int main()
{
    int cols = 0;      //Columns of game map
    int rows = 0;      //Rows of game map
    GetWindows(&cols,&rows);
    int generation = 0;     //Count loops
    int **oldGrid = (int **)malloc(cols * sizeof(int *));
    int **newGrid = (int **)malloc(cols * sizeof(int *));
    for (int i = 0;i < cols;i++)
    {
        oldGrid[i] = (int *)malloc(rows * sizeof(int));
        newGrid[i] = (int *)malloc(rows * sizeof(int));
    }       //Make 2D array
    InitGrid(oldGrid,cols,rows);
    CreatGrid(oldGrid,cols,rows);
    do{
        printf("\033[0;0H\033[?25l\033[0m");
        DispGrid(oldGrid,cols,rows);
        InitGrid(newGrid,cols,rows);
        ComputeGrid(newGrid,cols,rows,oldGrid);
        printf("\033[?25hGeneration: %3d Cell: %3d\033[0m",generation,CountCells(oldGrid,cols,rows));
        generation++;
    } while (1);
    FreeGrid(oldGrid,newGrid,cols,rows);
}
