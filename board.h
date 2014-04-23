/*
  RPG Studio, A chess game
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

#include <QtGui>

struct STATE {
    int white, black;
    char board[8][8];
};

class Board : public QWidget
{
    Q_OBJECT

public:
    Board(QWidget *parent = 0);
    void newGame(int level);

protected:
    void drawBoard(int mx = -1, int my = -1);
    void movePeace();
    void sumPoints(STATE *estado);
    int myTurn(int level, int MM, int player, int opponent);
    int yourTurn(int x, int y);
    void selectPiece(int x, int y);
    bool gameFinished(int player, int opponent);
    void play(int x, int y);

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:

public slots:

private:
    int m_piece, m_x, m_y, m_x2, m_y2;
    int m_mx, m_my;
    int m_points[7];
    int m_mmLevel, m_level;
    bool m_gameStopped, m_firstClick;
    STATE m_states[10];
    QPixmap m_board, m_black, m_white;
};

#endif
