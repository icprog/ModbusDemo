#ifndef ZTOOL_H
#define ZTOOL_H
#include<QtGlobal>
class ZTools
{
    ZTools();
public:
    static char* hex2ascii(int hex,char *,int len);
    static quint64 ascii2hex(const char *ascii,int len);
    static QByteArray readLineFromFile(const QString& fileName,int lineNo);
    static void msleep(int msecs);
};

#endif // ZTOOL_H
