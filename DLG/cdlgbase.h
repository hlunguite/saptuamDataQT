#ifndef CDLGBASE_H
#define CDLGBASE_H
#include <QDialog>

#include <QString>
class CdlgBase: public QDialog
{
public:
    CdlgBase(QWidget *parent = nullptr);
    void message(QString msg);
};

#endif // CDLGBASE_H
