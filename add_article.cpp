#include "add_article.h"
#include "ui_add_article.h"
#include "add_vendor.h"
#include "ui_add_vendor.h"
#include "add_categorie.h"
#include "ui_add_categorie.h"
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>

add_article::add_article(QWidget *parent, QString *command) :
    QWidget(parent),
    ui(new Ui::add_article)
{
    ui->setupUi(this);
    this->setWindowTitle("LetsStock");

    commande = QString(*command);

    actu();

    //Autocompletion Nom
    QStringList nomList;
    QSqlQuery nom("SELECT DISTINCT nom FROM Articles ORDER BY nom ASC");
    while (nom.next()) nomList.append(nom.value(0).toString());

    QCompleter *com(new QCompleter(nomList, this));
    com->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_name->setCompleter(com);
    ui->label_idArticle->hide();
}

add_article::~add_article()
{
    delete ui;
}

void add_article::on_pushButton_add_clicked()
{
    QSqlQuery ref("SELECT * FROM Articles WHERE ref='"+ui->lineEdit_ref->text()+"'");;
    ref.last();
    if(ui->comboBox_vendor->currentText().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le fournisseur n'est pas renseigné."), tr("Fermer"));
    else if(ui->comboBox_cat->currentText().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("La catégorie n'est pas renseignée."), tr("Fermer"));
    else if(ui->lineEdit_name->text().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le nom complet n'est pas renseigné."), tr("Fermer"));
    else if(ui->lineEdit_ref->text().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("La référence de l'article n'est pas renseignée."), tr("Fermer"));
    else if(ui->doubleSpinBox_achat->value() == 0) QMessageBox::warning(this, tr("Erreur"), tr("Le montant d'achat n'est pas renseigné."), tr("Fermer"));
    else if(ui->doubleSpinBox_vente->value() == 0) QMessageBox::warning(this, tr("Erreur"), tr("Le montant de vente n'est pas renseigné."), tr("Fermer"));
    else if(ref.at() >= 0 && commande == "Ajouter") QMessageBox::warning(this, tr("Erreur"), tr("Un article porte déjà cette référence."), tr("Fermer"));
    else
    {
        QSqlQuery vendor("SELECT id_vendor FROM Fournisseur WHERE nom='"+ui->comboBox_vendor->currentText().replace("'","''")+"'");
        if(vendor.next())
        {
            QSqlQuery cat("SELECT id_cat FROM Catégorie WHERE nom='"+ui->comboBox_cat->currentText().replace("'","''")+"'");
            if(cat.next())
            {
                QSqlQuery query;
                if(commande == "Ajouter")
                {
                    query.prepare("INSERT INTO Articles (id_fournisseur, ref, categorie, nom, quantité, montant_achat, montant_vente, image, image_min, total_achat, total_vente) VALUES ('"+vendor.value(0).toString()+"'"
                                     ",'"+ui->lineEdit_ref->text().replace("'","''")+"','"+cat.value(0).toString()+"'"
                                     ",'"+ui->lineEdit_name->text().replace("'","''")+"',0,'"+QString::number(ui->doubleSpinBox_achat->value())+"','"+QString::number(ui->doubleSpinBox_vente->value())+"',:img,:img_min,0,0);");

                    QMessageBox::information(this, tr("Article ajouté"), tr("L'article a bien été ajouté."), tr("Fermer"));
                }
                if(commande == "Modifier")
                {
                    query.prepare("UPDATE Articles SET ref='"+ui->lineEdit_ref->text().replace("'","''")+"', categorie='"+cat.value(0).toString()+"', nom='"+ui->lineEdit_name->text().replace("'","''")+"', "
                                 "montant_achat='"+QString::number(ui->doubleSpinBox_achat->value())+"', montant_vente='"+QString::number(ui->doubleSpinBox_vente->value())+"', image=:img, image_min=:img_min "
                                 "WHERE id_article='"+ui->label_idArticle->text()+"'");

                    QMessageBox::information(this, tr("Article modifié"), tr("L'article a bien été modifié."), tr("Fermer"));
                }

                if(ui->lineEdit_path->text().isEmpty())
                {
                    query.bindValue(":img","");
                    query.bindValue(":img_min","");
                    query.exec();
                }
                else
                {
                    query.bindValue(":img",IMGInByteArray);
                    query.bindValue(":img_min",IMGMinInByteArray);
                    query.exec();
                }

                emit actualiser();
                this->close();
            }
        }
    }
}

void add_article::actu()
{
    ui->comboBox_vendor->clear();
    QSqlQuery fournisseur("SELECT nom FROM Fournisseur ORDER BY nom");
    while(fournisseur.next()) ui->comboBox_vendor->addItem(fournisseur.value(0).toString());

    ui->comboBox_cat->clear();
    QSqlQuery cat("SELECT nom FROM Catégorie ORDER BY nom");
    while(cat.next()) ui->comboBox_cat->addItem(cat.value(0).toString());
}

void add_article::on_pushButton_addCat_clicked()
{
    add_categorie *add = new add_categorie;
    add->setWindowModality(Qt::ApplicationModal);
    add->show();
    connect(add, SIGNAL(actualiser()), this, SLOT(actu()));
}

void add_article::on_pushButton_addVendor_clicked()
{
    add_vendor *add = new add_vendor;
    add->setWindowModality(Qt::ApplicationModal);
    add->show();
    connect(add, SIGNAL(actualiser()), this, SLOT(actu()));
}

void add_article::on_pushButton_addImg_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choisir un fichier"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), tr("Image (*.jpg *.jpeg *.png)"));
    if(path != "")
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return;
        IMGInByteArray = file.readAll();

        QPixmap *pix = new QPixmap();
        pix->loadFromData(IMGInByteArray);
        QPixmap newPix = pix->scaledToHeight(32);

        QBuffer buffer(&IMGMinInByteArray);
        buffer.open(QIODevice::WriteOnly);
        newPix.save(&buffer, "JPG");

        ui->lineEdit_path->setText(path);
    }
}

void add_article::on_pushButton_delImg_clicked()
{
    IMGInByteArray.clear();
    IMGMinInByteArray.clear();
    ui->lineEdit_path->clear();
}

void add_article::on_lineEdit_ref_textChanged(const QString &arg1)
{
    if(ui->lineEdit_path->text().isEmpty())
    {
        QString pathJPG = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)+"/"+arg1+".jpg";
        QString pathJPEG = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)+"/"+arg1+".jpeg";
        QString pathPNG = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)+"/"+arg1+".png";

        if(QFile().exists(pathJPG))
        {
            QFile file(pathJPG);
            if (!file.open(QIODevice::ReadOnly)) return;
            IMGInByteArray = file.readAll();

            QPixmap *pix = new QPixmap();
            pix->loadFromData(IMGInByteArray,"JPG");
            QPixmap newPix = pix->scaledToHeight(32);

            QBuffer buffer(&IMGMinInByteArray);
            buffer.open(QIODevice::WriteOnly);
            newPix.save(&buffer, "JPG");

            ui->lineEdit_path->setText(file.fileName());
        }
        if(QFile().exists(pathJPEG))
        {
            QFile file(pathJPEG);
            if (!file.open(QIODevice::ReadOnly)) return;
            IMGInByteArray = file.readAll();

            QPixmap *pix = new QPixmap();
            pix->loadFromData(IMGInByteArray,"JPEG");
            QPixmap newPix = pix->scaledToHeight(32);

            QBuffer buffer(&IMGMinInByteArray);
            buffer.open(QIODevice::WriteOnly);
            newPix.save(&buffer, "JPG");

            ui->lineEdit_path->setText(file.fileName());
        }
        if(QFile().exists(pathPNG))
        {
            QFile file(pathPNG);
            if (!file.open(QIODevice::ReadOnly)) return;
            IMGInByteArray = file.readAll();

            QPixmap *pix = new QPixmap();
            pix->loadFromData(IMGInByteArray,"PNG");
            QPixmap newPix = pix->scaledToHeight(32);

            QBuffer buffer(&IMGMinInByteArray);
            buffer.open(QIODevice::WriteOnly);
            newPix.save(&buffer, "JPG");

            ui->lineEdit_path->setText(file.fileName());
        }
    }
}
