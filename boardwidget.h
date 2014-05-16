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

#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QtGui>

class Board;
class MiniMax;

class BoardWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPoint position READ position WRITE setPosition)

public:
    BoardWidget(QWidget *parent = 0);
    void newGame(int level);

protected:
    void animateMove(const char *method);
    void play();
    QPoint position() const;
    void setPosition(const QPoint &pos);

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

public slots:
    void playerMoveFinished();
    void computerMoveFinished();

signals:

private:
    int m_piece, m_x, m_y, m_x2, m_y2;
    bool m_gameStopped, m_firstClick;
    QPoint m_position;
    Board *m_board;
    MiniMax *m_minimax;
    QPixmap m_background, m_black, m_white;
    char *m_currentBoard;
};

#endif
