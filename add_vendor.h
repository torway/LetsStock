#ifndef ADD_VENDOR_H
#define ADD_VENDOR_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "letsstock.h"

namespace Ui {
class add_vendor;
}

class add_vendor : public QWidget
{
    Q_OBJECT

public:
    explicit add_vendor(QWidget *parent = nullptr);
    ~add_vendor();

private slots:
    void on_pushButton_add_clicked();

    void on_lineEdit_phone_editingFinished();

private:
    Ui::add_vendor *ui;

signals :
    void actualiser();
};

#endif // ADD_VENDOR_H
