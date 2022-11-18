#include "add_categorie.h"
#include "ui_add_categorie.h"

add_categorie::add_categorie(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::add_categorie)
{
    ui->setupUi(this);
    this->setWindowTitle("LetsStock");
}

add_categorie::~add_categorie()
{
    delete ui;
}

void add_categorie::on_pushButton_add_clicked()
{
    QSqlQuery nom("SELECT * FROM Catégorie WHERE nom='"+ui->lineEdit_name->text()+"'");
    nom.last();
    if(ui->lineEdit_name->text().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le nom de la catégorie n'est pas renseigné."), tr("Fermer"));
    else if(nom.at() >= 0) QMessageBox::warning(this, tr("Erreur"), tr("Une catégorie porte déjà ce nom."), tr("Fermer"));
    else {
        QSqlQuery ajouter("INSERT INTO Catégorie (nom) VALUES ('"+ui->lineEdit_name->text().replace("'","''")+"');");
        QMessageBox mb(QMessageBox::Information, tr("Catégorie ajoutée"), tr("La catégorie a bien été ajoutée."), QMessageBox::Close, this);
        mb.exec();
        emit actualiser();
        this->close();
    }
}
