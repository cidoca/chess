/*
  A chess game
  Copyright (C) 2014 Cidorvan Leite

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see [http://www.gnu.org/licenses/].
*/

#include <QDebug>
#include "minimax.h"
#include "board.h"

#define BOARD_SIZE (8 * 8)

#define PUSH(p, px, py) \
    memcpy(state, parent, BOARD_SIZE); \
    state[(py) * 8 + px] = p; \
    state[j * 8 + i] = 0; \
    mm = myTurn(level + 1, minimax, opponent, player);

#define MIN \
    if (mm < minimax) \
        minimax = mm; \
    if (mm < MM) \
        return minimax;

#define MAX(px, py) \
    if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) { \
        minimax = mm; \
        if (mm > MM) \
            return minimax; \
        if (!level) { \
            x = i; y = j; \
            x2 = px; y2 = py; \
        } \
    }

#define PUSH_MIN(p, px, py) { \
    PUSH(p, px, py) \
    MIN \
}

#define PUSH_MAX(p, px, py) { \
    PUSH(p, px, py) \
    MAX(px, py) \
}

#define MAXMIN(px, py) \
    if (player == WHITE) { \
        MIN \
    } else \
        MAX(px, py)

#define PUSH_MAXMIN(p, px, py) { \
    PUSH(p, px, py) \
    MAXMIN(px, py) \
}

#define FOR_PUSH_MAXMIN(p, px, py) { \
    if (parent[(py) * 8 + px] & player) \
        break; \
    PUSH_MAXMIN(p, px, py) \
    if (parent[(py) * 8 + px]) \
        break; \
}

MiniMax::MiniMax(Board *board, QObject *parent) :
    QThread(parent), m_points{0, 1, 5, 3, 3, 10, 100}
{
    m_board = board->current();
}

void MiniMax::setDificulty(int level)
{
    m_level = level;
}

bool MiniMax::isCheck(int x1, int y1, int x2, int y2)
{
    char *next_board = &m_board[BOARD_SIZE];

    m_mmLevel = 2;
    memcpy(next_board, m_board, BOARD_SIZE);
    next_board[y2 * 8 + x2] = next_board[y1 * 8 + x1] & ~SELECT;
    next_board[y1 * 8 + x1] = 0;

    return myTurn(1, MINIMAX, BLACK, WHITE) >= 60;
}

bool MiniMax::isCheck()
{
    char *next_board = &m_board[BOARD_SIZE];

    m_mmLevel = 2;
    memcpy(next_board, m_board, BOARD_SIZE);

    return myTurn(1, MINIMAX, BLACK, WHITE) >= 60;
}

bool MiniMax::isCheckMate(int player, int opponent)
{
    char *next_board = &m_board[BOARD_SIZE];

    m_mmLevel = 3;
    memcpy(next_board, m_board, BOARD_SIZE);

    return qAbs(myTurn(1, player == WHITE ? -MINIMAX : MINIMAX, player, opponent)) >= 60;
}

void MiniMax::cpuPlay()
{
    m_mmLevel = m_level;
    myTurn(0, MINIMAX, BLACK, WHITE);
}

void MiniMax::sumPoints(char *board)
{
    int piece;

    m_black = m_white = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        piece = board[i];
        if (piece & BLACK)
            m_black += m_points[piece & 0x07];
        else if (piece & WHITE)
            m_white += m_points[piece & 0x07];
    }
}

int MiniMax::myTurn(int level, int MM, int player, int opponent)
{
    int piece;
    int minimax, mm;
    int i, j, k, min, x, y, x2, y2;
    char *parent = &m_board[level * BOARD_SIZE];
    char *state = &m_board[(level + 1) * BOARD_SIZE];

    // Calcule pontuação deste caminho
    if (level == m_mmLevel) {
        sumPoints(parent);
        return m_black - m_white;
    }

    // Valor inicial para minimax
    minimax = player == WHITE ? MINIMAX : -MINIMAX;

    // Varre todo o board por jogadas
    for (j = 0; j < 8; j++) for (i = 0; i < 8; i++) {
        piece = parent[j * 8 + i];
        if (piece & player) {

            // Seleciona a peça
            switch (piece & 0x07) {

                case PAWN:
                    if (player == BLACK) {

                        // Tenta comer diagonal esquerda
                        if (i > 0 && j < 7 && parent[(j + 1) * 8 + i - 1] & WHITE)
                            PUSH_MAX(PAWN | BLACK, i - 1, j + 1)

                        // Tenta comer diagonal direita
                        if (i < 7 && j < 7 && parent[(j + 1) * 8 + i + 1] & WHITE)
                            PUSH_MAX(PAWN | BLACK, i + 1, j + 1)

                        // Tenta mover duas casas para frente
                        if (j == 1 && !parent[(j + 1) * 8 + i] && !parent[(j + 2) * 8 + i])
                            PUSH_MAX(PAWN | BLACK, i, j + 2)

                        // Tenta mover uma casa para frente
                        if (j < 7 && !parent[(j + 1) * 8 + i])
                            PUSH_MAX(PAWN | BLACK, i, j + 1)

                    } else {

                        // Tenta comer diagonal esquerda
                        if (i > 0 && j > 0 && parent[(j - 1) * 8 + i - 1] & BLACK)
                            PUSH_MIN(PAWN | WHITE, i - 1, j - 1)

                        // Tenta comer diagonal direita
                        if (i < 7 && j > 0 && parent[(j - 1) * 8 + i + 1] & BLACK)
                            PUSH_MIN(PAWN | WHITE, i + 1, j - 1)

                        // Tenta mover duas casas para frente
                        if (j == 6 && !parent[(j - 1) * 8 + i] && !parent[(j - 2) * 8 + i])
                            PUSH_MIN(PAWN | WHITE, i, j - 2)

                        // Tenta mover uma casa para frente
                        if (j > 0 && !parent[(j - 1) * 8 + i])
                            PUSH_MIN(PAWN | WHITE, i, j - 1)
                    }
                    break;

                // Cavalo
                case KNIGHT:

                    // Diagonal inferior esquerda
                    if (i > 0 && j < 6 && !(parent[(j + 2) * 8 + i - 1] & player))
                        PUSH_MAXMIN(KNIGHT | player, i - 1, j + 2)
                    if (i > 1 && j < 7 && !(parent[(j + 1) * 8 + i - 2] & player))
                        PUSH_MAXMIN(KNIGHT | player, i - 2, j + 1)

                    // Diagonal inferior direita
                    if (i < 7 && j < 6 && !(parent[(j + 2) * 8 + i + 1] & player))
                        PUSH_MAXMIN(KNIGHT | player, i + 1, j + 2)
                    if (i < 6 && j < 7 && !(parent[(j + 1) * 8 + i + 2] & player))
                        PUSH_MAXMIN(KNIGHT | player, i + 2, j + 1)

                    // Diagonal superior direita
                    if (i < 6 && j > 0 && !(parent[(j - 1) * 8 + i + 2] & player))
                        PUSH_MAXMIN(KNIGHT | player, i + 2, j - 1)
                    if (i < 7 && j > 1 && !(parent[(j - 2) * 8 + i + 1] & player))
                        PUSH_MAXMIN(KNIGHT | player, i + 1, j - 2)

                    // Diagonal superior esquerda
                    if (i > 0 && j > 1 && !(parent[(j - 2) * 8 + i - 1] & player))
                        PUSH_MAXMIN(KNIGHT | player, i - 1, j - 2)
                    if (i > 1 && j > 0 && !(parent[(j - 1) * 8 + i - 2] & player))
                        PUSH_MAXMIN(KNIGHT | player, i - 2, j - 1)
                    break;

                // Torre
                case ROOK:

                    // Tenta mover para cima
                    for (k = 1; k <= j; k++)
                        FOR_PUSH_MAXMIN(ROOK | player, i, j - k)

                    // Tenta mover para esquerda
                    for (k = 1; k <= i; k++)
                        FOR_PUSH_MAXMIN(ROOK | player, i - k, j)

                    // Tenta mover para baixo
                    for (k = 1; k <= 7 - j; k++)
                        FOR_PUSH_MAXMIN(ROOK | player, i, j + k)

                    // Tenta mover para direita
                    for (k = 1; k <= 7 - i; k++)
                        FOR_PUSH_MAXMIN(ROOK | player, i + k, j)

                    break;

                // Bispo
                case BISHOP:

                    // Tenta mover para diagonal superior direita
                    min = (j < 7 - i ? j : 7 - i);
                    for (k = 1; k <= min; k++)
                        FOR_PUSH_MAXMIN(BISHOP | player, i + k, j - k)

                    // Tenta mover para diagonal inferior direita
                    min = (7 - j < 7 - i ? 7 - j : 7 - i);
                    for (k = 1; k <= min; k++)
                        FOR_PUSH_MAXMIN(BISHOP | player, i + k, j + k)

                    // Tenta mover para diagonal inferior esquerda
                    min = (7 - j < i ? 7 - j : i);
                    for (k = 1; k <= min; k++)
                        FOR_PUSH_MAXMIN(BISHOP | player, i - k, j + k)

                    // Tenta mover para diagonal superior esquerda
                    min = (j < i ? j : i);
                    for (k = 1; k <= min; k++)
                        FOR_PUSH_MAXMIN(BISHOP | player, i - k, j - k)

                    break;

                // Rainha
                case QUEEN:

                    // Tenta mover para cima
                    for (k = 1; k <= j; k++)
                        FOR_PUSH_MAXMIN(QUEEN | player, i, j - k)

                    // Tenta mover para diagonal superior direita
                    min = (j < 7 - i ? j : 7 - i);
                    for (k = 1; k <= min; k++)
                        FOR_PUSH_MAXMIN(QUEEN | player, i + k, j - k)

                    // Tenta mover para direita
                    for (k = 1; k <= 7 - i; k++)
                        FOR_PUSH_MAXMIN(QUEEN | player, i + k, j)

                    // Tenta mover para diagonal inferior direita
                    min = (7 - j < 7 - i ? 7 - j : 7 - i);
                    for (k = 1; k <= min; k++)
                        FOR_PUSH_MAXMIN(QUEEN | player, i + k, j + k)

                    // Tenta mover para baixo
                    for (k = 1; k <= 7 - j; k++)
                        FOR_PUSH_MAXMIN(QUEEN | player, i, j + k)

                    // Tenta mover para diagonal inferior esquerda
                    min = (7 - j < i ? 7 - j : i);
                    for (k = 1; k <= min; k++)
                        FOR_PUSH_MAXMIN(QUEEN | player, i - k, j + k)

                    // Tenta mover para esquerda
                    for (k = 1; k <= i; k++)
                        FOR_PUSH_MAXMIN(QUEEN | player, i - k, j)

                    // Tenta mover para diagonal superior esquerda
                    min = (j < i ? j : i);
                    for (k = 1; k <= min; k++)
                        FOR_PUSH_MAXMIN(QUEEN | player, i - k, j - k)

                    break;

                // Rei
                case KING:

                    // Tenta mover para cima
                    if (j > 0 && !(parent[(j - 1) * 8 + i] & player))
                        PUSH_MAXMIN(KING | player, i, j - 1)

                    // Tenta mover para diagonal superior direita
                    if (j > 0 && i < 7 && !(parent[(j - 1) * 8 + i + 1] & player))
                        PUSH_MAXMIN(KING | player, i + 1, j - 1)

                    // Tenta mover para direita
                    if (i < 7 && !(parent[j * 8 + i + 1] & player))
                        PUSH_MAXMIN(KING | player, i + 1, j)

                    // Tenta mover para diagonal inferior direita
                    if (j < 7 && i < 7 && !(parent[(j + 1) * 8 + i + 1] & player))
                        PUSH_MAXMIN(KING | player, i + 1, j + 1)

                    // Tenta mover para baixo
                    if (j < 7 && !(parent[(j + 1) * 8 + i] & player))
                        PUSH_MAXMIN(KING | player, i, j + 1)

                    // Tenta mover para diagonal inferior esquerda
                    if (j < 7 && i > 0 && !(parent[(j + 1) * 8 + i - 1] & player))
                        PUSH_MAXMIN(KING | player, i - 1, j + 1)

                    // Tenta mover para esquerda
                    if (i > 0 && !(parent[j * 8 + i - 1] & player))
                        PUSH_MAXMIN(KING | player, i - 1, j)

                    // Tenta mover para diagonal superior esquerda
                    if (j > 0 && i > 0 && !(parent[(j - 1) * 8 + i - 1] & player))
                        PUSH_MAXMIN(KING | player, i - 1, j - 1)

                    break;
            }
        }
    }

    // Coordenadas da melhor jogada
    if (!level) {
        m_x = x; m_y = y;
        m_x2 = x2; m_y2 = y2;
//        animateMove(SLOT(computerMoveFinished()));
    }

    // Retorna MINIMAX deste nível
    return minimax;
}
