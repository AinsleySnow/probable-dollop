/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define SIZE 17

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32)
    {
        int _0, _1, _2, _3, _4, _5, _6, _7;

        for (int m = 0; m < 32; m += 8)
        {
            for (int n = 0; n < 32; n += 8)
            {
                if (m != n)
                {
                    for (int i = m; i < m + 8; ++i)
                    {
                        for (int j = n; j < n + 8; ++j)
                        {
                            B[j][i] = A[i][j];
                        }
                    }
                }
                else
                {
                    for (int i = m; i < m + 8; ++i)
                    {
                        _0 = A[i][n];
                        _1 = A[i][n + 1];
                        _2 = A[i][n + 2];
                        _3 = A[i][n + 3];
                        _4 = A[i][n + 4];
                        _5 = A[i][n + 5];
                        _6 = A[i][n + 6];
                        _7 = A[i][n + 7];

                        B[i][n] = _0;
                        B[i][n + 1] = _1;
                        B[i][n + 2] = _2;
                        B[i][n + 3] = _3;
                        B[i][n + 4] = _4;
                        B[i][n + 5] = _5;
                        B[i][n + 6] = _6;
                        B[i][n + 7] = _7;
                    }

                    for (int i = m; i < m + 8; ++i)
                    {
                        for (int j = n + (i - m + 1); j < n + 8; ++j)
                        {
                            if (i != j)
                            {
                                _0 = B[i][j];
                                B[i][j] = B[j][i];
                                B[j][i] = _0;
                            }
                        }
                    }
                }
            }
        }
    }
    else if (M == 64)
    {
        int _0, _1, _2, _3, _4, _5, _6, _7;

        for (int m = 0; m < 64; m += 8)
        {
            for (int n = m; n < 64; n += 8)
            {
                if (m != n)
                {
                    for (int i = n; i < n + 4; ++i)
                    {
                        N = i - n;

                        _0 = A[i][m];
                        _1 = A[i][m + 1];
                        _2 = A[i][m + 2];
                        _3 = A[i][m + 3];

                        _4 = A[i][m + 4];
                        _5 = A[i][m + 5];
                        _6 = A[i][m + 6];
                        _7 = A[i][m + 7];

                        B[m + 4][n + N] = _4;
                        B[m + 5][n + N] = _5;
                        B[m + 6][n + N] = _6;
                        B[m + 7][n + N] = _7;

                        B[m + 4][n + 4 + N] = _0;
                        B[m + 5][n + 4 + N] = _1;
                        B[m + 6][n + 4 + N] = _2;
                        B[m + 7][n + 4 + N] = _3;
                    }

                    for (int i = m + 4; i < m + 8; ++i)
                    {
                        N = i - (m + 4);

                        _0 = A[n + 4][m + 4 + N];
                        _1 = A[n + 5][m + 4 + N];
                        _2 = A[n + 6][m + 4 + N];
                        _3 = A[n + 7][m + 4 + N];

                        _4 = B[m + 4 + N][n + 4];
                        _5 = B[m + 4 + N][n + 5];
                        _6 = B[m + 4 + N][n + 6];
                        _7 = B[m + 4 + N][n + 7];

                        B[m + 4 + N][n + 4] = _0;
                        B[m + 4 + N][n + 5] = _1;
                        B[m + 4 + N][n + 6] = _2;
                        B[m + 4 + N][n + 7] = _3;

                        B[m + N][n] = _4;
                        B[m + N][n + 1] = _5;
                        B[m + N][n + 2] = _6;
                        B[m + N][n + 3] = _7;
                    }

                    for (int i = m; i < m + 4; ++i)
                    {
                        _0 = A[n + 4][i];
                        _1 = A[n + 5][i];
                        _2 = A[n + 6][i];
                        _3 = A[n + 7][i];

                        B[i][n + 4] = _0;
                        B[i][n + 5] = _1;
                        B[i][n + 6] = _2;
                        B[i][n + 7] = _3;
                    }
                    ///////////////////////////////////////////////////////////////////////////////////
                    for (int i = m + 4; i < m + 8; ++i)
                    {
                        M = i - (m + 4);

                        _0 = A[i][n];
                        _1 = A[i][n + 1];
                        _2 = A[i][n + 2];
                        _3 = A[i][n + 3];

                        _4 = A[i][n + 4];
                        _5 = A[i][n + 5];
                        _6 = A[i][n + 6];
                        _7 = A[i][n + 7];

                        B[n][m + M] = _4;
                        B[n + 1][m + M] = _5;
                        B[n + 2][m + M] = _6;
                        B[n + 3][m + M] = _7;

                        B[n][m + 4 + M] = _0;
                        B[n + 1][m + 4 + M] = _1;
                        B[n + 2][m + 4 + M] = _2;
                        B[n + 3][m + 4 + M] = _3;
                    }

                    for (int i = n; i < n + 4; ++i)
                    {
                        _0 = A[m][i];
                        _1 = A[m + 1][i];
                        _2 = A[m + 2][i];
                        _3 = A[m + 3][i];

                        _4 = B[i][m];
                        _5 = B[i][m + 1];
                        _6 = B[i][m + 2];
                        _7 = B[i][m + 3];

                        B[i][m] = _0;
                        B[i][m + 1] = _1;
                        B[i][m + 2] = _2;
                        B[i][m + 3] = _3;

                        B[i + 4][m + 4] = _4;
                        B[i + 4][m + 5] = _5;
                        B[i + 4][m + 6] = _6;
                        B[i + 4][m + 7] = _7;
                    }

                    for (int i = m; i < m + 4; ++i)
                    {
                        M = i - m;

                        _0 = A[i][n + 4];
                        _1 = A[i][n + 5];
                        _2 = A[i][n + 6];
                        _3 = A[i][n + 7];

                        B[n + 4][m + M] = _0;
                        B[n + 5][m + M] = _1;
                        B[n + 6][m + M] = _2;
                        B[n + 7][m + M] = _3;
                    }
                }
                //*******************************************************************************//
                else
                {
                    if (m != 56 && n != 56)
                    {
                        for (int i = m; i < m + 4; ++i)
                        {
                            M = i - m;

                            _0 = A[i][n];
                            _1 = A[i][n + 1];
                            _2 = A[i][n + 2];
                            _3 = A[i][n + 3];

                            _4 = A[i][n + 4];
                            _5 = A[i][n + 5];
                            _6 = A[i][n + 6];
                            _7 = A[i][n + 7];

                            B[m + 4][n + 8 + M] = _0;
                            B[m + 5][n + 8 + M] = _1;
                            B[m + 6][n + 8 + M] = _2;
                            B[m + 7][n + 8 + M] = _3;

                            B[m + 4][n + 12 + M] = _4;
                            B[m + 5][n + 12 + M] = _5;
                            B[m + 6][n + 12 + M] = _6;
                            B[m + 7][n + 12 + M] = _7;
                        }

                        for (int i = m + 4; i < m + 8; ++i)
                        {
                            M = i - (m + 4);

                            _0 = A[i][n];
                            _1 = A[i][n + 1];
                            _2 = A[i][n + 2];
                            _3 = A[i][n + 3];

                            _4 = A[i][n + 4];
                            _5 = A[i][n + 5];
                            _6 = A[i][n + 6];
                            _7 = A[i][n + 7];

                            B[m + 4][n + M] = _0;
                            B[m + 5][n + M] = _1;
                            B[m + 6][n + M] = _2;
                            B[m + 7][n + M] = _3;

                            B[m + 4][n + 4 + M] = _4;
                            B[m + 5][n + 4 + M] = _5;
                            B[m + 6][n + 4 + M] = _6;
                            B[m + 7][n + 4 + M] = _7;
                        }

                        for (int i = m + 4; i < m + 8; ++i)
                        {
                            _0 = B[i][n];
                            _1 = B[i][n + 1];
                            _2 = B[i][n + 2];
                            _3 = B[i][n + 3];

                            _4 = B[i][n + 12];
                            _5 = B[i][n + 13];
                            _6 = B[i][n + 14];
                            _7 = B[i][n + 15];

                            B[i][n] = _4;
                            B[i][n + 1] = _5;
                            B[i][n + 2] = _6;
                            B[i][n + 3] = _7;

                            B[i][n + 12] = _0;
                            B[i][n + 13] = _1;
                            B[i][n + 14] = _2;
                            B[i][n + 15] = _3;
                        }

                        for (int i = m; i < m + 4; ++i)
                            for (int j = n; j < n + 8; ++j)
                                B[i][j] = B[i + 4][j + 8];
                    }
                    else
                    {
                        for (int i = m; i < m + 4; ++i)
                        {
                            _0 = A[i][n];
                            _1 = A[i][n + 1];
                            _2 = A[i][n + 2];
                            _3 = A[i][n + 3];
                            _4 = A[i][n + 4];
                            _5 = A[i][n + 5];
                            _6 = A[i][n + 6];
                            _7 = A[i][n + 7];

                            B[i][n] = _0;
                            B[i][n + 1] = _1;
                            B[i][n + 2] = _2;
                            B[i][n + 3] = _3;
                            B[i][n + 4] = _4;
                            B[i][n + 5] = _5;
                            B[i][n + 6] = _6;
                            B[i][n + 7] = _7;
                        }

                        for (int i = m; i < m + 4; ++i)
                        {
                            for (int j = n + (i - m); j < n + 4; ++j)
                            {
                                if (i != j)
                                {
                                    _0 = B[i][j];
                                    B[i][j] = B[j][i];
                                    B[j][i] = _0;
                                }
                            }
                        }

                        for (int i = m; i < m + 4; ++i)
                        {
                            M = i - m;
                            for (int j = n + 4 + M; j < n + 8; ++j)
                            {
                                N = j - (n + 4);

                                _0 = B[i][j];
                                B[i][j] = B[m + N][n + 4 + M];
                                B[m + N][n + 4 + M] = _0;
                            }
                        }

                        for (int i = m + 4; i < m + 8; ++i)
                        {
                            _0 = A[i][n];
                            _1 = A[i][n + 1];
                            _2 = A[i][n + 2];
                            _3 = A[i][n + 3];
                            _4 = A[i][n + 4];
                            _5 = A[i][n + 5];
                            _6 = A[i][n + 6];
                            _7 = A[i][n + 7];

                            B[i][n] = _0;
                            B[i][n + 1] = _1;
                            B[i][n + 2] = _2;
                            B[i][n + 3] = _3;
                            B[i][n + 4] = _4;
                            B[i][n + 5] = _5;
                            B[i][n + 6] = _6;
                            B[i][n + 7] = _7;
                        }

                        for (int i = m + 4; i < m + 8; ++i)
                        {
                            for (int j = n + 4 + i - (m + 4); j < n + 8; ++j)
                            {
                                if (i != j)
                                {
                                    _0 = B[i][j];
                                    B[i][j] = B[j][i];
                                    B[j][i] = _0;
                                }
                            }
                        }

                        for (int i = m + 4; i < m + 8; ++i)
                        {
                            M = i - (m + 4);
                            for (int j = n + M; j < n + 4; ++j)
                            {
                                N = j - n;

                                _0 = B[i][j];
                                B[i][j] = B[m + 4 + N][n + M];
                                B[m + 4 + N][n + M] = _0;
                            }
                        }

                        for (int i = m + 4; i < m + 8; ++i)
                        {
                            _0 = B[i][n];
                            _1 = B[i][n + 1];
                            _2 = B[i][n + 2];
                            _3 = B[i][n + 3];

                            _4 = B[i - 4][n + 4];
                            _5 = B[i - 4][n + 5];
                            _6 = B[i - 4][n + 6];
                            _7 = B[i - 4][n + 7];

                            B[i - 4][n + 4] = _0;
                            B[i - 4][n + 5] = _1;
                            B[i - 4][n + 6] = _2;
                            B[i - 4][n + 7] = _3;

                            B[i][n] = _4;
                            B[i][n + 1] = _5;
                            B[i][n + 2] = _6;
                            B[i][n + 3] = _7;
                        }
                    }
                }
            }
        }
    }

    else if (M == 61)
    {
        for (int m = 0; m < 67; m += 17)
        {
            for (int n = 0; n < 61; n += 4)
            {
                for (int i = m; i < min(m + 17, 67); ++i)
                {
                    for (int j = n; j < min(n + 4, 61); ++j)
                    {
                        B[j][i] = A[i][j];
                    }
                }
            }
        }
    }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

 /*
  * trans - A simple baseline transpose function, not optimized for the cache.
  */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */

void registerFunctions()
{
    registerTransFunction(transpose_submit, transpose_submit_desc);
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

