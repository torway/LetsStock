#ifndef ADD_CATEGORIE_H
#define ADD_CATEGORIE_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "letsstock.h"

namespace Ui {
class add_categorie;
}

class add_categorie : public QWidget
{
    Q_OBJECT

public:
    explicit add_categorie(QWidget *parent = nullptr);
    ~add_categorie();

private slots:
    void on_pushButton_add_clicked();

private:
    Ui::add_categorie *ui;

signals :
    void actualiser();
};

#endif // ADD_CATEGORIE_H
