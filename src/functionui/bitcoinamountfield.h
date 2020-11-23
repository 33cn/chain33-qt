#ifndef BITCOINFIELD_H
#define BITCOINFIELD_H

/*
 * 发送金额输入框 可以控制数字的小数点等
 */

#include <QWidget>
#include <QLabel>
#include <QPushButton>

QT_BEGIN_NAMESPACE
class QDoubleSpinBox;
//class QValueComboBox;
QT_END_NAMESPACE

/** Widget for entering bitcoin amounts.
  */
class BitcoinAmountField: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qint64 value READ value WRITE setValue NOTIFY textChanged USER true)
public:
    explicit BitcoinAmountField(QWidget *parent = 0);

    qint64 value(bool *valid=0) const;
    void setValue(qint64 value);

    /** Mark current value as invalid in UI. */
    void setValid(bool valid);
    /** Perform input validation, mark field as invalid if entered value is not valid. */
    bool validate();

    /** Change unit used to display amount. */
    //void setDisplayUnit(int unit);

    /** Make field empty and ready for new input. */
    void clear();

    /** Qt messes up the tab chain by default in some cases (issue https://bugreports.qt-project.org/browse/QTBUG-10907),
        in these cases we have to set it up manually.
    */
    QWidget *setupTabChain(QWidget *prev);

    void setAmountRange(double dMax);

signals:
    void textChanged();

protected:
    /** Intercept focus-in event and ',' key presses */
    bool eventFilter(QObject *object, QEvent *event);

private:
    QPushButton* allbtn;
    QDoubleSpinBox *amount;
    QLabel* m_lpMaxUse;
//    QValueComboBox *unit;
    int currentUnit;

public:
    void setText(const QString &text);
    QString text() const;

private slots:
    //void unitChanged(int idx);

    void AllBtnClicked();

private:
  //  double  m_dminFee;
 //   QString m_strMaxBalance;
    double  m_dMax;

};


#endif // BITCOINFIELD_H
