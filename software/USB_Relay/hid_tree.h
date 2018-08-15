#ifndef HID_TREE_H
#define HID_TREE_H

#include <QWidget>
#include <QMenu>
#include <QTreeWidgetItem>
#include "hid.h"
namespace Ui {
class hid_tree;
}

class hid_tree : public QWidget
{
    Q_OBJECT
public slots:
    void device_founded(QString vid, QString pid, QString name);

public:
    explicit hid_tree(QWidget *parent = 0);
    ~hid_tree();

    void search_devices();
    void on();
    void off();
    void set_config();

private slots:
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

signals:
    void update_config(DEVICE*);

private:
    Ui::hid_tree *ui;
    hid_t* HID;
    int device_cnt;
    DEVICE device_config;

    void flush_tree();

};

#endif // HID_TREE_H
