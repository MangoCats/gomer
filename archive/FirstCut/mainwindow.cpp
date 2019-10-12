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
    board->territory->setScene( boardScene );
    // ui->boardView->setDragMode( QGraphicsView::ScrollHandDrag );
    ui->boardView->scale( 40.0, 40.0 );
    connect(           gameControl, SIGNAL(stateMessage(QString)), ui->stateLabel, SLOT(setText(QString))        );
    connect(        boardScene->op, SIGNAL(clicked(QPointF)),         gameControl, SLOT(clickAt(QPointF))        );
    connect(        boardScene->op, SIGNAL(hover(QPointF)),           gameControl, SLOT(hover(QPointF))          );
    connect(           gameControl, SIGNAL(newStonePlaced(Stone *)),   boardScene, SLOT(placeNewStone(Stone *))  );
    connect(                 board, SIGNAL(captured(Stone *)),         boardScene, SLOT(stoneCaptured(Stone *))  );
    connect(                 board, SIGNAL(clearLiberties()),      boardScene->lp, SLOT(clearCounts())           );
    connect( ui->showLibertyCounts, SIGNAL(toggled(bool)),             boardScene, SLOT(showLibertyCounts(bool)) );
    boardScene->showLibertyCounts( ui->showLibertyCounts->isChecked() );
    connect( ui->showTerritory    , SIGNAL(toggled(bool)),       board->territory, SLOT(setShow(bool)) );
    gameControl->start();
}

MainWindow::~MainWindow()
{ delete ui; }
