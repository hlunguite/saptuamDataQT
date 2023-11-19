#ifndef CFILTERTRANSACTIONDLG_H
#define CFILTERTRANSACTIONDLG_H

#include <QDialog>
#include <QString>
namespace Ui {
class CfilterTransactionDlg;
}

class CfilterTransactionDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CfilterTransactionDlg(QWidget *parent = nullptr);
    ~CfilterTransactionDlg();

    QString filter() const;

    void setFilter(const QString &newFilter);

private slots:
    void on_pushButton_clicked();

    void on_m_filterDateCheckbox_clicked();

    void on_m_deptBox_currentTextChanged(const QString &arg1);

    void on_m_accountBox_currentTextChanged(const QString &arg1);

private:
    Ui::CfilterTransactionDlg *ui;
    QString     m_filter;
};

#endif // CFILTERTRANSACTIONDLG_H
