#ifndef LETSSTOCK_H
#define LETSSTOCK_H

#include <QWidget>
#include <QMainWindow>
#include <QLabel>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDateTime>
#include <QCompleter>
#include <QListWidgetItem>
#include <QtCharts>
#include <QWebEngineView>
#include <QMessageBox>
#include "RangeSlider/RangeSlider.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class LetsStock;
}
QT_END_NAMESPACE

class LetsStock : public QMainWindow
{
    Q_OBJECT

public:
    LetsStock(QWidget *parent = nullptr);
    Ui::LetsStock *ui;
    ~LetsStock();

    void actu_sideMenu();
    void database();

private slots:
    void stats_dates();
    void stats_hover(bool status, int index, QBarSet *barSet);

    void on_pushButton_saveDB_clicked();

    void on_pushButton_dashboard_clicked();
    void on_pushButton_back_clicked();
    void on_pushButton_back2_clicked();
    void on_treeWidget_stock_itemClicked(QTreeWidgetItem *item, int column);
    void on_pushButton_stock_clicked();
    void on_checkBox_checkValue_stateChanged();
    void on_pushButton_stats_clicked();

    void on_pushButton_vendor_clicked();
    void on_pushButton_vendorAdd_2_clicked();
    void on_pushButton_vendorAdd_clicked();
    void on_pushButton_vendorDel_clicked();
    void on_tableWidget_vendor_cellChanged(int row, int column);

    void on_pushButton_order_clicked();
    void on_pushButton_orderAdd_clicked();
    void on_pushButton_orderAdd_3_clicked();
    void on_pushButton_orderDel_2_clicked();
    void on_treeWidget_order_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_client_clicked();
    void on_pushButton_clientDel_clicked();
    void on_pushButton_clientAdd_2_clicked();
    void on_pushButton_clientAdd_clicked();
    void on_treeWidget_client_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_sell_clicked();
    void actu_sell();
    void actu_sellClient();
    void on_pushButton_sellAdd_2_clicked();
    void on_pushButton_sellAdd_clicked();
    void on_pushButton_sellDel_clicked();
    void on_treeWidget_sell_itemClicked(QTreeWidgetItem *item, int column);
    void on_pushButton_modSell_clicked();
    void on_comboBox_clientSell_currentTextChanged(const QString &arg1);

    void on_actionUpdate_triggered();

    void on_treeWidget_stock_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_pushButton_back_2_clicked();

private:
    QSqlDatabase appDatabase;
    QList<int> clients;

    //Stats
    QList<QString> statsList;
    QBarSeries *series = new QBarSeries();
    RangeSlider *rangeSlider;
    QBarCategoryAxis *axisX = new QBarCategoryAxis;
};

//Class Label clickable
class ourLabel : public QLabel
{ Q_OBJECT

public:
    explicit ourLabel(const QPixmap pix, QWidget * parent = 0 );
    ~ourLabel();

signals:
    void clicked();

protected:
    void mousePressEvent ( QMouseEvent * event ) ;

private slots:
    void close();
};

#endif // LETSSTOCK_H
