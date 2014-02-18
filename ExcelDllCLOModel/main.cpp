#include "CommonFunctions.h"
#include "Waterfall.h"
#include "ExcelOutput.h"
#include "StressTest.h"
#include "StandaloneStress.h"
#include <QApplication>
//#include <QFile>
int main(int argc, char *argv[])
{
	
	QApplication a(argc, argv);
	StandaloneStress TempObj;
	TempObj.show();
	return a.exec();
/*
	Mortgage TempLoan;
	TempLoan.SetAnnuity("Y");
	TempLoan.SetSize(1000000.0);
	TempLoan.SetInterest("5");
	TempLoan.SetMaturityDate(QDate(2014,6,1));
	TempLoan.SetPaymentFreq(3);
	TempLoan.CalculateCashFlows("0","0","0",QDate(2014,1,1));
	*/
	/*qint32 VesionCheck;
	Waterfall TempWtf;
	QFile file("C:/Temp/.BaseCase.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> VersionChecker;
	if(VersionChecker!=qint32(ModelVersionNumber)){
		file.close();
		return 1;
	}
	out >> TempWtf;
	//out >> TempCallWaterfall;
	TempWtf.GetCreditEnhancement(12);
	file.close();*/
}
