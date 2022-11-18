#include "add_vendor.h"
#include "ui_add_vendor.h"

add_vendor::add_vendor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::add_vendor)
{
    ui->setupUi(this);
    this->setWindowTitle("LetsStock");
}

add_vendor::~add_vendor()
{
    delete ui;
}

void add_vendor::on_pushButton_add_clicked()
{
    QSqlQuery nom("SELECT * FROM Fournisseur WHERE nom='"+ui->lineEdit_name->text()+"'");
    nom.last();
    if(ui->lineEdit_name->text().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le nom du fournisseur n'est pas renseigné."), tr("Fermer"));
    else if(ui->lineEdit_siret->text().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le SIRET du fournisseur n'est pas renseigné."), tr("Fermer"));
    else if(ui->lineEdit_phone->text().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le téléphone du fournisseur n'est pas renseigné."), tr("Fermer"));
    else if(nom.at() >= 0) QMessageBox::warning(this, tr("Erreur"), tr("Un fournisseur porte déjà ce nom."), tr("Fermer"));
    else {
        QSqlQuery ajouter("INSERT INTO Fournisseur (nom, siret, tel) VALUES ('"+ui->lineEdit_name->text().replace("'","''")+"','"+ui->lineEdit_siret->text()+"','"+ui->lineEdit_phone->text()+"');");
        QMessageBox::information(this, tr("Fournisseur ajouté"), tr("Le fournisseur a bien été ajouté."), tr("Fermer"));
        emit actualiser();
        this->close();
    }
}

void add_vendor::on_lineEdit_phone_editingFinished()
{
    if(ui->lineEdit_phone->text()[0] == '0') ui->lineEdit_phone->setInputMask("99 99 99 99 99;_");
    else ui->lineEdit_phone->setInputMask("");
}
