#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "hid_tree.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void update_config(DEVICE*);

    void on_pushButton_ON_clicked();

    void on_pushButton_OFF_clicked();

    void on_pushButton_SET_CONFIG_clicked();

private:
    Ui::MainWindow *ui;
    hid_tree* HIDTree;
};

#endif // MAINWINDOW_H
