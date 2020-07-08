#ifndef BITCOIN_QT_RECEIVEREQUESTDIALOG_H
#define BITCOIN_QT_RECEIVEREQUESTDIALOG_H

/*
 * 保存二维码界面
 */

#include <QDialog>
#include <QImage>
#include <QLabel>
#include <QPainter>

namespace Ui {
    class ReceiveRequestDialog;
}

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

/* Label widget for QR code. This image can be dragged, dropped, copied and saved
 * to disk.
 */
class QRImageWidget : public QLabel
{
    Q_OBJECT

public:
    explicit QRImageWidget(QWidget *parent = 0);
    QImage exportImage();

public Q_SLOTS:
    void saveImage();
    void copyImage();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    QMenu *contextMenu;
};

class ReceiveRequestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReceiveRequestDialog(QWidget *parent = 0, QString strAddr = "", bool bShowWarn = false, QString strSelectedSymbol = "");
    ~ReceiveRequestDialog();


private Q_SLOTS:
    void on_btnCopyAddress_clicked();

    void update();

private:
    Ui::ReceiveRequestDialog *ui;
    QString m_strAddr;
};

#endif // BITCOIN_QT_RECEIVEREQUESTDIALOG_H
