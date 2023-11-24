#ifndef CFORMATFORAMOUNTDELIGATE_H
#define CFORMATFORAMOUNTDELIGATE_H

#include <QStyledItemDelegate>

class CformatForAmountDeligate : public QStyledItemDelegate
{
public:
    explicit CformatForAmountDeligate(QObject *parent = nullptr);

    // QStyledItemDelegate interface
public:
    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // CFORMATFORAMOUNTDELIGATE_H
