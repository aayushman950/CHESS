#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>


class BoardWidget;
QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);


public slots:
    void setupBoard();

private slots:
    void setCompleted();

private:
    void setupMenus();
    void setupWidgets();

    QPixmap pieceImage;
    BoardWidget *boardWidget;
};

#endif // MAINWINDOW_H
