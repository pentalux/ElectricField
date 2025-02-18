#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    fieldWidget = new FieldWidget(this);
    ui->gridLayout_2->addWidget(fieldWidget);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_addChargeButton_clicked() {
    double x = ui->xCoordSpinBox->value();
    double y = ui->yCoordSpinBox->value();
    double charge = ui->chargeSpinBox->value();
    fieldWidget->addCharge(QPointF(x, y), charge);
}

void MainWindow::on_clearButton_clicked() {
    fieldWidget->clearCharges();
}
