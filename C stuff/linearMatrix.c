#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 3
#define COLUMNS 3
#define SIZE ROWS *COLUMNS

void initArray(float **, int, int);
void populateArray(float **, int);
void pPointers(float ***, float **, int, int);
void IdMatrixRows(float **, int);
void IdMatrixColumns(float **, int);
void juvenileGaussJordan(float **, float ***, float ***);
void RowSwap(float ***, int, int);
void MultiplyRow(float ***, int, float, int);
void AddMultipleOfRowToRow(float ***, int, int, float);
void printArrayA(float ***);
void printArrayB(float ***);

int main(void)
{
    srand(time(NULL));
    float *A = NULL;
    float *b = NULL;
    initArray(&A, ROWS, COLUMNS);
    initArray(&b, ROWS, 1);
    populateArray(&A, SIZE); // square matrix
    // for (int x = 0; x < ROWS; x++)
    // {
    //     *(A + (x * COLUMNS)) = 0;
    // }
    populateArray(&b, ROWS); // just rows, since vector matrix
    float **ppA = NULL;
    float **ppb = NULL;
    pPointers(&ppA, &A, ROWS, COLUMNS);
    printf("Start\n");
    printArrayA(&ppA);
    pPointers(&ppb, &b, ROWS, 1);
    printArrayB(&ppb);

    float *pIdentity_matrixR = NULL;
    float *pIdentity_matrixL = NULL;
    IdMatrixColumns(&pIdentity_matrixR, COLUMNS);
    // IdMatrixRows(&pIdentity_matrixL, ROWS);
    juvenileGaussJordan(&A, &ppA, &ppb);
    printf("\n\n\n");
    printArrayA(&ppA);
    printArrayB(&ppb);
    free(ppA);
    free(pIdentity_matrixR);
    free(pIdentity_matrixL);
    free(A);
    free(ppb);
    free(b);
}

void printArrayA(float ***ppA)
{
    for (int y = 0; y < ROWS; y++)
    {
        for (int a = 0; a < COLUMNS; a++)
        {
            printf("[%0.5f],", *(*(*(ppA) + y) + a)); // keep in mind *(ppa + y) is the same as ppa[y]
        }
        printf("\n");
    }
}

void printArrayB(float ***ppB)
{
    for (int y = 0; y < ROWS; y++)
    {
        printf("[%0.5f],", *(*(*(ppB) + y))); // keep in mind *(ppa + y) is the same as ppa[y]
        printf("\n");
    }
}

void initArray(float **arr, int a, int b)
{
    // initialise array, given the pointer to it, with a x b size
    *arr = malloc(sizeof(float) * a * b); // *arr holds pointer to malloc'd memory
    // printf("malloc gave: %p\n", *arr);
}

void populateArray(float **arr, int size)
{
    for (int x = 0; x < size; x++)
    {
        *(*arr + x) = rand() % 10;
    }
}

void pPointers(float ***ppa, float **arr, int rows, int columns)
{
    *ppa = malloc(sizeof(float *) * rows);
    for (int x = 0; x < rows; x++)
    {
        *(*ppa + x) = (*arr + (columns * x));
    }
}

void IdMatrixColumns(float **pIdA, int columns)
{
    // this function name sucks; i picked it to denote the requirement
    // of A_rows*I_columns.
    *pIdA = malloc(sizeof(float) * (columns * columns));
    for (int i = 0; i < (columns * columns); i++)
    {
        if (i % (columns + 1) == 0)
        {
            *((*pIdA) + i) = 1;
        }
        else
        {
            *((*pIdA) + i) = 0;
        }
    }
    // printf("IdMatrixByColumnNumber:\n");
    // for (int a = 0; a<(columns*columns);a++){
    //     printf("[%d],", *(*(pIdA) + a));
    //     if (a % (columns) == (columns-1) && a>0) {
    //         printf("\n");
    //     }
    // }
}

void IdMatrixRows(float **pIdA, int rows)
{
    

    *pIdA = malloc(sizeof(float) * (rows * rows));
    for (int i = 0; i < (rows * rows); i++)
    {
        if (i % (rows + 1) == 0)
        {
            *(*(pIdA) + i) = 1;
        }
        else
        {
            *(*(pIdA) + i) = 0;
        }
    }
    printf("lIdMatrix:\n");
    for (int a = 0; a < (rows * rows); a++)
    {
        printf("[%f],", *(*(pIdA) + a));
        if (a % (rows) == (rows - 1) && a > 0)
        {
            printf("\n");
        }
    }
}

void juvenileGaussJordan(float **pa, float ***ppa, float ***ppb)
{
    // PIVOTFINDER
    float pivotValue = 0;
    int pivotRow = 0;
    int pivotColumn = 0;
    int a = 0;
    int b = 0;
    float k = 0;
    float k2 = 0;

    // for (int l = 0; l < ROWS; l++){
    //     printf("\n[  %p  ]\n", *(*(ppa)+l));
    //     for (int q = 0; q < COLUMNS; q++){
    //         printf("[%f]", *(*(*(ppa)+l)+q));
    //     }
    // }

    while (a < ROWS && b < COLUMNS)
    {
        for (int i = a; i<ROWS; i++){
            if (*(*(*(ppa)+i)+b) != 0) {
                pivotValue = *(*(*(ppa)+i)+b);
                pivotRow = i;
                pivotColumn = b; // do we even need this?
                break;
            }
        }
        if (pivotValue == 0) {
            b+=1;
            continue;
        }
        else {
            //a is rows
            //b is columns
            //ppa+b = []->[]
            //(*ppa)+a = [][]
            //           |
            //           []
            RowSwap(ppa, pivotRow, a);
            RowSwap(ppb, pivotRow, a);
            MultiplyRow(ppa, a, (1/pivotValue), COLUMNS);
            MultiplyRow(ppb, a, (1/pivotValue), 1);
            for (int x = 0; x<ROWS; x++){
                if (x != a) {
                    k = -(*(*(*(ppa)+x)+b));
                    AddMultipleOfRowToRow(ppa, a, x, k, COLUMNS); //rowb = rowb + k*rowa
                    AddMultipleOfRowToRow(ppb, a, x, k, 1);
                }
            }
            
            a +=1;
            b +=1;
        }
    }
}


void RowSwap(float ***ppA, int rowA, int rowB)
{
    if (rowA == rowB) {
        //do nothing
    }
    else {
    printf("%d <-> %d\n", rowA, rowB);
    float *row1 = *(*(ppA) + rowA);
    float *row2 = *(*(ppA) + rowB);
    float *temp = *(*(ppA) + rowA);
    *(*(ppA) + rowA) = *(*(ppA) + rowB);
    *(*(ppA) + rowB) = temp;
    }
}

void MultiplyRow(float ***ppA, int row, float x, int columns)
{
    if (columns != 1) {
        // printf("MultiplyRow %d by %.4f\n", row, x);
    }
    for (int i = 0; i < columns; i++)
    {
        // printf("ppA[%f] was: %f\n", row, *(*(*(ppA)+row)+i));
        *(*(*(ppA) + row) + i) *= x;
        // printf("ppA[%f] now: %f\n", row, *(*(*(ppA)+row)+i));
    }
}

void AddMultipleOfRowToRow(float ***ppa, int rowA, int rowB, float k, int columns) //rowB = rowB + k*rowA
{
    // printf("AddMultipleOfRowToRow: row[%d] = row[%d] + %f*row[%d]\n", rowB, rowB, k, rowA);
    // rowB = rowB + k*rowA
    for (int i = 0; i < columns; i++)
    {
        // printf("ppA[%d] was before multiple added: %d\n", rowB, *(*(*(ppA)+rowB)+i));
        // printf("k * ppA[%d][%d]: [%d]\n", rowA, i, k*(*(*(*(ppA)+rowA)+i)));;
        *(*(*(ppa)+rowB)+i) += k * (*(*(*(ppa)+rowA)+i));
        // printf("ppA[%d] now: %d\n", rowB, *(*(*(ppA)+rowB)+i));


    //for (int l = 0; l < ROWS; l++){
    //     printf("\n[  %p  ]\n", *(*(ppa)+l));
    //     for (int q = 0; q < COLUMNS; q++){
    //         printf("[%f]", *(*(*(ppa)+l)+q));
    //     }
    // }
    }
}