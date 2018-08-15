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

    switch(dev->state)
    {
        case DEVICE_STATE_UNAWARE:
            ui->lineEdit_STATE->setText("UNAWARE");
            break;
        case DEVICE_STATE_OFF:
            ui->lineEdit_STATE->setText("OFF");
            break;
        case DEVICE_STATE_ON:
            ui->lineEdit_STATE->setText("ON");
        break;
    }

    ui->lineEdit_DELAY_MS->setText(QString::number(dev->delay_ms, 10));
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
    bool no_error = false;
    bool standalone_flag = ui->checkBox_STANDALONE->isChecked();
    unsigned int delay_ms = ui->lineEdit_DELAY_MS->text().toUInt(&no_error, 10);
    unsigned int timeout_ms = ui->lineEdit_TIME_MS->text().toUInt(&no_error, 10);

    if(!no_error)
    {
        QMessageBox::critical(this, "ERROR", "Wrong parametres.");
        return;
    }

    HIDTree->set_config(standalone_flag, delay_ms, timeout_ms);
}
