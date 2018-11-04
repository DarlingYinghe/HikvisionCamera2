﻿#include "previewview.h"
#include "ui_previewview.h"

using namespace std;

//时间解析宏定义
#define GET_YEAR(_time_)      (((_time_)>>26) + 2000)
#define GET_MONTH(_time_)     (((_time_)>>22) & 15)
#define GET_DAY(_time_)       (((_time_)>>17) & 31)
#define GET_HOUR(_time_)      (((_time_)>>12) & 31)
#define GET_MINUTE(_time_)    (((_time_)>>6)  & 63)
#define GET_SECOND(_time_)    (((_time_)>>0)  & 63)

//预览属性设置
LONG PreviewView::lUserID;//用户句柄
LONG PreviewView::lRealPlayHandle;//播放句柄
//画面设置
WORD PreviewView::m_wPicSize;//预览画面大小设置
WORD PreviewView::m_wPicQuality;//预览画面质量设置
//个人信息
char PreviewView::name[32];
char PreviewView::sex[2];
//人脸库图
char* PreviewView::avatar;
DWORD PreviewView::avatarLen;
//抓拍图
char* PreviewView::capture;
DWORD PreviewView::captureLen;
//相似度
float PreviewView::similarity;
int PreviewView::currentRow;
//信息列表
QList<NET_VCA_FACESNAP_MATCH_ALARM> PreviewView::alarmList;
QString PreviewView::currentAlarmInfo;

NET_VCA_FACESNAP_MATCH_ALARM PreviewView::struFaceMatchAlarm = {0};

PreviewView* PreviewView::previewView = nullptr;

PreviewView::PreviewView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreviewView)
{
    previewView = this;

    connect(this, SIGNAL(toShowPersonInfo()), this, SLOT(showPersonInfo()));
    connect(this, SIGNAL(toAddAlarmItem()), this, SLOT(addAlarmItem()));

    ui->setupUi(this);
    loadPreview();

}

PreviewView::~PreviewView()
{
    delete ui;
}

void CALLBACK PreviewView::g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    char tempbuf[256] = { 0 };
    switch (dwType)
    {
    case EXCEPTION_RECONNECT:    //预览时重连
        printf_s("----------reconnect--------%d\n", time(NULL));
        break;
    default:
        break;
    }
}

//报警回调函数
BOOL CALLBACK PreviewView::MessageCallback(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
    qDebug("Callback start");
    switch(lCommand)
    {
    case COMM_SNAP_MATCH_ALARM: //人脸比对结果信息
    {
        qDebug("Detected face");
        memcpy(&struFaceMatchAlarm, pAlarmInfo, sizeof(NET_VCA_FACESNAP_MATCH_ALARM));

        alarmList.append(struFaceMatchAlarm);

        NET_DVR_TIME struAbsTime = {0};
        struAbsTime.dwYear = GET_YEAR(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
        struAbsTime.dwMonth = GET_MONTH(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
        struAbsTime.dwDay = GET_DAY(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
        struAbsTime.dwHour = GET_HOUR(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
        struAbsTime.dwMinute = GET_MINUTE(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
        struAbsTime.dwSecond = GET_SECOND(struFaceMatchAlarm.struSnapInfo.dwAbsTime);

        printf("人脸比对结果上传[0x%x]：[%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d]fSimilarity[%f]FaceID[%d]BlackListID[%d]"
               "Sex[%d]Glasses[%d]Age[%d]MatchPicNum[%d]PicTransType[%d]\n", lCommand,
               struAbsTime.dwYear, struAbsTime.dwMonth, struAbsTime.dwDay, struAbsTime.dwHour, struAbsTime.dwMinute,
               struAbsTime.dwSecond, struFaceMatchAlarm.fSimilarity, struFaceMatchAlarm.struSnapInfo.dwSnapFacePicID,
               struFaceMatchAlarm.struBlackListInfo.struBlackListInfo.dwRegisterID, struFaceMatchAlarm.struSnapInfo.bySex,
               struFaceMatchAlarm.struSnapInfo.byGlasses, struFaceMatchAlarm.struSnapInfo.byAge,
               struFaceMatchAlarm.byMatchPicNum, struFaceMatchAlarm.byPicTransType);

        //个人信息
        setPersonInfo(struFaceMatchAlarm);
        emit previewView->toShowPersonInfo();

        //报警信息
        currentAlarmInfo = QString::asprintf("%4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d ",struAbsTime.dwYear, struAbsTime.dwMonth, struAbsTime.dwDay, struAbsTime.dwHour, struAbsTime.dwMinute,
                                            struAbsTime.dwSecond);
        currentAlarmInfo.append(QString::fromLocal8Bit(name));
        emit previewView->toAddAlarmItem();

        //TAD：数据库操作


        //保存抓拍图片
        /*if (struFaceMatchAlarm.dwSnapPicLen > 0 && struFaceMatchAlarm.pSnapPicBuffer  != NULL && struFaceMatchAlarm.byPicTransType == 0)
        {
            qDebug()<<"capture";
            LPCWSTR cFilename = L"D:\\Cloud\\HIKVISION\\SDK\\build-HikvisionCamera2-Desktop_Qt_5_11_2_MSVC2017_64bit-Debug\\debug\\test.jpg";
            HANDLE hFile;
            DWORD dwReturn;

            char chTime[128];


            hFile = CreateFileW(cFilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE)
            {
                break;
            }
            qDebug()<<"created";
            WriteFile(hFile, struFaceMatchAlarm.pSnapPicBuffer, struFaceMatchAlarm.dwSnapPicLen, &dwReturn, NULL);
            qDebug()<<"writed";
            CloseHandle(hFile);
            qDebug()<<"closed";
            hFile = INVALID_HANDLE_VALUE;
        }*/
        break;
        }
        return TRUE;
    }
}

void PreviewView::loadPreview() {
    //---------------------------------------
    //关闭预览
    if (lRealPlayHandle >= 0) {
        NET_DVR_StopRealPlay(lRealPlayHandle);
        lRealPlayHandle = -1;
    }
    NET_DVR_Logout(lUserID);
    NET_DVR_Cleanup();

    //---------------------------------------
    //配置信息
    QSettings *config = new QSettings(":/config/config.ini", QSettings::IniFormat);

    QString ip = config->value("/Camera/ip").toString();
    int port = config->value("/Camera/port").toInt();
    QString username = config->value("/Camera/username").toString();
    QString password = config->value("/Camera/password").toString();


    if(ip.length()>0 && port>0 && username.length()>0 && password.length()>0) {
        qDebug("information is filled");
        //---------------------------------------
        // 初始化
        NET_DVR_Init();
        //设置连接时间与重连时间
        NET_DVR_SetConnectTime(2000, 1);
        NET_DVR_SetReconnect(10000, true);

        //---------------------------------------
        //设置异常消息回调函数
        NET_DVR_SetExceptionCallBack_V30(0, NULL,g_ExceptionCallBack, NULL);

        //---------------------------------------
        // 注册设备

        //登录参数，包括设备地址、登录用户、密码等
        NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
        struLoginInfo.bUseAsynLogin = 0; //同步登录方式
        strcpy(struLoginInfo.sDeviceAddress, ip.toLatin1().data()); //设备IP地址
        struLoginInfo.wPort = port; //设备服务端口
        strcpy(struLoginInfo.sUserName, username.toLatin1().data()); //设备登录用户名
        strcpy(struLoginInfo.sPassword, password.toLatin1().data()); //设备登录密码

        //设备信息, 输出参数
        NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};

        PreviewView::lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);

        qDebug()<<"lUserID is "<<lUserID<<endl;

        if (lUserID < 0)
        {
            printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
            NET_DVR_Cleanup();
            return;
        }


        //---------------------------------------
        //启动预览并设置回调数据流
        HWND hWnd = (HWND)ui->picPreview->winId();
        NET_DVR_PREVIEWINFO struPlayInfo = {0};
        struPlayInfo.hPlayWnd = hWnd;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
        struPlayInfo.lChannel     = 1;       //预览通道号
        struPlayInfo.dwStreamType = 0;       //0-主码流，1-子码流，2-码流3，3-码流4，以此类推
        struPlayInfo.dwLinkMode   = 0;       //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP
        struPlayInfo.bBlocked     = 1;       //0- 非阻塞取流，1- 阻塞取流

        lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, NULL, NULL);
        if (lRealPlayHandle < 0)
        {
            printf("NET_DVR_RealPlay_V40 error\n");
            NET_DVR_Logout(lUserID);
            NET_DVR_Cleanup();


            return;
        }


        //设置报警回调函数
        NET_DVR_SetDVRMessageCallBack_V31(MessageCallback, NULL);

        //启用布防
        LONG lHandle;
        NET_DVR_SETUPALARM_PARAM  struAlarmParam={0};
        struAlarmParam.dwSize=sizeof(struAlarmParam);
        //其他报警布防参数不需要设置

        lHandle = NET_DVR_SetupAlarmChan_V41(lUserID, & struAlarmParam);
        if (lHandle < 0)
        {
            printf("NET_DVR_SetupAlarmChan_V41 error, %d\n", NET_DVR_GetLastError());
            NET_DVR_Logout(lUserID);
            NET_DVR_Cleanup();
            return;
        }
    }


}

void PreviewView::setPersonInfo(NET_VCA_FACESNAP_MATCH_ALARM struFaceMatchAlarm) {
    /*********************************************设置个人信息******************************************/
    //人脸库头像图
    avatarLen = struFaceMatchAlarm.struBlackListInfo.dwBlackListPicLen;
    avatar = (char*)malloc(struFaceMatchAlarm.struBlackListInfo.dwBlackListPicLen);
    memcpy(avatar, struFaceMatchAlarm.struBlackListInfo.pBuffer1, avatarLen);
    //抓拍图
    captureLen = struFaceMatchAlarm.dwSnapPicLen;
    capture = (char*)malloc(struFaceMatchAlarm.dwSnapPicLen);
    memcpy(capture, struFaceMatchAlarm.pSnapPicBuffer, captureLen);
    //姓名
    char nameHexStr[32];
    BYTE nameBytes[NAME_LEN];
    for(int i =0 ;i<NAME_LEN; i++) {
        nameBytes[i] = struFaceMatchAlarm.struBlackListInfo.struBlackListInfo.struAttribute.byName[i];
    }
    convertUnCharToStr(nameBytes, nameHexStr, name, sizeof(nameBytes));
    //性别
    switch(struFaceMatchAlarm.struBlackListInfo.struBlackListInfo.struAttribute.bySex) {
    case 0x0:
        strcpy(sex, "男");
        break;
    case 0x1:
        strcpy(sex, "女");
        break;
    case 0xff:
        strcpy(sex, "未知");
        break;
    default:
        break;

    }
    //相似度
    similarity = struFaceMatchAlarm.fSimilarity;
    /*********************************************设置个人信息 END******************************************/
}

//更新Ui
void PreviewView::showPersonInfo() {
    qDebug() << "PreviewView: showPersonInfo exec";

    /*********************************************显示个人信息******************************************/
    if(avatarLen>0){
        QByteArray bytearray = QByteArray(avatar, avatarLen);

        QBuffer buffer(&bytearray);
        buffer.open(QIODevice::ReadOnly);

        QImageReader reader(&buffer, "JPG");
        QImage img = reader.read();

        QPixmap pix = QPixmap::fromImage(img);
        ui->picAvatar->setPixmap(pix.scaled(ui->picAvatar->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else {
        ui->picAvatar->setText(QString::fromLocal8Bit("图片缺失"));
    }

    if(captureLen>0){
        QByteArray bytearray = QByteArray(capture, captureLen);

        QBuffer buffer(&bytearray);
        buffer.open(QIODevice::ReadOnly);

        QImageReader reader(&buffer, "JPG");
        QImage img = reader.read();

        QPixmap pix = QPixmap::fromImage(img);
        ui->picCapture->setPixmap(pix.scaled(ui->picCapture->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else {
        ui->picCapture->setText(QString::fromLocal8Bit("图片缺失"));
    }

    ui->edName->setText(QString::fromLocal8Bit(name));
    ui->edSex->setText(QString::fromLocal8Bit(sex));
    ui->edSimilarity->setText(QString::number(similarity*100));
    /*********************************************显示个人信息 END******************************************/

    //数据库录入
    //QSqlDatabase db;
    //createConnect(db, "QMYSQL", "127.0.0.1", 3306, "camera", "root", "haoaihaoaini");
    //addRecord(db, name);
    //closeConnect(db);

}

//点击列表条目后更改个人信息
void PreviewView::changePersonInfo(QListWidgetItem* item) {
    currentRow = ui->alarmList->currentRow();
    NET_VCA_FACESNAP_MATCH_ALARM alarm = {0};
    memcpy(&alarm, &alarmList[currentRow], sizeof(NET_VCA_FACESNAP_MATCH_ALARM));

    setPersonInfo(alarm);
}

void PreviewView::addAlarmItem() {
    qDebug() << "PreviweView: addAlarmItem exec";
    QListWidgetItem* item = new QListWidgetItem(PreviewView::currentAlarmInfo, ui->alarmList, 0);
    connect(ui->alarmList, SIGNAL(itemDoubleClicked(item)), this, SLOT(changePersonInfo(item)));
}

void PreviewView::convertUnCharToStr(BYTE *UnChar,char *hexStr, char *str, int len)
{
    //将16进制数据转化为对应的16进制字符
    for(int i = 0; i < len; i++)
    {
        if(UnChar[i]>127) {
            sprintf(hexStr + i * 2, "%02x", UnChar[i]);
        }
    }

    //将16进制字符转化为字符串
    int i,v;
    i=0;
    while (1) {
        if (1!=sscanf(hexStr+i*2,"%2x",&v)) break;
        str[i]=(char)v;
        i++;
    }
    str[i]=0;
}
