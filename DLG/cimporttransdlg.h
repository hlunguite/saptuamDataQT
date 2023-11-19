#ifndef CIMPORTTRANSDLG_H
#define CIMPORTTRANSDLG_H

#include <QDialog>
#include "cimporttransactiontable.h"

namespace Ui {
class CimportTransDlg;
}

class CimportTransDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CimportTransDlg(QWidget *parent = nullptr);
    ~CimportTransDlg();

signals:
    void updateTransaction();
private:
    Ui::CimportTransDlg *ui;
    void resize();
    void populateTable();
    QVector<SimportTransData *>*  m_data;
    void clear();
    std::map<int, int> m_accountMap;
    std::map<int, int> m_contactMap;
    std::map<int, QString> m_contactComment;
    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override { resize();}
private slots:
    void on_m_selectBtn_clicked();
    void on_m_processBtn_clicked();
    void on_m_cancelBtn_clicked();
};

#endif // CIMPORTTRANSDLG_H
