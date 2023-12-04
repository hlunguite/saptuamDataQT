#ifndef CSUMMARYREPORTDLG_H
#define CSUMMARYREPORTDLG_H

#include <QDialog>
#include <map>
namespace Ui {
class CsummaryReportDlg;
}

class CsummaryReportDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CsummaryReportDlg(QWidget *parent = nullptr);
    ~CsummaryReportDlg();

private slots:
    void on_m_close_clicked();

    void on_m_savePDF_clicked();

    void on_m_fromDate_dateChanged(const QDate &date);

    void on_m_toDate_dateChanged(const QDate &date);

    void on_m_useAccountNameCkbox_clicked();

private:
    Ui::CsummaryReportDlg *ui;
    bool m_canpopulate;
    QString m_html;
    void resize();
    void populateTable();
    QString addInTable(std::map<QString, double>& amounts, QString str);
    QString addInTable(std::map<QString, QPair<double,double> >& amounts, QString str);
    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // CSUMMARYREPORTDLG_H
