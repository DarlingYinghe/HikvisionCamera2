﻿#include "database.h"
#include <QDateTime>
#include <QString>
QSqlDatabase Database::db;
int id;
int houseid;
Database::Database() {

}

void Database::setQSqlDatabase(QSqlDatabase db) {
    this->db = db;
}

void Database::setDbConfig() {
    db.setHostName(Config::getDataBaseInfoIP());
    db.setPort(Config::getDataBaseInfoPort());
    db.setDatabaseName(Config::getDataBaseInfoModel());
    db.setUserName(Config::getDataBaseInfoUserName());
    db.setPassword(Config::getDataBaseInfoPassWord());
}

//创建数据库连接
bool Database::openConnect() {
    try {
        setDbConfig();

        bool ok = db.open();//建立数据库连接
        if(ok) {
            qDebug() << "Database: connect to succeed";
            return false;
        }
        else {
            qDebug() << "Database: connect to failed";
            return true;
        }
    } catch(std::exception &e) {
        qDebug()<<"# ERR: SQLException:" <<e.what();
        //TAD：进行ui提示
    }
}

//关闭数据库连接
bool Database::closeConnect() {
    try {
        if(db.isOpen()) {
            db.close();
            qDebug() << "db has closed!";
        }

    } catch(std::exception &e) {
        qDebug()<<"# ERR: SQLException:" <<e.what();
        return false;
        //TAD：进行ui提示
    }
    return true;
}

//增加记录
bool Database::addRecord(QString applicant, QString idCapture,
                         QString idAvatar, QString idFace, bool isStranger, float similar) {
    try{
        if(&db!=NULL) {
            //执行sql语句
            QSqlQuery query;
            openConnect();
            query.prepare("INSERT INTO record(applicant, capture_id, avatar_id, face_id, stranger, similar) "
                          "VALUES(:applicant, :idCapture, :idAvatar, :idFace, :isStranger, :similar)");
            query.bindValue(":applicant", applicant);
            query.bindValue(":idCapture", idCapture);
            query.bindValue(":idAvatar", idAvatar);
            query.bindValue(":idFace", idFace);
            query.bindValue(":isStranger", isStranger);
            query.bindValue(":similar", similar);
            query.exec();
            closeConnect();

            //调试代码
            QString sql = "INSERT INTO record(applicant, capture_id, avatar_id, face_id, stranger, similar) "
                          "VALUES("+applicant+","+idCapture+","+idAvatar+","+idFace+","+isStranger+","+similar+")";
            qDebug() << "Test code:"<<sql;
            //End调试代码

            return true;
        }
        return false;
    }catch(std::exception &e)
    {
        qDebug()<<"# ERR: SQLException:" <<e.what();
        //TAD：进行ui提示
    }
}
//增加记录
bool Database::addApplicant(QString NameEdit, QString IdNumEdit,QString PhoneEdit, QString IdentityEdit) {
    try{
        if(&db!=NULL) {
            //执行sql语句
            QSqlQuery query;
            openConnect();
            query.prepare("INSERT INTO applicant(applicant, sfzno, contact, familyrole) "
                          "VALUES(:applicant, :sfzno, :contact, :familyrole)");
            query.bindValue(":applicant", NameEdit);
            query.bindValue(":sfzno", IdNumEdit);
            query.bindValue(":contact", PhoneEdit);
            query.bindValue(":familyrole", IdentityEdit);

            query.exec();
            closeConnect();

            //调试代码
            //            QString sql = "INSERT INTO record(applicant, sfzno, contact, familyrole) "
            //                    "VALUES("+applicant+","+idCapture+","+idAvatar+","+idFace+","+isStranger+","+similar+")";
            //qDebug() << "Test code:"<<sql;
            //End调试代码

            return true;
        }
        return false;
    }catch(std::exception &e)
    {
        qDebug()<<"# ERR: SQLException:" <<e.what();
        //TAD：进行ui提示
    }
}


//查找记录
bool Database::searchRecord(QString applicant,QString idAvatar) {
    try{
        if(&db!=NULL) {
            //执行sql语句
            QSqlQuery query;
            QString str= QString("SELECT * FROM applicant where applicant= '%1' AND sfzno= '%2'")
                    .arg(applicant).arg(idAvatar);
            qDebug() << "searchRecord:"<<applicant<<" "<<idAvatar;
            openConnect();
            query.exec(str);
            closeConnect();
            query.next();
            id=query.value(0).toInt();
            //调试代码
            qDebug() << "Test code:"<<str;
            qDebug() << "Test id:"<<id;
            //End调试代码
            return true;
        }
        return false;
    }catch(std::exception &e)
    {
        qDebug()<<"# ERR: SQLException:" <<e.what();
        //TAD：进行ui提示
    }
}

//根据id查找房间
QList<HouseInfo> Database::searchHouse(QString state) {
    try{
        QSqlQuery query;
        QString str;
        if(state=="search"){
            //执行sql语句
            str= QString("select community,building,unit,house from applicant,houseapplicant,house where applicant.id = houseapplicant.applicant_id and houseapplicant.house_id=house.id and applicant.id= '%1';")
                    .arg(id);
            qDebug() << "searchHouse:"<<str;
            qDebug() << "searchHouse id:"<<id;
        }else{
            str= QString("select community,building,unit,house from house;");
        }
        openConnect();
        query.exec(str);
        closeConnect();
        QList<HouseInfo> houseInfos;
        while(query.next())
        {
            HouseInfo houseInfo;
            houseInfo.community = query.value("community").toString();
            houseInfo.building = query.value("building").toString();
            houseInfo.unit = query.value("unit").toString();
            houseInfo.house = query.value("house").toString();
            houseInfos.append(houseInfo);
        }
        //End调试代码
        return houseInfos;
    }catch(std::exception &e)
    {
        qDebug()<<"# ERR: SQLException:" <<e.what();
        //TAD：进行ui提示
    }
}

//编辑记录
bool Database::editApplicant(QString NameEdit, QString IdNumEdit,QString PhoneEdit, QString IdentityEdit) {
    try{
        if(&db!=NULL) {
            //执行sql语句
            QSqlQuery query;
            QString str = QString("UPDATE applicant SET applicant = '%1', sfzno = '%2', contact = '%3', familyrole = '%4' WHERE id = %5")
                    .arg(NameEdit).arg(IdNumEdit).arg(PhoneEdit).arg(IdentityEdit).arg(id);
            openConnect();
            query.prepare(str);
            query.exec();
            closeConnect();
            //调试代码
            qDebug() << "Test code:"<<str;
            //End调试代码
            return true;
        }
        return false;
    }catch(std::exception &e)
    {
        qDebug()<<"# ERR: SQLException:" <<e.what();
        //TAD：进行ui提示
    }
}



bool Database::updatehouseInfo(HouseInfo houseInfo){
    if(&db!=NULL) {
        //执行sql语句
        QSqlQuery query;
        QString str = QString("select id from house where community='%1' and building='%2' and unit ='%3' and house ='%4'")
                .arg(houseInfo.community).arg(houseInfo.building).arg(houseInfo.unit).arg(houseInfo.house);
        openConnect();
        query.prepare(str);
        query.exec();
        closeConnect();
        query.next();
        houseid= query.value(0).toInt();;
        str = QString("UPDATE houseapplicant set house_id = '%1' where  applicant_id = '%2'")
                .arg(houseid).arg(id);
        openConnect();
        query.prepare(str);
        query.exec();
        closeConnect();

        //调试代码
        qDebug() << "Test houseid:"<<houseid;
        //End调试代码
        return true;
    }
    return false;
}

bool Database::addhouseInfo(HouseInfo houseInfo){
    if(&db!=NULL) {
        //执行sql语句
        QSqlQuery query;
        QString str = QString("select id from house where community='%1' and building='%2' and unit ='%3' and house ='%4'")
                .arg(houseInfo.community).arg(houseInfo.building).arg(houseInfo.unit).arg(houseInfo.house);
        openConnect();
        query.prepare(str);
        query.exec();
        closeConnect();
        query.next();
        houseid= query.value(0).toInt();
        str = QString("select max(id) from applicant where isdel=0");
        openConnect();
        query.prepare(str);
        query.exec();
        closeConnect();
        query.next();
        int tempid= query.value(0).toInt();
        qDebug() << "addhouseInfo houseid:"<<houseid<<"id:"<<tempid;
        openConnect();
        query.prepare("INSERT INTO houseapplicant (applicant_id, house_id) VALUES(:applicant_id, :house_id)");
        query.bindValue(":applicant_id", tempid);
        query.bindValue(":house_id", houseid);
        query.exec();
        closeConnect();
        return true;
    }
    return false;
}



//删除记录
bool Database::delRecord() {
    try{
        if(&db!=NULL) {
            //执行sql语句
            QSqlQuery query;
            QString str = QString("UPDATE applicant SET isdel = '%1' WHERE id = %2")
                    .arg(1).arg(id);
            openConnect();
            query.prepare(str);
            query.exec();
            closeConnect();
            //调试代码
            qDebug() << "Test code:"<<str;
            //End调试代码
            return true;
        }
        return false;
    }catch(std::exception &e)
    {
        qDebug()<<"# ERR: SQLException:" <<e.what();
        //TAD：进行ui提示
    }
}

//设置记录内容
QList<RECORD> Database::setRecord(QSqlQuery query) {
    QList<RECORD> records;
    while(query.next())
    {
        RECORD record;
        record.timesamp = query.value("time_value").toDateTime();
        record.applicant = query.value("applicant").toString();
        record.idAvatar = query.value("avatar_id").toString();
        record.idCapture = query.value("capture_id").toString();
        record.isStranger = query.value("stranger").toBool();
        records.append(record);
    }
    return records;
}

//根据时间、姓名、身份证号来获取记录
RECORD Database::selectRecord(QDateTime timesamp, QString applicant, QString idAvatar) {
    RECORD record;
    try{
        if(&db!=NULL) {
            //执行sql语句
            QSqlQuery query;
            openConnect();
            query.prepare("SELECT * "
                          "FROM record r where r.time_value=:timesamp "
                          "AND r.applicant=:applicant AND r.avatar_id=:avatar_id");
            query.bindValue(":timesamp", timesamp);
            query.bindValue(":applicant", applicant);
            query.bindValue(":avatar_id", idAvatar);
            query.exec();
            closeConnect();
            query.next();
            record.timesamp = query.value("time_value").toDateTime();
            record.applicant = query.value("applicant").toString();
            record.idAvatar = query.value("avatar_id").toString();
            record.idCapture = query.value("capture_id").toString();
            record.similar = query.value("similar").toInt();
            record.isStranger = query.value("stranger").toBool();
        }
        return record;
    }catch(std::exception &e)
    {
        qDebug()<<"# ERR: SQLException:" <<e.what();
        //TAD：进行ui提示
    }


}

QList<House> Database::setHouse(QSqlQuery query) {
    QList<House> houses;
    while(query.next()) {
        House house;
        house.community = query.value("community").toString();
        house.building = query.value("building").toString();
        house.unit = query.value("unit").toString();
        house.house = query.value("house").toString();
        house.area = query.value("area").toDouble();
        houses.append(house);

    }
    qDebug()<<"HouseSize::"<<houses.size();
    return houses;
}

//获取记录
QList<RECORD> Database::selectRecord() {
    QList<RECORD> records;
    openConnect();
    QSqlQuery query;
    query.exec("select * from record order by time_value desc");
    closeConnect();
    return setRecord(query);
}


//根据出入时间获取出入情况
QList<House> Database::selectHouse(QDateTime start, QDateTime end) {
    openConnect();
    QSqlQuery query;
    QString sqlSentence = "SELECT h.* FROM `houseapplicant` ha LEFT JOIN applicant a ON ha.applicant_id = a.id LEFT JOIN house h ON ha.house_id = h.id WHERE a.sfzno IN (SELECT DISTINCT	avatar_id	FROM	`record`	WHERE	stranger = 0	AND time_value >= :start	AND time_value <= :end)";
    query.prepare(sqlSentence);
    query.bindValue(":start",start.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":end", end.toString("yyyy-MM-dd hh:mm:ss"));
    query.exec();
    closeConnect();
    return setHouse(query);
}

//根据出入时间获取需要导出的记录
QList<ExcelExportInfo> Database::selectExportRecord(QDateTime start, QDateTime end) {
    openConnect();
    qDebug() << start.toString("yyyy-MM-dd hh:mm:ss") <<" " << end.toString("yyyy-MM-dd hh:mm:ss");
    QSqlQuery query;
    QString sqlSentence = "SELECT h.community, h.building, h.unit, h.house, a.applicant, a.sfzno, a.familyrole, r.time_value, r.similar\
            FROM record r, applicant a, house h, houseapplicant ha WHERE         \
            r.time_value >= :start AND r.time_value <= :end AND                  \
                             a.sfzno=r.avatar_id AND a.status=1 AND                               \
                             a.id = ha.id AND ha.applicant_id AND ha.house_id = h.id";
                             query.prepare(sqlSentence);
    query.bindValue(":start",start.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":end", end.toString("yyyy-MM-dd hh:mm:ss"));
    query.exec();
    closeConnect();
    QList<ExcelExportInfo> exportInfo;
    while(query.next()) {
        ExcelExportInfo info;
        info.community = query.value("community").toString();
        info.building = query.value("building").toString();
        info.unit = query.value("unit").toString();
        info.house = query.value("house").toString();
        info.applicant = query.value("applicant").toString();
        info.sfzno = query.value("sfzno").toString();
        info.familyrole = query.value("familyrole").toString();
        info.similar = query.value("similar").toInt();
        info.time_value = query.value("time_value").toDateTime();
        exportInfo.append(info);
    }
    qDebug() <<"exportInfo.size():" <<exportInfo.size();
    return exportInfo;
}

//根据计时器来获取出入情况
QList<House> Database::selectHouseAsTimer() {
    QDateTime curr = QDateTime::currentDateTime();
    QDateTime start = curr.addSecs(-5);

    qDebug()<<start.toString();
    openConnect();
    QSqlQuery query;
    QString sqlSentence = "SELECT h.* FROM `houseapplicant` ha LEFT JOIN applicant a ON ha.applicant_id = a.id LEFT JOIN house h ON ha.house_id = h.id WHERE a.sfzno IN (SELECT DISTINCT	avatar_id	FROM	`record`	WHERE	stranger = 0	AND time_value >= :start	AND time_value <= :end)";
    query.prepare(sqlSentence);
    query.bindValue(":start", start.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":end", curr.toString("yyyy-MM-dd hh:mm:ss"));
    query.exec();
    closeConnect();
    return setHouse(query);
}

//根据条件筛查
QList<ApplicantInfo> Database::selectById(int startId, int pageSize, QString applicant, QString idNumber, QString contact, QString status, int &totalRecordNum){
    getMaxapplicantNum(applicant,idNumber,contact,status,totalRecordNum);
    openConnect();
    QSqlQuery query;
    QString sqlSentence;
    if(!applicant.isEmpty()) {
        sqlSentence = "select applicant.id,applicant,sfzno,contact,familyrole,isdel,community,building,unit,house from applicant,houseapplicant,house where applicant.id = houseapplicant.applicant_id and houseapplicant.house_id=house.id and applicant=:applicant and isdel=0";
    } else if(!idNumber.isEmpty()) {
        sqlSentence = "select applicant.id,applicant,sfzno,contact,familyrole,isdel,community,building,unit,house from applicant,houseapplicant,house where applicant.id = houseapplicant.applicant_id and houseapplicant.house_id=house.id and sfzno=:idNumber and isdel=0";
    }else {
        sqlSentence = "select applicant.id,applicant,sfzno,contact,familyrole,isdel,community,building,unit,house from applicant,houseapplicant,house where applicant.id = houseapplicant.applicant_id and houseapplicant.house_id=house.id and isdel = 0";
    }
    sqlSentence += " limit :startId, :pageSize";
    query.prepare(sqlSentence);
    query.bindValue(":idNumber", idNumber);
    query.bindValue(":startId", (startId-1)*pageSize);
    query.bindValue(":pageSize", pageSize);
    query.bindValue(":applicant", applicant);
    query.exec();
    closeConnect();
    qDebug() << "selectById: " << sqlSentence;
    return setApplicant(query);
}

//获取记录总条数
void Database::getMaxapplicantNum(QString applicant, QString idNumber, QString contact, QString status, int &totalRecordNum){
    openConnect();
    QSqlQuery query;
    QString sqlSentence;
    if(!applicant.isEmpty()) {
        sqlSentence = "select count(*) from applicant where applicant=:applicant and isdel=0";
    } else if(!idNumber.isEmpty()) {
        sqlSentence = "select count(*) from applicant where isdel=0";
    }else {
        sqlSentence = "select count(*) from applicant where isdel=0";
    }
    query.prepare(sqlSentence);
    query.exec();
    closeConnect();
    query.next();
    totalRecordNum = query.value(0).toInt();

    qDebug() << "getMaxapplicantNum: " << totalRecordNum;
}

//设置记录内容
QList<ApplicantInfo> Database::setApplicant(QSqlQuery query) {
    QList<ApplicantInfo> applicantInfos;
    while(query.next())
    {
        ApplicantInfo applicantInfo;
        applicantInfo.applicant = query.value("applicant").toString();
        applicantInfo.sfzno = query.value("sfzno").toString();
        applicantInfo.contact = query.value("contact").toString();
        applicantInfo.status = query.value("familyrole").toString();
        applicantInfo.house = query.value("community").toString()+query.value("building").toString()+QStringLiteral("幢")+query.value("unit").toString()+QStringLiteral("单元")+query.value("house").toString();
        applicantInfos.append(applicantInfo);
    }
    return applicantInfos;
}

//获取记录总条数
void Database::getTotalRecordNum(QDateTime startDateTime, QDateTime endDateTime, int strangerIndex, QString idNumber,
                                 int startId, int pageSize, int &totalRecordNum) {
    openConnect();
    QSqlQuery query;
    QString sqlSentence = "select count(*) from record where time_value>:startDateTime and time_value<:endDateTime";
    if(strangerIndex == 0) {
        sqlSentence += " and stranger=1";
    } else if(strangerIndex == 1) {
        sqlSentence += " and stranger=0";
    }
    if(idNumber.length() > 0) {
        sqlSentence += " and avatar_id=:idNumber";
    }
    query.prepare(sqlSentence);
    query.bindValue(":startDateTime", startDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":endDateTime", endDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":idNumber", idNumber);
    query.exec();
    closeConnect();
    query.next();
    totalRecordNum = query.value(0).toInt();
}

//根据条件筛查记录
QList<RECORD> Database::selectByCondition(QDateTime startDateTime, QDateTime endDateTime, int strangerIndex, QString idNumber,
                                          int startId, int pageSize, int &totalRecordNum) {
    getTotalRecordNum(startDateTime, endDateTime, strangerIndex, idNumber, startId, pageSize, totalRecordNum);
    openConnect();
    QSqlQuery query;
    QString sqlSentence = "select * from record where time_value>=:startDateTime and time_value<=:endDateTime and del = 0";
    if(strangerIndex == 0) {
        sqlSentence += " and stranger=1";
    } else if(strangerIndex == 1) {
        sqlSentence += " and stranger=0";
    }
    if(idNumber.length() > 0) {
        sqlSentence += " and avatar_id=:idNumber";
    }
    sqlSentence += " limit :startId, :pageSize";
    query.prepare(sqlSentence);
    query.bindValue(":startDateTime", startDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":endDateTime", endDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":idNumber", idNumber);
    query.bindValue(":startId", (startId-1)*pageSize);
    query.bindValue(":pageSize", pageSize);
    query.exec();
    closeConnect();
    return setRecord(query);

}

//根据时间与门牌号来筛查记录
QList<RECORD> Database::selectByTimeDoorplate(QDateTime startDateTime, QDateTime endDateTime,
                                              QString doorPlate) {
    openConnect();
    QSqlQuery query;
    QString sqlSentence = "SELECT r.time_value,r.applicant,r.avatar_id,r.capture_id,r.stranger "
                          "FROM record r,house h,applicant a, houseapplicant ha "
                          "WHERE r.time_value>=:startDateTime AND r.time_value<=:endDateTime "
                          "AND h.house=:doorPlate "
                          "AND r.avatar_id=a.sfzno "
                          "AND ha.house_id=h.id AND ha.applicant_id=a.id";

    query.prepare(sqlSentence);
    query.bindValue(":startDateTime", startDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":endDateTime", endDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":doorPlate", doorPlate);
    query.exec();
    closeConnect();
    return setRecord(query);
}


QString Database::selectFamilyRole(QString applicant, QString sfzno) {
    openConnect();
    QSqlQuery query;
    QString sqlSentence = "SELECT a.familyrole FROM applicant a "
                          "WHERE a.applicant=:applicant AND a.sfzno=:sfzno";

    query.prepare(sqlSentence);
    query.bindValue(":applicant", applicant);
    query.bindValue(":sfzno", sfzno);
    query.exec();
    closeConnect();
    query.next();
    return query.value("familyrole").toString();
}


ADDRESS_INFO Database::selectAddress(QString applicant, QString sfzno) {
    openConnect();
    ADDRESS_INFO addressInfo;
    QSqlQuery query;
    QString sqlSentence = "SELECT  h.community,h.building,h.unit,h.house "
                          "FROM applicant a,house h,houseapplicant ha "
                          "WHERE a.applicant=:applicant AND a.sfzno=:sfzno "
                          "AND a.id=ha.id AND ha.house_id=h.id";

    query.prepare(sqlSentence);
    query.bindValue(":applicant", applicant);
    query.bindValue(":sfzno", sfzno);
    query.exec();
    query.next();
    addressInfo.community = query.value("community").toString();
    addressInfo.building = query.value("building").toString();
    addressInfo.unit = query.value("unit").toString();
    addressInfo.house = query.value("house").toString();
    closeConnect();
    return addressInfo;
}

ApplicantInfo Database::selectApplicantInfoBySfzNo(QString sfzNo) {
    openConnect();
    ApplicantInfo applicantInfo;
    QSqlQuery query;
    QString sql = "select * from applicant where sfzno = :sfzNo";
    query.prepare(sql);
    query.bindValue(":sfzNo", sfzNo);
    query.exec();
    closeConnect();
    query.next();
    applicantInfo.applicant = query.value("applicant").toString();
    return applicantInfo;
}

ApplicantInfo Database::selectApplicantInfo() {
    openConnect();
    ApplicantInfo applicantInfo;
    QSqlQuery query;
    QString sql = "SELECT a.applicant,a.sfzno,a.contact,a.status FROM applicant a";
    query.prepare(sql);
    query.exec();
    closeConnect();
    query.next();
    applicantInfo.applicant = query.value("applicant").toString();
    applicantInfo.contact = query.value("contact").toString();
    applicantInfo.sfzno = query.value("sfzno").toString();
    applicantInfo.status = query.value("status").toInt();
    return applicantInfo;
}

int Database::selectStrQuantityByTime(int dwYear, int dwMonth, int dwDay) {
    QDate date = QDate(dwYear, dwMonth, dwDay);
    QTime time = QTime(0, 0, 0);
    QDateTime dateTime = QDateTime(date, time);
    qDebug() << "dateTime: " << dateTime.toString("yyyy-MM-dd hh:mm:ss");
    openConnect();
    QSqlQuery query;
    QString sqlSentence = "select count(*) from stranger_quantity where time_value=:dateTime";
    query.prepare(sqlSentence);
    query.bindValue(":dateTime", dateTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.exec();
    query.next();
    int hasRecord = query.value(0).toInt();
    qDebug() << "hasRecord: " << hasRecord;
    if(hasRecord == 0) {
        query.prepare("INSERT INTO stranger_quantity(time_value, quantity) "
                      "VALUES(:time_value, :strQuantity)");
        query.bindValue(":time_value", dateTime.toString("yyyy-MM-dd hh:mm:ss"));
        query.bindValue(":strQuantity", 1);
        query.exec();
        closeConnect();
        return 1;
    } else {
        query.prepare("SELECT quantity FROM stranger_quantity WHERE time_value=:dateTime");
        query.bindValue(":dateTime", dateTime.toString("yyyy-MM-dd hh:mm:ss"));
        query.exec();
        query.next();
        int strangerQuantity = query.value("quantity").toInt() + 1;
        qDebug() << "dataBase Quantity: " << strangerQuantity;
        query.prepare("UPDATE stranger_quantity SET quantity=:strQuantity WHERE time_value=:dateTime");
        query.bindValue(":strQuantity", strangerQuantity);
        query.bindValue(":dateTime", dateTime.toString("yyyy-MM-dd hh:mm:ss"));
        query.exec();
        closeConnect();
        return strangerQuantity;
    }
}
/*
ApplicantInfo Database::setSingleApplicantInfo(QSqlQuery query) {
    ApplicantInfo applicantInfo;
    while(query.next()) {
        applicantInfo.applicant = query.value("applicant").toString();
    }
    return applicantInfo;
}
*/
