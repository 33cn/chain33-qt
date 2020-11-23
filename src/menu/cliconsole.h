#ifndef CLICONSOLE_H
#define CLICONSOLE_H

/*
 * 控制台界面
 */

#include <QDialog>

namespace Ui {
class CliConsole;
}

class CliConsole : public QDialog
{
    Q_OBJECT

public:
    explicit CliConsole(QWidget *parent = 0);
    ~CliConsole();

    enum MessageClass {
        MC_ERROR,
        MC_DEBUG,
        CMD_REQUEST,
        CMD_REPLY,
        CMD_ERROR
    };

protected:
    virtual bool eventFilter(QObject* obj, QEvent *event);

private slots:
    void on_lineEdit_returnPressed();

    void on_SendBtn_clicked();

    void on_ClearBtn_clicked();

private:
    void message(int category, const QString &message, bool html = false);

    void browseHistory(int offset);

    void cmdRequest(const QString &command);

    bool parseCommandLine(std::vector<std::string> &args, const std::string &strCommand);

private:
    Ui::CliConsole *ui;

    QStringList history;

    int historyPtr;
};

#endif // CLICONSOLE_H
