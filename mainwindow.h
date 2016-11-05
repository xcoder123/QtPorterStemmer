#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

#include "enporterstemmer.h"
#include "lvporterstemmer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum LANG_SELECT {LANG_LV, LANG_EN};

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyReleaseEvent(QKeyEvent *e);

private slots:
    void stemWord();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
