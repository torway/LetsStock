#include "add_excel.h"
#include "ui_add_excel.h"

#include "QXlsx/header/xlsxdocument.h"
#include "QXlsx/header/xlsxworkbook.h"
using namespace QXlsx;

add_excel::add_excel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::add_excel)
{
    ui->setupUi(this);
    this->setWindowTitle("LetsStock");

    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(7);
    QStringList headerLabels;
    headerLabels.append(tr("Référence"));
    headerLabels.append(tr("Fournisseur"));
    headerLabels.append(tr("Nom"));
    headerLabels.append(tr("Catégorie"));
    headerLabels.append(tr("Montant achat"));
    headerLabels.append(tr("Montant vente"));
    headerLabels.append(tr("img_path"));
    ui->treeWidget->setHeaderLabels(headerLabels);
    ui->treeWidget->hideColumn(6);

    disconnect(ui->comboBox_vendor,SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_vendor_currentIndexChanged(int)));
    ui->comboBox_vendor->clear();
    QSqlQuery fournisseur("SELECT nom FROM Fournisseur ORDER BY nom");
    while(fournisseur.next()) ui->comboBox_vendor->addItem(fournisseur.value(0).toString());
    connect(ui->comboBox_vendor,SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_vendor_currentIndexChanged(int)));
}

add_excel::~add_excel()
{
    delete ui;
}

void add_excel::actu()
{
    ui->treeWidget->clear();
    QApplication::processEvents();

    Document doc(QFile(ui->lineEdit_path->text()).fileName());
    AbstractSheet* currentSheet = doc.sheet(doc.sheetNames().at(0));

    int maxRow = -1;
    int maxCol = -1;
    currentSheet->workbook()->setActiveSheet(0);
    Worksheet* wsheet = (Worksheet*) currentSheet->workbook()->activeSheet();

    QVector<CellLocation> clList = wsheet->getFullCells( &maxRow, &maxCol );

    QVector< QVector<QString> > cellValues;
    for (int rc = 0; rc < maxRow; rc++)
    {
        QVector<QString> tempValue;
        for (int cc = 0; cc < maxCol; cc++)
            tempValue.push_back(QString(""));

        cellValues.push_back(tempValue);
    }

    for ( int ic = 0; ic < clList.size(); ++ic )
    {
        CellLocation cl = clList.at(ic); // cell location

        int row = cl.row - 1;
        int col = cl.col - 1;

        cellValues[row][col] = cl.cell.data()->value().toString();
    }

    int imgColumn = 0, refColumn = 0, nameColumn = 0, montantAchatColumn = 0, montantVenteColumn = 0;
    for (int cc = 0; cc < maxCol; cc++)
    {
        QString strCell = cellValues[ui->spinBox->value()-1][cc];
        if(strCell.toLower().contains("image"))
            imgColumn = cc;

        if(strCell.toLower().contains("référence"))
            refColumn = cc;

        if(strCell.toLower().contains("nom"))
            nameColumn = cc;

        if(strCell.toLower().contains("pu ttc"))
            montantAchatColumn = cc;

        if(strCell.toLower().contains("prix de vente ttc"))
            montantVenteColumn = cc;
    }

    QStringList columns = QStringList() << QString::number(imgColumn) << QString::number(refColumn) << QString::number(nameColumn) << QString::number(montantAchatColumn) << QString::number(montantVenteColumn);
    columns.removeDuplicates();
    if(columns.count() > 1)
    {
        QStringList refList;
        for(int rc = ui->spinBox->value()+1; rc < maxRow; rc++)
            if(!refList.contains(cellValues[rc][refColumn])) refList.append(cellValues[rc][refColumn]);

        QSqlQuery articles("SELECT ref FROM Articles");
        while(articles.next())
            if(refList.contains(articles.value(0).toString())) refList.removeAt(refList.indexOf(articles.value(0).toString()));
        if(refList.contains("")) refList.removeAt(refList.indexOf(""));

        //Images
        QFile drawing = QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Excel/xl/drawings/drawing1.xml");
        QFile relation = QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Excel/xl/drawings/_rels/drawing1.xml.rels");

        QStringList xmlList;
        QList<int> rowList;
        QStringList idList;
        QStringList fileList;

        if (drawing.open(QIODevice::ReadWrite))
        {
            if (relation.open(QIODevice::ReadOnly))
            {
                QTextStream streamD(&drawing);
                xmlList = streamD.readAll().split("<xdr:twoCellAnchor");
                xmlList.removeAt(0);
                foreach (QString xml, xmlList)
                    if(xml.contains("<xdr:oneCellAnchor")) xmlList.append(xml.split("<xdr:oneCellAnchor"));

                QTextStream streamR(&relation);
                QString read = streamR.readAll();
                foreach (QString xml, xmlList) {
                    int rowOff = xml.split("<xdr:rowOff>").at(1).split("</xdr:rowOff>").at(0).toInt();

                    if(rowOff > 300000) rowList.append(xml.split("<xdr:row>").at(1).split("</xdr:row>").at(0).toInt() + 1);
                    else rowList.append(xml.split("<xdr:row>").at(1).split("</xdr:row>").at(0).toInt());

                    idList.append(xml.split("r:embed=\"").at(1).split("\"").at(0));
                    fileList.append(read.split("\""+idList.last()+"\"").at(1).split("/media/").at(1).split("\"").at(0));
                }
            }
        }

        for (int rc = ui->spinBox->value()+1; rc < maxRow; rc++)
        {
            if(refList.contains(cellValues[rc][refColumn]))
            {
                QApplication::processEvents();

                QTreeWidgetItem *article = new QTreeWidgetItem();
                ui->treeWidget->addTopLevelItem(article);
                article->setCheckState(0, Qt::Checked);

                article->setText(0, cellValues[rc][refColumn]);

                QComboBox *comboBox_vendor = new QComboBox();
                QSqlQuery fournisseur("SELECT id_vendor,nom FROM Fournisseur ORDER BY nom");
                while(fournisseur.next()) comboBox_vendor->addItem(fournisseur.value(1).toString());
                ui->treeWidget->setItemWidget(article, 1, comboBox_vendor);
                comboBox_vendor->setCurrentIndex(ui->comboBox_vendor->currentIndex());

                QIcon iconImg;
                if(rowList.contains(rc))
                {
                    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Excel/xl/media/"+fileList.at(rowList.indexOf(rc));
                    QPixmap *mpixmap = new QPixmap(path);
                    iconImg.addPixmap(*mpixmap);
                    article->setText(6, path);
                }
                article->setIcon(2, iconImg);
                article->setText(2, cellValues[rc][nameColumn]);

                QComboBox *comboBox_cat = new QComboBox();
                QSqlQuery categorie("SELECT id_cat,nom FROM Catégorie ORDER BY nom");
                while(categorie.next()) comboBox_cat->addItem(categorie.value(1).toString());
                ui->treeWidget->setItemWidget(article, 3, comboBox_cat);

                QDoubleSpinBox *amountBuy = new QDoubleSpinBox();
                amountBuy->setMinimum(0);
                amountBuy->setMaximum(1000);
                amountBuy->setDecimals(4);
                amountBuy->setValue(cellValues[rc][montantAchatColumn].toDouble());
                amountBuy->setSuffix(" €");
                ui->treeWidget->setItemWidget(article, 4, amountBuy);

                QDoubleSpinBox *amountSell = new QDoubleSpinBox();
                amountSell->setMinimum(0);
                amountSell->setMaximum(1000);
                amountSell->setDecimals(4);
                amountSell->setValue(cellValues[rc][montantVenteColumn].toDouble());
                amountSell->setSuffix(" €");
                ui->treeWidget->setItemWidget(article, 5, amountSell);

                ui->treeWidget->expandItem(article);
            }
        }

        for(int i = 0; i < ui->treeWidget->columnCount(); i++)
            ui->treeWidget->resizeColumnToContents(i);
    }
}

void add_excel::on_lineEdit_path_textChanged(const QString &arg1)
{
    actu();
}

void add_excel::on_spinBox_valueChanged(int arg1)
{
    actu();
}

void add_excel::on_pushButton_cancel_clicked()
{
    this->~add_excel();
}

void add_excel::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if(column == 2 && !item->text(6).isEmpty())
    {
        ourLabel *labelImg = new ourLabel(QPixmap(item->text(6)));
        labelImg->setWindowModality(Qt::ApplicationModal);
        labelImg->show();
        connect(labelImg,SIGNAL(clicked()),labelImg,SLOT(close()));
    }
}

void add_excel::on_comboBox_vendor_currentIndexChanged(int index)
{
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        qobject_cast<QComboBox *>(ui->treeWidget->itemWidget(ui->treeWidget->topLevelItem(i), 1))->setCurrentIndex(index);
}

void add_excel::on_pushButton_valide_clicked()
{
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
        if(item->checkState(0) == Qt::Checked)
        {
            QSqlQuery ref("SELECT * FROM Articles WHERE ref='"+item->text(0)+"'");
            ref.last();
            if(item->text(2).isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("Le nom complet n'est pas renseigné."), tr("Fermer"));
            else if(item->text(0).isEmpty()) QMessageBox::warning(this, tr("Erreur"), tr("La référence de l'article n'est pas renseignée."), tr("Fermer"));
            else if(qobject_cast<QDoubleSpinBox *>(ui->treeWidget->itemWidget(item, 4))->value() == 0) QMessageBox::warning(this, tr("Erreur"), tr("Le montant d'achat n'est pas renseigné."), tr("Fermer"));
            else if(qobject_cast<QDoubleSpinBox *>(ui->treeWidget->itemWidget(item, 5))->value() == 0) QMessageBox::warning(this, tr("Erreur"), tr("Le montant de vente n'est pas renseigné."), tr("Fermer"));
            else if(ref.at() >= 0) QMessageBox::warning(this, tr("Erreur"), tr("Un article porte déjà cette référence."), tr("Fermer"));
            else
            {
                QSqlQuery vendor("SELECT id_vendor FROM Fournisseur WHERE nom='"+qobject_cast<QComboBox *>(ui->treeWidget->itemWidget(item, 1))->currentText().replace("'","''")+"'");
                if(vendor.next())
                {
                    QSqlQuery cat("SELECT id_cat FROM Catégorie WHERE nom='"+qobject_cast<QComboBox *>(ui->treeWidget->itemWidget(item, 3))->currentText().replace("'","''")+"'");
                    if(cat.next())
                    {
                        QSqlQuery query;
                        query.prepare("INSERT INTO Articles (id_fournisseur, ref, categorie, nom, quantité, montant_achat, montant_vente, image, image_min, total_achat, total_vente) VALUES ('"+vendor.value(0).toString()+"'"
                                             ",'"+item->text(0).replace("'","''")+"','"+cat.value(0).toString()+"'"
                                             ",'"+item->text(2).replace("'","''")+"',0,'"+QString::number(qobject_cast<QDoubleSpinBox *>(ui->treeWidget->itemWidget(item, 4))->value())+"'"
                                             ",'"+QString::number(qobject_cast<QDoubleSpinBox *>(ui->treeWidget->itemWidget(item, 5))->value())+"',:img,:img_min,0,0);");

                        if(item->icon(2).isNull())
                        {
                            query.bindValue(":img","");
                            query.bindValue(":img_min","");
                            query.exec();
                        }
                        else
                        {
                            QByteArray IMGInByteArray, IMGMinInByteArray;

                            QFile file(item->text(6));
                            if (!file.open(QIODevice::ReadOnly)) return;
                            IMGInByteArray = file.readAll();

                            QPixmap *pix = new QPixmap();
                            pix->loadFromData(IMGInByteArray,"JPG");
                            QPixmap newPix = pix->scaledToHeight(32);

                            QBuffer buffer(&IMGMinInByteArray);
                            buffer.open(QIODevice::WriteOnly);
                            newPix.save(&buffer, "JPG");

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
    }

    if(ui->treeWidget->topLevelItemCount() > 0) QMessageBox::information(this, tr("Articles ajoutés"), tr("Les articles ont bien été ajoutés."), tr("Fermer"));
}

