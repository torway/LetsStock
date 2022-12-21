#include "letsstock.h"
#include "ui_letsstock.h"
#include "RangeSlider/RangeSlider.h"
#include "add_vendor.h"
#include "ui_add_vendor.h"
#include "add_order.h"
#include "ui_add_order.h"
#include "add_client.h"
#include "ui_add_client.h"
#include "add_sell.h"
#include "ui_add_sell.h"
#include "add_excel.h"
#include "ui_add_excel.h"

LetsStock::LetsStock(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::LetsStock)
{
    ui->setupUi(this);
    this->setWindowTitle("LetsStock");
    this->setAcceptDrops(true);

    database();

    //Icones des différents onglets
    ui->pushButton_dashboard->setIcon(QIcon(":/ressources/images/home.png"));
    ui->pushButton_vendor->setIcon(QIcon(":/ressources/images/fournisseur.png"));
    ui->pushButton_order->setIcon(QIcon(":/ressources/images/commande.png"));
    ui->pushButton_client->setIcon(QIcon(":/ressources/images/client.png"));
    ui->pushButton_sell->setIcon(QIcon(":/ressources/images/ventes.png"));

    //Slider a deux sens
    rangeSlider = new RangeSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles, ui->statsPage);
    rangeSlider->setObjectName(QString::fromUtf8("rangeSlider"));
    rangeSlider->setMaximumSize(QSize(16777215, 50));
    ui->gridLayout_stats->addWidget(rangeSlider,1,0);

    on_pushButton_dashboard_clicked();

    if(QSysInfo::productType() == "windows")
    {
        QFont font;
        font.setPointSize(10);
        QFont font2;
        font2.setPointSize(15);
        font2.setBold(true);
        ui->pushButton_dashboard->setFont(font);
        ui->pushButton_back->setFont(font2);
        ui->pushButton_stats->setFont(font2);
        ui->pushButton_stock->setFont(font2);
        ui->label_6->setFont(font2);
        ui->label_11->setFont(font2);
        ui->pushButton_vendor->setFont(font);
        ui->pushButton_vendorAdd->setFont(font);
        ui->pushButton_vendorAdd_2->setFont(font);
        ui->pushButton_vendorDel->setFont(font);
        ui->pushButton_order->setFont(font);
        ui->pushButton_orderAdd->setFont(font);
        ui->pushButton_orderAdd_3->setFont(font);
        ui->pushButton_orderDel_2->setFont(font);
        ui->pushButton_client->setFont(font);
        ui->pushButton_clientAdd->setFont(font);
        ui->pushButton_clientAdd_2->setFont(font);
        ui->pushButton_clientDel->setFont(font);
        ui->pushButton_sell->setFont(font);
        ui->pushButton_sellAdd->setFont(font);
        ui->pushButton_sellAdd_2->setFont(font);
        ui->pushButton_sellDel->setFont(font);
        ui->pushButton_saveDB->setFont(font);
        ui->pushButton_modSell->setFont(font);
    }
}

LetsStock::~LetsStock()
{
    this->close();
    delete ui;
}

void QWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list") && event->mimeData()->text().endsWith(".xlsx"))
        event->acceptProposedAction();
}

void QWidget::dropEvent(QDropEvent *event)
{
    add_excel *add = new add_excel();
    add->setWindowModality(Qt::ApplicationModal);
    add->show();
    connect(add, SIGNAL(actualiser()), this, SLOT(on_pushButton_dashboard_clicked()));

    if(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Excel").exists()) QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Excel").removeRecursively();
    QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Excel");

    QProcess process;
    process.setWorkingDirectory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Excel");
    process.start("tar", QStringList() << "xzfv" << QUrl(event->mimeData()->text()).toLocalFile());
    process.waitForFinished();

    add->ui->lineEdit_path->setText(QUrl(event->mimeData()->text()).toLocalFile());
    event->acceptProposedAction();
}

void LetsStock::database()
{
    if(!QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).exists()) QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/database.db").exists())
    {
        appDatabase = QSqlDatabase::addDatabase("QSQLITE");
        appDatabase.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/database.db");

        if(appDatabase.open()) qDebug() << "Connexion à la base de donnée réussie.";
        else qDebug() << "Impossible de se connecter à la base de donnée.";

        QSqlQuery fournisseur("CREATE TABLE \"Fournisseur\" (\"id_vendor\" INTEGER NOT NULL,\"nom\" TEXT NOT NULL,\"siret\" TEXT NOT NULL,\"tel\" TEXT NOT NULL, PRIMARY KEY(\"id_vendor\" AUTOINCREMENT));");
        QSqlQuery commande("CREATE TABLE \"Commande\" (\"id_order\" INTEGER NOT NULL,\"id_vendor\" INTEGER NOT NULL,\"date\" TEXT,\"numero\" TEXT NOT NULL,\"articles\" TEXT NOT NULL,\"montant\" DOUBLE NOT NULL,\"facture\" BLOB, PRIMARY KEY(\"id_order\" AUTOINCREMENT));");
        QSqlQuery articles("CREATE TABLE \"Articles\" (\"id_article\" INTEGER NOT NULL,\"id_fournisseur\" INTEGER NOT NULL,\"ref\" TEXT NOT NULL,\"categorie\" TEXT NOT NULL,\"nom\" TEXT NOT NULL,\"quantité\" DOUBLE NOT NULL,\"montant_achat\" DOUBLE NOT NULL,\"montant_vente\" DOUBLE NOT NULL,\"image\" BLOB,\"total_achat\" DOUBLE NOT NULL,\"total_vente\" DOUBLE NOT NULL,\"image_min\" BLOB, PRIMARY KEY(\"id_article\" AUTOINCREMENT));");
        QSqlQuery categories("CREATE TABLE \"Catégorie\" (\"id_cat\" INTEGER NOT NULL,\"nom\" TEXT NOT NULL, PRIMARY KEY(\"id_cat\" AUTOINCREMENT))");
        QSqlQuery clients("CREATE TABLE \"Client\" (\"id_client\" INTEGER NOT NULL,\"prenom\" TEXT,\"nom\" TEXT NOT NULL, PRIMARY KEY(\"id_client\" AUTOINCREMENT);");
        QSqlQuery ventes("CREATE TABLE \"Vente\" (\"id_vente\" INTEGER NOT NULL,\"id_client\" INTEGER NOT NULL,\"date\" TEXT NOT NULL,\"montant\" DOUBLE NOT NULL,\"articles\" TEXT NOT NULL,\"commentaire\" TEXT, PRIMARY KEY(\"id_vente\" AUTOINCREMENT))");
    }
    else
    {
        appDatabase = QSqlDatabase::addDatabase("QSQLITE");
        appDatabase.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/database.db");

        if(appDatabase.open()) qDebug() << "Connexion à la base de donnée réussie.";
        else qDebug() << "Impossible de se connecter à la base de donnée.";
    }
}

void LetsStock::on_pushButton_saveDB_clicked()
{
    QString dbFile = QFileDialog::getSaveFileName(this, tr("Sauvegarder la base de données"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+"/"+"LetsStock database "+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm")+".db"), tr("Base de données (*.db)");
    if(!dbFile.isEmpty()) QFile::copy(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/database.db", dbFile);
}

void LetsStock::on_actionUpdate_triggered()
{
    QSqlQuery image("SELECT id_article,image FROM Articles");
    while(image.next())
    {
        if(image.value(1).toByteArray() != "")
        {
            QByteArray data = image.value(1).toByteArray();
            QPixmap *mpixmap = new QPixmap();
            mpixmap->loadFromData(data);

            QPixmap newPix = mpixmap->scaledToHeight(32);

            QSqlQuery query;
            query.prepare("UPDATE Articles SET image_min=:img_min WHERE id_article='"+image.value(0).toString()+"'");
            QByteArray newData;
            QBuffer buffer(&newData);
            buffer.open(QIODevice::WriteOnly);
            newPix.save(&buffer, "JPG");
            query.bindValue(":img_min",newData);
            query.exec();
        }
    }
}

//---------------------- Tableau de bord ------------------------//


void LetsStock::on_pushButton_dashboard_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);
    actu_sideMenu();

    disconnect(ui->checkBox_checkValue,SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_checkValue_stateChanged()));
    ui->checkBox_checkValue->setChecked(false);
    connect(ui->checkBox_checkValue,SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_checkValue_stateChanged()));
}

//---------------------- Stock ------------------------//

void LetsStock::on_pushButton_back_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);

    disconnect(ui->checkBox_checkValue,SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_checkValue_stateChanged()));
    ui->checkBox_checkValue->setChecked(false);
    connect(ui->checkBox_checkValue,SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_checkValue_stateChanged()));
}

void LetsStock::on_checkBox_checkValue_stateChanged()
{
    on_pushButton_stock_clicked();
}

void LetsStock::on_pushButton_stock_clicked()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QApplication::processEvents();

    ui->stackedWidget->setCurrentWidget(ui->stockPage);

    ui->treeWidget_stock->clear();
    ui->treeWidget_stock->setColumnCount(3);
    QStringList headerLabels;
    headerLabels.append(tr("Catégorie"));
    headerLabels.append(tr("Nom"));
    headerLabels.append(tr("Quantité en stock"));
    headerLabels.append(tr("Montant achat en stock"));
    headerLabels.append(tr("id_article"));
    if(ui->checkBox_checkValue->checkState())
    {
        headerLabels.append(tr("Quantité achetée"));
        headerLabels.append(tr("Quantité vendue"));
        headerLabels.append(tr("Montant total achat"));
        headerLabels.append(tr("Montant total vente"));
        headerLabels.append(tr("Marge net"));
    }
    ui->treeWidget_stock->setHeaderLabels(headerLabels);
    ui->treeWidget_stock->hideColumn(4);

    QFont bold;
    bold.setBold(true);

    QTreeWidgetItem *total = new QTreeWidgetItem(ui->treeWidget_stock);
    total->setText(0, "Total");
    total->setFont(0, bold);
    total->setFont(2, bold);
    total->setFont(3, bold);
    ui->treeWidget_stock->addTopLevelItem(total);

    QTreeWidgetItem *stock2 = new QTreeWidgetItem(ui->treeWidget_stock);
    ui->treeWidget_stock->addTopLevelItem(stock2);

    double nbTotal_quantite = 0;
    double nbTotal_montantAchatStock = 0;

    QSqlQuery cat("SELECT id_cat,nom FROM Catégorie ORDER BY nom ASC");
    while(cat.next())
    {
        QTreeWidgetItem *stock = new QTreeWidgetItem(ui->treeWidget_stock);
        stock->setText(0, cat.value(1).toString());
        stock->setFont(0, bold);
        stock->setFont(2, bold);
        stock->setFont(3, bold);
        ui->treeWidget_stock->addTopLevelItem(stock);

        double nbCat_quantite = 0;
        double nbCat_montantAchatStock = 0;
        QSqlQuery articles("SELECT nom,SUM(quantité),image_min,id_article,SUM(total_achat),SUM(total_vente),montant_achat,id_fournisseur FROM Articles WHERE categorie='"+cat.value(0).toString()+"' GROUP BY nom,id_fournisseur ORDER BY nom ASC");
        while(articles.next())
        {
            QTreeWidgetItem *art = new QTreeWidgetItem();

            QIcon iconImg;
            if(!articles.value(2).toByteArray().isNull())
            {
                QByteArray data = articles.value(2).toByteArray();
                QPixmap *mpixmap = new QPixmap();
                mpixmap->loadFromData(data);
                iconImg.addPixmap(mpixmap->scaledToHeight(128,Qt::SmoothTransformation));
            }
            art->setIcon(1, iconImg);

            QSqlQuery nomVendor("SELECT nom FROM Fournisseur WHERE id_vendor='"+articles.value("id_fournisseur").toString()+"'");
            if(nomVendor.next()) art->setText(1, articles.value(0).toString()+" ["+nomVendor.value(0).toString()+"]");

            art->setText(4, articles.value(3).toString());

            art->setText(2, "     "+articles.value(1).toString());
            nbCat_quantite += articles.value(1).toDouble();
            nbTotal_quantite += articles.value(1).toDouble();
            stock->addChild(art);

            QSqlQuery montantAchatStock("SELECT * FROM Commande WHERE articles LIKE '%:"+articles.value(3).toString()+":%'");
            if(montantAchatStock.next())
            {
                QStringList articleList = montantAchatStock.value(4).toString().split(";");
                QString article;
                foreach(article, articleList)
                    if(article.split(":").at(1) == articles.value(3).toString())
                    {
                        if(articles.value(1).toString().toInt() * article.split(":").at(2).toDouble() != 0)
                            art->setText(3, QString::number(articles.value(1).toString().toInt() * article.split(":").at(2).toDouble(),'f',2)+" €");
                        nbCat_montantAchatStock += articles.value(1).toString().toInt() * article.split(":").at(2).toDouble();
                        nbTotal_montantAchatStock += articles.value(1).toString().toInt() * article.split(":").at(2).toDouble();
                    }
            }

            if(ui->checkBox_checkValue->checkState())
            {
                double montantTotalAchat = 0;
                double montantTotalVente = 0;
                if(articles.value(4).toDouble() !=  0) art->setText(7, tr("%1 €").arg(QString::number(articles.value(4).toDouble(),'f',2)));
                if(articles.value(5).toDouble() !=  0) art->setText(8, tr("%1 €").arg(QString::number(articles.value(5).toDouble(),'f',2)));

                QSqlQuery commande("SELECT * FROM Commande WHERE articles LIKE '%:"+articles.value(3).toString()+":%'");
                while(commande.next())
                {
                    QStringList articleList = commande.value(4).toString().split(";");
                    QString article;
                    foreach(article, articleList)
                        if(article.split(":").at(1) == articles.value(3).toString())
                        {
                            art->setText(5, QString::number(article.split(":").at(0).toInt() + art->text(5).toInt()));
                            montantTotalAchat += article.split(":").at(0).toInt() * article.split(":").at(2).toDouble();
                        }
                }
                if(art->text(7) == QString::number(montantTotalAchat,'f',2)+" €") art->setForeground(7, Qt::green);
                else art->setForeground(7, Qt::red);

                double marge = 0;
                QSqlQuery vente("SELECT * FROM Vente WHERE articles LIKE '%:"+articles.value(3).toString()+":%'");
                while(vente.next())
                {
                    QStringList articleList = vente.value(4).toString().split(";");
                    QString article;
                    foreach(article, articleList)
                    {
                        if(article.split(":").at(1) == articles.value(3).toString())
                        {
                            art->setText(6, QString::number(article.split(":").at(0).toInt() + art->text(6).toInt()));
                            montantTotalVente += article.split(":").at(0).toInt() * article.split(":").at(2).toDouble();

                            double montantTotal = article.split(":").at(0).toDouble() * article.split(":").at(2).toDouble();
                            marge += montantTotal - articles.value(6).toDouble()*article.split(":").at(0).toDouble() - montantTotal * 0.14 - montantTotal * 0.128;
                            art->setText(9, QString::number(marge,'f',2)+" €");
                        }
                    }
                }
                if(art->text(8) == tr("%1 €").arg(QString::number(montantTotalVente,'f',2))) art->setForeground(8, Qt::green);
                else art->setForeground(8, Qt::red);
            }
        }

        stock->setText(2, QString::number(nbCat_quantite));
        if(nbCat_montantAchatStock != 0) stock->setText(3, QString::number(nbCat_montantAchatStock,'f',2)+" €");

        ui->treeWidget_stock->expandItem(stock);
    }

    total->setText(2, QString::number(nbTotal_quantite));
    total->setText(3, QString::number(nbTotal_montantAchatStock,'f',2)+" €");

    //Redimension de chaque colonne
    for(int i = 0; i < ui->treeWidget_stock->columnCount(); i++)
    {
        ui->treeWidget_stock->resizeColumnToContents(i);
        if(i != 1) ui->treeWidget_stock->setColumnWidth(i, ui->treeWidget_stock->columnWidth(i)*1.1);
    }

    QApplication::restoreOverrideCursor();
}

void LetsStock::on_treeWidget_stock_itemClicked(QTreeWidgetItem *item, int column)
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QApplication::processEvents();

    if(column == 1)
    {
        QSqlQuery image("SELECT image FROM Articles WHERE id_article='"+item->text(4)+"'");
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

    if(column == 7 && item->foreground(column) == Qt::red)
    {
        QString total_achat;
        QSqlQuery articles("SELECT SUM(total_achat) FROM Articles WHERE id_article='"+item->text(4)+"'");
        if(articles.next())
            total_achat = QString::number(articles.value(0).toDouble(),'f',2)+" €";

        if(total_achat == item->text(7))
        {
            double montantTotalAchat = 0;
            QSqlQuery commande("SELECT * FROM Commandes WHERE articles LIKE '%:"+item->text(4)+":%'");
            while(commande.next())
            {
                QStringList articleList = commande.value(4).toString().split(";");
                QString article;
                foreach(article, articleList)
                    if(article.split(":").at(1) == item->text(4))
                        montantTotalAchat += article.split(":").at(0).toInt() * article.split(":").at(2).toDouble();
            }
            item->setText(7, QString::number(montantTotalAchat,'f',2)+" €");
        }
        else item->setText(7, total_achat);
    }

    if(column == 8 && item->foreground(column) == Qt::red)
    {
        QString total_vente;
        QSqlQuery articles("SELECT SUM(total_vente) FROM Articles WHERE id_article='"+item->text(4)+"'");
        if(articles.next())
            total_vente = QString::number(articles.value(0).toDouble(),'f',2)+" €";

        if(total_vente == item->text(8))
        {
            double montantTotalVente = 0;
            QSqlQuery vente("SELECT * FROM Vente WHERE articles LIKE '%:"+item->text(4)+":%'");
            while(vente.next())
            {
                QStringList articleList = vente.value(4).toString().split(";");
                QString article;
                foreach(article, articleList)
                    if(article.split(":").at(1) == item->text(4))
                        montantTotalVente += article.split(":").at(0).toInt() * article.split(":").at(2).toDouble();
            }
            item->setText(8, QString::number(montantTotalVente,'f',2)+" €");
        }
        else item->setText(8, total_vente);
    }

    QApplication::restoreOverrideCursor();
}

//---------------------- Historique ------------------------//

void LetsStock::on_treeWidget_stock_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(item->text(4) != "")
    {
        QApplication::setOverrideCursor(Qt::BusyCursor);
        QApplication::processEvents();

        ui->stackedWidget->setCurrentWidget(ui->historiquePage);
        ui->treeWidget_histo->clear();
        ui->label_article->setText(tr("Article : ")+item->text(1));
        ui->treeWidget_histo->hideColumn(6);
        ui->treeWidget_histo->hideColumn(7);
        ui->treeWidget_histo->hideColumn(8);

        QSqlQuery achat("SELECT * FROM Commande WHERE articles LIKE '%:"+item->text(4)+":%' ORDER BY date");
        while(achat.next())
        {
            QTreeWidgetItem *numero = new QTreeWidgetItem();

            if(achat.value(6).toByteArray() != "") numero->setIcon(0, QIcon(":/ressources/images/pdf.png"));
            numero->setText(0, achat.value(3).toString());

            ui->treeWidget_histo->addTopLevelItem(numero);

            QSqlQuery nomVendor("SELECT nom FROM Fournisseur WHERE id_vendor='"+achat.value(1).toString()+"'");
            if(nomVendor.next())
            {
                numero->setText(3, locale().toString(QDate::fromString(achat.value(2).toString(),"yyyyMMdd"),"ddd dd MMM yyyy"));
                numero->setText(8, locale().toString(QDate::fromString(achat.value(2).toString(),"yyyyMMdd"),"yyyyMMdd"));

                numero->setText(4, nomVendor.value(0).toString());
                numero->setText(5, QString::number(achat.value(5).toDouble(),'f',2)+" €");
                numero->setForeground(5, QColor(Qt::red));
                numero->setText(6, achat.value(0).toString());

                QStringList articles = achat.value(4).toString().split(";");
                QString article;
                if(achat.value(4).toString() != "")
                {
                    foreach(article, articles)
                    {
                        QTreeWidgetItem *art = new QTreeWidgetItem();

                        if(article.split(":").at(1) == item->text(4)) art->setIcon(0, QIcon(":/ressources/images/exclamation.png"));

                        art->setText(1, article.split(":").at(0));
                        art->setText(7, article.split(":").at(1));
                        QSqlQuery nomArticle("SELECT nom,image_min FROM Articles WHERE id_article='"+article.split(":").at(1)+"'");
                        if(nomArticle.next())
                        {
                            QIcon iconImg;
                            if(!nomArticle.value(1).toByteArray().isNull())
                            {
                                QByteArray data = nomArticle.value(1).toByteArray();
                                QPixmap *mpixmap = new QPixmap();
                                mpixmap->loadFromData(data);
                                iconImg.addPixmap(*mpixmap);
                            }
                            art->setIcon(2, iconImg);
                            art->setText(2, nomArticle.value(0).toString());
                        }

                        numero->addChild(art);
                    }
                }
            }
        }

        QSqlQuery vente("SELECT * FROM Vente WHERE articles LIKE '%:"+item->text(4)+":%' ORDER BY date");
        while(vente.next())
        {
            QTreeWidgetItem *client = new QTreeWidgetItem();

            QSqlQuery nomClient("SELECT nom,prenom FROM Client WHERE id_client='"+vente.value(1).toString()+"'");
            if(nomClient.next())
            {
                client->setText(0, nomClient.value(0).toString()+" "+nomClient.value(1).toString());
                ui->treeWidget_histo->addTopLevelItem(client);

                client->setText(3, locale().toString(QDate::fromString(vente.value(2).toString(),"yyyyMMdd"),"ddd dd MMM yyyy"));
                client->setText(8, locale().toString(QDate::fromString(vente.value(2).toString(),"yyyyMMdd"),"yyyyMMdd"));

                client->setText(5, QString::number(vente.value(3).toDouble(),'f',2)+" €");
                client->setForeground(5, QColor(Qt::darkGreen));
                client->setText(6, vente.value(0).toString());

                client->setText(2, vente.value("commentaire").toString());
                client->setTextAlignment(2, Qt::AlignCenter);

                QStringList articles = vente.value(4).toString().split(";");
                QString article;
                if(vente.value(4).toString() != "")
                {
                    foreach(article, articles)
                    {
                        QTreeWidgetItem *art = new QTreeWidgetItem();

                        if(article.split(":").at(1) == item->text(4)) art->setIcon(0, QIcon(":/ressources/images/exclamation.png"));

                        art->setText(1, article.split(":").at(0));
                        art->setText(5, tr("       %1 €").arg(QString::number(article.split(":").at(0).toDouble() * article.split(":").at(2).toDouble(),'f',2)));
                        art->setText(7, article.split(":").at(1));

                        QSqlQuery nomArticle("SELECT nom,id_fournisseur,image_min FROM Articles WHERE id_article='"+article.split(":").at(1)+"'");
                        if(nomArticle.next())
                        {
                            QIcon iconImg;
                            if(!nomArticle.value(2).toByteArray().isNull())
                            {
                                QByteArray data = nomArticle.value(2).toByteArray();
                                QPixmap *mpixmap = new QPixmap();
                                mpixmap->loadFromData(data);
                                iconImg.addPixmap(*mpixmap);
                            }
                            art->setIcon(2, iconImg);

                            QSqlQuery nomVendor("SELECT nom FROM Fournisseur WHERE id_vendor='"+nomArticle.value(1).toString()+"'");
                            if(nomVendor.next()) art->setText(2, nomArticle.value(0).toString()+" ["+nomVendor.value(0).toString()+"]");

                            art->setText(6, vente.value(0).toString());
                        }


                        client->addChild(art);
                    }
                }
            }
        }

        ui->treeWidget_histo->expandAll();
        for(int i = 0; i < ui->treeWidget_histo->columnCount(); i++)
            ui->treeWidget_histo->resizeColumnToContents(i);
        ui->treeWidget_histo->collapseAll();

        ui->treeWidget_histo->sortItems(8, Qt::AscendingOrder);

        QApplication::restoreOverrideCursor();
    }
}

void LetsStock::on_pushButton_back_2_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->stockPage);
}

//---------------------- Statistiques ------------------------//

void LetsStock::on_pushButton_stats_clicked()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QApplication::processEvents();

    ui->stackedWidget->setCurrentWidget(ui->statsPage);

    //Reinitialiser tout
    axisX->~QBarCategoryAxis();
    axisX = new QBarCategoryAxis();
    series->~QBarSeries();
    series = new QBarSeries();

    statsList.clear();

    if(ui->gridLayout_stats->itemAtPosition(0,0) != nullptr)
    {
        qobject_cast<QChartView*>(ui->gridLayout_stats->itemAtPosition(0,0)->widget())->~QChartView();
        ui->gridLayout_stats->removeItem(ui->gridLayout_stats->itemAtPosition(0,0));
    }
    if(ui->gridLayout_stats->itemAtPosition(1,0) != nullptr)
    {
        qobject_cast<RangeSlider*>(ui->gridLayout_stats->itemAtPosition(1,0)->widget())->~RangeSlider();
        ui->gridLayout_stats->removeItem(ui->gridLayout_stats->itemAtPosition(1,0));
    }

    rangeSlider = new RangeSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles, ui->statsPage);
    rangeSlider->setObjectName(QString::fromUtf8("rangeSlider"));
    rangeSlider->setMaximumSize(QSize(16777215, 50));
    ui->gridLayout_stats->addWidget(rangeSlider,1,0);

    //----------

    QBarSet *set = new QBarSet("a");

    QSqlQuery CA("SELECT substr(date,1,6),SUM(montant) FROM Vente GROUP BY substr(date,1,6)");
    while(CA.next())
    {
        set->append(CA.value(1).toDouble());
        series->append(set);
        statsList.append(locale().toString(QDate::fromString(CA.value(0).toString(),"yyyyMM"),"MMM yyyy"));
    }

    series->setLabelsVisible(true);
    series->setLabelsAngle(-90);

    set->setColor(QColor(176, 52, 220));
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->hide();
    chart->layout()->setContentsMargins(0, 0, 0, 0);

    if(series->count() == 0)
    {
        chart->setTitle("Aucune transaction avec ces filtres");
        rangeSlider->setEnabled(false);
    }
    else chart->setTitle("Chiffre d'affaire par mois");

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    axisY->applyNiceNumbers();

    axisX->setCategories(statsList);
    axisX->setLabelsAngle(-90);
    rangeSlider->SetRange(0,statsList.count()-1);
    if(rangeSlider->GetMaximun()-11 > rangeSlider->GetMinimun()) rangeSlider->setLowerValue(rangeSlider->GetMaximun()-11);
    axisX->setMin(axisX->at(rangeSlider->GetLowerValue()));
    axisX->setMax(axisX->at(rangeSlider->GetUpperValue()));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("border: 3px solid rgb(123, 97, 170); border-radius: 3px;");

    connect(series, &QAbstractBarSeries::hovered, this, &LetsStock::stats_hover);

    ui->gridLayout_stats->addWidget(chartView,0,0);

    connect(rangeSlider, SIGNAL(lowerValueChanged(int)), this, SLOT(stats_dates()));
    connect(rangeSlider, SIGNAL(upperValueChanged(int)), this, SLOT(stats_dates()));

    QApplication::restoreOverrideCursor();
}

void LetsStock::stats_dates()
{
    axisX->setMin(axisX->at(rangeSlider->GetLowerValue()));
    axisX->setMax(axisX->at(rangeSlider->GetUpperValue()));
}

void LetsStock::stats_hover(bool status, int index, QBarSet *barSet)
{
    QChart *chart = qobject_cast<QChartView*>(ui->gridLayout_stats->itemAtPosition(0,0)->widget())->chart();

    if(status) chart->setTitle(statsList.at(index)+" : "+QString::number(barSet->at(index),'f',2).replace(".",",")+" €");
    else chart->setTitle("Chiffre d'affaire par mois");
}

void LetsStock::on_pushButton_back2_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);
}


//---------------------- Fournisseurs ------------------------//
void LetsStock::on_pushButton_vendor_clicked()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QApplication::processEvents();

    disconnect(ui->tableWidget_vendor,SIGNAL(cellChanged(int, int)), this, SLOT(on_tableWidget_vendor_cellChanged(int, int)));

    ui->stackedWidget->setCurrentWidget(ui->vendorPage);
    actu_sideMenu();

    QSqlQuery nombre("SELECT COUNT(*) FROM Fournisseur");
    if(nombre.next()) ui->label_venorNumber->setText(nombre.value(0).toString());
    QSqlQuery favori("SELECT nom FROM Fournisseur AS f JOIN Commande AS c WHERE f.id_vendor = c.id_vendor GROUP BY f.id_vendor ORDER BY COUNT(*) DESC LIMIT 1");
    if(favori.next()) ui->label_vendorFav->setText(favori.value(0).toString());
    else ui->label_vendorFav->clear();

    ui->tableWidget_vendor->clear();
    ui->tableWidget_vendor->setColumnCount(4);
    ui->tableWidget_vendor->setRowCount(ui->label_venorNumber->text().toInt());

    QStringList header = (QStringList() << "id" << "Nom" << "SIRET" << "Téléphone");
    ui->tableWidget_vendor->setHorizontalHeaderLabels(header);
    ui->tableWidget_vendor->setColumnHidden(0, true);
    ui->tableWidget_vendor->verticalHeader()->hide();

    QSqlQuery affichage("SELECT * FROM Fournisseur");
    int i = 0;
    while(affichage.next())
    {
        QTableWidgetItem *item0 = new QTableWidgetItem(affichage.value(0).toString());
        ui->tableWidget_vendor->setItem(i,0,item0);
        QTableWidgetItem *item1 = new QTableWidgetItem(affichage.value(1).toString());
        ui->tableWidget_vendor->setItem(i,1,item1);
        QTableWidgetItem *item2 = new QTableWidgetItem(affichage.value(2).toString());
        ui->tableWidget_vendor->setItem(i,2,item2);
        QTableWidgetItem *item3 = new QTableWidgetItem(affichage.value(3).toString());
        ui->tableWidget_vendor->setItem(i,3,item3);
        i++;
    }

    ui->tableWidget_vendor->resizeColumnsToContents();
    for(int i = 0; i < ui->tableWidget_vendor->columnCount(); i++)
    {
        ui->tableWidget_vendor->setColumnWidth(i, ui->tableWidget_vendor->columnWidth(i)+100);
    }

    connect(ui->tableWidget_vendor,SIGNAL(cellChanged(int, int)), this, SLOT(on_tableWidget_vendor_cellChanged(int, int)));

    QApplication::restoreOverrideCursor();
}

void LetsStock::on_pushButton_vendorAdd_clicked()
{
    on_pushButton_vendorAdd_2_clicked();
}

void LetsStock::on_pushButton_vendorAdd_2_clicked()
{
    add_vendor *add = new add_vendor;
    add->setWindowModality(Qt::ApplicationModal);
    add->show();
    connect(add, SIGNAL(actualiser()), this, SLOT(on_pushButton_vendor_clicked()));
}

void LetsStock::on_pushButton_vendorDel_clicked()
{
    if(ui->tableWidget_vendor->currentItem() != nullptr)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Supprimer le fournisseur"));
        msgBox.setInformativeText(tr("Etes-vous sûr(e)(s) de vouloir supprimer le fournisseur ")+ui->tableWidget_vendor->item(ui->tableWidget_vendor->currentItem()->row(),1)->text()+" ?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.setStyleSheet("QLabel{min-width: 350px;}");
        int ret = msgBox.exec();

        if (ret == QMessageBox::Ok)
        {
            QSqlQuery remove("DELETE FROM Fournisseur WHERE id_vendor='"+ui->tableWidget_vendor->item(ui->tableWidget_vendor->currentItem()->row(),0)->text()+"'");

            //Supprimer les commandes liées au fournisseur
            QSqlQuery commandes("SELECT * FROM Commande WHERE id_vendor='"+ui->tableWidget_vendor->item(ui->tableWidget_vendor->currentItem()->row(),0)->text()+"'");
            while(commandes.next())
            {
                QSqlQuery updates("SELECT * FROM Commande WHERE id_order='"+commandes.value(0).toString()+"'");
                while(updates.next())
                {
                    QStringList articles = updates.value(4).toString().split(";");
                    QString article;
                    if(updates.value(4).toString() != "") foreach(article, articles)
                    {
                        QSqlQuery update("UPDATE Articles SET quantité=quantité-'"+article.split(":").at(0)+"' WHERE id_article='"+article.split(":").at(1)+"'");
                        QSqlQuery update2("UPDATE Articles SET total_achat=total_achat-'"+QString::number(article.split(":").at(0).toDouble()*article.split(":").at(2).toDouble())+"' WHERE id_article='"+article.split(":").at(1)+"'");
                    }
                }

                QSqlQuery remove("DELETE FROM Commande WHERE id_order='"+commandes.value(0).toString()+"'");
            }

            //Supprimer les articles liées au fournisseur
            QSqlQuery removeArticles("DELETE FROM Articles WHERE id_vendor='"+ui->tableWidget_vendor->item(ui->tableWidget_vendor->currentItem()->row(),0)->text()+"'");

            on_pushButton_vendor_clicked();
        }
    }
    else QMessageBox::warning(this, tr("Erreur"), tr("Aucun fournisseur n'est sélectionné."), tr("Fermer"));
}

void LetsStock::on_tableWidget_vendor_cellChanged(int row, int column)
{
    QSqlQuery modifier("UPDATE Fournisseur SET nom='"+ui->tableWidget_vendor->item(row,1)->text()+"', siret='"+ui->tableWidget_vendor->item(row,2)->text()+"', tel='"+ui->tableWidget_vendor->item(row,3)->text()+"' "
                       "WHERE id_vendor='"+ui->tableWidget_vendor->item(row,0)->text()+"'");
}

//---------------------- Commandes ------------------------//
void LetsStock::on_pushButton_order_clicked()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QApplication::processEvents();

    ui->stackedWidget->setCurrentWidget(ui->orderPage);
    actu_sideMenu();

    QSqlQuery nombre("SELECT COUNT(*) FROM Commande");
    if(nombre.next()) ui->label_orderNumber->setText(nombre.value(0).toString());
    QSqlQuery total("SELECT SUM(montant) FROM Commande");
    if(total.next()) ui->label_orderTotal->setText(QString::number(total.value(0).toDouble(),'f',2)+" €");

    ui->treeWidget_order->clear();
    ui->treeWidget_order->setColumnCount(8);
    QStringList headerLabels;
    headerLabels.append(tr("Numéro"));
    headerLabels.append(tr("Quantité"));
    headerLabels.append(tr("Nom"));
    headerLabels.append(tr("Date"));
    headerLabels.append(tr("Fournisseur"));
    headerLabels.append(tr("Montant"));
    headerLabels.append("id");
    headerLabels.append("id_article");
    ui->treeWidget_order->setHeaderLabels(headerLabels);
    ui->treeWidget_order->hideColumn(6);
    ui->treeWidget_order->hideColumn(7);

    QSqlQuery affichage("SELECT * FROM Commande ORDER BY date DESC");
    while(affichage.next())
    {
        QTreeWidgetItem *numero = new QTreeWidgetItem();

        if(affichage.value(6).toByteArray() != "") numero->setIcon(0, QIcon(":/ressources/images/pdf.png"));
        numero->setText(0, affichage.value(3).toString());

        ui->treeWidget_order->addTopLevelItem(numero);

        QSqlQuery nomVendor("SELECT nom FROM Fournisseur WHERE id_vendor='"+affichage.value(1).toString()+"'");
        if(nomVendor.next())
        {
            numero->setText(3, QDate::fromString(affichage.value(2).toString(),"yyyyMMdd").toString("ddd dd MMM yyyy"));
            numero->setText(4, nomVendor.value(0).toString());
            numero->setText(5, QString::number(affichage.value(5).toDouble(),'f',2)+" €");
            numero->setText(6, affichage.value(0).toString());

            QStringList articles = affichage.value(4).toString().split(";");
            QString article;
            if(affichage.value(4).toString() != "")  foreach(article, articles)
            {
                QTreeWidgetItem *art = new QTreeWidgetItem();

                art->setText(1, article.split(":").at(0));
                art->setText(7, article.split(":").at(1));
                QSqlQuery nomArticle("SELECT nom,image_min FROM Articles WHERE id_article='"+article.split(":").at(1)+"'");
                if(nomArticle.next())
                {
                    QIcon iconImg;
                    if(!nomArticle.value(1).toByteArray().isNull())
                    {
                        QByteArray data = nomArticle.value(1).toByteArray();
                        QPixmap *mpixmap = new QPixmap();
                        mpixmap->loadFromData(data);
                        iconImg.addPixmap(*mpixmap);
                    }
                    art->setIcon(2, iconImg);
                    art->setText(2, nomArticle.value(0).toString());
                }

                numero->addChild(art);
            }
        }
    }

    //Redimension de chaque colonne
    ui->treeWidget_order->expandAll();
    for(int i = 0; i < ui->treeWidget_order->columnCount()-3; i++)
    {
        ui->treeWidget_order->resizeColumnToContents(i);
        if(i != 2) ui->treeWidget_order->setColumnWidth(i, ui->treeWidget_order->columnWidth(i)*1.1);
    }
    ui->treeWidget_order->collapseAll();
    ui->treeWidget_order->expandItem(ui->treeWidget_order->topLevelItem(0));

    QApplication::restoreOverrideCursor();
}

void LetsStock::on_pushButton_orderAdd_clicked()
{
    on_pushButton_orderAdd_3_clicked();
}

void LetsStock::on_pushButton_orderAdd_3_clicked()
{
    add_order *add = new add_order;
    add->setWindowModality(Qt::ApplicationModal);

    QSqlQuery favori("SELECT nom FROM Fournisseur AS f JOIN Commande AS c WHERE f.id_vendor = c.id_vendor GROUP BY f.id_vendor ORDER BY COUNT(*) DESC LIMIT 1");
    if(favori.next()) add->ui->comboBox_vendor->setCurrentIndex(add->ui->comboBox_vendor->findText(favori.value(0).toString()));

    add->show();
    connect(add, SIGNAL(actualiser()), this, SLOT(on_pushButton_order_clicked()));
}

void LetsStock::on_pushButton_orderDel_2_clicked()
{
    if(ui->treeWidget_order->currentItem() != nullptr && ui->treeWidget_order->currentItem()->text(6) != "")
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Supprimer la commande"));
        msgBox.setInformativeText(tr("Etes-vous sûr(e)(s) de vouloir supprimer la commande n°")+ui->treeWidget_order->currentItem()->text(0)+" ?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.setStyleSheet("QLabel{min-width: 350px;}");
        int ret = msgBox.exec();

        if (ret == QMessageBox::Ok)
        {
            QSqlQuery updates("SELECT * FROM Commande WHERE id_order='"+ui->treeWidget_order->currentItem()->text(6)+"'");
            while(updates.next())
            {
                QStringList articles = updates.value(4).toString().split(";");
                QString article;
                if(updates.value(4).toString() != "")  foreach(article, articles)
                {
                    QSqlQuery update("UPDATE Articles SET quantité=quantité-'"+article.split(":").at(0)+"' WHERE id_article='"+article.split(":").at(1)+"'");
                    QSqlQuery update2("UPDATE Articles SET total_achat=total_achat-'"+QString::number(article.split(":").at(0).toDouble()*article.split(":").at(2).toDouble())+"' WHERE id_article='"+article.split(":").at(1)+"'");
                }
            }

            QSqlQuery remove("DELETE FROM Commande WHERE id_order='"+ui->treeWidget_order->currentItem()->text(6)+"'");

            on_pushButton_order_clicked();
        }
    }
    else QMessageBox::warning(this, tr("Erreur"), tr("Aucune commande n'est sélectionnée."), tr("Fermer"));
}

void LetsStock::on_treeWidget_order_itemClicked(QTreeWidgetItem *item, int column)
{
    if(column == 0)
    {
        QSqlQuery facture("SELECT facture FROM Commande WHERE id_order='"+item->text(6)+"'");
        if(facture.next() && facture.value(0).toByteArray()!="")
        {
            QByteArray pdf = facture.value(0).toByteArray();

            QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Facture.pdf");

            if (file.open(QFile::WriteOnly))
            {
                file.write(pdf);
                file.close();
                QDesktopServices::openUrl(QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Facture.pdf"));
            }
        }
    }
    if(column == 2 && !item->text(7).isEmpty())
    {
        QSqlQuery image("SELECT image FROM Articles WHERE id_article='"+item->text(7)+"'");
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

//---------------------- Clients ------------------------//
void LetsStock::on_pushButton_client_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->clientPage);
    actu_sideMenu();

    QSqlQuery nombre("SELECT COUNT(*) FROM Client");
    if(nombre.next()) ui->label_clientNumber->setText(nombre.value(0).toString());
    QSqlQuery favori("SELECT nom,prenom FROM Client AS c JOIN Vente AS v WHERE c.id_client = v.id_client AND c.nom!='AUTRE' GROUP BY v.id_client ORDER BY SUM(v.montant) DESC LIMIT 1");
    if(favori.next()) ui->label_clientFav->setText(favori.value(0).toString()+" "+favori.value(1).toString());
    else ui->label_clientFav->clear();

    ui->treeWidget_client->clear();
    ui->treeWidget_client->setColumnCount(2);
    QStringList headerLabels;
    headerLabels.append(tr("Client"));
    ui->treeWidget_client->setHeaderLabels(headerLabels);
    ui->treeWidget_client->hideColumn(1);

    QSqlQuery clients("SELECT * FROM Client");
    while (clients.next())
    {
        QTreeWidgetItem *client = new QTreeWidgetItem();
        client->setText(0, clients.value(2).toString()+" "+clients.value(1).toString());
        client->setText(1, clients.value(0).toString());

        ui->treeWidget_client->addTopLevelItem(client);
    }
}

void LetsStock::on_pushButton_clientAdd_2_clicked()
{
    on_pushButton_clientAdd_clicked();
}

void LetsStock::on_pushButton_clientAdd_clicked()
{
    add_client *add = new add_client;
    add->setWindowModality(Qt::ApplicationModal);
    add->show();
    connect(add, SIGNAL(actualiser()), this, SLOT(on_pushButton_client_clicked()));
}

void LetsStock::on_pushButton_clientDel_clicked()
{
    if(ui->treeWidget_client->currentItem() != nullptr)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Supprimer le client"));
        msgBox.setInformativeText(tr("Etes-vous sûr(e)(s) de vouloir supprimer le client ")+ui->treeWidget_client->currentItem()->text(1)+" ?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.setStyleSheet("QLabel{min-width: 350px;}");
        int ret = msgBox.exec();

        if (ret == QMessageBox::Ok)
        {
            QSqlQuery remove("DELETE FROM Client WHERE id_client='"+ui->treeWidget_client->currentItem()->text(1)+"'");

            QSqlQuery ventes("SELECT * FROM Vente WHERE id_client='"+ui->treeWidget_client->currentItem()->text(1)+"'");
            while(ventes.next())
            {
                QSqlQuery updates("SELECT * FROM Vente WHERE id_vente='"+ventes.value(0).toString()+"'");
                while(updates.next())
                {
                    QStringList articles = updates.value(4).toString().split(";");
                    QString article;
                    if(updates.value(4).toString() != "") foreach(article, articles)
                    {
                        QSqlQuery update("UPDATE Articles SET quantité=quantité+'"+article.split(":").at(0)+"' WHERE id_article='"+article.split(":").at(1)+"'");
                        QSqlQuery update2("UPDATE Articles SET total_vente=total_vente-'"+QString::number(article.split(":").at(0).toDouble()*article.split(":").at(2).toDouble())+"' WHERE id_article='"+article.split(":").at(1)+"'");
                    }
                }

                QSqlQuery remove("DELETE FROM Vente WHERE id_vente='"+ventes.value(0).toString()+"'");
            }

            on_pushButton_client_clicked();
        }
    }
    else QMessageBox::warning(this, tr("Erreur"), tr("Aucun client n'est sélectionné."), tr("Fermer"));
}

void LetsStock::on_treeWidget_client_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    ui->stackedWidget->setCurrentWidget(ui->sellPage);
    actu_sideMenu();

    actu_sellClient();
    ui->comboBox_clientSell->setCurrentIndex(clients.indexOf(item->text(1).toInt()));
}

//---------------------- Ventes ------------------------//
void LetsStock::on_pushButton_sell_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->sellPage);
    actu_sideMenu();

    actu_sellClient();
    actu_sell();
}

void LetsStock::actu_sellClient()
{
    disconnect(ui->comboBox_clientSell,SIGNAL(currentTextChanged(const QString)), this, SLOT(on_comboBox_clientSell_currentTextChanged(const QString)));
    ui->comboBox_clientSell->clear();
    ui->comboBox_clientSell->addItem("");
    clients.clear();
    clients.append(-1);
    QSqlQuery client("SELECT id_client,nom,prenom FROM Client ORDER BY nom");
    while(client.next())
    {
        ui->comboBox_clientSell->addItem(client.value(1).toString()+" "+client.value(2).toString());
        clients.append(client.value(0).toInt());
    }
    connect(ui->comboBox_clientSell,SIGNAL(currentTextChanged(const QString)), this, SLOT(on_comboBox_clientSell_currentTextChanged(const QString)));
}

void LetsStock::actu_sell()
{
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QApplication::processEvents();

    QString id_client;
    if(ui->comboBox_clientSell->currentIndex() == 0) id_client = "1+1";
    else id_client = "id_client='"+QString::number(clients.at(ui->comboBox_clientSell->currentIndex()))+"'";

    QSqlQuery nombre("SELECT COUNT(*) FROM Vente WHERE "+id_client+"");
    if(nombre.next()) ui->label_sellNumber->setText(nombre.value(0).toString());
    QSqlQuery total("SELECT SUM(montant) FROM Vente WHERE "+id_client+"");
    if(total.next()) ui->label_sellTotal->setText(QString::number(total.value(0).toDouble(),'f',2)+" €");

    ui->treeWidget_sell->clear();
    ui->treeWidget_sell->setColumnCount(7);
    QStringList headerLabels;
    headerLabels.append(tr("Client"));
    headerLabels.append(tr("Quantité"));
    headerLabels.append(tr("Nom"));
    headerLabels.append(tr("Date"));
    headerLabels.append(tr("Montant"));
    headerLabels.append("id");
    headerLabels.append("id_artcle");
    ui->treeWidget_sell->setHeaderLabels(headerLabels);
    ui->treeWidget_sell->hideColumn(5);
    ui->treeWidget_sell->hideColumn(6);

    QSqlQuery affichage("SELECT * FROM Vente WHERE "+id_client+" ORDER BY date DESC");
    while(affichage.next())
    {
        QTreeWidgetItem *client = new QTreeWidgetItem(ui->treeWidget_sell);

        QSqlQuery nomClient("SELECT nom,prenom FROM Client WHERE id_client='"+affichage.value(1).toString()+"'");
        if(nomClient.next())
        {
            client->setText(0, nomClient.value(0).toString()+" "+nomClient.value(1).toString());
            ui->treeWidget_sell->addTopLevelItem(client);

            client->setText(3, QDate::fromString(affichage.value(2).toString(),"yyyyMMdd").toString("ddd dd MMM yyyy"));
            client->setText(4, QString::number(affichage.value(3).toDouble(),'f',2)+" €");
            client->setText(5, affichage.value(0).toString());

            client->setText(2, affichage.value("commentaire").toString());
            client->setTextAlignment(2, Qt::AlignCenter);

            QStringList articles = affichage.value(4).toString().split(";");
            QString article;
            if(affichage.value(4).toString() != "")
            {
                foreach(article, articles)
                {
                    QTreeWidgetItem *art = new QTreeWidgetItem();

                    art->setText(1, article.split(":").at(0));
                    art->setText(4, tr("       %1 €").arg(QString::number(article.split(":").at(0).toDouble() * article.split(":").at(2).toDouble(),'f',2)));
                    art->setText(6, article.split(":").at(1));

                    QSqlQuery nomArticle("SELECT nom,id_fournisseur,image_min FROM Articles WHERE id_article='"+article.split(":").at(1)+"'");
                    if(nomArticle.next())
                    {
                        QIcon iconImg;
                        if(!nomArticle.value(2).toByteArray().isNull())
                        {
                            QByteArray data = nomArticle.value(2).toByteArray();
                            QPixmap *mpixmap = new QPixmap();
                            mpixmap->loadFromData(data);
                            iconImg.addPixmap(*mpixmap);
                        }
                        art->setIcon(2, iconImg);

                        QSqlQuery nomVendor("SELECT nom FROM Fournisseur WHERE id_vendor='"+nomArticle.value(1).toString()+"'");
                        if(nomVendor.next()) art->setText(2, nomArticle.value(0).toString()+" ["+nomVendor.value(0).toString()+"]");

                        art->setText(5, affichage.value(0).toString());
                    }


                    client->addChild(art);
                }
            }
        }
    }

    //Redimension de chaque colonne
    ui->treeWidget_sell->expandAll();
    for(int i = 0; i < ui->treeWidget_sell->columnCount()-3; i++)
    {
        ui->treeWidget_sell->resizeColumnToContents(i);
        if(i != 2) ui->treeWidget_sell->setColumnWidth(i, ui->treeWidget_sell->columnWidth(i)*1.2);
    }
    ui->treeWidget_sell->collapseAll();
    ui->treeWidget_sell->expandItem(ui->treeWidget_sell->topLevelItem(0));

    QApplication::restoreOverrideCursor();
}

void LetsStock::on_comboBox_clientSell_currentTextChanged(const QString &arg1)
{
    actu_sell();
}

void LetsStock::on_pushButton_sellAdd_2_clicked()
{
    on_pushButton_sellAdd_clicked();
}

void LetsStock::on_pushButton_sellAdd_clicked()
{
    add_sell *add = new add_sell(nullptr, new QString("Ajouter"));
    add->setWindowModality(Qt::ApplicationModal);
    add->show();
    connect(add, SIGNAL(actualiser()), this, SLOT(on_pushButton_sell_clicked()));
}

void LetsStock::on_pushButton_sellDel_clicked()
{
    if(ui->treeWidget_sell->currentItem() != nullptr && ui->treeWidget_sell->currentItem()->text(5) != "")
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Supprimer la vente"));
        msgBox.setInformativeText(tr("Etes-vous sûr(e)(s) de vouloir supprimer la vente de ")+ui->treeWidget_sell->currentItem()->text(4)+" pour "+ui->treeWidget_sell->currentItem()->text(0)+" ?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.setStyleSheet("QLabel{min-width: 350px;}");
        int ret = msgBox.exec();

        if (ret == QMessageBox::Ok)
        {
            QSqlQuery updates("SELECT * FROM Vente WHERE id_vente='"+ui->treeWidget_sell->currentItem()->text(5)+"'");
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

            QSqlQuery remove("DELETE FROM Vente WHERE id_vente='"+ui->treeWidget_sell->currentItem()->text(5)+"'");

            on_pushButton_sell_clicked();
        }
    }
    else QMessageBox::warning(this, tr("Erreur"), tr("Aucune vente n'est sélectionnée."), tr("Fermer"));
}

void LetsStock::on_treeWidget_sell_itemClicked(QTreeWidgetItem *item, int column)
{
    if(column == 2 && !item->text(6).isEmpty())
    {
        QSqlQuery image("SELECT image FROM Articles WHERE id_article='"+item->text(6)+"'");
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

void LetsStock::on_pushButton_modSell_clicked()
{
    QTreeWidgetItem *item = ui->treeWidget_sell->currentItem();
    if(item != nullptr && ui->treeWidget_sell->currentItem()->text(5) != "")
    {
        add_sell *mod = new add_sell(nullptr, new QString("Modifier"));
        mod->setWindowModality(Qt::ApplicationModal);
        mod->show();
        mod->ui->label_4->setText("Modifier une vente");
        mod->ui->pushButton_add->setText("Modifier");
        connect(mod, SIGNAL(actualiser()), this, SLOT(on_pushButton_sell_clicked()));

        QSqlQuery modifier("SELECT * FROM Vente WHERE id_vente='"+ui->treeWidget_sell->currentItem()->text(5)+"'");
        if(modifier.next())
        {
            mod->ui->comboBox_client->setCurrentIndex(mod->clients.indexOf(modifier.value(1).toInt()));
            mod->ui->dateEdit->setDate(QDate::fromString(modifier.value(2).toString(), "yyyyMMdd"));
            mod->ui->label_id->setText(ui->treeWidget_sell->currentItem()->text(5));
            mod->ui->lineEdit_commentaire->setText(ui->treeWidget_sell->currentItem()->text(2));

            QStringList articles = modifier.value(4).toString().split(";");
            QString article;
            if(modifier.value(4).toString() != "")
            {
                foreach(article, articles)
                {
                    int index;
                    int indexTop;
                    QList<QTreeWidgetItem*> clist = mod->ui->treeWidget->findItems(article.split(":").at(1), Qt::MatchRecursive, 8);
                    foreach(QTreeWidgetItem* item, clist)
                    {
                        indexTop = mod->ui->treeWidget->indexOfTopLevelItem(item->parent());
                        index = mod->ui->treeWidget->topLevelItem(indexTop)->indexOfChild(item);
                    }

                    QSpinBox *spin = qobject_cast<QSpinBox*>(mod->ui->treeWidget->itemWidget(mod->ui->treeWidget->topLevelItem(indexTop)->child(index), 0));
                    QDoubleSpinBox *doubleSpin = qobject_cast<QDoubleSpinBox*>(mod->ui->treeWidget->itemWidget(mod->ui->treeWidget->topLevelItem(indexTop)->child(index), 6));

                    mod->ui->treeWidget->topLevelItem(indexTop)->child(index)->setText(4, QString::number(mod->ui->treeWidget->topLevelItem(indexTop)->child(index)->text(4).toInt() + article.split(":").at(0).toInt()));
                    spin->setMaximum(spin->maximum() + article.split(":").at(0).toInt());
                    spin->setValue(article.split(":").at(0).toInt());
                    doubleSpin->setValue(article.split(":").at(2).toDouble());
                }
            }

            if(mod->ui->label_montant->text().replace(" €","").toDouble() != modifier.value(3).toDouble()) mod->ui->doubleSpinBox->setValue(modifier.value(3).toDouble() - mod->ui->label_montant->text().replace(" €","").toDouble());
        }
    }
    else QMessageBox::warning(this, tr("Erreur"), tr("Aucune vente n'est sélectionnée."), tr("Fermer"));
}


//---------------------- Actualisation ------------------------//
void LetsStock::actu_sideMenu()
{
    if(ui->stackedWidget->currentWidget() == ui->dashboardPage || ui->stackedWidget->currentWidget() == ui->stockPage) ui->pushButton_dashboard->setStyleSheet("background-color: rgba(255, 255, 255, 100); color: rgb(255, 255, 255)");
    else ui->pushButton_dashboard->setStyleSheet("background-color: 0; color: rgb(255, 255, 255)");

    if(ui->stackedWidget->currentWidget() == ui->vendorPage)
    {
        ui->pushButton_vendor->setStyleSheet("background-color: rgba(255, 255, 255, 100); color: rgb(255, 255, 255)");
        ui->pushButton_vendorAdd->setStyleSheet("background-color: rgba(255, 255, 255, 100); color: rgb(255, 255, 255)");
    }
    else
    {
        ui->pushButton_vendor->setStyleSheet("background-color: 0; color: rgb(255, 255, 255)");
        ui->pushButton_vendorAdd->setStyleSheet("background-color: 0; color: rgb(255, 255, 255)");
    }

    if(ui->stackedWidget->currentWidget() == ui->orderPage)
    {
        ui->pushButton_order->setStyleSheet("background-color: rgba(255, 255, 255, 100); color: rgb(255, 255, 255)");
        ui->pushButton_orderAdd->setStyleSheet("background-color: rgba(255, 255, 255, 100); color: rgb(255, 255, 255)");
    }
    else
    {
        ui->pushButton_order->setStyleSheet("background-color: 0; color: rgb(255, 255, 255)");
        ui->pushButton_orderAdd->setStyleSheet("background-color: 0; color: rgb(255, 255, 255)");
    }

    if(ui->stackedWidget->currentWidget() == ui->clientPage)
    {
        ui->pushButton_client->setStyleSheet("background-color: rgba(255, 255, 255, 100); color: rgb(255, 255, 255)");
        ui->pushButton_clientAdd->setStyleSheet("background-color: rgba(255, 255, 255, 100); color: rgb(255, 255, 255)");
    }
    else
    {
        ui->pushButton_client->setStyleSheet("background-color: 0; color: rgb(255, 255, 255)");
        ui->pushButton_clientAdd->setStyleSheet("background-color: 0; color: rgb(255, 255, 255)");
    }

    if(ui->stackedWidget->currentWidget() == ui->sellPage)
    {
        ui->pushButton_sell->setStyleSheet("background-color: rgba(255, 255, 255, 100); color: rgb(255, 255, 255)");
        ui->pushButton_sellAdd->setStyleSheet("background-color: rgba(255, 255, 255, 100); color: rgb(255, 255, 255)");
    }
    else
    {
        ui->pushButton_sell->setStyleSheet("background-color: 0; color: rgb(255, 255, 255)");
        ui->pushButton_sellAdd->setStyleSheet("background-color: 0; color: rgb(255, 255, 255)");
    }
}

//ourLabel class cpp
ourLabel::ourLabel(const QPixmap pix, QWidget *parent) : QLabel(parent)
{
    setPixmap(pix);
    setGeometry(QCursor().pos().x()-pix.width()/2, QCursor().pos().y()-pix.height()/2, pix.width(), pix.height());
}

ourLabel::~ourLabel(){ }

void ourLabel::mousePressEvent (QMouseEvent *event)
{ emit clicked(); }

void ourLabel::close()
{ this->~ourLabel(); }
