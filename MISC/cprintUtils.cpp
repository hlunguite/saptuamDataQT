#include "cprintUtils.h"
#include "csdutils.h"
#include <QFileDialog>
#include <QPrinter>
#include <QMarginsF>
#include <QTextDocument>
#include "MISC/chtmlutils.h"
//#include <QWebEngineView>

CprintUtils::CprintUtils(QString text, bool isLandscape):
    m_landscape(isLandscape),
    m_text(text)
{

}

CprintUtils::CprintUtils(QStringList csvlist): m_csvTransection(csvlist)
{

}

void CprintUtils::printPDF()
{
    if (m_text.isEmpty()) {
        return;
    }
    QString pdf = CsdUtils::makedir("pdf");


    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                    "Save SaptuamData", pdf,
                                                    "SaptuamData files (*.pdf)");
    if (fileName.isEmpty()) {
        return ;
    }
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize::A4);
    printer.setFullPage(true);
    QMarginsF margin(20, 10, 10, 10);
    printer.setPageMargins(margin, QPageLayout::Millimeter);
    printer.setOutputFormat(QPrinter::PdfFormat);

    if(m_landscape) {
        printer.setPageOrientation(QPageLayout::Landscape);
    } else {
        printer.setPageOrientation(QPageLayout::Portrait);
    }
    //m_text = "<p>page1</p>" + ChtmlUtils::getPageBreak() + "<p>page2</p>";
    QTextDocument doc;
    doc.setHtml(m_text);
    doc.print(&printer);





}

void CprintUtils::printCSV()
{
    if (m_csvTransection.isEmpty()) {
        return;
    }

    QString csv = CsdUtils::makedir("csv");
    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                    "Save SaptuamData", csv,
                                                    "SaptuamData files (*.csv)");
    if (fileName.isEmpty()) {
        return ;
    }
    QFile csvFile(fileName);
    if(csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {

        QTextStream out(&csvFile);
        foreach(const QString & line , m_csvTransection){
            out<<line;
        }


        csvFile.close();
    }
}


