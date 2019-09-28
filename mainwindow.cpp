#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    board      = new Board( this );
    boardScene = new BoardScene( board, this );
    ui->boardView->setScene( boardScene );
    ui->boardView->setDragMode( QGraphicsView::ScrollHandDrag );
    ui->boardView->scale( 40.0, 40.0 );
    drawBoard();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawBoard()
{ }
