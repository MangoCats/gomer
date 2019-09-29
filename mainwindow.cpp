#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    board       = new Board( this );
    boardScene  = new BoardScene( board, this );
    gameControl = new GameControl( board, this );
    ui->boardView->setScene( boardScene );
    // ui->boardView->setDragMode( QGraphicsView::ScrollHandDrag );
    ui->boardView->scale( 40.0, 40.0 );
    connect(    gameControl, SIGNAL(stateMessage(QString)),   ui->stateLabel, SLOT(setText(QString))           );
    connect( boardScene->op, SIGNAL(clicked(QPointF)),           gameControl, SLOT(clickAt(QPointF))           );
    connect( boardScene->op, SIGNAL(hover(QPointF)),             gameControl, SLOT(hover(QPointF))             );
    connect(    gameControl, SIGNAL(newStonePlaced(int,int,int)), boardScene, SLOT(placeNewStone(int,int,int)) );
    gameControl->start();
    drawBoard();
}

MainWindow::~MainWindow()
{ delete ui; }

void MainWindow::drawBoard()
{ }
