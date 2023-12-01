#-------------------------------------------------
#
# Project created by QtCreator 2022-09-12T22:05:20
#
#-------------------------------------------------

QT       += core gui help sql
#QT       += webkitwidgets printsupport webenginewidgets
QT       += printsupport
#webenginewidgets



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SaptuamData
TEMPLATE = app

INCLUDEPATH += $$PWD/DB
INCLUDEPATH += $$PWD/DLG
INCLUDEPATH += $$PWD/TAB
INCLUDEPATH += $$PWD/VIEWANDMODEL
INCLUDEPATH += $$PWD/DELIGATE


SOURCES += main.cpp\
    DB/caccountreceiptbook.cpp \
    DB/cbanktransactiontable.cpp \
    DB/cclosingbalancetable.cpp \
    DB/cimportaccounttable.cpp \
    DB/cimportmintable.cpp \
    DB/cimporttransactiontable.cpp \
    DB/creceiptbookmap.cpp \
    DB/cremittancesetuptable.cpp \
    DELIGATE/ccustomdeligateforreceiptsetup.cpp \
    DELIGATE/ccustomdeligateforremittance.cpp \
    DELIGATE/cformatforamountdeligate.cpp \
    DLG/cassignreceiptdlg.cpp \
    DLG/cclosingbalancedlg.cpp \
    DLG/cdlgbase.cpp \
    DLG/cfiltertransactiondlg.cpp \
    DLG/cimportaccountdlg.cpp \
    DLG/cimportmindlg.cpp \
    DLG/cimporttransdlg.cpp \
    DLG/cledgerreportdlg.cpp \
    DLG/cpassworddlg.cpp \
    DLG/cpaymentaccountdlg.cpp \
    DLG/creceiptsetupdlg.cpp \
    DLG/cremittancedlg.cpp \
    DLG/cremittancepercentsetup.cpp \
    DLG/cremittancesetupdlg.cpp \
    DLG/cselectremittance.cpp \
    DLG/csettingdlg.cpp \
    DLG/csmsdlg.cpp \
    DLG/csummaryreportdlg.cpp \
    MISC/cclosingcalculator.cpp \
    MISC/chtmlutils.cpp \
    MISC/cprintUtils.cpp \
    MISC/creadbankstatement.cpp \
    MISC/cremittancedetails.cpp \
    TAB/cpaymentaccounttab.cpp \
    mainwindow.cpp \
    DB/cdbtablebase.cpp \
    DB/caccounttable.cpp \
    csdutils.cpp \
    DB/cpersontable.cpp \
    DB/cothercontacttable.cpp \
    DB/ccontacttable.cpp \
    DB/cdepartmenttable.cpp \
    DB/cpaymentaccounttable.cpp \
    DB/creceiptbooktable.cpp \
    DB/csaptuamdatadbmgr.cpp \
    DB/cimportbanktransactiontable.cpp \
    DB/cremittancetable.cpp \
    DB/ctransactiontable.cpp \
    DB/csettingtable.cpp \
    TAB/cmaintab.cpp \
    TAB/caccounttab.cpp \
    VIEWSANDMODEL/cdeselectabletreeview.cpp \
    VIEWSANDMODEL/csortfilterproxymodelfortreemodel.cpp \
    VIEWSANDMODEL/caccounttreemodel.cpp \
    DB/cdepartmentaccounttable.cpp \
    DLG/cnewaccountdlg.cpp \
    DLG/cnewdeptdlg.cpp \
    TAB/caddresstab.cpp \
    TAB/ccontacttab.cpp \
    VIEWSANDMODEL/ccontacttablemodel.cpp \
    DLG/ccontactdlg.cpp \
    VIEWSANDMODEL/ctransactiontablemodel.cpp \
    DB/caccountmap.cpp \
    DB/ccontactmap.cpp \
    TAB/ctransactiontab.cpp \
    DLG/cnewtransactiondlg.cpp \
    DELIGATE/ccustomdeligatefortransaction.cpp \
    DB/ctransactionutils.cpp \
    DLG/cedittransactiondlg.cpp

HEADERS  += mainwindow.h \
    CsdDefine.h \
    DB/caccountreceiptbook.h \
    DB/cbanktransactiontable.h \
    DB/cclosingbalancetable.h \
    DB/cdbtablebase.h \
    DB/caccounttable.h \
    DB/cimportaccounttable.h \
    DB/cimportmintable.h \
    DB/cimporttransactiontable.h \
    DB/creceiptbookmap.h \
    DB/cremittancesetuptable.h \
    DELIGATE/ccustomdeligateforreceiptsetup.h \
    DELIGATE/ccustomdeligateforremittance.h \
    DELIGATE/cformatforamountdeligate.h \
    DLG/cassignreceiptdlg.h \
    DLG/cclosingbalancedlg.h \
    DLG/cdlgbase.h \
    DLG/cfiltertransactiondlg.h \
    DLG/cimportaccountdlg.h \
    DLG/cimportmindlg.h \
    DLG/cimporttransdlg.h \
    DLG/cledgerreportdlg.h \
    DLG/cpassworddlg.h \
    DLG/cpaymentaccountdlg.h \
    DLG/creceiptsetupdlg.h \
    DLG/cremittancedlg.h \
    DLG/cremittancepercentsetup.h \
    DLG/cremittancesetupdlg.h \
    DLG/cselectremittance.h \
    DLG/csettingdlg.h \
    DLG/csmsdlg.h \
    DLG/csummaryreportdlg.h \
    MISC/CdlgDefine.h \
    MISC/cclosingcalculator.h \
    MISC/chtmlutils.h \
    MISC/cprintUtils.h \
    MISC/creadbankstatement.h \
    MISC/cremittancedetails.h \
    TAB/cpaymentaccounttab.h \
    csdutils.h \
    DB/cpasswordtable.h \
    DB/cpersontable.h \
    DB/cothercontacttable.h \
    DB/ccontacttable.h \
    DB/cdepartmenttable.h \
    DB/cpaymentaccounttable.h \
    DB/creceiptbooktable.h \
    DB/csaptuamdatadbmgr.h \
    DB/cimportbanktransactiontable.h \
    DB/cremittancetable.h \
    DB/cdbdefine.h \
    DB/ctransactiontable.h \
    DB/csettingtable.h \
    TAB/cmaintab.h \
    TAB/caccounttab.h \
    VIEWSANDMODEL/cdeselectabletreeview.h \
    VIEWSANDMODEL/csortfilterproxymodelfortreemodel.h \
    VIEWSANDMODEL/caccounttreemodel.h \
    DB/cdepartmentaccounttable.h \
    DLG/cnewaccountdlg.h \
    DLG/cnewdeptdlg.h \
    TAB/caddresstab.h \
    TAB/ccontacttab.h \
    VIEWSANDMODEL/ccontacttablemodel.h \
    DLG/ccontactdlg.h \
    VIEWSANDMODEL/ctransactiontablemodel.h \
    csdsingleton.h \
    DB/caccountmap.h \
    DB/ccontactmap.h \
    TAB/ctransactiontab.h \
    DLG/cnewtransactiondlg.h \
    DELIGATE/ccustomdeligatefortransaction.h \
    DB/ctransactionutils.h \
    DLG/cedittransactiondlg.h

FORMS    += mainwindow.ui \
    DLG/cassignreceiptdlg.ui \
    DLG/cclosingbalancedlg.ui \
    DLG/cfiltertransactiondlg.ui \
    DLG/cimportaccountdlg.ui \
    DLG/cimportmindlg.ui \
    DLG/cimporttransdlg.ui \
    DLG/cledgerreportdlg.ui \
    DLG/cnewaccountdlg.ui \
    DLG/cnewdeptdlg.ui \
    DLG/ccontactdlg.ui \
    DLG/cnewtransactiondlg.ui \
    DLG/cedittransactiondlg.ui \
    DLG/cpassworddlg.ui \
    DLG/cpaymentaccountdlg.ui \
    DLG/creceiptsetupdlg.ui \
    DLG/cremittancedlg.ui \
    DLG/cremittancepercentsetup.ui \
    DLG/cremittancesetupdlg.ui \
    DLG/cselectremittance.ui \
    DLG/csettingdlg.ui \
    DLG/csmsdlg.ui \
    DLG/csummaryreportdlg.ui


macx {
#QMAKE_MAC_SDK = macosx12.3
#QMAKE_MAC_SDK = macosx13.1
# check cd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs
#sudo xcode-select -s /Applications/Xcode.app/Contents/Developer

}
QMAKE_CXXFLAGS += -std=c++17
#-std=c++11
RESOURCES += \
    icon.qrc
