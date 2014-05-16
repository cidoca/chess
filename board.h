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

#ifndef BOARD_H
#define BOARD_H

#define PAWN    0x01
#define ROOK    0x02
#define KNIGHT  0x03
#define BISHOP  0x04
#define QUEEN   0x05
#define KING    0x06
#define WHITE   0x10
#define BLACK   0x20
#define SELECT  0x40
#define MOVE    0x80

class Board {
public:
    Board();

    char *current();
    void reset();
    void clearMoves();
    void findMoves(int x, int y);

protected:
    bool validateMove(int x1, int y1, int x2, int y2);
    bool horizontal(int x1, int y1, int x2);
    bool vertical(int x1, int y1, int y2);
    bool diagonal(int x1, int y1, int x2, int y2, int len);

private:
    char m_board[8 * 8 * 10];
};

#endif
