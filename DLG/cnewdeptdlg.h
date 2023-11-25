#ifndef CNEWDEPTDLG_H
#define CNEWDEPTDLG_H

#include <QDialog>
#include <QMap>
#include "cdlgbase.h"
#include "cdepartmenttable.h"
#include "cdepartmentaccounttable.h"

namespace Ui {
class CnewDeptDlg;
}

class CnewDeptDlg :  public CdlgBase
{
    Q_OBJECT

public:
    explicit CnewDeptDlg(bool isEdit, QWidget *parent = 0);
    ~CnewDeptDlg();
    bool setDeptTableID(int);
private slots:

    void on_m_cancelBtn_clicked();

    void on_pushButton_clicked();

private:
    Ui::CnewDeptDlg *ui;
    int						m_deptTableID;
    SdepartmentData*		m_deptData;
    SdepartmentAccountData*	m_deptAccountData;
    QMap<int, QString> 		m_paymentAccountIDNameMap;
    QMap<QString, int> 		m_paymentAccountNameIDMap;
    void resize();


    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override {resize();}
};

#endif // CNEWDEPTDLG_H
