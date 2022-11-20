#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H
#include <QPoint>
#include <QPixmap>
#include <QList>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
QT_END_NAMESPACE

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardWidget( QWidget *parent = nullptr);
    void clear();

    int pieceSize() const;
    int imageSize() const;
    void setBoard();
  static QString puzzleMimeType() { return QStringLiteral("image/x-puzzle-piece"); }
  bool rule_movement(QRect fromsquare,QRect tosquare,int idp,int idc);

signals:
    void puzzleCompleted();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;


    struct Piece {
        QPixmap pixmap;
        QRect rect;
        QPoint location;
        int id_piece;
        int id_color;

    };
private:
    int findPiece(const QRect &pieceRect) const;
    const QRect targetSquare(const QPoint &position) const;

    QList<Piece> pieces;
    QRect highlightedRect;
    int inPlace;
    QRect from_square;
    QRect to_square;


};

#endif // BOARDWIDGET_H
