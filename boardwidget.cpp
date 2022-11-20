#include "boardwidget.h"
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPainter>
#include<QDebug>
BoardWidget::BoardWidget( QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);
    setMinimumSize(800, 800);
    setMaximumSize(800, 800);
    setBoard();
}

void BoardWidget::clear()
{
    pieces.clear();
    highlightedRect = QRect();
    inPlace = 0;
    update();
}

void BoardWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(BoardWidget::puzzleMimeType()))
        event->accept();
    else
        event->ignore();
}

void BoardWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QRect updateRect = highlightedRect;
    highlightedRect = QRect();
    update(updateRect);
    event->accept();
}

void BoardWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QRect updateRect = highlightedRect.united(targetSquare(event->position().toPoint()));

    if (event->mimeData()->hasFormat(BoardWidget::puzzleMimeType())
        && findPiece(targetSquare(event->position().toPoint())) == -1) {

        highlightedRect = targetSquare(event->position().toPoint());
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        highlightedRect = QRect();
        event->ignore();
    }

    update(updateRect);
}

void BoardWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(BoardWidget::puzzleMimeType())
        && findPiece(targetSquare(event->position().toPoint())) == -1) {

        QByteArray pieceData = event->mimeData()->data(BoardWidget::puzzleMimeType());
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        Piece piece;

        dataStream >> piece.pixmap >> piece.location>>piece.id_piece>>piece.id_color;
       qDebug()<<piece.id_color<<piece.id_piece;
       qDebug()<<targetSquare(event->position().toPoint());
        if(rule_movement(from_square,targetSquare(event->position().toPoint()),piece.id_piece,piece.id_color)){

            piece.rect = targetSquare(event->position().toPoint());
            pieces.append(piece);
            update(piece.rect);
        }
                else return;
        highlightedRect = QRect();
        update(piece.rect);

        event->setDropAction(Qt::MoveAction);
        event->accept();

        if (piece.location == piece.rect.topLeft() / pieceSize()) {
            inPlace++;
            if (inPlace == 32)
                emit puzzleCompleted();
        }
    } else {
        highlightedRect = QRect();
        event->ignore();
    }
}

int BoardWidget::findPiece(const QRect &pieceRect) const
{
    for (int i = 0, size = pieces.size(); i < size; ++i) {
        if (pieces.at(i).rect == pieceRect)
            return i;
    }
    return -1;
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    QRect square = targetSquare(event->position().toPoint());
    const int found = findPiece(square);
    from_square = square;
    qDebug()<<from_square;

    if (found == -1)
        return;

    Piece piece = pieces.takeAt(found);

    if (piece.location == square.topLeft() / pieceSize())
        inPlace--;

    update(square);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    dataStream << piece.pixmap << piece.location<<piece.id_piece<<piece.id_color;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(BoardWidget::puzzleMimeType(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(event->position().toPoint() - square.topLeft());
    drag->setPixmap(piece.pixmap);

    if (drag->exec(Qt::MoveAction) != Qt::MoveAction) {
        pieces.insert(found, piece);
        update(targetSquare(event->position().toPoint()));

        if (piece.location == square.topLeft() / pieceSize())
            inPlace++;
    }
}

void BoardWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

   for(int i=0;i<8;++i){
           for(int j=0;j<8;++j){


               if((i+j) %2 != 0){
                   painter.drawRect(i*100,j*100,100,100);
                  painter.fillRect(i*100,j*100,100,100,Qt::black);
              }
              else
              {
                   painter.drawRect(i*100,j*100,100,100);
                  painter.fillRect(i*100,j*100,100,100,Qt::white);
              }

           }
   }


    if (highlightedRect.isValid()) {
        painter.setBrush(QColor("#ccccff"));
        painter.setPen(Qt::NoPen);
        painter.drawRect(highlightedRect.adjusted(0, 0, -1, -1));
    }

    for (const Piece &piece : pieces)

        painter.drawPixmap(piece.rect, piece.pixmap);

}

const QRect BoardWidget::targetSquare(const QPoint &position) const
{
    QPoint topLeft = QPoint(position.x() / pieceSize(), position.y() / pieceSize()) * pieceSize();
    return QRect(topLeft, QSize(pieceSize(), pieceSize()));
}

int BoardWidget::pieceSize() const
{
    return 800 / 8;

}

int BoardWidget::imageSize() const
{

    return 800;

}

/*
    PIECE IDs                   COLOR IDs
    white pawns = 1             white = 13
    rooks = 2                   black = 14
    knights = 3
    bishops = 4
    kings = 5
    queens = 6
    black pawns = 7
*/
void BoardWidget::setBoard()
{
    clear();
    for(int i=0;i<8;++i){
            for(int j=0;j<8;++j){
             if(j==0||j==1||j==6||j==7)   {
    Piece piece;
    QPixmap pixmap;

    if((i==0 || i==7) && (j==0||j==1)){
     pixmap.load(":/images/brook.png");
     piece.id_piece = 2;
     piece.id_color=14;
    }

    if((i==1 || i==6) && (j==0||j==1)){
    pixmap.load(":/images/bknight.png");
    piece.id_piece = 3;
    piece.id_color=14;
    }

    if((i==2 || i==5)&& (j==0||j==1)){
    pixmap.load(":/images/bbishop.png");
    piece.id_piece = 4;
    piece.id_color=14;
    }

    //this is the black king
    if((i==3 )&& (j==0)){
    pixmap.load(":/images/bqueen.png");
    piece.id_piece = 5;
    piece.id_color=14;
    }

    if((i==4)&& (j==0)){
    pixmap.load(":/images/bking.png");
    piece.id_piece = 6;
    piece.id_color=14;
    }

    if(j==1){
    pixmap.load(":/images/bpawn.png");
    piece.id_piece = 7;
    piece.id_color=14;
    }

    if((i==0 || i==7) && (j==6||j==7)){
     pixmap.load(":/images/rook.png");
     piece.id_piece = 2;
     piece.id_color=13;
    }

    if((i==1 || i==6) && (j==6||j==7)){
    pixmap.load(":/images/knight.png");
    piece.id_piece = 3;
    piece.id_color=13;
    }

    if((i==2 || i==5)&& (j==6||j==7)){
    pixmap.load(":/images/bishop.png");
    piece.id_piece = 4;
    piece.id_color=13;
    }

    //this is actually the white king
    if((i==3)&& (j==7)){
    pixmap.load(":/images/queen.png");
    piece.id_piece = 5;
    piece.id_color=13;
    }

    //this is the white queen
    if((i==4)&& (j==7)){
    pixmap.load(":/images/king.png");
    piece.id_piece = 6;
    piece.id_color=13;
    }

    if(j==6){
    pixmap.load(":/images/pawn.png");
    piece.id_piece = 1;
    piece.id_color=13;
    }

    QRect dummy(i*100,j*100,100,100);
    piece.rect = dummy;
    piece.pixmap=pixmap;


    pieces.append(piece);

//    highlightedRect = QRect();
    update(piece.rect);
            }
    }
    }
}

bool BoardWidget::rule_movement(QRect fromsquare, QRect tosquare, int idp, int idc)
{
    int x1 = fromsquare.x()/100;
    int y1 = fromsquare.y()/100;
    int x2 = tosquare.x()/100;
    int y2 = tosquare.y()/100;

    switch(idp){
    //white pawns valid moves
    case 1:
    {
        if(idc==13){
            if((y2==(y1-1)|| y1==y2)&& x1==x2){
                return true;
        }
           else if((y1==6)&& y2==(y1-2) && x1==x2){
                return true;
        }
           else return false;

    }
    }

    //both rooks valid moves
   /* case 2:
    {
        if (idc == 13 || idc == 14){
            if ((((y2==(y1-1))||(y2==(y1-2))||(y2==(y1-3))||(y2==(y1-4))||(y2==(y1-5))||(y2==(y1-6))||(y2==(y1-7)))
                    || ((x2==(x1-1))||(x2==(x1-2))||(x2==(x1-3))||(x2==(x1-4))||(x2==(x1-5))||(x2==(x1-6))||(x2==(x1-7)))
                    || ((x2==(x1+1))||(x2==(x1+2))||(x2==(x1+3))||(x2==(x1+4))||(x2==(x1+5))||(x2==(x1+6))||(x2==(x1+7)))
                    || ((y2==(y1+1))||(y2==(y1+2))||(y2==(y1+3))||(y2==(y1+4))||(y2==(y1+5))||(y2==(y1+6))||(y2==(y1+7))))
                    && ((x1==x2) || (y1==y2))){
                    return true;
                    } else return false;
        }
    }
    */

    //both rooks valid moves(better code)
    case 2:
    {
        if(idc == 13||14) {
                if (x2==x1 || y2==y1) {
                    return true;
                } else return false;
        }
    }

    //black pawns valid moves
    case 7:
    {
        if(idc == 14){
            if((y2==(y1 + 1)|| y1==y2)&& x1==x2){
                return true;
            }

         else if((y1==1)&& y2==(y1+2) && x1==x2){
            return true;
        }
        else return false;
      }
    }

    //both bishops valid moves
    /*case 4:
    {
        if (idc == 13 || idc == 14){
            if ( ( (y2==y1+1 && x2==x1+1) || (y2==y1+2 && x2==x1+2)|| (y2==y1+3 && x2==x1+3)|| (y2==y1+4 && x2==x1+4)|| (y2==y1+5 && x2==x1+5)|| (y2==y1+6 && x2==x1+6)|| (y2==y1+7 && x2==x1+7) ||
                   (y2==y1+1 && x2==x1-1) || (y2==y1+2 && x2==x1-2)|| (y2==y1+3 && x2==x1-3)|| (y2==y1+4 && x2==x1-4)|| (y2==y1+5 && x2==x1-5)|| (y2==y1+6 && x2==x1-6)|| (y2==y1+7 && x2==x1-7) ||
                   (y2==y1-1 && x2==x1+1) || (y2==y1-2 && x2==x1+2)|| (y2==y1-3 && x2==x1+3)|| (y2==y1-4 && x2==x1+4)|| (y2==y1-5 && x2==x1+5)|| (y2==y1-6 && x2==x1+6)|| (y2==y1-7 && x2==x1+7) ||
                   (y2==y1-1 && x2==x1-1) || (y2==y1-2 && x2==x1-2)|| (y2==y1-3 && x2==x1-3)|| (y2==y1-4 && x2==x1-4)|| (y2==y1-5 && x2==x1-5)|| (y2==y1-6 && x2==x1-6)|| (y2==y1-7 && x2==x1-7))
                 && (x1!=x2 && y1!=y2)){
                return true;
            } else return false;
        }
    } */

    //both bishops valid moves
    case  4:
    {
        bool isEmpty (int x, int y);

        if(idc == 13 || 14) {
            if (abs(x2-x1)== abs(y2-y1)){
                return true;
            } else return false;
        }


    }

    //both queens valid moves
    /*case 6:
    {
        if (idc == 13 || idc == 14 ) {
            if (   (((y2==y1+1 && x2==x1+1) || (y2==y1+2 && x2==x1+2)|| (y2==y1+3 && x2==x1+3)|| (y2==y1+4 && x2==x1+4)|| (y2==y1+5 && x2==x1+5)|| (y2==y1+6 && x2==x1+6)|| (y2==y1+7 && x2==x1+7) ||
                   (y2==y1+1 && x2==x1-1) || (y2==y1+2 && x2==x1-2)|| (y2==y1+3 && x2==x1-3)|| (y2==y1+4 && x2==x1-4)|| (y2==y1+5 && x2==x1-5)|| (y2==y1+6 && x2==x1-6)|| (y2==y1+7 && x2==x1-7) ||
                   (y2==y1-1 && x2==x1+1) || (y2==y1-2 && x2==x1+2)|| (y2==y1-3 && x2==x1+3)|| (y2==y1-4 && x2==x1+4)|| (y2==y1-5 && x2==x1+5)|| (y2==y1-6 && x2==x1+6)|| (y2==y1-7 && x2==x1+7) ||
                   (y2==y1-1 && x2==x1-1) || (y2==y1-2 && x2==x1-2)|| (y2==y1-3 && x2==x1-3)|| (y2==y1-4 && x2==x1-4)|| (y2==y1-5 && x2==x1-5)|| (y2==y1-6 && x2==x1-6)|| (y2==y1-7 && x2==x1-7))
                    && (x1!=x2 && y1!=y2))
                 ||
                   ((((y2==(y1-1))||(y2==(y1-2))||(y2==(y1-3))||(y2==(y1-4))||(y2==(y1-5))||(y2==(y1-6))||(y2==(y1-7)))
                   || ((x2==(x1-1))||(x2==(x1-2))||(x2==(x1-3))||(x2==(x1-4))||(x2==(x1-5))||(x2==(x1-6))||(x2==(x1-7)))
                   || ((x2==(x1+1))||(x2==(x1+2))||(x2==(x1+3))||(x2==(x1+4))||(x2==(x1+5))||(x2==(x1+6))||(x2==(x1+7)))
                   || ((y2==(y1+1))||(y2==(y1+2))||(y2==(y1+3))||(y2==(y1+4))||(y2==(y1+5))||(y2==(y1+6))||(y2==(y1+7))))
                   && ((x1==x2) || (y1==y2)))
                 ){ return true; } else return false;
        }
    }
*/

    //both queens valid moves(better code)
    case 6:
    {
        if (idc == 13 || 14) {
            if (( abs(x2-x1)== abs(y2-y1) ) || ( x2==x1 || y2==y1 )) {
                return true;
            } else return false;
        }
    }

    //both horses valid moves
    case 3:
    {
        if (idc == 13 || idc == 14) {
             if (   (x2==x1+1 && y2==y1-2) || (x2==x1+2 && y2==y1-1)
                  ||(x2==x1+2 && y2==y1+1) || (x2==x1+1 && y2==y1+2)
                  ||(x2==x1-2 && y2==y1+1) || (x2==x1-1 && y2==y1+2)
                  ||(x2==x1-1 && y2==y1-2) || (x2==x1-2 && y2==y1-1)  ){
                 return true;
             } else return false;
        }
    }

    //both kings valid moves
    case 5:
    {
        if(idc == 13 || idc == 14){
            if ( (x2==x1 && y2==y1-1) ||
                 (x2==x1+1 && y2==y1-1) ||
                 (x2==x1-1 && y2==y1-1) ||
                 (x2==x1-1 && y2==y1) ||
                 (x2==x1+1 && y2==y1) ||
                 (x2==x1-1 && y2==y1+1) ||
                 (x2==x1 && y2==y1+1) ||
                 (x2==x1+1 && y2==y1+1)
                 ) { return true; } else return false;
        }
    }

   }
}

//public enum PieceColor { Black, White }
//   public interface IBoard
//   {
//       bool IsEmpty(int x, int y);
//       PieceColor GetPieceColor(int x, int y);
//   }
//
//   IBoard board;
//
//   bool BishopCanMove(PieceColor bishopColor, int fromX, int fromY, int toX, int toY)
//   {
//       int pathLength = Mathf.Abs(toX - fromX);
//       if (pathLength != Mathf.Abs(toY - fromY)) return false; // Not diagonal
//       // Also validate if the coordinates are in the 0-7 range
//
//       // Check all cells before the target
//       for (int i = 1; i < pathLength; i++)
//       {
//           int x = fromX + i;
//           int y = fromY + i;
//
//           if(board.IsEmpty(x, y)) continue; // No obstacles here: keep going
//           else return false; // Obstacle found before reaching target: the move is invalid
//       }
//
//       // Check target cell
//       if (board.IsEmpty(toX, toY)) return true; // No piece: move is valid
//
//       // There's a piece here: the move is valid only if we can capture
//       return board.GetPieceColor(toX, toY) == bishopColor;
//   }
//


