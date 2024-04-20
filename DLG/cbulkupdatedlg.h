#ifndef CBULKUPDATEDLG_H
#define CBULKUPDATEDLG_H

#include <QDialog>
#include <QCompleter>
#include "cdlgbase.h"

namespace Ui {
class CbulkUpdateDlg;
}

class CbulkUpdateDlg : public  CdlgBase
{
    Q_OBJECT
signals:
    void updateContactTable();
    void updateTransaction();


public:
    explicit CbulkUpdateDlg(QWidget *parent = nullptr);
    ~CbulkUpdateDlg();

private slots:
    void on_m_mergeNameOpt_clicked(bool checked);


    void on_m_noOpt_clicked(bool checked);

    void on_m_changeTransDateOpt_clicked(bool checked);

    void on_m_changeTrannsactionNameOpt_clicked(bool checked);

    void on_m_cancelBtn_clicked();

    void on_m_okBtn_clicked();

    void on_m_nameToChangeLineEdit_editingFinished();

    void on_m_accountCombo_currentTextChanged(const QString &arg1);

    void on_m_dateTo_dateChanged(const QDate &date);

    void on_m_dateFrom_dateChanged(const QDate &date);

    void on_m_selectAllBtn_clicked();

private:
    void resize();
    bool mergeName();
    void populateTable();
    bool updateData(bool updateName);
    Ui::CbulkUpdateDlg *ui;
    QCompleter*         m_nameCompleter;
    QStringList         m_minList;
    QMap<QString, int>  m_nameContactIDMap;
    QMap<int, QString>  m_contactIDNameMap;
    QMap<QString, int>  m_namePersonIDMap;



    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
};

#endif // CBULKUPDATEDLG_H
