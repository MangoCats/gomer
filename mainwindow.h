#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class Board;
#include <QMainWindow>
#include <QPointer>
#include "board.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
            ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPointer<Board> board;
};

#endif // MAINWINDOW_H
