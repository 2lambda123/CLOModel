#ifndef CommonFunctions_h__
#define CommonFunctions_h__

class QDate;
class QString;
#include <QList>
//! The current Version of the Model
#define ModelVersionNumber 176
#define MinimumSupportedVersion 173
#define MaximumIRRIterations INT_MAX-1
#define NumReserves 2
//#define SaveLoanTape
int MonthDiff(const QDate& FutureDte,const QDate& PresentDte);

/*
bool ValidAnnuityVector(const QString& AnnVect);
bool ValidBloombergVector(const QString& BloombergVector);
QList<double> UnpackVect(QString Vect, int PaymFreq=1, bool AdjustFreq=true);
QString ShiftBloombergVector(const QString& OriginalVect, int ByMonths);
QList<QString> UnpackAnnuityVect(QString Vect);*/
double RoundUp(double a);
template<class T> bool LessThanPoint(T* a,T* b){return (*a)<(*b);}
QString Commarize(double num,unsigned int precision=0);
double CalculateIRR(const QList<QDate>& Dte, const QList<double>& Flws, int Daycount=360, double Guess=0.05,int precision=3);
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, int Daycount=360);
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate,int Daycount=360, double Guess=0.05,int precision=3);
double CalculateIRRSimple(const QList<QDate>& Dte, const QList<double>& Flws, int Daycount=360, double Guess=0.05,int precision=3);
double CalculateNPVSimple(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, int Daycount=360);
double CalculateDMSimple(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate,int Daycount=360, double Guess=0.05,int precision=3);
bool removeDir(const QString & dirName);
#endif // CommonFunctions_h__