﻿#include "houseview.h"
#include "ui_houseview.h"


Database HouseView::database;
QList<House> HouseView::houses;
int HouseView::front;
int HouseView::end;

HouseView::HouseView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HouseView)
{
    ui->setupUi(this);

    initDatabase();
    initTable(18, 6);
    setHouse();
}

HouseView::~HouseView()
{
    delete ui;
}

void HouseView::initDatabase() {
    QSqlDatabase qSqlDatabase = QSqlDatabase::addDatabase("QMYSQL");
    database.setQSqlDatabase(qSqlDatabase);
}

void HouseView::initTable(int row, int col) {

    ui->table->setRowCount(row);
    ui->table->setColumnCount(col);

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            //门牌号
            QString door;
            if(col<10) {
                door = QString::number(row-i-1)+"0"+QString::number(j);
            } else {
                door = QString::number(row-i-1)+QString::number(j);
            }

            //单元格
            QTableWidgetItem *item = new QTableWidgetItem(door);

            //设置内容
            item->setTextAlignment(Qt::AlignCenter);

            //设置背景色
            QColor color(238, 240, 244, 255);
            item->setBackgroundColor(color);

            ui->table->setItem(i, j, item);
        }
    }
}

void HouseView::setHouse() {

    QDateTime start = QDateTime();
    QDateTime end = QDateTime();

    database.openConnect();
    houses = database.selectHouse(start, end);
    database.closeConnect();

    changeHouseStatus();

}

void HouseView::changeHouseStatus() {

    QColor color(129, 166, 96, 255);//绿色

    for(int i = 0; i < houses.size(); i++) {
        QString houseNo = houses[i].house;
        if(houseNo.length() == 3) {

            QString frontString = houseNo.mid(0, 1);
            QString endString = houseNo.mid(1, 2);

            front = frontString.toInt();
            end = endString.toInt();

            qDebug() << "HouseView::front:" << front;
            qDebug() << "HouseView::end" << end;

            QTableWidgetItem *item = ui->table->takeItem(front, end);
            item->setBackgroundColor(color);
            ui->table->setItem(front, end, item);

        } else if(houseNo.length() == 4){

            QString frontString = houseNo.mid(0, 2);
            QString endString = houseNo.mid(2, 2);

            front = frontString.toInt();
            end = endString.toInt();

            qDebug() << "HouseView::front:" << front;
            qDebug() << "HouseView::end" << end;

        } else {

        }
    }
}

