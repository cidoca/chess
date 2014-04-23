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

#include "mainwindow.h"
#include "board.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("Chess");
    setFixedSize(400, 400);

    createMenuBar();

    board = new Board;
    setCentralWidget(board);

    board->newGame(3);
}

void MainWindow::createMenuBar()
{
    QMenu *game = menuBar()->addMenu("&Game");
    QMenu *gnew = game->addMenu("&New");
    QMenu *help = menuBar()->addMenu("&Help");

    gnew->addAction("&Easy", this, SLOT(newGameEasy()));
    gnew->addAction("&Normal", this, SLOT(newGameNormal()));
    gnew->addAction("&Hard", this, SLOT(newGameHard()));
    gnew->addAction("E&xpert", this, SLOT(newGameExpert()));
    game->addSeparator();
    game->addAction("E&xit", qApp, SLOT(quit()));

    help->addAction("&About", qApp, SLOT(aboutQt()));
}

void MainWindow::newGameEasy()
{
    board->newGame(3);
}

void MainWindow::newGameNormal()
{
    board->newGame(4);
}

void MainWindow::newGameHard()
{
    board->newGame(5);
}

void MainWindow::newGameExpert()
{
    board->newGame(6);
}
