#include <QApplication>
#include "BloombergVector.h"
#include "ConsoleTestObj.h"
#include <QtDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QClipboard>
#include "MtgCalculator.h"
#include "CommonFunctions.h"
#include "Waterfall.h"
#include "StressTest.h"
#include "ExcelOutput.h"
#include "BloombergVector.h"
#include "BaseRateVect.h"
#include "BaseRateTable.h"
#include "QBbgRequest.h"
#include "QBbgWorker.h"
#include <QTime>
#include <QTextStream>
#include "DateTrigger.h"
#include "PoolSizeTrigger.h"
#include "StressViewer.h"
#include "SummaryView.h"
#include "WaterfallStepHelperDialog.h"
#include "TriggerStructHelperWidget.h"
#include "TriggerHelperDialog.h"
#include "FloorCapVector.h"
#include "LoanAssumptionsEditor.h"
#include "CheckAndEdit.h"
int main(int argc, char *argv[]) {

	QList<QDate> FlowDates;
	FlowDates 
		<< QDate(2013, 6, 1)
// 		<< QDate(2015, 9, 1)
// 		<< QDate(2015, 12, 1)
// 		<< QDate(2016, 3, 1)
// 		<< QDate(2016, 6, 1)
// 		<< QDate(2016, 9, 1)
// 		<< QDate(2016, 12, 1)
// 		<< QDate(2017, 3, 1)
// 		<< QDate(2017, 6, 1)
// 		<< QDate(2017, 9, 1)
// 		<< QDate(2017, 12, 1)
// 		<< QDate(2018, 3, 1)
// 		<< QDate(2018, 6, 1)
// 		<< QDate(2018, 9, 1)
// 		<< QDate(2018, 12, 1)
// 		<< QDate(2019, 3, 1)
// 		<< QDate(2019, 6, 1)
// 		<< QDate(2019, 9, 1)
// 		<< QDate(2019, 12, 1)
// 		<< QDate(2020, 3, 1)
		<< QDate(2015, 6, 1)
	;
	QList<double> Flows;
	Flows << -100.00000
// 		<< 1.27778
// 		<< 1.26389
// 		<< 1.26389
// 		<< 1.27778
// 		<< 1.27778
// 		<< 1.26389
// 		<< 1.25000
// 		<< 1.27778
// 		<< 1.27778
// 		<< 1.26389
// 		<< 1.25000
// 		<< 1.27778
// 		<< 1.27778
// 		<< 1.26389
// 		<< 1.25000
// 		<< 1.27778
// 		<< 1.27778
// 		<< 1.26389
// 		<< 1.26389
		<< 110.1389
	;
	qDebug() << CalculateNPV(FlowDates, Flows, "A 06/01/2013 5 12S 6" , DayCountVector("106"));
	qDebug() << CalculateNPV(FlowDates, Flows, "A 06/01/2013 5 11S 6", DayCountVector("106"));
	qDebug() << CalculateNPV(FlowDates, Flows, 0.06, DayCountVector("106"));

	//QApplication a(argc, argv);
	//ConsoleTestObj b;
	//return a.exec();


	/*QApplication a(argc, argv);
	LoanAssumptionsEditor b;
	b.FillFromQuery();
	b.show();
	return a.exec();*/

	/*QApplication a(argc, argv);
	StressViewer b;
	b.show();
	b.LoadStress("C:/Temp/StressResult.fcsr");
	return a.exec();*/

	/*Waterfall TempWtf, TempCallWaterfall;
	QFile file("Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/Converted Models/BaseCase.clo");
	//QFile file("C:/Temp/.SavedInputs.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		file.close();
		return 1;
	}
	{bool Junk; out >> Junk; }
	TempWtf.SetLoadProtocolVersion(VersionChecker);
	out >> TempWtf;
	TempCallWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempCallWaterfall;
	file.close();
	if (TempWtf.CalculateTranchesCashFlows()) qDebug() << "Calculation Finished Correctly";
	else qDebug() << "Critical Error in Waterfall Calculation";*/

	/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QApplication a(argc, argv);
	RegisterAsMetaType<Waterfall>();
	RegisterAsMetaType<MtgCashFlow>();

	Tranche TeResult = *( StressTest::GetScenarioFromFile(
		"Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/Converted Models/StressResult.fcsr"
		, "20", "0.5", "0", "0", "0", "0"
		).GetTranche("HARVT 10X A"));
	return a.exec();
	*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Waterfall TempWtf, TempCallWaterfall;
	MtgCalculator TempMtg;
	QFile file("Z:/24AM/Monitoring/Model Results/HARVT 10X.clom");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
		file.close();
		return 1;
	}
	{QDate Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	{bool Junk; out >> Junk; }
	TempWtf.SetLoadProtocolVersion(VersionChecker);
	out >> TempWtf;
	TempCallWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempCallWaterfall;
	TempMtg.SetLoadProtocolVersion(VersionChecker);
	out >> TempMtg;
	file.close();
	
	TempWtf.CalculateTranchesCashFlows();
// 	QApplication a(argc, argv);
// 	TempMtg.DownloadScenarios();
// 	TempMtg.SetSequentialComputation(true);
// 	TempMtg.StartCalculation();
// 	return a.exec();
	/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	QApplication a(argc, argv);
	//QFile file("C:/Temp/Wrong 20#,#8#,#100#,#0#,#0#,#0.csw");
	QFile file("C:/Temp/20#,#8#,#100#,#0#,#0#,#0.csw");
	//QFile file("Z:/24AM/Personal Folders/LB/CLO 2.0/Analytics/CLO Model/Converted Models/BaseCase.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_3);
	out >> VersionChecker;
	if (VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) {
	file.close();
	return 1;
	}
	//{bool Junk; out >> Junk; }
	Waterfall TempWtf;
	Tranche TempTr;
	TempWtf.AddTranche(TempTr);
// 	TempWtf.SetLoadProtocolVersion(VersionChecker);
// 	out >> TempWtf;
// 	file.close();
	//TempWtf.CalculateTranchesCashFlows();
	SummaryView Check;
	Check.show();
	Check.SetStructure(TempWtf);
	return a.exec();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

/*Mortgage TempMtg1;
	TempMtg1.SetAnnuity("I");
	TempMtg1.SetInterest("4.25");
	TempMtg1.SetFloatingRateBase("EUR003M");
	TempMtg1.SetPaymentFreq("3");
	TempMtg1.SetMaturityDate(QDate(2020, 11, 15));
	TempMtg1.SetSize(2030713.0);
	TempMtg1.SetProperty("Scenario", "AVR");
	TempMtg1.SetProperty("Mezzanine", "No");
	TempMtg1.CalculateCashFlows(QDate(2014, 9, 19), "10", "2", "50");
	PrintToTempFile("DownloadResults", TempMtg1.GetCashFlow().ToPlainText());
	


	Mortgage TempMtg2;
	TempMtg2.SetAnnuity("I");
	TempMtg2.SetInterest("4.52");
	TempMtg2.SetFloatingRateBase("EUR003M");
	TempMtg2.SetPaymentFreq("3");
	TempMtg2.SetMaturityDate(QDate(2021, 5, 30));
	TempMtg2.SetSize(3687803.0);
	TempMtg2.CalculateCashFlows(QDate(2014, 9, 19), "0", "0", "0");

	MtgCashFlow BaseFlows = TempMtg1.GetCashFlow();
	BaseFlows += TempMtg2.GetCashFlow();


	MtgCashFlow LegacyFlows = BaseFlows.ApplyScenario("10", "0", "50");
	TempMtg1.CalculateCashFlows(QDate(2014, 9, 19), "10", "0", "50");
	TempMtg2.CalculateCashFlows(QDate(2014, 9, 19), "10", "0", "50");

	MtgCashFlow TempMtgsFlows = TempMtg1.GetCashFlow();
	TempMtgsFlows += TempMtg2.GetCashFlow();
	
	QFile file1("C:/Temp/Scenario.log");
	QFile file2("C:/Temp/Model.log");
	QFile file3("C:/Temp/Base.log");
	if (file1.exists()) file1.remove();
	if (file2.exists()) file2.remove();
	if (file3.exists()) file3.remove();
	if (/ *true  ||* / TempMtgsFlows != LegacyFlows) {
		PrintToTempFile("Model", TempMtgsFlows.ToPlainText(), false);
		PrintToTempFile("Scenario", LegacyFlows.ToPlainText(), false);
		PrintToTempFile("Base", BaseFlows.ToPlainText(), false);
	}
	else {
		qDebug() << "Success, Cash Flows are identical";
	}*/
	return 0;

	
}
