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

#ifndef MINIMAX_H
#define MINIMAX_H

#include <QThread>

#define MINIMAX 1000

class Board;

class MiniMax : public QThread
{
    Q_OBJECT

public:
    MiniMax(Board *board, QObject *parent = 0);

    void setDificulty(int level);
    bool isCheck(int x1, int y1, int x2, int y2);
    bool isCheck();
    bool isCheckMate(int player, int opponent);
    void cpuPlay();
    int m_x, m_y, m_x2, m_y2;

protected:
    void sumPoints(char *state);
    int myTurn(int level, int MM, int player, int opponent);

signals:
    void cpuDone(int x1, int y1, int x2, int y2);

private:
    int m_points[7];
    int m_mmLevel, m_level;
    int m_black, m_white;
    char *m_board;
};

#endif
