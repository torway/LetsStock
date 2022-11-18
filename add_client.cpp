#include "add_client.h"
#include "ui_add_client.h"

add_client::add_client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::add_client)
{
    ui->setupUi(this);
    this->setWindowTitle("LetsStock");
}

add_client::~add_client()
{
    delete ui;
}

void add_client::on_pushButton_add_clicked()
{
    QString first_name;
    if(ui->lineEdit_firstName->text() != "")
    {
        if(ui->lineEdit_firstName->text().contains(" "))
        {
            first_name = ui->lineEdit_firstName->text().toLower();
            QStringList parts = ui->lineEdit_firstName->text().split(' ', Qt::SkipEmptyParts);
            for (int i = 0; i < parts.size(); ++i) parts[i].replace(0, 1, parts[i][0].toUpper());
            first_name = parts.join(" ");
        }
        else if(ui->lineEdit_firstName->text().contains("-"))
        {
            first_name = ui->lineEdit_firstName->text().toLower();
            QStringList parts = ui->lineEdit_firstName->text().split('-', Qt::SkipEmptyParts);
            for (int i = 0; i < parts.size(); ++i) parts[i].replace(0, 1, parts[i][0].toUpper());
            first_name = parts.join("-");
        }
        else
        {
            first_name = ui->lineEdit_firstName->text().toLower();
            first_name[0] = ui->lineEdit_firstName->text()[0].toUpper();
        }
    }

    QSqlQuery ref("SELECT * FROM Client WHERE nom='"+ui->lineEdit_lastName->text().toUpper()+"' AND prenom='"+first_name+"'");;
    ref.last();
    if(ui->lineEdit_firstName->text().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le nom n'est pas renseigné."), tr("Fermer"));
    else if(ref.at() >= 0) QMessageBox::warning(this, tr("Erreur"), tr("Un client porte déjà ce nom."), tr("Fermer"));
    else
    {
        QSqlQuery ajouter("INSERT INTO Client (nom, prenom) VALUES ('"+ui->lineEdit_lastName->text().toUpper().replace("'","''")+"','"+first_name.replace("'","''")+"');");
        QMessageBox::information(this, tr("Client ajouté"), tr("Le client a bien été ajouté."), tr("Fermer"));
        emit actualiser();
        this->close();
    }
}
