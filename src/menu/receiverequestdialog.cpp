#include <QClipboard>
#include <QDrag>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QPixmap>
#if QT_VERSION < 0x050000
#include <QUrl>
#endif

#include "receiverequestdialog.h"
#include "ui_receiverequestdialog.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "qrencode.h"
#include "cstyleconfig.h"

#define QR_IMAGE_SIZE 300

QRImageWidget::QRImageWidget(QWidget *parent):
    QLabel(parent), contextMenu(0)
{
    contextMenu = new QMenu(this);
    QAction *saveImageAction = new QAction(tr("保存二维码"), this);
    connect(saveImageAction, SIGNAL(triggered()), this, SLOT(saveImage()));
    contextMenu->addAction(saveImageAction);
    QAction *copyImageAction = new QAction(tr("拷贝二维码"), this);
    connect(copyImageAction, SIGNAL(triggered()), this, SLOT(copyImage()));
    contextMenu->addAction(copyImageAction);
}

QImage QRImageWidget::exportImage()
{
    if(!pixmap())
        return QImage();
    return pixmap()->toImage();
}

void QRImageWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && pixmap())
    {
        event->accept();
        QMimeData *mimeData = new QMimeData;
        mimeData->setImageData(exportImage());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec();
    } else {
        QLabel::mousePressEvent(event);
    }
}

void QRImageWidget::saveImage()
{
    if(!pixmap())
        return;
    QString fn = GUIUtil::getSaveFileName(this, tr("保存二维码"), QString(), tr("PNG Image (*.png)"), NULL);
    if (!fn.isEmpty())
    {
        exportImage().save(fn);
    }
}

void QRImageWidget::copyImage()
{
    if(!pixmap())
        return;
    QApplication::clipboard()->setImage(exportImage());
}

void QRImageWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if(!pixmap())
        return;
    contextMenu->exec(event->globalPos());
}

ReceiveRequestDialog::ReceiveRequestDialog(QWidget *parent, QString strAddr, bool bShowWarn, QString strSelectedSymbol)
    : QDialog(parent)
    , ui(new Ui::ReceiveRequestDialog)
    , m_strAddr(strAddr)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint); // 没有帮助按钮
    ui->WarnWidget->setVisible(bShowWarn);
    if(bShowWarn)
    {
        ui->labelWarn->setText(tr("请注意：\r\n严禁向该地址转入非该钱包资产，误转入的其他资产将无法找回；\r\n需要 12 个网络确认后才能到账，最低手续费 0.001。"));
        this->setWindowTitle(strSelectedSymbol + tr(" 充币"));
    }
    else
    {
        this->setWindowTitle(tr("详情"));
    }

    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("关闭")); //将buttonbox 汉化
    update();
    connect(ui->btnSaveAs, SIGNAL(clicked()), ui->lblQRCode, SLOT(saveImage()));
    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
}

ReceiveRequestDialog::~ReceiveRequestDialog()
{
    delete ui;
}

void ReceiveRequestDialog::update()
{
    ui->lblQRCode->setText(m_strAddr);

    // limit URI length
    if (m_strAddr.length() > MAX_URI_LENGTH)
    {
        ui->lblQRCode->setText(tr("Resulting URI too long, try to reduce the text for label / message."));
    } else {
        QRcode *code = QRcode_encodeString(m_strAddr.toUtf8().constData(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
        if (!code)
        {
            ui->lblQRCode->setText(tr("Error encoding URI into QR Code."));
            return;
        }
        QImage qrImage = QImage(code->width + 8, code->width + 8, QImage::Format_RGB32);
        qrImage.fill(0xffffff);
        unsigned char *p = code->data;
        for (int y = 0; y < code->width; y++)
        {
            for (int x = 0; x < code->width; x++)
            {
                qrImage.setPixel(x + 4, y + 4, ((*p & 1) ? 0x0 : 0xffffff));
                p++;
            }
        }
        QRcode_free(code);

        QImage qrAddrImage = QImage(QR_IMAGE_SIZE, QR_IMAGE_SIZE+20, QImage::Format_RGB32);
        qrAddrImage.fill(0xffffff);
        QPainter painter(&qrAddrImage);
        painter.drawImage(0, 0, qrImage.scaled(QR_IMAGE_SIZE, QR_IMAGE_SIZE));
        QFont font = GUIUtil::fixedPitchFont();
        font.setPixelSize(12);
        painter.setFont(font);
        QRect paddedRect = qrAddrImage.rect();
        paddedRect.setHeight(QR_IMAGE_SIZE+12);
        painter.drawText(paddedRect, Qt::AlignBottom|Qt::AlignCenter, m_strAddr);
        painter.end();

        ui->lblQRCode->setPixmap(QPixmap::fromImage(qrAddrImage));
        ui->btnSaveAs->setEnabled(true);
    }
}

void ReceiveRequestDialog::on_btnCopyAddress_clicked()
{
    GUIUtil::setClipboard(m_strAddr);
}
