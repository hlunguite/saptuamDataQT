#ifndef CHTMLUTILS_H
#define CHTMLUTILS_H
#include <QVector>
#include <QString>
enum EhtmlCellAlign {
    HTML_NO_ALIGN,
    HTML_LEFT_ALIGN,
    HTML_CENTER_ALIGN,
    HTML_RIGHT_ALIGN
};
enum EhtmlCellValign {
    HTML_V_TOP_ALIGN,
    HTML_V_MIDDLE_ALIGN,
    HTML_V_BOTTOM_ALIGN
};
enum EhtmlCellBoarder{
    HTML_NO_BORDER = 0,
    HTML_LEFT_BORDER=1,
    HTML_RIGHT_BORDER =2,
    HTML_BOTTOM_BORDER=4,
    HTML_TOP_BORDER=8,
    HTML_TOP_BOTTOM_BORDER = (8 | 4)
};
enum EhtmlCellBorderStle{
    HTML_NORMAL_BORDER_STYLE,
    HTML_DOUBLE_BORDER_STYLE,
    HTML_DOTTED_BORDER_STYLE,
    HTML_DOUBLE_BOTTOM_STYLE
};

enum EhtmlTableFlow {NO_HTML_TABLE,
                     CREATE_HTML_TABLE,
                     CREATE_HTML_TABLE_HEADER/*OPEN_TABLE_ROW*/,
                     ADD_HTML_HEADER_CELL,
                     CLOSE_HTML_HEADER_ROW,
                     ADD_HTML_TABLE_ROW,
                     ADD_HTML_TABLE_CELL,
                     CLOSE_HTML_TABLE_ROW,
                     CLOSE_HTML_TABLE };


class ChtmlUtils
{
public:
    ChtmlUtils();
    QString getHtmlPage(QStringList list);
    QString getHtml(bool header = true, bool localHeader = true );

    QString cellHeight() const;
    void setCellHeight(const QString &newCellHeight);

    //QString fontSize() const;
    //void setFontSize(const QString &newFontSize);

    int tableColCount() const;
    void setTableColCount(int newTableColCount);

    QVector<int> tableColSize() const;
    void addTableColSize(int size);

    bool isTableHeader() const;
    void setIsTableHeader(bool newIsTableHeader);


    void openTable();
    void createHeaderRow();
    void createTableCellHeader(QString colname, int colno, int cellsize);
    void closeHeaderRow();
    void createRow(int defaultcolno = 0);
    void createCell(QString celldata,
                    EhtmlCellAlign align = HTML_LEFT_ALIGN,
                    EhtmlCellValign valign = HTML_V_MIDDLE_ALIGN,
                    EhtmlCellBoarder  border = HTML_NO_BORDER,
                    EhtmlCellBorderStle borderStyle = HTML_NORMAL_BORDER_STYLE,
                    QString bgcolor = "");
    void createMergeCell(QString cellData,
                         int noofcoltomerge,
                         bool colmerge = true,
                         EhtmlCellAlign align = HTML_LEFT_ALIGN,
                         EhtmlCellValign vaign = HTML_V_MIDDLE_ALIGN,
                         EhtmlCellBoarder  border = HTML_NO_BORDER,
                         EhtmlCellBorderStle borderStyle = HTML_NORMAL_BORDER_STYLE,
                         QString bgcolor = "");
    void closeRow(int colwithmergerow =0);
    void closeTable();

    int getNoOfRowInTable(){ return m_tableRowCount;}
    void formatString(QString str, bool paragraph = false);
    static QString addMultipleBlank(int noofline);
    static QString getTwoSignator();
    static QString getHeader();
    static QString getPageBreak();
    void resetTableData();
private:

    QString			m_cellHeight;
    QString			m_fontSize;
    //current table data
    QString 		m_htmlTable;
    int 			m_tableColCount;
    int				m_tableRowCount;
    QVector<int>	m_tableColSize;
    bool			m_isTableHeader;
    EhtmlTableFlow	m_tableStage;



    QString getTableHTML();

    QString createAlign(EhtmlCellAlign align);
    QString createValign(EhtmlCellValign align);
    QString createBorder(EhtmlCellBoarder border,
                         EhtmlCellBorderStle borderStyle);


};

#endif // CHTMLUTILS_H
