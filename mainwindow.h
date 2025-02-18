#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fieldwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addChargeButton_clicked();
    void on_clearButton_clicked();

private:
    Ui::MainWindow *ui;
    FieldWidget *fieldWidget;
};

#endif // MAINWINDOW_H
