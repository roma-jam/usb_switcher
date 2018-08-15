#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(TITLE);

    HIDTree = new hid_tree();
    ui->HID_Tree_Layout->addWidget(HIDTree);

    HIDTree->search_devices();

    connect(HIDTree, SIGNAL(update_config(DEVICE*)),
            this, SLOT(update_config(DEVICE*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_config(DEVICE* dev)
{
    ui->checkBox_STANDALONE->setChecked(dev->standalone_flag);
    ui->lineEdit_STATE->setText(QString::number(dev->state, 10));
    ui->lineEdit_TIME_MS->setText(QString::number(dev->timeout_ms, 10));
    ui->lineEdit_COUNTER->setText(QString::number(dev->switch_counter, 10));
}

void MainWindow::on_pushButton_ON_clicked()
{
    HIDTree->on();
}

void MainWindow::on_pushButton_OFF_clicked()
{
    HIDTree->off();
}

void MainWindow::on_pushButton_SET_CONFIG_clicked()
{
    HIDTree->set_config();
}
