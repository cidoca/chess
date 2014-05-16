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

#include <string.h>
#include <stdlib.h>
#include "board.h"

Board::Board()
{
    reset();
}

char *Board::current()
{
    return m_board;
}

void Board::reset()
{
    memset(m_board, 0, sizeof(m_board));

    for (int i = 0; i < 8; i++) {
        m_board[1 * 8 + i] = PAWN | BLACK;
        m_board[6 * 8 + i] = PAWN | WHITE;
    }
    m_board[0 * 8 + 0] = m_board[0 * 8 + 7] = ROOK | BLACK;
    m_board[7 * 8 + 0] = m_board[7 * 8 + 7] = ROOK | WHITE;
    m_board[0 * 8 + 1] = m_board[0 * 8 + 6] = KNIGHT | BLACK;
    m_board[7 * 8 + 1] = m_board[7 * 8 + 6] = KNIGHT | WHITE;
    m_board[0 * 8 + 2] = m_board[0 * 8 + 5] = BISHOP | BLACK;
    m_board[7 * 8 + 2] = m_board[7 * 8 + 5] = BISHOP | WHITE;
    m_board[0 * 8 + 3] = QUEEN | BLACK;
    m_board[7 * 8 + 3] = QUEEN | WHITE;
    m_board[0 * 8 + 4] = KING | BLACK;
    m_board[7 * 8 + 4] = KING | WHITE;
}

void Board::clearMoves()
{
    for (int i = 0; i < 8 * 8; i++)
        m_board[i] &= ~(MOVE | SELECT);
}

void Board::findMoves(int x, int y)
{
    clearMoves();
    m_board[y * 8 + x] |= SELECT;

    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            if (validateMove(x, y, i, j))
                m_board[j * 8 + i] |= MOVE;
        }
    }
}

bool Board::validateMove(int x1, int y1, int x2, int y2)
{
    int len;

    if ((x1 == x2 && y1 == y2) || m_board[y2 * 8 + x2] & WHITE)
        return false;

    switch (m_board[y1 * 8 + x1] & 0x07) {

        case PAWN:
            // Moving forward
            if (x1 == x2 && ((y1 - y2 == 1 && !(m_board[y2 * 8 + x1] & 0x07)) || (y1 - y2 == 2 &&
                      y1 == 6 && !(m_board[5 * 8 + x1] & 0x07) && !(m_board[4 * 8 + x1] & 0x07))))
                return true;

            // Capturing move
            if (abs(x1 - x2) == 1 && y1 - y2 == 1 && m_board[y2 * 8 + x2] & BLACK)
                return true;

            return false;

        case ROOK:
            // Only vertical or horizontal move
            if (x1 != x2 && y1 != y2)
                return false;

            // Vertical way is free?
            if (x1 == x2)
                return vertical(x1, y1, y2);

            // Horizontal way is free?
            else
                return horizontal(x1, y1, x2);

        case KNIGHT:
            // Is it a "L" shape?
            return ((abs(x1 - x2) == 2 && abs(y1 - y2) == 1) ||
                    (abs(x1 - x2) == 1 && abs(y1 - y2) == 2));

        case BISHOP:
            // Only diagonal way
            len = abs(x1 - x2);
            if (len != abs(y1 - y2))
                return false;

            // Diagonal way is free?
            return diagonal(x1, y1, x2, y2, len);

        case QUEEN:
            // Diagonal, vertical or horizontal
            len = abs(x1 - x2);
            if (x1 != x2 && y1 != y2 && len != abs(y1 - y2))
                return false;

            // Vertical way is free?
            if (x1 == x2)
                return vertical(x1, y1, y2);

            // Horizontal way is free?
            else if (y1 == y2)
                return horizontal(x1, y1, x2);

            // Diagonal way is free?
            else
                return diagonal(x1, y1, x2, y2, len);

        case KING:
            // Only one square
            return !(abs(x1 - x2) > 1 || abs(y1 - y2) > 1);
    }

    return false;
}

bool Board::horizontal(int x1, int y1, int x2)
{
    if (x1 > x2) {
        for (int i = x1 - 1; i > x2; i--)
            if (m_board[y1 * 8 + i] & 0x07)
                return false;
    } else {
        for (int i = x1 + 1; i < x2; i++)
            if (m_board[y1 * 8 + i] & 0x07)
                return false;
    }

    return true;
}

bool Board::vertical(int x1, int y1, int y2)
{
    if (y1 > y2) {
        for (int i = y1 - 1; i > y2; i--)
            if (m_board[i * 8 + x1] & 0x07)
                return false;
    } else {
        for (int i = y1 + 1; i < y2; i++)
            if (m_board[i * 8 + x1] & 0x07)
                return false;
    }

    return true;
}

bool Board::diagonal(int x1, int y1, int x2, int y2, int len)
{
    for (int i = 1; i < len; i++) {
        if (x1 < x2) {
            if (y1 > y2) {
                if (m_board[(y1 - i) * 8 + x1 + i] & 0x07)
                    return false;
            } else {
                if (m_board[(y1 + i) * 8 + x1 + i] & 0x07)
                    return false;
            }
        } else {
            if (y1 > y2) {
                if (m_board[(y1 - i) * 8 + x1 - i] & 0x07)
                    return false;
            } else {
                if (m_board[(y1 + i) * 8 + x1 - i] & 0x07)
                    return false;
            }
        }
    }

    return true;
}
