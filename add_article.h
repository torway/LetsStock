#ifndef ADD_ARTICLE_H
#define ADD_ARTICLE_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QCompleter>
#include "letsstock.h"

namespace Ui {
class add_article;
}

class add_article : public QWidget
{
    Q_OBJECT

public:
    explicit add_article(QWidget *parent = nullptr, QString *command = nullptr);
    Ui::add_article *ui;

    QByteArray IMGInByteArray;
    QByteArray IMGMinInByteArray;

    ~add_article();

private:
    QString commande;

signals :
    void actualiser();

private slots:
    void on_pushButton_add_clicked();
    void on_pushButton_addCat_clicked();
    void on_pushButton_addVendor_clicked();

    void on_pushButton_addImg_clicked();
    void on_pushButton_delImg_clicked();

    void on_lineEdit_ref_textChanged(const QString &arg1);

public slots:
    void actu();
};

#endif // ADD_ARTICLE_H
