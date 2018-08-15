#include "hid_tree.h"
#include "ui_hid_tree.h"
#include "config.h"

hid_tree::hid_tree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::hid_tree)
{
    ui->setupUi(this);
    HID = new hid_t();
    device_cnt = 0;
    connect(HID, SIGNAL(device_founded(QString,QString,QString)),
            this, SLOT(device_founded(QString,QString,QString)));
}

hid_tree::~hid_tree()
{
    if(HID->is_open())
        HID->close();

    delete ui;
}

void hid_tree::popup_window(const char* Title, const char* Text)
{
    QMessageBox::critical(this, Title, Text);
}

void hid_tree::flush_tree()
{
    while(device_cnt)
       delete ui->treeWidget->takeTopLevelItem(--device_cnt);
    device_cnt = 0;
}

void hid_tree::search_devices()
{
    flush_tree();
    HID->search_devices();
}

void hid_tree::on()
{
    if(!HID->is_open())
    {
        popup_window("ERROR", "No device selected.");
        return;
    }

    if(!HID->set_state(true))
    {
        popup_window("ERROR", "Communication failed.");
        return;
    }

    if(HID->get_info(&device_config))
        emit update_config(&device_config);
    else
        popup_window("ERROR", "Get state failed.");
}

void hid_tree::off()
{
    if(!HID->is_open())
    {
        popup_window("ERROR", "No device selected.");
        return;
    }

    if(!HID->set_state(false))
    {
        popup_window("ERROR", "Communication failed.");
        return;
    }

    if(HID->get_info(&device_config))
        emit update_config(&device_config);
    else
        popup_window("ERROR", "Get state failed.");
}

void hid_tree::set_config(bool standalone_flag, unsigned int delay_ms, unsigned int timeout_ms)
{
    if(!HID->is_open())
    {
        popup_window("ERROR", "No device selected.");
        return;
    }

    if(!HID->set_config(standalone_flag, delay_ms, timeout_ms))
    {
        popup_window("ERROR", "Communication failed.");
        return;
    }

    if(HID->get_info(&device_config))
        emit update_config(&device_config);
    else
        popup_window("ERROR", "Get state failed.");
}

void hid_tree::device_founded(QString vid, QString pid, QString name)
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem;
    newItem->setText(0, vid  + " / " + pid + " [" + name + "]");
    newItem->setIcon(0, QIcon(":/resources/relay.png"));
    ui->treeWidget->addTopLevelItem(newItem);
    ui->treeWidget->setCurrentItem(newItem);
    ui->treeWidget->currentItem()->setSelected(true);
    device_cnt++;
}

void hid_tree::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    QMenu* context_menu = new QMenu(this);
    QAction* Refresh = context_menu->addAction("Refresh");
    QAction* FirmwareUpdate = context_menu->addAction("Update Firmware");
    Refresh->setIcon(QIcon(":/resources/refresh.png"));
    FirmwareUpdate->setIcon(QIcon(":/resources/download.png"));
    QAction *sel = context_menu->exec(ui->treeWidget->viewport()->mapToGlobal(pos));

    if (Refresh == sel)
        this->search_devices();

    if (FirmwareUpdate == sel)
    {
        /* TODO */
    }
}

void hid_tree::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    /* set active device */
    if(HID->is_open())
        HID->close();

    HID->open(VID, PID);

    if(HID->get_info(&device_config))
        emit update_config(&device_config);
}
