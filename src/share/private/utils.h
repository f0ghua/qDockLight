#pragma once

#include <Qt>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QTime>

namespace Utils {

template<typename T, size_t N>
size_t ArraySize( T (&array)[N] )
{
    return N;
}

template<typename T, int N>
T *ArrayBegin(T (&array)[N])
{
    return &array[0];
}

template<typename T, int N>
T *ArrayEnd(T (&array)[N])
{
    return &array[N-1];
}

template<typename T, int N>
const T *ArrayBeginConst(const T (&array)[N])
{
    return &array[0];
}

template<typename T, int N>
const T *ArrayEndConst(const T (&array)[N])
{
    return &array[N-1];
}

class Base
{
	public:

		static bool decimalMode;
		static quint8 linVersion;
		static bool isSaintDevice;

		static uint64_t parseStringToNum(QByteArray input)
		{
			uint64_t temp = 0;

			input = input.toUpper();
			if (input.startsWith("0X") || input.startsWith("X")) //hex number
			{
				if (input.length() < 3) temp = 0;
				else temp = input.right(input.size() - 2).toLongLong(NULL, 16);
			}
			else if (input.startsWith("B")) //binary number
			{
				input = input.right(input.size() - 1); //remove the B
				for (int i = 0; i < input.length(); i++)
				{
					if (input[i] == '1') temp += (uint64_t)1 << (input.length() - i - 1);
				}
			}
			else //decimal number
			{
				temp = input.toLongLong();
			}

			return temp;
		}

		static uint64_t parseStringToNum(QString input)
		{
			return parseStringToNum(input.toUtf8());
		}

		// either with "0x" prepend or not return the right value
		static uint64_t parseHexStringToNum(const QString &input)
		{
			return input.toLongLong(NULL, 16);
		}

		static uint64_t parseHexStringToNum(const QVariant &input)
		{
			return input.toString().toLongLong(NULL, 16);
		}

		static long getTimeMS()
		{
			QDateTime stamp = QDateTime::currentDateTime();
			return (long)(((stamp.time().hour() * 3600) + (stamp.time().minute() * 60) + (stamp.time().second()) * 1000) + stamp.time().msec());
		}

		//prints hex numbers in uppercase with 0's filling out the number depending
		//on the size needed. Promotes hex numbers to either 2, 4, or 8 digits
		static QString formatHexNum(uint64_t input)
		{
			if (input < 256)
				return "0x" + QString::number(input, 16).toUpper().rightJustified(2,'0');
			if (input < 65536)
				return "0x" + QString::number(input, 16).toUpper().rightJustified(4,'0');
			if (input < 4294967296)
				return "0x" + QString::number(input, 16).toUpper().rightJustified(8,'0');
			return "0x" + QString::number(input, 16).toUpper().rightJustified(16,'0');
		}

		static QString formatHexNumber(uint64_t input)
		{
			if (input < 256)
				return QString::number(input, 16).toUpper().rightJustified(2,'0');
			if (input < 65536)
				return QString::number(input, 16).toUpper().rightJustified(4,'0');
			if (input < 4294967296)
				return QString::number(input, 16).toUpper().rightJustified(8,'0');
			return QString::number(input, 16).toUpper().rightJustified(16,'0');
		}

		static QString formatByteArray(const QByteArray *pba)
		{
			QString tempStr;

			for (int i = 0; i < pba->count(); i++)
			{
				tempStr.append(formatHexNumber(pba->at(i)&0xFF));
				if (i < pba->count()-1)
					tempStr.append(" ");
			}
			return tempStr;
		}

		static QString formatByteArrayML(const QByteArray *pba)
		{
			QString tempStr;

			for (int i = 0; i < pba->count(); i++)
			{
				tempStr.append(formatHexNumber(pba->at(i)&0xFF));
				if (i < pba->count()-1)
				{
					if ((i+1) % 16 == 0) tempStr.append("\n");
					else tempStr.append(" ");
				}
			}
			return tempStr;
		}

		//uses decimalMode to see if it should show value as decimal or hex
		static QString formatNumber(uint64_t value)
		{
			if (decimalMode)
			{
				return QString::number(value, 10);
			}
			else return formatHexNum(value);
		}

		static QString formatByteAsBinary(uint8_t value)
		{
			QString output;
			for (int b = 7; b >= 0; b--)
			{
				if (value & (1 << b)) output += "1";
				else output += "0";
			}
			return output;
		}

		static QByteArray hexString2ByteArray(const QString &s)
		{
			QString tmpStr = s;
			tmpStr.simplified().remove(' ');
			return QByteArray::fromHex(s.toLatin1());
		}

		static uint64_t bcd2Dec(const QByteArray bcd)   
        {    
            uint64_t dec = 0;   
      
            for(int i = 0; i < bcd.count(); i++)   
            {   
                dec *= 10;
                dec += (bcd.at(i) >> 4) & 0x0F;
                dec *= 10;
                dec += bcd.at(i) & 0x0F;
            }   

            return dec;   
        }

        static QByteArray dec2Bcd(uint64_t dec)   
        {   
            int tmp; 
            QByteArray ba;
            
            while(dec > 0)
            {   
                tmp = dec % 100;   
                char c = ((tmp / 10) << 4) + ((tmp % 10) & 0x0F);   
                ba.append(c);
                dec /= 100;   
            }   

            QByteArray reverse;
            reverse.reserve(ba.size());
            for(int i = ba.size()-1; i >= 0; --i) 
                reverse.append(ba.at(i));
            return reverse;  
        }  

        static QString getDateTimeFormat1()
        {
            QDateTime time = QDateTime::currentDateTime();
            QString date = QLocale( QLocale::C ).toString(time, "yyyyMMddhhmmss");
            
            return date;
        }

		static int isDigitStr(QString src)
		{
			QByteArray ba = src.toLatin1();
			const char *s = ba.data();

			while(*s && *s>='0' && *s<='9') s++;

			return ((*s) ? -1 : 0);
		}

        static inline quint64 byteArray2Ux(const QByteArray &ba, int len)
        {
            quint64 v = 0;
            for (quint8 j = 0; j < len; j++) {
                v += ((ba.at(j)&0xFFULL)<<((len-j-1)*8));
            }
            return v;
        }

        static inline QByteArray ux2ByteArray(quint64 v, int len)
        {
            QByteArray ba(len, 0);
            for (quint8 j = 0; j < len; j++) {
                ba[j] = (v >> ((len-j-1)*8))&0xFF;
            }
            return ba;
        }

        static inline double byteArray2Double(const QByteArray &ba)
        {
            quint64 v = byteArray2Ux(ba, sizeof(quint64));
            return *reinterpret_cast<double *>(&v);
        }

        static inline QByteArray double2ByteArray(double b)
        {
            quint64 v = *reinterpret_cast<quint64 *>(&b);
            return ux2ByteArray(v, sizeof(v));
        }

        static void msleep(int ms)
        {
            QTime dieTime = QTime::currentTime().addMSecs(ms);

            while (QTime::currentTime() < dieTime) {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            }
        }
};

#ifdef HAS_QDOUBLEVALIDATOR
#include <QDoubleValidator>
class MyDoubleValidator : public QDoubleValidator
{
public:
    MyDoubleValidator(double bottom, double top, int decimals, QObject *parent = 0)
        :QDoubleValidator( bottom, top, decimals, parent) {};
    QValidator::State validate(QString &input, int &pos) const
    {
        /*
         * Original Qt Documentation:
         * ---------
         * QDoubleValidator::validate()
         * Returns Intermediate if input contains a double that is
         * outside the range or is in the wrong format; e.g. with too many
         * digits after the decimal point or is empty.
         * ---------
         * Problem: Not what the user expects.
         * Example: Range 0.0-10.0 with 1 digit (QDoubleValidator( 0, 10, 1, parent ) ):
         * QDoubleValidator::validate() reports intermediate for "10.3".
         * However we expect invalid instead and QLineEdit to decline input.
         * Fix this by overloading the validate() operator.
         */
        const QValidator::State origState = QDoubleValidator::validate( input, pos );
        if( ( origState == QValidator::Intermediate ) && ( input.toDouble() > top() ) )
        {
            return QValidator::Invalid;
        }
        else
        {
            return origState;
        }
    }
};
#endif

} // namespace Helper

