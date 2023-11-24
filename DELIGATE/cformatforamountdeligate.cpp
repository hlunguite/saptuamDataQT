#include "cformatforamountdeligate.h"

CformatForAmountDeligate::CformatForAmountDeligate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QString CformatForAmountDeligate::displayText(const QVariant &value, const QLocale &locale) const
{
    QString formatStr = locale.toCurrencyString(value.toDouble(), "₹");

    return formatStr;
}
