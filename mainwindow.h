#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class Board;
class BoardScene;
#include <QMainWindow>
#include <QPointer>
#include "board.h"
#include "boardscene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit  MainWindow(QWidget *parent = nullptr);
             ~MainWindow();
        void  drawBoard();

private:
     Ui::MainWindow *ui;
     QPointer<Board> board;
QPointer<BoardScene> boardScene;
};

#endif // MAINWINDOW_H
