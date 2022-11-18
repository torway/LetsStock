#ifndef ADD_EXCEL_H
#define ADD_EXCEL_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "letsstock.h"

namespace Ui {
class add_excel;
}

class add_excel : public QWidget
{
    Q_OBJECT

public:
    explicit add_excel(QWidget *parent = nullptr);
    Ui::add_excel *ui;

    void actu();

    ~add_excel();

private slots:
    void on_lineEdit_path_textChanged(const QString &arg1);

    void on_pushButton_cancel_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_comboBox_vendor_currentIndexChanged(int index);

    void on_pushButton_valide_clicked();

private:

signals:
    void actualiser();
};

#endif // ADD_EXCEL_H
