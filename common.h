#ifndef COMMON_H
#define COMMON_H


#define INSERT_DATA_SIM 0
#define NEED_LOGIN 1
#define USE_LOCALHOST_DATABASE 1

typedef  struct msg{
    unsigned char device;
    unsigned char version;
    unsigned short CO2;
    unsigned short CH2O;
    unsigned short TVOC;
    unsigned short PM2_5;
    unsigned short PM10;
    float TEMP;
    float HUM;
    unsigned char sum;
}Msg;


#endif // COMMON_H
