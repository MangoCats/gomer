#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class Board;
class BoardScene;
#include <QMainWindow>
#include <QPointer>
#include "board.h"
#include "boardscene.h"
#include "gamecontrol.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{ Q_OBJECT
public:
    explicit  MainWindow(QWidget *parent = nullptr);
             ~MainWindow();

private:
      Ui::MainWindow *ui;
      QPointer<Board> board;
 QPointer<BoardScene> boardScene;
QPointer<GameControl> gameControl;
};

#endif // MAINWINDOW_H
