﻿#ifndef DATABASE_H
#define DATABASE_H

#include "headinfo.h"
#include "config.h"


class Database
{
public:
    explicit Database();
    QList<RECORD> setRecord(QSqlQuery query);
    QList<ApplicantInfo> setApplicant(QSqlQuery query);


    QList<RECORD> selectRecord();

    QList<ApplicantInfo> selectById(int startId,int pageSize, QString applicant, QString idNumber, QString contact, QString status, int &totalRecordNum);
    //根据条件查找历史纪录
    QList<RECORD> selectByCondition(QDateTime startDateTime, QDateTime endDateTime, int strangerIndex, QString idNumber,
                                    int startId, int pageSize, int &totalRecordNum);
    void getTotalRecordNum(QDateTime startDateTime, QDateTime endDateTime, int strangerIndex, QString idNumber,
                                     int startId, int pageSize, int &totalRecordNum);
    //根据时间与门牌号来查找记录
    QList<RECORD> selectByTimeDoorplate(QDateTime startDateTime, QDateTime endDateTime, QString doorPlate);

    RECORD selectRecord(QDateTime timesamp, QString applicant, QString idAvatar);
    QString selectFamilyRole(QString applicant, QString sfzno);
    ADDRESS_INFO selectAddress(QString applicant, QString sfzno);

    void setDbConfig();
    bool openConnect();
    bool closeConnect();
 bool addRecord(QString applicant, QString idCapture, QString idFace, QString idAvatar, bool isStranger, float similar);

    bool addApplicant(QString NameEdit, QString IdNumEdit, QString PhoneEdit, QString IdentityEdit);
    bool editApplicant(QString NameEdit, QString IdNumEdit, QString PhoneEdit, QString IdentityEdit);
    bool updatehouseInfo(HouseInfo houseInfo);
    bool addhouseInfo(HouseInfo houseInfo);
    bool searchRecord(QString applicant, QString idAvatar);
    QList<HouseInfo> searchHouse(QString state);


    bool delRecord();

    void setQSqlDatabase(QSqlDatabase db);
 void getMaxapplicantNum(QString applicant, QString idNumber, QString contact, QString status, int &totalRecordNum);
    QList<House> setHouse(QSqlQuery query);

    QList<House> selectHouse(QDateTime start, QDateTime end);
    QList<House> selectHouseAsTimer();
    QList<ExcelExportInfo>selectExportRecord(QDateTime start, QDateTime end);

    House selectSingleHouse();

    ApplicantInfo selectApplicantInfoBySfzNo(QString sfzNo);
    ApplicantInfo selectApplicantInfo();
    //ApplicantInfo setSingleApplicantInfo(QSqlQuery query);
    //获取该日的陌生人数量
    int selectStrQuantityByTime(int dwYear, int dwMonth, int dwDay);

private:
    static QSqlDatabase db;

};

#endif // DATABASE_H
