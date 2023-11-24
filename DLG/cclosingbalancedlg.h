#ifndef CCLOSINGBALANCEDLG_H
#define CCLOSINGBALANCEDLG_H

#include <QDialog>
#include "cdlgbase.h"
#include <set>
#include <QTableWidgetItem>
#include <QDate>
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

    void on_m_table_itemChanged(QTableWidgetItem *item);

private:
    Ui::CclosingBalanceDlg *ui;
    void resize();
    void populateTable(QDate date);
    std::map<int, int>  m_accountDeptMap;
    std::set<int>       m_accounts;
    std::map<int, std::pair<int, bool> > m_rowAccountMap;
    std::map<QString, int>  m_cashBankRow;
    bool                m_calcTotal;
    QDate               m_dateToUse;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override { resize();}
};

#endif // CCLOSINGBALANCEDLG_H
