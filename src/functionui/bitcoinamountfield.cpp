#include "bitcoinamountfield.h"
#include "bitcoinunits.h"
#include "guiconstants.h"
#include "mainui.h"
#include "basefuntion.h"
#include <QLabel>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QApplication>
#include <qmath.h>
#include <QSpacerItem>

BitcoinAmountField::BitcoinAmountField(QWidget *parent):
        QWidget(parent), amount(0), currentUnit(0), /*m_dminFee(0.001), */m_dMax (0.0)
{
 //   this->setStyleSheet("QWidget { background: transparent; }");

    allbtn = new QPushButton(this);
    allbtn->setText(tr("全部"));
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE) {
        allbtn->setStyleSheet("QPushButton { color: #1B2740; }");
    }

    amount = new QDoubleSpinBox(this);
    amount->setLocale(QLocale::c());
    amount->setDecimals(4);
    amount->setRange(0.0, pow(10, 10));  // 范围
 //   amount->setSuffix("  " + CStyleConfig::GetInstance().GetUnitName());  // 后缀
    amount->installEventFilter(this);
    // 385
    amount->setMaximumWidth(210);
    amount->setMinimumWidth(210);
  //  amount->setSingleStep(0.001);
    amount->setSingleStep(0.1);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(amount);
    layout->addWidget(allbtn);
 //   unit = new QValueComboBox(this);
 //   unit->setModel(new BitcoinUnits(this));
 //   layout->addWidget(unit);
    QSpacerItem* lpSpacerItem = new QSpacerItem(20,20,QSizePolicy::Expanding);
    layout->addItem(lpSpacerItem);
    m_lpMaxUse = new QLabel(this);
    layout->addWidget(m_lpMaxUse);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);

    setLayout(layout);

    setFocusPolicy(Qt::TabFocus);
    setFocusProxy(amount);

    connect(allbtn, SIGNAL(clicked()), this, SLOT(AllBtnClicked()));
    amount->setContextMenuPolicy(Qt::NoContextMenu);
}

void BitcoinAmountField::setText(const QString &text)
{
    if (text.isEmpty())
        amount->clear();
    else
        amount->setValue(text.toDouble());
}

void BitcoinAmountField::clear()
{
    amount->clear();
//    unit->setCurrentIndex(0);
}

bool BitcoinAmountField::validate()
{
    bool valid = true;
    if (amount->value() == 0.0)
        valid = false;
    if (valid && !BitcoinUnits::parse(currentUnit, text(), 0))
        valid = false;

    setValid(valid);

    return valid;
}

void BitcoinAmountField::setValid(bool valid)
{
    if (valid)
        amount->setStyleSheet("");
    else
        amount->setStyleSheet(STYLE_INVALID);
}

QString BitcoinAmountField::text() const
{
    if (amount->text().isEmpty())
        return QString();
    else
        return amount->text();
}

bool BitcoinAmountField::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusIn) {
        // Clear invalid flag on focus
        setValid(true);
    } else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Comma) {
            // Translate a comma into a period
            QKeyEvent periodKeyEvent(event->type(), Qt::Key_Period, keyEvent->modifiers(), ".", keyEvent->isAutoRepeat(), keyEvent->count());
            qApp->sendEvent(object, &periodKeyEvent);
            return true;
        }
    }
    return QWidget::eventFilter(object, event);
}

QWidget *BitcoinAmountField::setupTabChain(QWidget *prev)
{
    QWidget::setTabOrder(prev, amount);
    return amount;
}

void BitcoinAmountField::setAmountRange(double dMax)
{
    if(dMax >= CStyleConfig::GetInstance().GetMinFee()){
        dMax -= CStyleConfig::GetInstance().GetMinFee();
    }
    m_dMax = dMax;
    amount->setRange(0.0, dMax);  // 范围

    QString strMaxUse = tr("可发送: ") + QString::number(dMax, 'f', 4) + " " + CStyleConfig::GetInstance().GetUnitName();
    m_lpMaxUse->setText(strMaxUse);
}

qint64 BitcoinAmountField::value(bool *valid_out) const
{
    qint64 val_out = 0;
    bool valid = BitcoinUnits::parse(currentUnit, text(), &val_out);
    if(valid_out)
    {
        *valid_out = valid;
    }
    return val_out;
}

void BitcoinAmountField::setValue(qint64 value)
{
    setText(BitcoinUnits::format(currentUnit, value));
}

/*
void BitcoinAmountField::unitChanged(int idx)
{
    // Use description tooltip for current unit for the combobox
   unit->setToolTip(unit->itemData(idx, Qt::ToolTipRole).toString());

    // Determine new unit ID
    int newUnit = unit->itemData(idx, BitcoinUnits::UnitRole).toInt();

    // Parse current value and convert to new unit
    bool valid = false;
    qint64 currentValue = value(&valid);

    currentUnit = newUnit;

    // Set max length after retrieving the value, to prevent truncation
    amount->setDecimals(BitcoinUnits::decimals(currentUnit));
    amount->setMaximum(qPow(10, BitcoinUnits::amountDigits(currentUnit)) - qPow(10, -amount->decimals()));

    if(valid)
    {
        // If value was valid, re-place it in the widget with the new unit
        setValue(currentValue);
    }
    else
    {
        // If current value is invalid, just clear field
        setText("");
    }
    setValid(true);
}
*/

void BitcoinAmountField::AllBtnClicked()
{
    amount->setValue(m_dMax);
}

/*
void BitcoinAmountField::setDisplayUnit(int newUnit)
{
    unit->setValue(newUnit);
}
*/
