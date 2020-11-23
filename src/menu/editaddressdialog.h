#ifndef EDITADDRESSDIALOG_H
#define EDITADDRESSDIALOG_H

/*
 * 新增或修改 标签和地址 界面
 */

#include <QDialog>
#include "basejsonconnector.h"
#include "enumtype.h"

QT_BEGIN_NAMESPACE
class QDataWidgetMapper;
QT_END_NAMESPACE

namespace Ui {
    class EditAddressDialog;
}
class AddressTableModel;

/** Dialog for editing an address and associated information.
 */
class EditAddressDialog : public JsonConnectorDialog
{
    Q_OBJECT

public:
    explicit EditAddressDialog(EditAddressDialogMode mode, QWidget *parent = 0);
    ~EditAddressDialog();

    void setModel(AddressTableModel *model);
    void loadRow(int row, QString strLabel = "", QString strAddr = "");

    QString getlabel() const;
    QString getAddress() const;
    void setAddress(const QString &address);

public slots:
    void accept();

private:
    bool saveCurrentRow();

protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

private:
    Ui::EditAddressDialog *ui;
    QDataWidgetMapper *mapper;
    EditAddressDialogMode mode;
    AddressTableModel *model;

    QString label;
    QString address;
};

#endif // EDITADDRESSDIALOG_H
