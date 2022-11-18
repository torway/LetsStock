#ifndef MOD_SELL_H
#define MOD_SELL_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QListWidgetItem>
#include <QtCharts>
#include "lithostock.h"

namespace Ui {
class mod_sell;
}

class mod_sell : public QWidget
{
    Q_OBJECT

public:
    explicit mod_sell(QWidget *parent = nullptr);
    Ui::mod_sell *ui;

    QList<int> clients;

    ~mod_sell();

private:
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

#endif // mod_sell_H
