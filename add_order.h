#ifndef ADD_ORDER_H
#define ADD_ORDER_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QListWidgetItem>
#include <QtCharts>
#include "letsstock.h"

namespace Ui {
class add_order;
}

class add_order : public QWidget
{
    Q_OBJECT

public:
    explicit add_order(QWidget *parent = nullptr);
    Ui::add_order *ui;
    ~add_order();

private:
    QByteArray PDFInByteArray;
    QString articlesSave;
    QString fournisseurSave;
    int articleChanged;

signals :
    void actualiser();

private slots:
    void on_pushButton_add_clicked();
    void on_comboBox_vendor_currentTextChanged();
    void on_doubleSpinBox_valueChanged(double value);
    void on_pushButton_addArticle_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_pushButton_addImg_clicked();
    void on_pushButton_delImg_clicked();

    void on_pushButton_clicked();

public slots:
    void actu_montant();
};

#endif // ADD_ORDER_H
