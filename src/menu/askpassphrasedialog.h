#ifndef ASKPASSPHRASEDIALOG_H
#define ASKPASSPHRASEDIALOG_H

/*
 * 输入密码 修改密码 界面
 */

#include <QDialog>
#include "basejsonconnector.h"
#include "enumtype.h"

namespace Ui {
    class AskPassphraseDialog;
}

class AskPassphraseDialog : public JsonConnectorDialog
{
    Q_OBJECT

public:
    enum Mode {
        Unlock,        /**< Ask passphrase and unlock */
        ChangePass,    /**< Ask old passphrase + new passphrase twice */
        Decrypt        /**< Ask passphrase and decrypt wallet */
    };

    explicit AskPassphraseDialog(Mode mode, QWidget *parent = 0);
    ~AskPassphraseDialog();

    void accept();

public:
    EncryptionStatus    m_nStatus;

protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

private:
    Ui::AskPassphraseDialog *ui;
    Mode mode;
    bool fCapsLock;

private slots:
    void textChanged();
    bool event(QEvent *event);
    bool eventFilter(QObject *, QEvent *event);
    void secureClearPassFields();

private:
    QString m_strOldPsd;
    QString m_strNewPsd;
};

#endif // ASKPASSPHRASEDIALOG_H
