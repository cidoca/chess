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

#include "board.h"
#include "minimax.h"
#include "boardwidget.h"

#define SIZE 50

BoardWidget::BoardWidget(QWidget *parent) :
    QWidget(parent), m_gameStopped(true), m_firstClick(true),
    m_background(QPixmap(":/images/background.png")),
    m_black(QPixmap(":/images/black.png")), m_white(QPixmap(":/images/white.png"))
{
    m_board = new Board;
    m_minimax = new MiniMax(m_board);
    m_currentBoard = m_board->current();
}

void BoardWidget::newGame(int level)
{
    m_firstClick = true;
    m_gameStopped = false;
    m_board->reset();
    m_minimax->setDificulty(level);
    update();
}

void BoardWidget::animateMove(const char *method)
{
    QPropertyAnimation *anim;
    QSequentialAnimationGroup *move = new QSequentialAnimationGroup;

    m_gameStopped = true;
    m_piece = m_currentBoard[m_y * 8 + m_x] & ~SELECT;
    m_currentBoard[m_y * 8 + m_x] = 0;
    connect(move, SIGNAL(finished()), this, method);

    // Movimentos do cavalo
    if ((m_piece & 0x07) == KNIGHT) {
        if (qAbs(m_x2 - m_x) > qAbs(m_y2 - m_y)) {
            anim = new QPropertyAnimation(this, "position");
            anim->setEasingCurve(QEasingCurve::OutCubic);
            anim->setDuration(3 * 200);
            anim->setStartValue(QPoint(m_x * SIZE, m_y * SIZE));
            anim->setEndValue(QPoint(m_x2 * SIZE, m_y * SIZE));
            move->addAnimation(anim);

            anim = new QPropertyAnimation(this, "position");
            anim->setEasingCurve(QEasingCurve::OutCubic);
            anim->setDuration(2 * 200);
            anim->setStartValue(QPoint(m_x2 * SIZE, m_y * SIZE));
            anim->setEndValue(QPoint(m_x2 * SIZE, m_y2 * SIZE));
        } else {
            anim = new QPropertyAnimation(this, "position");
            anim->setEasingCurve(QEasingCurve::OutCubic);
            anim->setDuration(3 * 200);
            anim->setStartValue(QPoint(m_x * SIZE, m_y * SIZE));
            anim->setEndValue(QPoint(m_x * SIZE, m_y2 * SIZE));
            move->addAnimation(anim);

            anim = new QPropertyAnimation(this, "position");
            anim->setEasingCurve(QEasingCurve::OutCubic);
            anim->setDuration(2 * 200);
            anim->setStartValue(QPoint(m_x * SIZE, m_y2 * SIZE));
            anim->setEndValue(QPoint(m_x2 * SIZE, m_y2 * SIZE));
        }

    // Demais peças
    } else {
        anim = new QPropertyAnimation(this, "position");
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->setDuration(qMax(qAbs(m_x2 - m_x), qAbs(m_y2 - m_y)) * 200);
        anim->setStartValue(QPoint(m_x * SIZE, m_y * SIZE));
        anim->setEndValue(QPoint(m_x2 * SIZE, m_y2 * SIZE));
    }

    move->addAnimation(anim);
    move->start(QAbstractAnimation::DeleteWhenStopped);
}

/*bool BoardWidget::gameFinished(int player, int opponent)
{

    // Verifica se é cheque
    m_mmLevel = 2;
    memcpy(&m_states[1], &m_states[0], sizeof(STATE));
    myTurn(1, player == WHITE ? -MINIMAX : MINIMAX, player, opponent);
    if (m_states[2].white < 100 || m_states[2].black < 100) {

        // Verifica se é cheque mate
        m_mmLevel = 3;
        memcpy(&m_states[1], &m_states[0], sizeof(STATE));
        if (qAbs(myTurn(1, opponent == WHITE ? -MINIMAX : MINIMAX, opponent, player)) >= 60) {
            m_gameStopped = true;
            QMessageBox::warning(this, "Chess", "Cheque Mate!!!");
            return true;
        }

        // É apenas cheque
        QMessageBox::warning(this, "Chess", "Cheque !!!");
    }

    // Ninguém venceu
    return false;
}*/

void BoardWidget::play()
{
    m_board->clearMoves();
    if (m_minimax->isCheck(m_x, m_y, m_x2, m_y2)) {
        m_board->findMoves(m_x, m_y);
        QMessageBox::warning(this, "Chess", "Invalid move! Your king is in check.");
        return;
    }

    animateMove(SLOT(playerMoveFinished()));
}

void BoardWidget::playerMoveFinished()
{
    m_position.setX(0);
    m_position.setY(0);
    m_currentBoard[m_y2 * 8 + m_x2] = m_piece;

    if (m_minimax->isCheckMate(BLACK, WHITE)) {
        QMessageBox::warning(this, "Chess", "Check mate !!!");
        return;
    }

    m_minimax->cpuPlay();

    m_x = m_minimax->m_x;
    m_y = m_minimax->m_y;
    m_x2 = m_minimax->m_x2;
    m_y2 = m_minimax->m_y2;
    animateMove(SLOT(computerMoveFinished()));

}

void BoardWidget::computerMoveFinished()
{
    m_position.setX(0);
    m_position.setY(0);
    m_currentBoard[m_y2 * 8 + m_x2] = m_piece;

    if (m_minimax->isCheckMate(WHITE, BLACK)) {
        QMessageBox::warning(this, "Chess", "Check mate !!!");
        return;
    } else if (m_minimax->isCheck())
        QMessageBox::warning(this, "Chess", "Check !!!");

    m_gameStopped = false;
}

QPoint BoardWidget::position() const
{
    return m_position;
}

void BoardWidget::setPosition(const QPoint &pos)
{
    m_position = pos;
    update();
}

void BoardWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(0, 0, width() - 1, height() - 1, m_background);

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int piece = m_currentBoard[y * 8 + x];

            if (piece & SELECT) {
                painter.setPen(QPen(Qt::red, 2));
                painter.drawRect(x * SIZE, y * SIZE, SIZE, SIZE);
            } else if (piece & MOVE) {
                painter.setPen(QPen(Qt::blue, 2));
                painter.drawRect(x * SIZE, y * SIZE, SIZE, SIZE);
            }

            if (piece)
                painter.drawPixmap(x * SIZE + 1, y * SIZE + 1, piece & BLACK ? m_black : m_white,
                                   ((piece & 0x07) - 1) * 48, 0, 48, 48);
        }
    }

    if (!m_position.isNull())
        painter.drawPixmap(m_position.x() + 1, m_position.y() + 1, m_piece & BLACK ? m_black : m_white,
                           ((m_piece & 0x07) - 1) * 48, 0, 48, 48);
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_gameStopped) {
        int x = event->x() / SIZE, y = event->y() / SIZE;

        if (m_firstClick) {
            if (!(m_currentBoard[y * 8 + x] & WHITE))
                QMessageBox::warning(this, "Chess", "Please, select a white piece.");
            else {
                m_x = x;
                m_y = y;
                m_firstClick = false;
                m_board->findMoves(x, y);
                update();
            }
        } else if (m_x == x && m_y == y) {
            m_firstClick = true;
            m_board->clearMoves();
            update();
        } else if (m_currentBoard[y * 8 + x] & WHITE) {
            m_x = x;
            m_y = y;
            m_board->findMoves(x, y);
            update();
        } else if (!(m_currentBoard[y * 8 + x] & MOVE))
            QMessageBox::warning(this, "Chess", "Invalid move! Please, try again.");
        else {
            m_x2 = x;
            m_y2 = y;
            play();
        }
    }
}
