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

#include "board.h"

#define SIZE    50
#define MINIMAX 1000

#define PAWN    0x01
#define ROOK    0x02
#define KNIGHT  0x03
#define BISHOP  0x04
#define QUEEN   0x05
#define KING    0x06
#define SELECT  0x10
#define WHITE   0x20
#define BLACK   0x40

Board::Board(QWidget *parent) :
    QWidget(parent), m_points{0, 1, 5, 3, 3, 10, 100}, m_gameStopped(true), m_firstClick(true),
    m_board(QPixmap(":/images/board.png")),
    m_black(QPixmap(":/images/black.png")), m_white(QPixmap(":/images/white.png"))
{
}

void Board::newGame(int level) {
    int i;
//    char tit[32];

    // Limpa board
    memset(m_states, 0, sizeof(m_states));

    // Ponhe peças
    for (i = 0; i < 8; i++) {
        m_states[0].board[1][i] = PAWN | BLACK;
        m_states[0].board[6][i] = PAWN | WHITE;
    }
    m_states[0].board[0][0] = m_states[0].board[0][7] = ROOK | BLACK;
    m_states[0].board[7][0] = m_states[0].board[7][7] = ROOK | WHITE;
    m_states[0].board[0][1] = m_states[0].board[0][6] = KNIGHT | BLACK;
    m_states[0].board[7][1] = m_states[0].board[7][6] = KNIGHT | WHITE;
    m_states[0].board[0][2] = m_states[0].board[0][5] = BISHOP | BLACK;
    m_states[0].board[7][2] = m_states[0].board[7][5] = BISHOP | WHITE;
    m_states[0].board[0][3] = QUEEN | BLACK;
    m_states[0].board[7][3] = QUEEN | WHITE;
    m_states[0].board[0][4] = KING | BLACK;
    m_states[0].board[7][4] = KING | WHITE;

    // Vamos jogar!
    m_firstClick = true;
    m_gameStopped = false;
    m_level = level;
    update();
/*        strcpy(tit, szAppName);
        if (level == FACIL) strcat(tit, " (Fácil)");
        else if (level == NORMAL) strcat(tit, " (Normal)");
        else strcat(tit, " (Difícil)");
        SetWindowText(hDlg, tit); */
}

void Board::animateMove(const char *method)
{
    QPropertyAnimation *anim;
    QSequentialAnimationGroup *move = new QSequentialAnimationGroup;

    m_gameStopped = true;
    m_piece = m_states[0].board[m_y][m_x] & ~SELECT;
    m_states[0].board[m_y][m_x] = 0;
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

void Board::sumPoints(STATE *state) {
    int x, y, piece;

    // Soma todas as peças
    state->white = state->black = 0;
    for (y = 0; y < 8; y++) for (x = 0; x < 8; x++) {
        piece = state->board[y][x];
        if (piece & BLACK) state->black += m_points[piece & 0x07];
        else if (piece & WHITE) state->white += m_points[piece & 0x07];
    }
}

int Board::myTurn(int level, int MM, int player, int opponent) {
    int minimax, mm;
    int i, j, k, min, x, y, x2, y2;
    int piece;
    STATE *parent = &m_states[level], *state = &m_states[level + 1];

    // Calcule pontuação deste caminho
    if (level == m_mmLevel) {
        sumPoints(parent);
        return parent->black - parent->white;
    }

    // Valor inicial para minimax
    minimax = player == WHITE ? MINIMAX : -MINIMAX;

    // Varre todo o board por jogadas
    for (j = 0; j < 8; j++) for (i = 0; i < 8; i++) {
        piece = parent->board[j][i];
        if (piece & player) {

            // Seleciona a peça
            switch (piece & 0x07) {

                // Pião
                case PAWN:

                    // Branco
                    if (player == BLACK) {

                        // Tenta comer diagonal esquerda
                        if (i > 0 && j < 7 && parent->board[j + 1][i - 1] & WHITE) {
                            memcpy(state, parent, sizeof(STATE));
                            state->board[j + 1][i - 1] = PAWN | BLACK;
                            state->board[j][i] = 0;
                            mm = myTurn(level + 1, minimax, opponent, player);

                            // Calcula máximo ou mínimo
                            if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                                minimax = mm;
                                if (mm > MM) return minimax;
                                if (!level) {
                                    x = i; y = j;
                                    x2 = i - 1; y2 = j + 1;
                                }
                            }
                        }

                        // Tenta comer diagonal direita
                        if (i < 7 && j < 7 && parent->board[j + 1][i + 1] & WHITE) {
                            memcpy(state, parent, sizeof(STATE));
                            state->board[j + 1][i + 1] = PAWN | BLACK;
                            state->board[j][i] = 0;
                            mm = myTurn(level + 1, minimax, opponent, player);

                            // Calcula máximo ou mínimo
                            if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                                minimax = mm;
                                if (mm > MM) return minimax;
                                if (!level) {
                                    x = i; y = j;
                                    x2 = i + 1; y2 = j + 1;
                                }
                            }
                        }

                        // Tenta mover duas casas para frente
                        if (j == 1 && !parent->board[j + 1][i] && !parent->board[j + 2][i]) {
                            memcpy(state, parent, sizeof(STATE));
                            state->board[j + 2][i] = PAWN | BLACK;
                            state->board[j][i] = 0;
                            mm = myTurn(level + 1, minimax, opponent, player);

                            // Calcula máximo ou mínimo
                            if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                                minimax = mm;
                                if (mm > MM) return minimax;
                                if (!level) {
                                    x = i; y = j;
                                    x2 = i; y2 = j + 2;
                                }
                            }
                        }

                        // Tenta mover uma casa para frente
                        if (j < 7 && !parent->board[j + 1][i]) {
                            memcpy(state, parent, sizeof(STATE));
                            state->board[j + 1][i] = PAWN | BLACK;
                            state->board[j][i] = 0;
                            mm = myTurn(level + 1, minimax, opponent, player);

                            // Calcula máximo ou mínimo
                            if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                                minimax = mm;
                                if (mm > MM) return minimax;
                                if (!level) {
                                    x = i; y = j;
                                    x2 = i; y2 = j + 1;
                                }
                            }
                        }

                    // Preto
                    } else {

                        // Tenta comer diagonal esquerda
                        if (i > 0 && j > 0 && parent->board[j - 1][i - 1] & BLACK) {
                            memcpy(state, parent, sizeof(STATE));
                            state->board[j - 1][i - 1] = PAWN | WHITE;
                            state->board[j][i] = 0;
                            mm = myTurn(level + 1, minimax, opponent, player);

                            // Calcula mínimo
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        }

                        // Tenta comer diagonal direita
                        if (i < 7 && j > 0 && parent->board[j - 1][i + 1] & BLACK) {
                            memcpy(state, parent, sizeof(STATE));
                            state->board[j - 1][i + 1] = PAWN | WHITE;
                            state->board[j][i] = 0;
                            mm = myTurn(level + 1, minimax, opponent, player);

                            // Calcula mínimo
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        }

                        // Tenta mover duas casas para frente
                        if (j == 6 && !parent->board[j - 1][i] && !parent->board[j - 2][i]) {
                            memcpy(state, parent, sizeof(STATE));
                            state->board[j - 2][i] = PAWN | WHITE;
                            state->board[j][i] = 0;
                            mm = myTurn(level + 1, minimax, opponent, player);

                            // Calcula mínimo
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        }

                        // Tenta mover uma casa para frente
                        if (j > 0 && !parent->board[j - 1][i]) {
                            memcpy(state, parent, sizeof(STATE));
                            state->board[j - 1][i] = PAWN | WHITE;
                            state->board[j][i] = 0;
                            mm = myTurn(level + 1, minimax, opponent, player);

                            // Calcula mínimo
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        }
                    }
                    break;

                // Cavalo
                case KNIGHT:

                    // Diagonal inferior esquerda
                    if (i > 0 && j < 6 && !(parent->board[j + 2][i - 1] & player)) {
                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + 2][i - 1] = KNIGHT | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - 1; y2 = j + 2;
                            }
                        }
                    }
                    if (i > 1 && j < 7 && !(parent->board[j + 1][i - 2] & player)) {
                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + 1][i - 2] = KNIGHT | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - 2; y2 = j + 1;
                            }
                        }
                    }

                    // Diagonal inferior direita
                    if (i < 7 && j < 6 && !(parent->board[j + 2][i + 1] & player)) {
                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + 2][i + 1] = KNIGHT | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + 1; y2 = j + 2;
                            }
                        }
                    }
                    if (i < 6 && j < 7 && !(parent->board[j + 1][i + 2] & player)) {
                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + 1][i + 2] = KNIGHT | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + 2; y2 = j + 1;
                            }
                        }
                    }

                    // Diagonal superior direita
                    if (i < 6 && j > 0 && !(parent->board[j - 1][i + 2] & player)) {
                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - 1][i + 2] = KNIGHT | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + 2; y2 = j - 1;
                            }
                        }
                    }
                    if (i < 7 && j > 1 && !(parent->board[j - 2][i + 1] & player)) {
                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - 2][i + 1] = KNIGHT | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + 1; y2 = j - 2;
                            }
                        }
                    }

                    // Diagonal superior esquerda
                    if (i > 0 && j > 1 && !(parent->board[j - 2][i - 1] & player)) {
                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - 2][i - 1] = KNIGHT | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - 1; y2 = j - 2;
                            }
                        }
                    }
                    if (i > 1 && j > 0 && !(parent->board[j - 1][i - 2] & player)) {
                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - 1][i - 2] = KNIGHT | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - 2; y2 = j - 1;
                            }
                        }
                    }
                    break;

                // Torre
                case ROOK:

                    // Tenta mover para cima
                    for (k = 1; k <= j; k++) {
                        if (parent->board[j - k][i] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - k][i] = ROOK | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i; y2 = j - k;
                            }
                        }

                        if (parent->board[j - k][i]) break;
                    }

                    // Tenta mover para esquerda
                    for (k = 1; k <= i; k++) {
                        if (parent->board[j][i - k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j][i - k] = ROOK | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - k; y2 = j;
                            }
                        }

                        if (parent->board[j][i - k]) break;
                    }

                    // Tenta mover para baixo
                    for (k = 1; k <= 7 - j; k++) {
                        if (parent->board[j + k][i] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + k][i] = ROOK | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i; y2 = j + k;
                            }
                        }

                        if (parent->board[j + k][i]) break;
                    }

                    // Tenta mover para direita
                    for (k = 1; k <= 7 - i; k++) {
                        if (parent->board[j][i + k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j][i + k] = ROOK | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + k; y2 = j;
                            }
                        }

                        if (parent->board[j][i + k]) break;
                    }

                    break;

                // Bispo
                case BISHOP:

                    // Tenta mover para diagonal superior direita
                    min = (j < 7 - i ? j : 7 - i);
                    for (k = 1; k <= min; k++) {
                        if (parent->board[j - k][i + k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - k][i + k] = BISHOP | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + k; y2 = j - k;
                            }
                        }

                        if (parent->board[j - k][i + k]) break;
                    }

                    // Tenta mover para diagonal inferior direita
                    min = (7 - j < 7 - i ? 7 - j : 7 - i);
                    for (k = 1; k <= min; k++) {
                        if (parent->board[j + k][i + k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + k][i + k] = BISHOP | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + k; y2 = j + k;
                            }
                        }

                        if (parent->board[j + k][i + k]) break;
                    }

                    // Tenta mover para diagonal inferior esquerda
                    min = (7 - j < i ? 7 - j : i);
                    for (k = 1; k <= min; k++) {
                        if (parent->board[j + k][i - k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + k][i - k] = BISHOP | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - k; y2 = j + k;
                            }
                        }

                        if (parent->board[j + k][i - k]) break;
                    }

                    // Tenta mover para diagonal superior esquerda
                    min = (j < i ? j : i);
                    for (k = 1; k <= min; k++) {
                        if (parent->board[j - k][i - k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - k][i - k] = BISHOP | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - k; y2 = j - k;
                            }
                        }

                        if (parent->board[j - k][i - k]) break;
                    }

                    break;

                // Rainha
                case QUEEN:

                    // Tenta mover para cima
                    for (k = 1; k <= j; k++) {
                        if (parent->board[j - k][i] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - k][i] = QUEEN | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i; y2 = j - k;
                            }
                        }

                        if (parent->board[j - k][i]) break;
                    }

                    // Tenta mover para diagonal superior direita
                    min = (j < 7 - i ? j : 7 - i);
                    for (k = 1; k <= min; k++) {
                        if (parent->board[j - k][i + k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - k][i + k] = QUEEN | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + k; y2 = j - k;
                            }
                        }

                        if (parent->board[j - k][i + k]) break;
                    }

                    // Tenta mover para direita
                    for (k = 1; k <= 7 - i; k++) {
                        if (parent->board[j][i + k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j][i + k] = QUEEN | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + k; y2 = j;
                            }
                        }

                        if (parent->board[j][i + k]) break;
                    }

                    // Tenta mover para diagonal inferior direita
                    min = (7 - j < 7 - i ? 7 - j : 7 - i);
                    for (k = 1; k <= min; k++) {
                        if (parent->board[j + k][i + k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + k][i + k] = QUEEN | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + k; y2 = j + k;
                            }
                        }

                        if (parent->board[j + k][i + k]) break;
                    }

                    // Tenta mover para baixo
                    for (k = 1; k <= 7 - j; k++) {
                        if (parent->board[j + k][i] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + k][i] = QUEEN | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i; y2 = j + k;
                            }
                        }

                        if (parent->board[j + k][i]) break;
                    }

                    // Tenta mover para diagonal inferior esquerda
                    min = (7 - j < i ? 7 - j : i);
                    for (k = 1; k <= min; k++) {
                        if (parent->board[j + k][i - k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + k][i - k] = QUEEN | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - k; y2 = j + k;
                            }
                        }

                        if (parent->board[j + k][i - k]) break;
                    }

                    // Tenta mover para esquerda
                    for (k = 1; k <= i; k++) {
                        if (parent->board[j][i - k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j][i - k] = QUEEN | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - k; y2 = j;
                            }
                        }

                        if (parent->board[j][i - k]) break;
                    }

                    // Tenta mover para diagonal superior esquerda
                    min = (j < i ? j : i);
                    for (k = 1; k <= min; k++) {
                        if (parent->board[j - k][i - k] & player) break;

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - k][i - k] = QUEEN | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - k; y2 = j - k;
                            }
                        }

                        if (parent->board[j - k][i - k]) break;
                    }

                    break;

                // Rei
                case KING:

                    // Tenta mover para cima
                    if (j > 0 && !(parent->board[j - 1][i] & player)) {

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - 1][i] = KING | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i; y2 = j - 1;
                            }
                        }
                    }

                    // Tenta mover para diagonal superior direita
                    if (j > 0 && i < 7 && !(parent->board[j - 1][i + 1] & player)) {

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - 1][i + 1] = KING | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + 1; y2 = j - 1;
                            }
                        }
                    }

                    // Tenta mover para direita
                    if (i < 7 && !(parent->board[j][i + 1] & player)) {

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j][i + 1] = KING | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + 1; y2 = j;
                            }
                        }
                    }

                    // Tenta mover para diagonal inferior direita
                    if (j < 7 && i < 7 && !(parent->board[j + 1][i + 1] & player)) {

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + 1][i + 1] = KING | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i + 1; y2 = j + 1;
                            }
                        }
                    }

                    // Tenta mover para baixo
                    if (j < 7 && !(parent->board[j + 1][i] & player)) {

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + 1][i] = KING | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i; y2 = j + 1;
                            }
                        }
                    }

                    // Tenta mover para diagonal inferior esquerda
                    if (j < 7 && i > 0 && !(parent->board[j + 1][i - 1] & player)) {

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j + 1][i - 1] = KING | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - 1; y2 = j + 1;
                            }
                        }
                    }

                    // Tenta mover para esquerda
                    if (i > 0 && !(parent->board[j][i - 1] & player)) {

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j][i - 1] = KING | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - 1; y2 = j;
                            }
                        }
                    }

                    // Tenta mover para diagonal superior esquerda
                    if (j > 0 && i > 0 && !(parent->board[j - 1][i - 1] & player)) {

                        memcpy(state, parent, sizeof(STATE));
                        state->board[j - 1][i - 1] = KING | player;
                        state->board[j][i] = 0;
                        mm = myTurn(level + 1, minimax, opponent, player);

                        // Calcula máximo ou mínimo
                        if (player == WHITE) {
                            if (mm < minimax) minimax = mm;
                            if (mm < MM) return minimax;
                        } else if (mm > minimax || (!level && mm == minimax && !(rand() % 4))) {
                            minimax = mm;
                            if (mm > MM) return minimax;
                            if (!level) {
                                x = i; y = j;
                                x2 = i - 1; y2 = j - 1;
                            }
                        }
                    }

                    break;
            }
        }
    }

    // Coordenadas da melhor jogada
    if (!level) {
        m_x = x; m_y = y;
        m_x2 = x2; m_y2 = y2;
        animateMove(SLOT(computerMoveFinished()));
    }

    // Retorna MINIMAX deste nível
    return minimax;
}

bool Board::validateMove() {
    int i, j;
    STATE *state = &m_states[0];

    // Valida o movimento da peça escolhida
    switch (state->board[m_y][m_x] & 0x07) {

        case PAWN:

            // Movimento para frente
            if (m_x == m_x2 && (m_y - m_y2 == 1 || (m_y - m_y2 == 2 &&
                             m_y == 6 && !state->board[5][m_x])))
                return true;

            // Movimento para diagonal comendo
            if (qAbs(m_x - m_x2) == 1 && m_y - m_y2 == 1 && state->board[m_y2][m_x2] & BLACK)
                return true;

            return false;

        case ROOK:

            // Verifica se moveu na horizontal ou vertical
            if (m_x != m_x2 && m_y != m_y2)
                return false;

            // Verifica se há alguma peça no caminho (vertical)
            if (m_x == m_x2) {
                if (m_y > m_y2) {
                    for (i = m_y - 1; i > m_y2; i--)
                        if (state->board[i][m_x])
                            return false;
                } else {
                    for (i = m_y + 1; i < m_y2; i++)
                        if (state->board[i][m_x])
                            return false;
                }

            // Verifica se há alguma peça no caminho (horizontal)
            } else {
                if (m_x > m_x2) {
                    for (i = m_x - 1; i > m_x2; i--)
                        if (state->board[m_y][i])
                            return false;
                } else {
                    for (i = m_x + 1; i < m_x2; i++)
                        if (state->board[m_y][i])
                            return false;
                }
            }

            return true;

        case KNIGHT:

            // Verifica se é um "L" válido
            return ((qAbs(m_x - m_x2) == 2 && qAbs(m_y - m_y2) == 1) ||
                    (qAbs(m_x - m_x2) == 1 && qAbs(m_y - m_y2) == 2));

        case BISHOP:

            // Verifica se moveu na diagonal
            j = qAbs(m_x - m_x2);
            if (j != qAbs(m_y - m_y2))
                return false;

            // Verifica se há alguma peça no caminho diagonal
            for (i = 1; i < j; i++) {
                if (m_x < m_x2) {
                    if (m_y > m_y2) {
                        if (state->board[m_y - i][m_x + i])
                            return false;
                    } else {
                        if (state->board[m_y + i][m_x + i])
                            return false;
                    }
                } else {
                    if (m_y > m_y2) {
                        if (state->board[m_y - i][m_x - i])
                            return false;
                    } else {
                        if (state->board[m_y + i][m_x - i])
                            return false;
                    }
                }
            }

            return true;

        case QUEEN:

            // Verifica se moveu na horizontal, vertical ou diagonal
            j = qAbs(m_x - m_x2);
            if (m_x != m_x2 && m_y != m_y2 && j != qAbs(m_y - m_y2))
                return false;

            // Verifica se há alguma peça no caminho (vertical)
            if (m_x == m_x2) {
                if (m_y > m_y2) {
                    for (i = m_y - 1; i > m_y2; i--)
                        if (state->board[i][m_x])
                            return false;
                } else {
                    for (i = m_y + 1; i < m_y2; i++)
                        if (state->board[i][m_x])
                            return false;
                }

            // Verifica se há alguma peça no caminho (horizontal)
            } else if (m_y == m_y2) {
                if (m_x > m_x2) {
                    for (i = m_x - 1; i > m_x2; i--)
                        if (state->board[m_y][i])
                            return false;
                } else {
                    for (i = m_x + 1; i < m_x2; i++)
                        if (state->board[m_y][i])
                            return false;
                }

            // Verifica se há alguma peça no caminho diagonal
            } else for (i = 1; i < j; i++) {
                if (m_x < m_x2) {
                    if (m_y > m_y2) {
                        if (state->board[m_y - i][m_x + i])
                            return false;
                    } else {
                        if (state->board[m_y + i][m_x + i])
                            return false;
                    }
                } else {
                    if (m_y > m_y2) {
                        if (state->board[m_y - i][m_x - i])
                            return false;
                    } else {
                        if (state->board[m_y + i][m_x - i])
                            return false;
                    }
                }
            }

            return true;

        case KING:
            return !(qAbs(m_x - m_x2) > 1 || qAbs(m_y - m_y2) > 1);
    }

    return false;
}

bool Board::gameFinished(int player, int opponent) {

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
}

void Board::play() {
    if (!validateMove()) {
        QMessageBox::warning(this, "Chess", "Jogada inválida! Tente outra vez.");
        return;
    }

    // Verifica se ponhe o rei em cheque
    m_mmLevel = 2;
    memcpy(&m_states[1], &m_states[0], sizeof(STATE));
    m_states[1].board[m_y2][m_x2] = m_states[1].board[m_y][m_x] & ~SELECT;
    m_states[1].board[m_y][m_x] = 0;
    if (myTurn(1, MINIMAX, BLACK, WHITE) >= 60) {
        QMessageBox::warning(this, "Chess", "Jogada inválida! Ela ponhe seu rei em cheque.");
        return;
    }

    animateMove(SLOT(playerMoveFinished()));
}

void Board::playerMoveFinished() {
    m_position.setX(0);
    m_position.setY(0);
    m_states[0].board[m_y2][m_x2] = m_piece;
    m_gameStopped = false;

    if (!gameFinished(WHITE, BLACK)) {
        m_mmLevel = m_level;
        myTurn(0, MINIMAX, BLACK, WHITE);
    }
}

void Board::computerMoveFinished() {
    m_position.setX(0);
    m_position.setY(0);
    m_states[0].board[m_y2][m_x2] = m_piece;
    m_gameStopped = false;

    gameFinished(BLACK, WHITE);
}

QPoint Board::position() const
{
    return m_position;
}

void Board::setPosition(const QPoint &pos)
{
    m_position = pos;
    update();
}

void Board::paintEvent(QPaintEvent *)
{
    int x, y;
    QPainter painter(this);

    painter.setPen(QPen(Qt::red, 2));
    painter.fillRect(0, 0, width() - 1, height() - 1, m_board);

    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            int piece = m_states[0].board[y][x];

            if (piece & SELECT)
                painter.drawRect(x * SIZE, y * SIZE, SIZE, SIZE);

            if (piece)
                painter.drawPixmap(x * SIZE + 1, y * SIZE + 1, piece & BLACK ? m_black : m_white,
                                   ((piece & 0x07) - 1) * 48, 0, 48, 48);
        }
    }

    if (!m_position.isNull())
        painter.drawPixmap(m_position.x() + 1, m_position.y() + 1, m_piece & BLACK ? m_black : m_white,
                           ((m_piece & 0x07) - 1) * 48, 0, 48, 48);
}

void Board::mousePressEvent(QMouseEvent *event)
{
    if (!m_gameStopped) {
        int x = event->x() / SIZE, y = event->y() / SIZE;

        if (m_firstClick) {
            if (!(m_states[0].board[y][x] & WHITE))
                QMessageBox::warning(this, "Chess", "Please, select a white piece.");
            else {
                m_x = x;
                m_y = y;
                m_firstClick = false;
                m_states[0].board[y][x] |= SELECT;
                update();
            }
        } else if (m_x == x && m_y == y) {
            m_firstClick = true;
            m_states[0].board[y][x] &= ~SELECT;
            update();
        } else if (m_states[0].board[y][x] & WHITE) {
            m_states[0].board[m_y][m_x] &= ~SELECT;
            m_x = x;
            m_y = y;
            m_states[0].board[y][x] |= SELECT;
            update();
        } else {
            m_x2 = x;
            m_y2 = y;
            play();
        }
    }
}
