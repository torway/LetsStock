#ifndef ADD_CLIENT_H
#define ADD_CLIENT_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "letsstock.h"

namespace Ui {
class add_client;
}

class add_client : public QWidget
{
    Q_OBJECT

public:
    explicit add_client(QWidget *parent = nullptr);
    ~add_client();

private slots:
    void on_pushButton_add_clicked();

private:
    Ui::add_client *ui;

signals:
    void actualiser();
};

#endif // ADD_CLIENT_H
