#include "fw_update_dialog.h"
#include "ui_fw_update_dialog.h"
#include "config.h"

fw_update_dialog::fw_update_dialog(QWidget *parent, hid_t *hid) :
    QDialog(parent),
    ui(new Ui::fw_update_dialog)
{
    ui->setupUi(this);
    this->setAcceptDrops(true);
    this->hid = hid;
}

fw_update_dialog::~fw_update_dialog()
{
    delete ui;
}

void fw_update_dialog::percent(int value)
{
   ui->progressBar->setValue(value);
}

void fw_update_dialog::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void fw_update_dialog::dragMoveEvent(QDragMoveEvent* event)
{
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
}

void fw_update_dialog::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void fw_update_dialog::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls())
    {
        pathList.clear();
        QList<QUrl> urlList = mimeData->urls(); // create url list
        for (int i = 0; i < urlList.size() && i < 32; ++i) // extract the local paths of the files
            pathList.append(urlList.at(i).toLocalFile()); // append local paths to pathList
        // process pathList
        drop_files_process();
    }
}

void fw_update_dialog::drop_files_process()
{
    QRegExp rx("\\/");
    QIcon icon(":/resources/firmware.png");
    QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(50, 50)));
    if(pathList.size() > 1)
    {
        /* Ask what exactly file user want to upload */
    }
    else
    {
        bin_path = pathList.at(0);
    }

    ui->label_BIN_NAME->setText(bin_path.split(rx).last());
    ui->label_BIN_PIXMAP->setPixmap(pixmap);
}

void fw_update_dialog::on_label_BIN_PIXMAP_customContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    QAction *Remove  = menu->addAction("Remove");

    Remove->setIcon(QIcon(":/resources/trash.png"));

    QAction *sel = menu->exec(ui->label_BIN_PIXMAP->mapToGlobal(pos));
    if(Remove == sel)
    {
        bin_path.clear();
        ui->label_BIN_NAME->setText("Drag firmware file here ...");
        ui->label_BIN_PIXMAP->clear();
    }
}

void fw_update_dialog::on_pushButton_UPDATE_clicked()
{
    uint8_t fw_chunk[WRITE_CHUNK_SIZE] = { 0 };
    unsigned int offset = 0;
    unsigned int file_size = 0;
    unsigned int total = 0;
    FILE* file = fopen(bin_path.toLocal8Bit().constData(), "rb");

    if(NULL == file)
    {
        /* TODO: */
        return;
    }
    fseek(file, 0, SEEK_END);
    total = file_size = ftell(file);
    hid->start_update(file_size);

    /************************** MAIN DATA BLOCK *************************/
    while(file_size / WRITE_CHUNK_SIZE)
    {
        fseek(file, offset, SEEK_SET);
        fread_s(fw_chunk, WRITE_CHUNK_SIZE, sizeof(uint8_t), WRITE_CHUNK_SIZE, file);
        hid->proceed_update(fw_chunk, offset);
        offset += WRITE_CHUNK_SIZE;
        file_size -= WRITE_CHUNK_SIZE;

        percent(100 - (((total - offset) * 100) / total));
        qApp->processEvents();
    }

    /************************** REMAINING DATA *************************/
    if(file_size)
    {
        memset(fw_chunk, 0xFF, WRITE_CHUNK_SIZE);
        fseek(file, offset, SEEK_SET);
        fread_s(fw_chunk, WRITE_CHUNK_SIZE, sizeof(uint8_t), file_size, file);
        hid->proceed_update(fw_chunk, offset);
        offset += file_size;
        file_size = 0;

        percent(100 - (((total - offset) * 100) / total));
    }

    hid->update_fw();
    fclose(file);
    QMessageBox::warning(this, "ATTENTION!", "Do not remove device until it has been restarted!");
}
