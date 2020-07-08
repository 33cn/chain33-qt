#ifndef ADDRESSUI_H
#define ADDRESSUI_H

/*
 * 我的地址主界面
 */

#include <QWidget>

class AddressListUI;
class PlatformStyle;

namespace Ui {
class AddressUI;
}

class AddressUI : public QWidget
{
    Q_OBJECT

public:
    explicit AddressUI(QWidget *parent, const PlatformStyle *platformStyle);
    ~AddressUI();

    AddressListUI*  m_lpMyAddressList;
    AddressListUI*  m_lpFriendsAddressList;

    void StopMyAddressListThread();
    void ResumeMyAddressListThread();
signals:
    void signalupdateWalletInfo(const QList<QVariant> &walletsList);

public slots:
    void slotupdateWalletInfo(const QList<QVariant> &walletsList);

private:
    Ui::AddressUI *ui;
};

#endif // ADDRESSUI_H
