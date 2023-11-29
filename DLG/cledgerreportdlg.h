#ifndef CLEDGERREPORTDLG_H
#define CLEDGERREPORTDLG_H

#include <QDialog>
#include <QCompleter>
#include <QMap>
#include <QVector>
#include <QDate>
#include <map>
struct StransactionData;
class CclosingCalculator;
namespace Ui {
class CledgerReportDlg;
}

class CaccountLedger {
private:
    bool                                        m_isBankOrCashLedger;
    double                                      m_totalIncome;
    double                                      m_totalPayment;
    QMap<QDate, QVector<StransactionData*> >    m_transactions;
    QStringList                                 m_csvTrans;
public:
    CaccountLedger(bool isBankOrCash = false);
    ~CaccountLedger();
    void addTransaction(StransactionData* trans);
    void clear() { m_transactions.clear(); }
    QStringList getCSVTransList() const { return m_csvTrans; }
    double getTotalIncome() const { return m_totalIncome;}
    double getTotalPayment() const { return m_totalPayment;}
    QString getHTMLTable(int accountID, bool addDeptInCSV, bool indiviualAccount, QString replaceAccountName = QString());
};

class CledgerReportDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CledgerReportDlg(QWidget *parent = nullptr);
    ~CledgerReportDlg();

private:
    struct SdeptStatement{

        double m_openintBalance = 0;
        double m_closingBalance = 0;
        std::map<QString, std::pair<double, double>> m_incomePaymentAmt;
    };
    Ui::CledgerReportDlg *ui;
    bool                               m_isConstructor;
    QCompleter*                        m_nameCompleter;
    QMap<int, QMap<int,
                   CaccountLedger*> > m_allAccountLedger;
    QStringList                       m_csvTrans;
    QStringList                       m_htmlList;
    QString                           m_html;
    std::map<QString, SdeptStatement> m_deptStatement;
    void clear();
    // QWidget interface
    void resize();
    void populateTable();
    void printForOpeningClosing(CclosingCalculator* calc);
    void printForSummary(CclosingCalculator* calc);
    void printForClosingSummary(CclosingCalculator* calc);
    void printDeptSummary(CclosingCalculator* calc);


protected:
    void resizeEvent(QResizeEvent *event);
private slots:
    void on_m_fromDate_dateChanged(const QDate &date);
    void on_m_toDate_dateChanged(const QDate &date);
    void on_m_DeptBox_currentTextChanged(const QString &arg1);
    void on_m_accountBox_currentTextChanged(const QString &arg1);
    void on_m_nameFilter_editingFinished();
    void on_m_cancelBtn_clicked();
    void on_m_csvBtn_clicked();
    void on_m_pdfBtn_clicked();

    void on_m_deptInCSV_clicked();
    void on_m_individualLedger_clicked();
};

#endif // CLEDGERREPORTDLG_H
