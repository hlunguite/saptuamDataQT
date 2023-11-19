#ifndef CCLOSINGBALANCEDLG_H
#define CCLOSINGBALANCEDLG_H

#include <QDialog>
#include "cdlgbase.h"
namespace Ui {
class CclosingBalanceDlg;
}

class CclosingBalanceDlg : public CdlgBase
{
    Q_OBJECT

public:
    explicit CclosingBalanceDlg(QWidget *parent = nullptr);
    ~CclosingBalanceDlg();



private slots:
    void on_m_saveCloseBtn_clicked();

    void on_m_cancelBtn_clicked();

    void on_m_closingDate_dateChanged(const QDate &date);

private:
    Ui::CclosingBalanceDlg *ui;
    void resize();
    void populateTable(QDate date);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override { resize();}
};

#endif // CCLOSINGBALANCEDLG_H
