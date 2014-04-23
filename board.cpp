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
    char tit[32];

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
    if (level) {
        drawBoard();
        m_gameStopped = false;
        m_mmLevel = m_level = level;
/*        strcpy(tit, szAppName);
        if (level == FACIL) strcat(tit, " (Fácil)");
        else if (level == NORMAL) strcat(tit, " (Normal)");
        else strcat(tit, " (Difícil)");
        SetWindowText(hDlg, tit); */
    }
}

void Board::drawBoard(int mx, int my)
{
    m_mx = mx;
    m_my = my;
    update();
}

void Board::movePeace()
{
    int i, dif;
    STATE *state = &m_states[0];

    // Movimentos do cavalo
    if ((m_piece & 0x07) == KNIGHT) {

        // Move na diagonal superior direita 1
        if (m_x2 - m_x == 1 && m_y - m_y2 == 2) {
            for (i = m_y * 32; i >= m_y2 * 32; i -= 2)
                drawBoard(m_x * 32, i);
            for (i = m_x * 32; i < m_x2 * 32; i += 2)
                drawBoard(i, m_y2 * 32);

        // Move na diagonal superior direita 2
        } else if (m_x2 - m_x == 2 && m_y - m_y2 == 1) {
            for (i = m_x * 32; i <= m_x2 * 32; i += 2)
                drawBoard(i, m_y * 32);
            for (i = m_y * 32; i > m_y2 * 32; i -= 2)
                drawBoard(m_x2 * 32, i);

        // Move na diagonal inferior direita 1
        } else if (m_x2 - m_x == 2 && m_y2 - m_y == 1) {
            for (i = m_x * 32; i <= m_x2 * 32; i += 2)
                drawBoard(i, m_y * 32);
            for (i = m_y * 32; i < m_y2 * 32; i += 2)
                drawBoard(m_x2 * 32, i);

        // Move na diagonal inferior direita 2
        } else if (m_x2 - m_x == 1 && m_y2 - m_y == 2) {
            for (i = m_y * 32; i <= m_y2 * 32; i += 2)
                drawBoard(m_x * 32, i);
            for (i = m_x * 32; i < m_x2 * 32; i += 2)
                drawBoard(i, m_y2 * 32);

        // Move na diagonal inferior esquerda 1
        } else if (m_x - m_x2 == 1 && m_y2 - m_y == 2) {
            for (i = m_y * 32; i <= m_y2 * 32; i += 2)
                drawBoard(m_x * 32, i);
            for (i = m_x * 32; i > m_x2 * 32; i -= 2)
                drawBoard(i, m_y2 * 32);

        // Move na diagonal inferior esquerda 2
        } else if (m_x - m_x2 == 2 && m_y2 - m_y == 1) {
            for (i = m_x * 32; i >= m_x2 * 32; i -= 2)
                drawBoard(i, m_y * 32);
            for (i = m_y * 32; i < m_y2 * 32; i += 2)
                drawBoard(m_x2 * 32, i);

        // Move na diagonal superior esquerda 1
        } else if (m_x - m_x2 == 2 && m_y - m_y2 == 1) {
            for (i = m_x * 32; i >= m_x2 * 32; i -= 2)
                drawBoard(i, m_y * 32);
            for (i = m_y * 32; i > m_y2 * 32; i -= 2)
                drawBoard(m_x2 * 32, i);

        // Move na diagonal superior esquerda 2
        } else /*if (m_x - m_x2 == 1 && m_y - m_y2 == 2)*/ {
            for (i = m_y * 32; i >= m_y2 * 32; i -= 2)
                drawBoard(m_x * 32, i);
            for (i = m_x * 32; i > m_x2 * 32; i -= 2)
                drawBoard(i, m_y2 * 32);
        }

    // Demais peças
    } else {

        // Move na vertical
        if (m_x == m_x2) {
            if (m_y > m_y2) for (i = m_y * 32; i > m_y2 * 32; i -= 2)
                drawBoard(m_x * 32, i);
            else for (i = m_y * 32; i < m_y2 * 32; i += 2)
                drawBoard(m_x * 32, i);

        // Move na horizontal	
        } else if (m_y == m_y2) {
            if (m_x > m_x2) for (i = m_x * 32; i > m_x2 * 32; i -= 2)
                drawBoard(i, m_y * 32);
            else for (i = m_x * 32; i < m_x2 * 32; i += 2)
                drawBoard(i, m_y * 32);

        // Move na diagonal superior direita
        } else if (m_y > m_y2 && m_x < m_x2) {
            dif = (m_x2 - m_x) * 32;
            for (i = 0; i < dif; i += 2) drawBoard(32 * m_x + i, 32 * m_y - i);

        // Move na diagonal inferior direita
        } else if (m_y < m_y2 && m_x < m_x2) {
            dif = (m_x2 - m_x) * 32;
            for (i = 0; i < dif; i += 2) drawBoard(32 * m_x + i, 32 * m_y + i);

        // Move na diagonal inferior esquerda
        } else if (m_y < m_y2 && m_x > m_x2) {
            dif = (m_x - m_x2) * 32;
            for (i = 0; i < dif; i += 2) drawBoard(32 * m_x - i, 32 * m_y + i);

        // Move na diagonal superior esquerda
        } else /*if (m_y > m_y2 && m_x > m_x2)*/ {
            dif = (m_x - m_x2) * 32;
            for (i = 0; i < dif; i += 2) drawBoard(32 * m_x - i, 32 * m_y - i);
        }
    }

    // Ponhe a peça definitivamente no lugar
    state->board[m_y2][m_x2] = m_piece;
    drawBoard();
}

void Board::sumPoints(STATE *state) {
    int x, y, piece;

    // Soma todas as peças
    state->white = state->black = 0;
    for (y = 0; y < 8; y++) for (x = 0; x < 8; x++) {
        piece = state->board[y][x];
        if (piece & BLACK) state->white += m_points[piece & 0x07];
        else if (piece & WHITE) state->black += m_points[piece & 0x07];
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
        return parent->white - parent->black;
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
        m_piece = parent->board[y][x];
        parent->board[y][x] = 0;
        movePeace();
    }

    // Retorna MINIMAX deste nível
    return minimax;
}

int Board::yourTurn(int x, int y) {
    int i, j;
    STATE *state = &m_states[0];

    // Verifica se é uma peça preta
    if (state->board[y][x] & WHITE) return 0;

    // Valida o movimento da peça escolhida
    switch (state->board[m_y][m_x] & 0x07) {

        case PAWN:

            // Movimento para frente
            if (m_x == x && (m_y - y == 1 || (m_y - y == 2 &&
              m_y == 6 && !state->board[5][x]))) break;

            // Movimento para diagonal comendo
            if (abs(m_x - x) == 1 && m_y - y == 1 && state->board[y][x] & BLACK) break;
            return 0;

        case ROOK:

            // Verifica se moveu na horizontal ou vertical
            if (m_x != x && m_y != y) return 0;

            // Verifica se há alguma peça no caminho (vertical)
            if (m_x == x) {
                if (m_y > y) {
                    for (i = m_y - 1; i > y; i--)
                        if (state->board[i][x]) return 0;
                } else {
                    for (i = m_y + 1; i < y; i++)
                        if (state->board[i][x]) return 0;
                }

            // Verifica se há alguma peça no caminho (horizontal)
            } else {
                if (m_x > x) {
                    for (i = m_x - 1; i > x; i--)
                        if (state->board[y][i]) return 0;
                } else {
                    for (i = m_x + 1; i < x; i++)
                        if (state->board[y][i]) return 0;
                }
            }
            break;

        case KNIGHT:

            // Verifica se é um "L" válido
            if ((abs(m_x - x) == 2 && abs(m_y - y) == 1) ||
              (abs(m_x - x) == 1 && abs(m_y - y) == 2)) break;
            return 0;

        case BISHOP:

            // Verifica se moveu na diagonal
            j = abs(m_x - x);
            if (j != abs(m_y - y)) return 0;

            // Verifica se há alguma peça no caminho diagonal
            for (i = 1; i < j; i++) {
                if (m_x < x) {
                    if (m_y > y) {
                        if (state->board[m_y - i][m_x + i]) return 0;
                    } else {
                        if (state->board[m_y + i][m_x + i]) return 0;
                    }
                } else {
                    if (m_y > y) {
                        if (state->board[m_y - i][m_x - i]) return 0;
                    } else {
                        if (state->board[m_y + i][m_x - i]) return 0;
                    }
                }
            }
            break;

        case QUEEN:

            // Verifica se moveu na horizontal, vertical ou diagonal
            j = abs(m_x - x);
            if (m_x != x && m_y != y && j != abs(m_y - y)) return 0;

            // Verifica se há alguma peça no caminho (vertical)
            if (m_x == x) {
                if (m_y > y) {
                    for (i = m_y - 1; i > y; i--)
                        if (state->board[i][x]) return 0;
                } else {
                    for (i = m_y + 1; i < y; i++)
                        if (state->board[i][x]) return 0;
                }

            // Verifica se há alguma peça no caminho (horizontal)
            } else if (m_y == y) {
                if (m_x > x) {
                    for (i = m_x - 1; i > x; i--)
                        if (state->board[y][i]) return 0;
                } else {
                    for (i = m_x + 1; i < x; i++)
                        if (state->board[y][i]) return 0;
                }

            // Verifica se há alguma peça no caminho diagonal
            } else for (i = 1; i < j; i++) {
                if (m_x < x) {
                    if (m_y > y) {
                        if (state->board[m_y - i][m_x + i]) return 0;
                    } else {
                        if (state->board[m_y + i][m_x + i]) return 0;
                    }
                } else {
                    if (m_y > y) {
                        if (state->board[m_y - i][m_x - i]) return 0;
                    } else {
                        if (state->board[m_y + i][m_x - i]) return 0;
                    }
                }
            }
            break;

        case KING:
            if (abs(m_x - x) > 1 || abs(m_y - y) > 1) return 0;
            break;
    }

    // Atualiza board e executa animação
    m_x2 = x;
    m_y2 = y;

    // Verifica se ponhe o rei em cheque
    m_mmLevel = 2;
    memcpy(&m_states[1], state, sizeof(STATE));
    m_states[1].board[m_y2][m_x2] = state->board[m_y][m_x] & ~SELECT;
    m_states[1].board[m_y][m_x] = 0;
    if (myTurn(1, MINIMAX, BLACK, WHITE) >= 60) {
        m_mmLevel = m_level;
        return 2;
    }

    // Movimento válido
    m_mmLevel = m_level;
    m_piece = state->board[m_y][m_x] & ~SELECT;
    state->board[m_y][m_x] = 0;
    movePeace();
    return 1;
}

void Board::selectPiece(int x, int y) {
    m_x = x;
    m_y = y;

    // Verifica se há uma peça do player neste local
    if (!(m_states[0].board[m_y][m_x] & WHITE)) {
        QMessageBox::warning(this, "Chess", "Você deve selecionar uma peça sua !!!");
        return;
    }

    // Peça selecionada
    m_firstClick = false;
    m_states[0].board[m_y][m_x] |= SELECT;
    drawBoard();
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
        if (abs(myTurn(1, opponent == WHITE ? -MINIMAX : MINIMAX, opponent, player)) >= 60) {
            m_gameStopped = true;
            QMessageBox::warning(this, "Chess", "Cheque Mate!!!");
            return true;
        }

        // É apenas cheque
        QMessageBox::warning(this, "Chess", "Cheque !!!");
    }

    // Ninguém venceu
    m_mmLevel = m_level;
    return false;
}

void Board::play(int x, int y) {
    int result;
//    HCURSOR hCursor;

    // Cancela jogada
    if (m_x == x && m_y == y) {
        m_firstClick = true;
        m_states[0].board[y][x] &= ~SELECT;
        drawBoard();
        return;
    }

    // Sua vez
    result = yourTurn(x, y);
    if (!result)
        QMessageBox::warning(this, "Chess", "Jogada inválida! Tente outra vez.");
    else if (result == 2)
        QMessageBox::warning(this, "Chess", "Jogada inválida! Ela ponhe seu rei em cheque.");
    else {
        m_firstClick = true;
        if (!gameFinished(WHITE, BLACK)) {

            // Minha vez !!!
//            hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
            myTurn(0, MINIMAX, BLACK, WHITE);
//            SetCursor(hCursor);
            gameFinished(BLACK, WHITE);
        }
    }
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

    if (m_mx >= 0)
        painter.drawPixmap(m_mx + 1, m_my + 1, m_piece & BLACK ? m_black : m_white,
                           ((m_piece & 0x07) - 1) * 48, 0, 48, 48);
}

void Board::mousePressEvent(QMouseEvent *event)
{
    int x = event->x() / SIZE, y = event->y() / SIZE;

    if (!m_gameStopped) {
        if (m_firstClick)
            selectPiece(x, y);
        else
            play(x, y);
    }
}
