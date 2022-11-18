#include "add_sell.h"
#include "ui_add_sell.h"
#include "add_client.h"
#include "ui_add_client.h"
#include "add_article.h"
#include "ui_add_article.h"

add_sell::add_sell(QWidget *parent, QString *command) :
    QWidget(parent),
    ui(new Ui::add_sell)
{
    ui->setupUi(this);
    this->setWindowTitle("LetsStock");

    commande = QString(*command);
    ui->label_id->hide();

    actu_clients();
    actu_articles();
    actu_montant();
}

add_sell::~add_sell()
{
    delete ui;
}

void add_sell::actu_clients()
{
    ui->comboBox_client->clear();
    clients.clear();
    QSqlQuery client("SELECT id_client,nom,prenom FROM Client ORDER BY nom");
    while(client.next())
    {
        ui->comboBox_client->addItem(client.value(1).toString()+" "+client.value(2).toString());
        clients.append(client.value(0).toInt());
    }
}

void add_sell::actu_articles()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QApplication::processEvents();

    ui->label_montant->clear();

    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(9);
    QStringList headerLabels;
    headerLabels.append(tr("Quantité vendue"));
    headerLabels.append(tr("Référence"));
    headerLabels.append(tr("Fournisseur"));
    headerLabels.append(tr("Nom"));
    headerLabels.append(tr("Stock"));
    headerLabels.append(tr("Montant achat"));
    headerLabels.append(tr("Montant vente"));
    headerLabels.append(tr("Montant total vente"));
    headerLabels.append("id");
    ui->treeWidget->setHeaderLabels(headerLabels);
    ui->treeWidget->hideColumn(8);

    QSqlQuery categorie("SELECT * FROM Catégorie ORDER BY nom");
    while(categorie.next())
    {
        QTreeWidgetItem *cat = new QTreeWidgetItem(ui->treeWidget);
        cat->setText(0, categorie.value(1).toString());
        ui->treeWidget->addTopLevelItem(cat);

        QSqlQuery articles("SELECT * FROM Articles WHERE categorie='"+categorie.value(0).toString()+"' ORDER BY ref");
        while(articles.next())
        {
            QTreeWidgetItem *art = new QTreeWidgetItem();
            cat->addChild(art);

            //Avoir le thème light/dark
            QWidget tmpWidget;
            QColor backGroundColor=tmpWidget.palette().color(QPalette::Window);
            QIcon editDark = QPixmap(":/ressources/images/edit_dark.png");
            QIcon editLight = QPixmap(":/ressources/images/edit_light.png");
            if(backGroundColor.red() > 128) art->setIcon(0, editDark);
            if(backGroundColor.red() <= 128) art->setIcon(0, editLight);

            QIcon iconImg;
            if(!articles.value(11).toByteArray().isNull())
            {
                QByteArray data = articles.value(11).toByteArray();
                QPixmap *mpixmap = new QPixmap();
                mpixmap->loadFromData(data,"JPG");
                iconImg.addPixmap(*mpixmap);
            }
            art->setIcon(1, iconImg);
            art->setText(1, articles.value(2).toString());

            QSqlQuery nomFournisseur("SELECT nom FROM Fournisseur WHERE id_vendor='"+articles.value(1).toString()+"'");
            if(nomFournisseur.next()) art->setText(2, nomFournisseur.value(0).toString());

            art->setText(3, articles.value(4).toString());
            art->setText(4, articles.value(5).toString());
            art->setText(5, QString::number(articles.value(6).toDouble()) + " €");

            QSpinBox *amount = new QSpinBox();
            ui->treeWidget->setItemWidget(art, 0, amount);
            amount->setMinimum(0);
            amount->setMaximum(articles.value(5).toInt());
            connect(amount, SIGNAL(valueChanged(int)), this, SLOT(actu_montant()));

            QDoubleSpinBox *amountSell = new QDoubleSpinBox();
            amountSell->setMinimum(0);
            amountSell->setMaximum(1000);
            amountSell->setDecimals(4);
            amountSell->setValue(articles.value(7).toDouble());
            amountSell->setSuffix(" €");
            ui->treeWidget->setItemWidget(art, 6, amountSell);
            connect(amountSell, SIGNAL(valueChanged(double)), this, SLOT(actu_montant()));

            art->setText(7, "0.00 €");
            art->setText(8, articles.value(0).toString());
        }
    }

    ui->treeWidget->expandAll();

    //Si des données étaient déjà entrées
    if(!articlesSave.isEmpty())
    {
        QStringList articles = articlesSave.split(";");
        QString article;
        foreach(article, articles)
        {
            int index, indexTop;
            QList<QTreeWidgetItem*> clist = ui->treeWidget->findItems(article.split(":").at(1), Qt::MatchRecursive, 8);
            foreach(QTreeWidgetItem* item, clist)
            {
                indexTop = ui->treeWidget->indexOfTopLevelItem(item->parent());
                index = ui->treeWidget->topLevelItem(indexTop)->indexOfChild(item);
            }

            QSpinBox *spin = qobject_cast<QSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(indexTop)->child(index), 0));
            QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(indexTop)->child(index), 6));
            spin->setValue(article.split(":").at(0).toInt());
            if(QString::number(articleChanged) != article.split(":").at(1) && articleChanged != -1) doubleSpin->setValue(article.split(":").at(2).toDouble());
        }

        articlesSave.clear();
        articleChanged = 0;
    }

    //Redimension de chaque colonne
    for(int i = 0; i < ui->treeWidget->columnCount(); i++)
    {
        ui->treeWidget->resizeColumnToContents(i);
        if(i != 3) ui->treeWidget->setColumnWidth(i, ui->treeWidget->columnWidth(i)*1.2);
    }

    QApplication::restoreOverrideCursor();
}

void add_sell::on_doubleSpinBox_valueChanged(double arg1)
{
    actu_montant();
}

void add_sell::actu_montant()
{
    double amount = 0;
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
    {
        for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
        {
            QSpinBox *spin = qobject_cast<QSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 0));
            QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 6));
            ui->treeWidget->topLevelItem(i)->child(j)->setText(7, QString::number(spin->value() * doubleSpin->value(),'f',2)+" €");
            amount += spin->value() * doubleSpin->value();
        }
    }
    if(ui->comboBox_client->currentText().startsWith("PERSO")) ui->doubleSpinBox->setValue(-amount);
    amount += ui->doubleSpinBox->value();
    ui->label_montant->setText(QString::number(amount,'f',2)+" €");
}

void add_sell::on_pushButton_add_clicked()
{
    if(ui->comboBox_client->currentText().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le fournisseur n'est pas renseigné."), tr("Fermer"));
    else if(ui->dateEdit->date() == QDate(2000, 1, 1)) QMessageBox::warning(this, tr("Erreur"), tr("La date n'est pas renseignée."), tr("Fermer"));
    else
    {
        QString articles;
        for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        {
            for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
            {
                QSpinBox *spin = qobject_cast<QSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 0));
                QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 6));
                if(spin->value() != 0)
                {
                    if(!articles.isEmpty()) articles.append(";");
                    articles.append(QString::number(spin->value())+":"+ui->treeWidget->topLevelItem(i)->child(j)->text(8)+":"+QString::number(doubleSpin->value()));

                    QSqlQuery update("UPDATE Articles SET quantité=quantité-'"+QString::number(spin->value())+"' WHERE id_article='"+ui->treeWidget->topLevelItem(i)->child(j)->text(8)+"'");
                    QSqlQuery update2("UPDATE Articles SET total_vente=total_vente+'"+QString::number(spin->value()*doubleSpin->value())+"' WHERE id_article='"+ui->treeWidget->topLevelItem(i)->child(j)->text(8)+"'");
                }
            }
        }

        if(commande == "Ajouter")
        {
            QSqlQuery ajouter("INSERT INTO Vente (id_client, date, montant, articles) VALUES ('"+QString::number(clients.at(ui->comboBox_client->currentIndex()))+"','"+ui->dateEdit->date().toString("yyyyMMdd")+"'"
                              ",'"+ui->label_montant->text().replace(" €","")+"','"+articles+"');");

            QMessageBox::information(this, tr("Vente ajoutée"), tr("La vente a bien été ajoutée."), tr("Fermer"));
        }
        if(commande == "Modifier")
        {
            //Suppression
            QSqlQuery updates("SELECT * FROM Vente WHERE id_vente='"+ui->label_id->text()+"'");
            while(updates.next())
            {
                QStringList articles = updates.value(4).toString().split(";");
                QString article;
                if(updates.value(4).toString() != "")  foreach(article, articles)
                {
                    QSqlQuery update("UPDATE Articles SET quantité=quantité+'"+article.split(":").at(0)+"' WHERE id_article='"+article.split(":").at(1)+"'");
                    QSqlQuery update2("UPDATE Articles SET total_vente=total_vente-'"+QString::number(article.split(":").at(0).toDouble()*article.split(":").at(2).toDouble())+"' WHERE id_article='"+article.split(":").at(1)+"'");
                }
            }

            //Rajout
            QString articles;
            for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
            {
                for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
                {
                    QSpinBox *spin = qobject_cast<QSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 0));
                    QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 6));
                    if(spin->value() != 0)
                    {
                        if(!articles.isEmpty()) articles.append(";");
                        articles.append(QString::number(spin->value())+":"+ui->treeWidget->topLevelItem(i)->child(j)->text(8)+":"+QString::number(doubleSpin->value()));

                        QSqlQuery update("UPDATE Articles SET quantité=quantité-'"+QString::number(spin->value())+"' WHERE id_article='"+ui->treeWidget->topLevelItem(i)->child(j)->text(8)+"'");
                        QSqlQuery update2("UPDATE Articles SET total_vente=total_vente+'"+QString::number(spin->value()*doubleSpin->value())+"' WHERE id_article='"+ui->treeWidget->topLevelItem(i)->child(j)->text(8)+"'");
                    }
                }
            }

            QSqlQuery ajouter("UPDATE Vente SET id_client='"+QString::number(clients.at(ui->comboBox_client->currentIndex()))+"', date='"+ui->dateEdit->date().toString("yyyyMMdd")+"', montant='"+ui->label_montant->text().replace(" €","")+"'"
                              ", articles='"+articles+"' WHERE id_vente='"+ui->label_id->text()+"'");

            QMessageBox::information(this, tr("Vente modifiée"), tr("La vente a bien été modifiée."), tr("Fermer"));
        }

        emit actualiser();
        this->close();
    }
}

void add_sell::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    //Modifier un article
    if(column == 0 && item->text(8) != "")
    {
        add_article *mod = new add_article(nullptr, new QString("Modifier"));
        mod->setWindowModality(Qt::ApplicationModal);
        mod->show();
        mod->ui->label_4->setText("Modifier un article");
        mod->ui->pushButton_add->setText("Modifier");
        connect(mod, SIGNAL(actualiser()), this, SLOT(actu_articles()));

        QSqlQuery article("SELECT * FROM Articles WHERE id_article='"+item->text(8)+"'");
        if(article.next())
        {
            QSqlQuery categorie("SELECT nom FROM Catégorie WHERE id_cat='"+article.value(3).toString()+"'");
            if(categorie.next()) mod->ui->comboBox_cat->setCurrentText(categorie.value(0).toString());

            QSqlQuery fournisseur("SELECT nom FROM Fournisseur WHERE id_vendor='"+article.value(1).toString()+"'");
            if(fournisseur.next()) mod->ui->comboBox_vendor->setCurrentIndex(mod->ui->comboBox_vendor->findText(fournisseur.value(0).toString()));

            mod->ui->doubleSpinBox_achat->setValue(article.value(6).toDouble());
            mod->ui->doubleSpinBox_vente->setValue(article.value(7).toDouble());
            mod->ui->label_idArticle->setText(item->text(8));
            mod->ui->lineEdit_name->setText(article.value(4).toString());

            if(article.value(8).toByteArray()!="")
            {
                QByteArray image = article.value(8).toByteArray();

                QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Image.jpg");

                if (file.open(QFile::WriteOnly))
                {
                    file.write(image);
                    mod->IMGInByteArray = article.value(8).toByteArray();
                    mod->IMGMinInByteArray = article.value(11).toByteArray();
                    file.close();
                }

                mod->ui->lineEdit_path->setText(file.fileName());
            }
            mod->ui->lineEdit_ref->setText(article.value(2).toString());
        }

        for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        {
            for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
            {
                QSpinBox *spin = qobject_cast<QSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 0));
                QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 6));
                if(spin->value() != 0)
                {
                    if(!articlesSave.isEmpty()) articlesSave.append(";");
                    articlesSave.append(QString::number(spin->value())+":"+ui->treeWidget->topLevelItem(i)->child(j)->text(8)+":"+QString::number(doubleSpin->value()));
                }
            }
        }

        articleChanged = item->text(8).toInt();
    }

    if(column == 1 && !item->text(8).isEmpty())
    {
        QSqlQuery image("SELECT image FROM Articles WHERE id_article='"+item->text(8)+"'");
        if(image.next() && image.value(0).toByteArray()!="")
        {
            QByteArray data = image.value(0).toByteArray();
            QPixmap *mpixmap = new QPixmap();
            mpixmap->loadFromData(data,"JPG");
            ourLabel *labelImg = new ourLabel(*mpixmap);
            labelImg->setWindowModality(Qt::ApplicationModal);
            labelImg->show();
            connect(labelImg,SIGNAL(clicked()),labelImg,SLOT(close()));
        }
    }
}

void add_sell::on_pushButton_addClient_clicked()
{
    add_client *add = new add_client;
    add->setWindowModality(Qt::ApplicationModal);
    add->show();
    connect(add, SIGNAL(actualiser()), this, SLOT(actu_clients()));
}

void add_sell::on_comboBox_client_currentTextChanged(const QString &arg1)
{
    if(persoBefore == true)
    {
        for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        {
            for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
            {
                QSpinBox *spin = qobject_cast<QSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 0));
                QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 6));
                if(spin->value() != 0)
                {
                    if(!articlesSave.isEmpty()) articlesSave.append(";");
                    articlesSave.append(QString::number(spin->value())+":"+ui->treeWidget->topLevelItem(i)->child(j)->text(8)+":"+QString::number(doubleSpin->value()));
                }
            }
        }

        articleChanged = -1;
        actu_articles();
        persoBefore = false;
    }

    if(arg1.startsWith("PERSO"))
    {
        for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        {
            for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
            {
                QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 6));
                doubleSpin->setValue(ui->treeWidget->topLevelItem(i)->child(j)->text(5).split(" €").at(0).toDouble());
            }
        }

        if(arg1.startsWith("PERSO")) persoBefore = true;
    }
}
