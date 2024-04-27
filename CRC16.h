#ifndef CRC16_H
#define CRC16_H

#include <QtGlobal>
#include <QByteArray>

//计算CRC16-modbus冗余码
quint16 calculateCRC16(const QByteArray &data);

bool verifyCRC16(const QByteArray &data);

#endif // CRC16_H
