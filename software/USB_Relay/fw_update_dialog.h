#ifndef FW_UPDATE_DIALOG_H
#define FW_UPDATE_DIALOG_H

#include <QDialog>
#include <QMenu>
#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>
#include <QUrl>
#include <QMessageBox>
#include "hid.h"


namespace Ui {
class fw_update_dialog;
}

class fw_update_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit fw_update_dialog(QWidget *parent = 0, hid_t* hid = NULL);
    ~fw_update_dialog();
public slots:
    void percent(int);

private slots:
    void dragEnterEvent(QDragEnterEvent *event);

    void dropEvent(QDropEvent *event);

    void dragMoveEvent(QDragMoveEvent* event);

    void dragLeaveEvent(QDragLeaveEvent* event);

    void on_label_BIN_PIXMAP_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_UPDATE_clicked();

private:
    Ui::fw_update_dialog *ui;
    QStringList pathList;        // create list of files
    QString bin_path;
    hid_t* hid;
    void drop_files_process();
};

#endif // FW_UPDATE_DIALOG_H
