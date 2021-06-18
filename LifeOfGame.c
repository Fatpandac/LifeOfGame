#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>

// #define DEBUG

typedef struct {
    char cursorTag[5];
    int  status;
    int  x;
    int  y;
}Cursor;    //There are three status in a total of 'Move' 'Insert' 'Remove' by '0' '1' '2'

enum key {
    MOVE    = 77,
    INSERT  = 73,
    REMOVE  = 82,
    W_UP    = 119,
    A_LEFT  = 97,
    S_DOWN  = 115,
    D_RIGHT = 100,
    Q_QUIT  = 81,
};

char Cell[9] = "⬜";
char unCell[2] = "  ";      //one cell's place is tow rows and one cols

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
    printf("\033[0;0H\033[?25l\033[0m");
    for (int i = 0;i < cols;i++)
    {
        for (int j = 0;j < rows;j++)
        {
            printf("%s",grid[i][j] ? Cell : unCell);
        }
        printf("\n");
    }
}

void ClearFootPrint(int **grid,int x,int y,int cols,int rows)
{
#ifdef DEBUG
    system("cls");
    if (!grid[y - 1][((x - 1)/2 - 1 + rows) % rows]) printf("\33[%d;%dH\33[37m%s\33[0m",y,((x - 2 + rows*2) % (rows*2)),Cell);
    if (!grid[((y - 1) - 1 + cols) % cols][((x - 1)/2 + rows) % rows]) printf("\33[%d;%dH\33[37m%s\33[0m",y - 1,x,Cell);
    if (!grid[y - 1][((x - 1)/2 + 1 + rows) % rows]) printf("\33[%d;%dH\33[37m%s\33[0m",y,((x + 2 + rows*2) % (rows*2)),Cell);
    if (!grid[((y - 1) + 1 + cols) % cols][((x - 1)/2 + rows) % rows]) printf("\33[%d;%dH\33[37m%s\33[0m",y + 1,x,Cell);
#else
    if (!grid[y - 1][((x - 1)/2 - 1 + rows) % rows]) printf("\33[%d;%dH\33[37m%s\33[0m",y,((x - 2 + rows*2) % (rows*2)),unCell);
    if (!grid[((y - 1) - 1 + cols) % cols][((x - 1)/2 + rows) % rows]) printf("\33[%d;%dH\33[37m%s\33[0m",y - 1,x,unCell);
    if (!grid[y - 1][((x - 1)/2 + 1 + rows) % rows]) printf("\33[%d;%dH\33[37m%s\33[0m",y,((x + 2 + rows*2) % (rows*2)),unCell);
    if (!grid[((y - 1) + 1 + cols) % cols][((x - 1)/2 + rows) % rows]) printf("\33[%d;%dH\33[37m%s\33[0m",y + 1,x,unCell);
#endif
}

void DrawDispGrid(int **grid,int cols,int rows)
{
    printf("\033[?25l\033[0m");
    for (int i = 0;i < cols;i++)
    {
        for (int j = 0;j < rows;j++)
        {
            if (grid[i][j]) printf("\33[%d;%dH\33[37m%s\33[0m",i+1,j*2+1,Cell);
        }
    }
}

void ControlCursor(char input,Cursor *cursor,int cols,int rows)
{
    switch (input)
    {
    case MOVE:
        cursor->status = 0;
        break;
    case INSERT:
        cursor->status = 1;
        break;
    case REMOVE:
        cursor->status = 2;
        break;
    case A_LEFT:
        cursor->x -= 2 * (cursor->x - 2 > 0);
        break;
    case D_RIGHT:
        cursor->x += 2 * (cursor->x + 2 <= rows * 2);
        break;
    case W_UP:
        cursor->y -= (cursor->y - 1 > 0);
        break;
    case S_DOWN:
        cursor->y += (cursor->y + 1 <= cols);
        break;
    default:
        break;
    }
}

char *FormatCursor(char *cursorContent,Cursor cursor)
{
    switch (cursor.status)
    {
    case 0:
        sprintf(cursorContent,"\33[%d;%dH\33[37m\33[5m%s\33[0m",cursor.y,cursor.x,cursor.cursorTag);
        break;
    case 1:
        sprintf(cursorContent,"\33[%d;%dH\33[32m%s\33[0m",cursor.y,cursor.x,cursor.cursorTag);
        break;
    case 2:
        sprintf(cursorContent,"\33[%d;%dH\33[31m%s\33[0m",cursor.y,cursor.x,cursor.cursorTag);
        break;
    default:
        break;
    }
    return cursorContent;
}

void DispCursor(Cursor cursor)
{
    char cursorContent[30];
    printf("%s",FormatCursor(cursorContent,cursor));
}

void Draw(int **grid,int cols,int rows)
{
    Cursor cursor = {
        .cursorTag = "⬜",
        .status = 0,
        .x      = 1,
        .y      = 1,
    };
    char input;
    DispGrid(grid,cols,rows);
    DispCursor(cursor);
    while (input = getch())
    {
        if (input == Q_QUIT) break;
        ControlCursor(input,&cursor,cols,rows);
        if (cursor.status == 1) grid[cursor.y - 1][(cursor.x) / 2] = 1;
        if (cursor.status == 2) grid[cursor.y - 1][(cursor.x) / 2] = 0;
#ifdef DEBUG
        printf("\33[40;0HX:%d Y:%d status:%d",cursor.x,cursor.y,cursor.status);
#endif
        ClearFootPrint(grid,cursor.x,cursor.y,cols,rows);
        DrawDispGrid(grid,cols,rows);
        DispCursor(cursor);
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
    *cols = csbi.srWindow.Bottom - csbi.srWindow.Top;
}

int CheckExit()
{
    char input;
    if (kbhit())
    {
        input = getch();
        if (input == Q_QUIT)
        {
            printf("\nDo you want to exit? (Y/any) ");
            input = getche();
            return (input == 'Y' || input == 'y') ? 1 : 0;
        }
    }
    return 0;
}

int CheckRedraw()
{
    char input;
    printf("\nDo you want to redraw? (Y/any)");
    input = getche();
    return (input == 'Y' || input == 'y') ? 0 : 1;
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
#ifdef DEBUG
    printf("%d %d",cols,rows);
    getch();
#endif
    do{
        InitGrid(oldGrid,cols,rows);
        Draw(oldGrid,cols,rows);
        do{
            DispGrid(oldGrid,cols,rows);
            InitGrid(newGrid,cols,rows);
            ComputeGrid(newGrid,cols,rows,oldGrid);
            printf("\033[?25hGeneration: %-6d Cell: %-6d\033[0m",generation,CountCells(oldGrid,cols,rows));
            generation++;
            if (CheckExit()) break;
        } while (1);
        if (CheckRedraw()) break;
    }while (1);
    FreeGrid(oldGrid,newGrid,cols,rows);
}
