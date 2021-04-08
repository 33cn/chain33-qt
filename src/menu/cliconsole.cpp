#include "cliconsole.h"
#include "ui_cliconsole.h"
#include "guiutil.h"
#include "mainui.h"
#include "basefuntion.h"

#include <QTime>
#include <QTimer>
#include <QThread>
#include <QTextEdit>
#include <QKeyEvent>
#include <QUrl>
#include <QScrollBar>
#include <QProcess>

const int CONSOLE_SCROLLBACK = 50;
const int CONSOLE_HISTORY = 50;

const QSize ICON_SIZE(24, 24);

const struct {
    const char *url;
    const char *source;
} ICON_MAPPING[] = {
    {"cmd-request", ":/icons/tx_input"},
    {"cmd-reply", ":/icons/tx_output"},
    {"cmd-error", ":/icons/tx_output"},
    {"misc", ":/icons/tx_inout"},
    {NULL, NULL}
};

static QString categoryClass(int category)
{
    switch(category)
    {
    case CliConsole::CMD_REQUEST:  return "cmd-request"; break;
    case CliConsole::CMD_REPLY:    return "cmd-reply"; break;
    case CliConsole::CMD_ERROR:    return "cmd-error"; break;
    default:                       return "misc";
    }
}

CliConsole::CliConsole(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CliConsole)
    , historyPtr(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());

    ui->lineEdit->installEventFilter(this);
    ui->messagesWidget->installEventFilter(this);

    on_ClearBtn_clicked();
}

CliConsole::~CliConsole()
{
    delete ui;
}

bool CliConsole::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyPress) // Special key handling
    {
        QKeyEvent *keyevt = static_cast<QKeyEvent*>(event);
        int key = keyevt->key();
        Qt::KeyboardModifiers mod = keyevt->modifiers();
        switch(key)
        {
        case Qt::Key_Up: if(obj == ui->lineEdit) { browseHistory(-1); return true; } break;
        case Qt::Key_Down: if(obj == ui->lineEdit) { browseHistory(1); return true; } break;
        case Qt::Key_PageUp: /* pass paging keys to messages widget */
        case Qt::Key_PageDown:
            if(obj == ui->lineEdit)
            {
                QApplication::postEvent(ui->messagesWidget, new QKeyEvent(*keyevt));
                return true;
            }
            break;
        default:
            // Typing in messages widget brings focus to line edit, and redirects key there
            // Exclude most combinations and keys that emit no text, except paste shortcuts
            if(obj == ui->messagesWidget && (
                  (!mod && !keyevt->text().isEmpty() && key != Qt::Key_Tab) ||
                  ((mod & Qt::ControlModifier) && key == Qt::Key_V) ||
                  ((mod & Qt::ShiftModifier) && key == Qt::Key_Insert)))
            {
                ui->lineEdit->setFocus();
                QApplication::postEvent(ui->lineEdit, new QKeyEvent(*keyevt));
                return true;
            }
        }
    }
    return QDialog::eventFilter(obj, event);
}

void CliConsole::on_lineEdit_returnPressed()
{
    on_SendBtn_clicked();
}

void CliConsole::on_SendBtn_clicked()
{
    QString cmd = ui->lineEdit->text();
    ui->lineEdit->clear();

    if(!cmd.isEmpty())
    {
        message(CMD_REQUEST, cmd);
        cmdRequest(cmd);
        // Remove command, if already in history
        history.removeOne(cmd);
        // Append command to history
        history.append(cmd);
        // Enforce maximum history size
        while(history.size() > CONSOLE_HISTORY)
            history.removeFirst();
        // Set pointer to end of history
        historyPtr = history.size();
        // Scroll console view to end
        // scrollToEnd();
        QScrollBar *scrollbar = ui->messagesWidget->verticalScrollBar();
        scrollbar->setValue(scrollbar->maximum());
    }
}

void CliConsole::on_ClearBtn_clicked()
{
    ui->messagesWidget->clear();
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();

    // Add smoothly scaled icon images.
    // (when using width/height on an img, Qt uses nearest instead of linear interpolation)
    for(int i=0; ICON_MAPPING[i].url; ++i)
    {
        ui->messagesWidget->document()->addResource(
                    QTextDocument::ImageResource,
                    QUrl(ICON_MAPPING[i].url),
                    QImage(ICON_MAPPING[i].source).scaled(ICON_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    // TODO
    // Set default style sheet
    if(CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE){
        ui->messagesWidget->document()->setDefaultStyleSheet(
                    "table { }"
                    "td.time { color: #969BA6; padding-top: 3px; } "
                    "td.message { font-family: Monospace; } " // font-size: 12px;
                    "td.cmd-request { color: #2c77ef; } "
                    "td.cmd-error { color: red; } "
                    "b { color: #2c77ef; } "
                    );
    } else {
        ui->messagesWidget->document()->setDefaultStyleSheet(
                    "table { }"
                    "td.time { color: #969BA6; padding-top: 3px; } "
                    "td.message { font-family: Monospace; } " // font-size: 12px;
                    "td.cmd-request { color: #ffba26; } "
                    "td.cmd-error { color: red; } "
                    "b { color: #ffba26; } "
                    );
    }

    message(CMD_REPLY, (tr("欢迎使用%1RPC控制台。").arg(CStyleConfig::GetInstance().GetAppName()) + "<br>" +
                        tr("Use up and down arrows to navigate history, and <b>click clear</b> to clear screen.") + "<br>" +
                        tr("Type <b>help</b> for an overview of available commands.")), true);

}

void CliConsole::message(int category, const QString &message, bool html)
{
    QTime time = QTime::currentTime();
    QString timeString = time.toString();
    QString out;
    out += "<table><tr><td class=\"time\" width=\"65\">" + timeString + "</td>";
    out += "<td class=\"icon\" width=\"32\"><img src=\"" + categoryClass(category) + "\"></td>";
    out += "<td class=\"message " + categoryClass(category) + "\" valign=\"middle\">";
    if(html)
        out += message;
    else
        out += GUIUtil::HtmlEscape(message, true);
    out += "</td></tr></table>";
    ui->messagesWidget->append(out);
}

void CliConsole::browseHistory(int offset)
{
    historyPtr += offset;
    if(historyPtr < 0)
        historyPtr = 0;
    if(historyPtr > history.size())
        historyPtr = history.size();
    QString cmd;
    if(historyPtr < history.size())
        cmd = history.at(historyPtr);
    ui->lineEdit->setText(cmd);
}

void CliConsole::cmdRequest(const QString &command)
{
    std::vector<std::string> args;
    if(!parseCommandLine(args, command.toStdString()))
    {
        message(CMD_ERROR, QString("Parse error: unbalanced ' or \""));
        return;
    }

    if(args.empty())
        return;

    QString strCli = CStyleConfig::GetInstance().GetChain33cliPath();
    QStringList strList;

    std::string strCommand = command.toStdString();
/*    if(strCommand.find("saveseed") != std::string::npos)
    {
        message(CMD_REPLY, "seed is exit!");
        return;
    }
*/
    if(command != "help")
    {
        strList = command.split(" ");
    }

    QProcess process(this);

    if(strCommand.find("ticket bind_miner") != std::string::npos)
    {
       process.start(strCli, strList);
    }
    else
    {
        if(strCommand.find("\"") != std::string::npos)
        {
            strCli = strCli + " " + command;
            process.start(strCli);
        }
        else
        {
            process.start(strCli, strList);
        }
    }

    process.waitForStarted();
    process.waitForFinished();
    QString strResult = QString::fromUtf8(process.readAllStandardOutput());
    // 去掉换行
    if(strResult.length() > 1)
    {
        strResult = strResult.left(strResult.length() - 1);
    }

    QString strError = QString::fromUtf8(process.readAllStandardError());
    // 去掉换行
    if(strError.length() > 1)
    {
        strError = strError.left(strError.length() - 1);
    }

    if (!g_mapErrorCode[strError].isEmpty())
        strError = g_mapErrorCode[strError];

    strResult += strError;
    message(CMD_REPLY, strResult);
    process.close();
}

bool CliConsole::parseCommandLine(std::vector<std::string> &args, const std::string &strCommand)
{
    enum CmdParseState
    {
        STATE_EATING_SPACES,
        STATE_ARGUMENT,
        STATE_SINGLEQUOTED,
        STATE_DOUBLEQUOTED,
        STATE_ESCAPE_OUTER,
        STATE_ESCAPE_DOUBLEQUOTED
    } state = STATE_EATING_SPACES;
    std::string curarg;
    foreach(char ch, strCommand)
    {
        switch(state)
        {
        case STATE_ARGUMENT: // In or after argument
        case STATE_EATING_SPACES: // Handle runs of whitespace
            switch(ch)
            {
            case '"': state = STATE_DOUBLEQUOTED; break;
            case '\'': state = STATE_SINGLEQUOTED; break;
            case '\\': state = STATE_ESCAPE_OUTER; break;
            case ' ': case '\n': case '\t':
                if(state == STATE_ARGUMENT) // Space ends argument
                {
                    args.push_back(curarg);
                    curarg.clear();
                }
                state = STATE_EATING_SPACES;
                break;
            default: curarg += ch; state = STATE_ARGUMENT;
            }
            break;
        case STATE_SINGLEQUOTED: // Single-quoted string
            switch(ch)
            {
            case '\'': state = STATE_ARGUMENT; break;
            default: curarg += ch;
            }
            break;
        case STATE_DOUBLEQUOTED: // Double-quoted string
            switch(ch)
            {
            case '"': state = STATE_ARGUMENT; break;
            case '\\': state = STATE_ESCAPE_DOUBLEQUOTED; break;
            default: curarg += ch;
            }
            break;
        case STATE_ESCAPE_OUTER: // '\' outside quotes
            curarg += ch; state = STATE_ARGUMENT;
            break;
        case STATE_ESCAPE_DOUBLEQUOTED: // '\' in double-quoted text
            if(ch != '"' && ch != '\\') curarg += '\\'; // keep '\' for everything but the quote and '\' itself
            curarg += ch; state = STATE_DOUBLEQUOTED;
            break;
        }
    }
    switch(state) // final state
    {
    case STATE_EATING_SPACES:
        return true;
    case STATE_ARGUMENT:
        args.push_back(curarg);
        return true;
    default: // ERROR to end in one of the other states
        return false;
    }
}
