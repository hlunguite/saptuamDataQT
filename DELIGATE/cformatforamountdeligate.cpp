#include "cformatforamountdeligate.h"
#include "csdutils.h"
CformatForAmountDeligate::CformatForAmountDeligate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QString CformatForAmountDeligate::displayText(const QVariant &value, const QLocale &locale) const
{
    QString formatStr = CsdUtils::convertAmountToStringWithSign(value.toDouble());

    return formatStr;
}
