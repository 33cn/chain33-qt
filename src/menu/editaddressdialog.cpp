#include "editaddressdialog.h"
#include "ui_editaddressdialog.h"
#include "addresstablemodel.h"
#include "guiutil.h"
#include "enumtype.h"
#include "base58.h"
#include "basefuntion.h"

#include <QDataWidgetMapper>
#include <QMessageBox>
#include <QPushButton>

EditAddressDialog::EditAddressDialog(EditAddressDialogMode mode, QWidget *parent) :
    JsonConnectorDialog(parent),
    ui(new Ui::EditAddressDialog), mapper(0), mode(mode), model(0)
{
    ui->setupUi(this);

    GUIUtil::setupAddressWidget(ui->addressEdit, this);

    switch(mode)
    {
    case NewReceivingAddress:
        setWindowTitle(tr("New receiving address"));
        ui->addressEdit->setVisible(false);
        ui->addrLabel->setVisible(false);
        break;
    case NewSendingAddress:
        setWindowTitle(tr("New sending address"));
        break;
    case EditReceivingAddress:
        setWindowTitle(tr("Edit receiving address"));
        ui->addressEdit->setVisible(false);
        ui->addrLabel->setVisible(false);
        break;
    case EditSendingAddress:
        setWindowTitle(tr("Edit sending address"));
        break;
    }

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定")); //将buttonbox中的ok 变成汉化
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));

    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());

//    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE) {
//       this->setStyleSheet("QWidget {background-color:#E7EDF1;border:none;}" + CStyleConfig::GetInstance().GetStylesheet_child());
//    }
}

EditAddressDialog::~EditAddressDialog()
{
    delete ui;
}

void EditAddressDialog::setModel(AddressTableModel *model)
{
    this->model = model;
    if(!model)
        return;

    mapper->setModel(model);
    model->SetRole(0);

    // 关联失败 不知道原因 自己添加数据
    mapper->addMapping(ui->labelEdit, Item_Label);
    mapper->addMapping(ui->addressEdit, Item_Address);
}

void EditAddressDialog::loadRow(int row, QString strLabel, QString strAddr)
{
    mapper->setCurrentIndex(row);

    ui->labelEdit->setText(strLabel);
    ui->addressEdit->setText(strAddr);
}

bool EditAddressDialog::saveCurrentRow()
{
    if(!model)
        return false;

    switch(mode)
    {
    case NewReceivingAddress:
    {
        QJsonObject jsonParms;
        jsonParms.insert("label", ui->labelEdit->text());
        QJsonArray params;
        params.insert(0, jsonParms);
        PostJsonMessage(ID_NewAccount, params);
    }
        break;
    case NewSendingAddress:
    {
        address = ui->addressEdit->text();
        label = ui->labelEdit->text();
        if(!IsAddrValid(address.toStdString().c_str())) {
            QMessageBox::warning(this, windowTitle(), tr("输入的地址\"%1\"不是有效的%2地址。").arg(ui->addressEdit->text(), CStyleConfig::GetInstance().GetAppName()), tr("确定"));
            return false;
        }
        model->updateEntry(AddressTableEntry(TabsSending, ui->labelEdit->text(), address), CT_NEW);
    }
        break;
    case EditReceivingAddress:
    case EditSendingAddress:
        if(mapper->submit()) {
            label = ui->labelEdit->text();

            if (model->getEditStatus() == EditStatus_OK || model->getEditStatus() == EditStatus_NO_CHANGES) {
                address = ui->addressEdit->text();
            } else {
                address = "";
            }

            if(mode == EditReceivingAddress) {
                QJsonObject jsonParms;
                jsonParms.insert("addr", address);
                jsonParms.insert("label", label);
                QJsonArray params;
                params.insert(0, jsonParms);
                PostJsonMessage(ID_SetLabl, params);
                address = "";
            }
        }
        model->SetRole(0);
        break;
    }

    return !address.isEmpty();
}

void EditAddressDialog::requestFinished(const QVariant &result, const QString &error)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    address = (resultMap["acc"].toMap())["addr"].toString();
    QString label = resultMap["label"].toString();

    if (!address.isEmpty() && !label.isEmpty())
    {        
        if(m_nID == ID_NewAccount)
        {
            model->updateEntry(AddressTableEntry(TabsReceiving, label, address), CT_NEW);
        }
        else if(m_nID == ID_SetLabl)
        {
            model->updateEntry(AddressTableEntry(TabsReceiving, label, address, Getbalance(resultMap["acc"].toMap()["balance"]), Getbalance(resultMap["acc"].toMap()["frozen"])), CT_UPDATED);
        }
        QDialog::accept();
    }
    else
    {
        QMessageBox::critical(this, windowTitle(), error, tr("确定"));
    }
}

void EditAddressDialog::accept()
{
    if(!model)
        return;

    if(!saveCurrentRow())
    {
        switch(model->getEditStatus())
        {
        case EditStatus_OK:
            // Failed with unknown reason. Just reject.
            break;
        case EditStatus_NO_CHANGES:
            // No changes were made during edit operation. Just reject.
            break;
        case EditStatus_INVALID_ADDRESS:
            QMessageBox::warning(this, windowTitle(), tr("输入的地址\"%1\"不是有效的%2地址。").arg(ui->addressEdit->text(), CStyleConfig::GetInstance().GetAppName()), tr("确定"));
            break;
        case EditStatus_DUPLICATE_ADDRESS:
            QMessageBox::warning(this, windowTitle(), tr("输入的地址\"%1\"已经存在于地址簿中。").arg(ui->addressEdit->text()), tr("确定"));
            break;
        case EditStatus_WALLET_UNLOCK_FAILURE:
            QMessageBox::critical(this, windowTitle(), tr("无法解锁钱包。"), tr("确定"));
            break;
        case EditStatus_KEY_GENERATION_FAILURE:
            QMessageBox::critical(this, windowTitle(), tr("新的密钥生成失败。"), tr("确定"));
            break;
        }
        return;
    }

    QDialog::accept();
}

QString EditAddressDialog::getlabel() const
{
    return label;
}

QString EditAddressDialog::getAddress() const
{
    return address;
}

void EditAddressDialog::setAddress(const QString &address)
{
    QMessageBox::warning(this, windowTitle(), tr(address.toStdString().c_str()),  tr("确定"));

    this->address = address;
    ui->addressEdit->setText(address);
}
