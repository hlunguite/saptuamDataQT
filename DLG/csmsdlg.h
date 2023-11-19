#ifndef CSMSDLG_H
#define CSMSDLG_H

#include <QDialog>

namespace Ui {
class CsmsDlg;
}

class CsmsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CsmsDlg(QWidget *parent = nullptr);
    ~CsmsDlg();

private:
    Ui::CsmsDlg *ui;
    QString     m_query;
    void resize();
    void populateTable();
    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override { resize();}
private slots:
    void on_m_fromDate_dateChanged(const QDate &date);
    void on_m_toDate_dateChanged(const QDate &date);
    void on_m_cancel_clicked();
    void on_m_save_clicked();
};

#endif // CSMSDLG_H
