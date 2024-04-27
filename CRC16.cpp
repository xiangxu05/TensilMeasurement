#include "CRC16.h"

quint16 calculateCRC16(const QByteArray &data){

    quint16 crc = 0xFFFF;

    for (int i = 0; i < data.size(); ++i) {
        crc ^= static_cast<quint8>(data.at(i));

        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    quint16 CRC = ((crc & 0x00FF) << 8) | ((crc & 0xFF00) >> 8);
    return CRC;

}

bool verifyCRC16(const QByteArray &data)
{
    if (data.size() < 2) {
        return false; // 数据长度不足
    }
    quint8 receivedCRC_1 = data[data.size() - 1];
    quint8 receivedCRC_2 = data[data.size() - 2];
    quint16 receivedCRC = receivedCRC_1 | receivedCRC_2<<8;
    QByteArray dataWithoutCRC = data.mid(0,data.size()-2);
    quint16 calculatedCRC = calculateCRC16(dataWithoutCRC);

    return (receivedCRC == calculatedCRC);
}
