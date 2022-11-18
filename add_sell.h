#ifndef ADD_SELL_H
#define ADD_SELL_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QListWidgetItem>
#include <QtCharts>
#include "letsstock.h"

namespace Ui {
class add_sell;
}

class add_sell : public QWidget
{
    Q_OBJECT

public:
    explicit add_sell(QWidget *parent = nullptr, QString *command = nullptr);
    Ui::add_sell *ui;

    QList<int> clients;

    ~add_sell();

private:
    QString commande;

    QString articlesSave;
    int articleChanged;
    bool persoBefore = false;

signals :
    void actualiser();

private slots:
    void on_doubleSpinBox_valueChanged(double arg1);

    void on_pushButton_add_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_addClient_clicked();

    void on_comboBox_client_currentTextChanged(const QString &arg1);

public slots:
    void actu_montant();
    void actu_articles();
    void actu_clients();
};

#endif // ADD_SELL_H
