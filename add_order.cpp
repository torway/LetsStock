#include "add_order.h"
#include "ui_add_order.h"
#include "add_article.h"
#include "ui_add_article.h"

add_order::add_order(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::add_order)
{
    ui->setupUi(this);
    this->setWindowTitle("LetsStock");

    articlesSave.clear();
    fournisseurSave.clear();

    disconnect(ui->comboBox_vendor,SIGNAL(currentTextChanged(const QString)), this, SLOT(on_comboBox_vendor_currentTextChanged()));
    ui->comboBox_vendor->clear();
    QSqlQuery fournisseur("SELECT nom FROM Fournisseur ORDER BY nom");
    while(fournisseur.next()) ui->comboBox_vendor->addItem(fournisseur.value(0).toString());
    connect(ui->comboBox_vendor,SIGNAL(currentTextChanged(const QString)), this, SLOT(on_comboBox_vendor_currentTextChanged()));
}

add_order::~add_order()
{
    delete ui;
}

void add_order::on_pushButton_add_clicked()
{
    if(ui->comboBox_vendor->currentText().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le fournisseur n'est pas renseigné."), tr("Fermer"));
    else if(ui->lineEdit_number->text().isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le numéro de commande n'est pas renseigné."), tr("Fermer"));
    else if(ui->dateEdit->date() == QDate(2000, 1, 1)) QMessageBox::warning(this, tr("Erreur"), tr("La date n'est pas renseignée."), tr("Fermer"));
    else
    {
        QSqlQuery vendor("SELECT id_vendor FROM Fournisseur WHERE nom='"+ui->comboBox_vendor->currentText()+"'");
        if(vendor.next())
        {
            QString articles;
            for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
            {
                for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
                {
                    QSpinBox *spin = qobject_cast<QSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 0));
                    QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 3));
                    if(spin->value() != 0)
                    {
                        if(!articles.isEmpty()) articles.append(";");
                        articles.append(QString::number(spin->value())+":"+ui->treeWidget->topLevelItem(i)->child(j)->text(5)+":"+QString::number(doubleSpin->value()));

                        QSqlQuery update("UPDATE Articles SET quantité=quantité+'"+QString::number(spin->value())+"' WHERE id_article='"+ui->treeWidget->topLevelItem(i)->child(j)->text(5)+"'");
                        QSqlQuery update2("UPDATE Articles SET total_achat=total_achat+'"+QString::number(spin->value()*doubleSpin->value())+"' WHERE id_article='"+ui->treeWidget->topLevelItem(i)->child(j)->text(5)+"'");
                    }
                }
            }

            QSqlQuery ajouter;
            ajouter.prepare("INSERT INTO Commande (id_vendor, date, numero, articles, montant, facture) VALUES ('"+vendor.value(0).toString()+"','"+ui->dateEdit->date().toString("yyyyMMdd")+"','"+ui->lineEdit_number->text().replace("'","''")+"'"
                              ",'"+articles+"','"+ui->label_montant->text().replace(" €","")+"',:pdf);");
            if(ui->lineEdit_path->text().isEmpty())
            {
                ajouter.bindValue(":pdf","");
                ajouter.exec();
            }
            else
            {
                ajouter.bindValue(":pdf",PDFInByteArray);
                ajouter.exec();
            }

            QMessageBox::information(this, tr("Commande ajoutée"), tr("La commande a bien été ajoutée."), tr("Fermer"));
            emit actualiser();
            this->close();
        }
    }
}

void add_order::on_comboBox_vendor_currentTextChanged()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QApplication::processEvents();

    ui->label_montant->clear();

    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(6);
    QStringList headerLabels;
    headerLabels.append(tr("Quantité commandée"));
    headerLabels.append(tr("Référence"));
    headerLabels.append(tr("Nom"));
    headerLabels.append(tr("Montant achat"));
    headerLabels.append(tr("Montant total"));
    headerLabels.append("id");
    ui->treeWidget->setHeaderLabels(headerLabels);
    ui->treeWidget->hideColumn(5);

    //Si un fournisseur était déjà selectionné
    if(!fournisseurSave.isEmpty())
    {
        ui->comboBox_vendor->setCurrentIndex(ui->comboBox_vendor->findText(fournisseurSave));
        fournisseurSave.clear();
    }

    QSqlQuery nomVendor("SELECT id_vendor FROM Fournisseur WHERE nom='"+ui->comboBox_vendor->currentText()+"'");
    if(nomVendor.next())
    {
        QSqlQuery categorie("SELECT * FROM Catégorie ORDER BY nom");
        while(categorie.next())
        {
            QTreeWidgetItem *cat = new QTreeWidgetItem(ui->treeWidget);
            cat->setText(0, categorie.value(1).toString());
            ui->treeWidget->addTopLevelItem(cat);

            QSqlQuery articles("SELECT * FROM Articles WHERE categorie='"+categorie.value(0).toString()+"' AND id_fournisseur='"+nomVendor.value(0).toString()+"' ORDER BY ref");
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
                    mpixmap->loadFromData(data);
                    iconImg.addPixmap(*mpixmap);
                }
                art->setIcon(1, iconImg);
                art->setText(1, articles.value(2).toString());

                art->setText(2, articles.value(4).toString());

                QSpinBox *amount = new QSpinBox();
                ui->treeWidget->setItemWidget(art, 0, amount);
                connect(amount, SIGNAL(valueChanged(int)), this, SLOT(actu_montant()));

                QDoubleSpinBox *amountOrder = new QDoubleSpinBox();
                amountOrder->setMinimum(0);
                amountOrder->setMaximum(1000);
                amountOrder->setDecimals(4);
                amountOrder->setValue(articles.value(6).toDouble());
                amountOrder->setSuffix(" €");
                ui->treeWidget->setItemWidget(art, 3, amountOrder);
                connect(amountOrder, SIGNAL(valueChanged(double)), this, SLOT(actu_montant()));

                art->setText(4, "0.00 €");
                art->setText(5, articles.value(0).toString());
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
                QList<QTreeWidgetItem*> clist = ui->treeWidget->findItems(article.split(":").at(1), Qt::MatchRecursive, 5);
                foreach(QTreeWidgetItem* item, clist)
                {
                    indexTop = ui->treeWidget->indexOfTopLevelItem(item->parent());
                    index = ui->treeWidget->topLevelItem(indexTop)->indexOfChild(item);
                }

                QSpinBox *spin = qobject_cast<QSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(indexTop)->child(index), 0));
                QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(indexTop)->child(index), 3));
                spin->setValue(article.split(":").at(0).toInt());
                if(QString::number(articleChanged) != article.split(":").at(1)) doubleSpin->setValue(article.split(":").at(2).toDouble());
            }

            articlesSave.clear();
            articleChanged = 0;
        }

        //Redimension de chaque colonne
        for(int i = 0; i < ui->treeWidget->columnCount(); i++)
        {
            ui->treeWidget->resizeColumnToContents(i);
            if(i != 2) ui->treeWidget->setColumnWidth(i, ui->treeWidget->columnWidth(i)*1.2);
        }
    }

    QApplication::restoreOverrideCursor();
}

void add_order::on_doubleSpinBox_valueChanged(double value)
{
    actu_montant();
}

void add_order::actu_montant()
{
    double amount = 0;
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
    {
        for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
        {
            QSpinBox *spin = qobject_cast<QSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 0));
            QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 3));
            ui->treeWidget->topLevelItem(i)->child(j)->setText(4, QString::number(spin->value() * doubleSpin->value(),'f',2)+" €");
            amount += spin->value() * doubleSpin->value();
        }
    }
    amount += ui->doubleSpinBox->value();
    ui->label_montant->setText(QString::number(amount,'f',2)+" €");
}

void add_order::on_pushButton_addArticle_clicked()
{
    add_article *add = new add_article(nullptr, new QString("Ajouter"));
    add->setWindowModality(Qt::ApplicationModal);
    add->show();
    connect(add, SIGNAL(actualiser()), this, SLOT(on_comboBox_vendor_currentTextChanged()));

    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
    {
        for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
        {
            QSpinBox *spin = qobject_cast<QSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 0));
            QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 3));
            if(spin->value() != 0)
            {
                if(!articlesSave.isEmpty()) articlesSave.append(";");
                articlesSave.append(QString::number(spin->value())+":"+ui->treeWidget->topLevelItem(i)->child(j)->text(5)+":"+QString::number(doubleSpin->value()));
            }
        }
    }

    fournisseurSave = ui->comboBox_vendor->currentText();
    add->ui->comboBox_vendor->setCurrentIndex(ui->comboBox_vendor->currentIndex());
}

void add_order::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    //Modifier un article
    if(column == 0 && item->text(5) != "")
    {
        add_article *mod = new add_article(nullptr, new QString("Modifier"));;
        mod->setWindowModality(Qt::ApplicationModal);
        mod->show();
        mod->ui->label_4->setText("Modifier un article");
        mod->ui->pushButton_add->setText("Modifier");
        connect(mod, SIGNAL(actualiser()), this, SLOT(on_comboBox_vendor_currentTextChanged()));

        QSqlQuery article("SELECT * FROM Articles WHERE id_article='"+item->text(5)+"'");
        if(article.next())
        {
            QSqlQuery categorie("SELECT nom FROM Catégorie WHERE id_cat='"+article.value(3).toString()+"'");
            if(categorie.next()) mod->ui->comboBox_cat->setCurrentText(categorie.value(0).toString());

            mod->ui->comboBox_vendor->setCurrentIndex(ui->comboBox_vendor->currentIndex());
            mod->ui->doubleSpinBox_achat->setValue(article.value(6).toDouble());
            mod->ui->doubleSpinBox_vente->setValue(article.value(7).toDouble());
            mod->ui->label_idArticle->setText(item->text(5));
            mod->ui->lineEdit_name->setText(article.value(4).toString());

            if(article.value(8).toByteArray()!="")
            {
                QByteArray image = article.value(8).toByteArray();

                QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Image.jpg");

                if (file.open(QFile::ReadWrite))
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
                QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i)->child(j), 3));
                if(spin->value() != 0)
                {
                    if(!articlesSave.isEmpty()) articlesSave.append(";");
                    articlesSave.append(QString::number(spin->value())+":"+ui->treeWidget->topLevelItem(i)->child(j)->text(5)+":"+QString::number(doubleSpin->value()));
                }
            }
        }

        fournisseurSave = ui->comboBox_vendor->currentText();
        articleChanged = item->text(5).toInt();
    }

    //Si on clique sur une image pour l'agrandir
    if(column == 1)
    {
        QSqlQuery image("SELECT image FROM Articles WHERE id_article='"+item->text(5)+"'");
        if(image.next() && image.value(0).toByteArray()!="")
        {
            QByteArray data = image.value(0).toByteArray();
            QPixmap *mpixmap = new QPixmap();
            mpixmap->loadFromData(data);
            ourLabel *labelImg = new ourLabel(*mpixmap);
            labelImg->setWindowModality(Qt::ApplicationModal);
            labelImg->show();
            connect(labelImg,SIGNAL(clicked()),labelImg,SLOT(close()));
        }
    }
}

void add_order::on_pushButton_addImg_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choisir un fichier"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr("PDF (*.pdf)"));
    if(path != "")
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return;
        PDFInByteArray = file.readAll();

        ui->lineEdit_path->setText(path);
    }
}

void add_order::on_pushButton_delImg_clicked()
{
    PDFInByteArray.clear();
    ui->lineEdit_path->clear();
}
