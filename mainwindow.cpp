/*
Copyright (c) 2014 Raivis Strogonovs


*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->stemBtn, SIGNAL(clicked(bool)), this, SLOT(stemWord()));

}

void MainWindow::stemWord()
{
    if(ui->wordEdit->text().isEmpty())
        return;

    switch (ui->langComboBox->currentIndex()) {
    case LANG_EN:
        ui->stemResultLbl->setText( ENPorterStemmer::stem(ui->wordEdit->text()) );
        break;
    case LANG_LV:
        ui->stemResultLbl->setText( LVPorterStemmer::stem(ui->wordEdit->text()) );
        break;
    default:
        break;
    }

    ui->wordEdit->clear();
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        stemWord();

    QMainWindow::keyReleaseEvent( e );
}

MainWindow::~MainWindow()
{
    delete ui;
}
