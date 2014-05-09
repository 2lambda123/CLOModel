#include "CentralUnit.h"
#include <QMetaType>
#include <QApplication>
#include <QTimer>
#include <QFile>
#include "WaterfallCalculator.h"
#include "CommonFunctions.h"
#include <QMessageBox>
#include "ProgressWidget.h"
#include "ExcelCommons.h"
#include "ExcelOutput.h"
CentralUnit::CentralUnit(QObject* parent)
	:QObject(parent)
	,Stresser(NULL)
	,MtgsProgress(NULL)
	,RunCall(false)
{
	for(int i=0;i<NumberOfPlots;i++) PlotIndexes[i]=0;
	if(!QMetaType::isRegistered(qMetaTypeId<Waterfall>()))
		qRegisterMetaType<Waterfall>("Waterfall");
	if(!QMetaType::isRegistered(qMetaTypeId<MtgCashFlow>()))
		qRegisterMetaType<MtgCashFlow>("MtgCashFlow");
	connect(&LoansCalculator,SIGNAL(Calculated()),this,SLOT(CalculationStep2()));
	connect(this,SIGNAL(LoopStarted()),this,SLOT(CalculationStep1()),Qt::QueuedConnection);
	ParallWatFalls=new WaterfallCalculator(this);
	connect(ParallWatFalls,SIGNAL(Calculated()),this,SLOT(CheckCalculationDone()));
}
void CentralUnit::SetPoolCutOff(const QDate& a){PoolCutOff=a; if(Stresser) Stresser->SetStartDate(PoolCutOff);}
void CentralUnit::SetFolderPath(const QString& a){FolderPath=a;}
void CentralUnit::AddLoan(const QDate& Maturity, double Size,  const QString& Interest, const QString& Annuity, int Freq,double floatBase,const QString& LossMult, const QString& PrepayMult, const QString& HaicutVect){
	Mortgage TempMtg;
	TempMtg.SetMaturityDate(Maturity);
	TempMtg.SetSize(Size);
	TempMtg.SetPrepayMultiplier(PrepayMult);
	TempMtg.SetLossMultiplier(LossMult);
	TempMtg.SetInterest(Interest);
	TempMtg.SetFloatingRateBase(floatBase);
	TempMtg.SetPaymentFreq(Freq);
	TempMtg.SetAnnuity(Annuity);
	TempMtg.SetHaircutVector(HaicutVect);
	LoansCalculator.AddLoan(TempMtg);
	if(Stresser)Stresser->AddLoan(TempMtg);
}
void CentralUnit::AddTranche(const QString& Name,int ProRataGroup, double MinOC, double MinIC, double Price,double FxRate,const QString& BbgExt, int DayCount){
	Tranche TempTrnch;
	TempTrnch.SetTrancheName(Name);
	TempTrnch.SetProrataGroup(ProRataGroup);
	TempTrnch.SetMinIClevel(MinIC);
	TempTrnch.SetMinOClevel(MinOC);
	TempTrnch.SetPrice(Price);
	TempTrnch.SetExchangeRate(FxRate);
	TempTrnch.SetBloombergExtension(BbgExt);
	TempTrnch.SetDayCount(DayCount);
	TempTrnch.GetDataFromBloomberg();
	Structure.AddTranche(TempTrnch);
	if(Stresser)Stresser->SetStructure(Structure);
}
void CentralUnit::AddTranche(
	const QString& Name
	,const QString& ISIN
	,int ProRataGroup
	,double OrigAmnt
	,const QString& Crncy
	,double OutstandingAmt
	,Tranche::TrancheInterestType IntrTyp
	,const QString& Coupon
	,const QString& RefRte
	,const QDate& LastPayDte
	,const QString& DefRefRte
	,const QString& PayFreq
	,const QDate& SettlementDate
	,double AccruedInterest
	,const QString& RefRteValue
	,double MinOC
	,double MinIC
	,double Price
	,double FxRate
	,const QString& BbgExt
	,int DayCount
){
	Tranche TempTrnch;
	TempTrnch.SetTrancheName(Name);
	TempTrnch.SetISIN(ISIN);
	TempTrnch.SetProrataGroup(ProRataGroup);
	TempTrnch.SetOriginalAmount(OrigAmnt);
	TempTrnch.SetCurrency(Crncy);
	TempTrnch.SetOutstandingAmt(OutstandingAmt);
	TempTrnch.SetInterestType(IntrTyp);
	TempTrnch.SetCoupon(Coupon);
	TempTrnch.SetReferenceRate(RefRte);
	TempTrnch.SetLastPaymentDate(LastPayDte);
	TempTrnch.SetDefaultRefRate(DefRefRte);
	TempTrnch.SetPaymentFrequency(PayFreq);
	TempTrnch.SetSettlementDate(SettlementDate);
	TempTrnch.SetAccruedInterest(AccruedInterest);
	TempTrnch.SetReferenceRateValue(RefRteValue);
	TempTrnch.SetMinIClevel(MinIC);
	TempTrnch.SetMinOClevel(MinOC);
	TempTrnch.SetPrice(Price);
	TempTrnch.SetExchangeRate(FxRate);
	TempTrnch.SetBloombergExtension(BbgExt);
	TempTrnch.SetDayCount(DayCount);
	Structure.AddTranche(TempTrnch);
	if(Stresser)Stresser->SetStructure(Structure);
}
void CentralUnit::AddWaterfallStep(WatFalPrior::WaterfallStepType Tpe,int GrpTgt, int RdmpGrp, double RdmpShare){
	WatFalPrior TempStep;
	TempStep.SetGroupTarget(GrpTgt);
	TempStep.SetRedemptionGroup(RdmpGrp);
	TempStep.SetRedemptionShare(RdmpShare);
	TempStep.SetPriorityType(Tpe);
	Structure.AddStep(TempStep);
	if(Stresser)Stresser->SetStructure(Structure);
}
void CentralUnit::Reset(){
	LoansCalculator.Reset();
	Structure.ResetMtgFlows();
	Structure.ResetSteps();
	Structure.ResetTranches();
	Structure.ResetReserve();
	if(Stresser){
		Stresser->deleteLater();
		Stresser=NULL;
	}
}
void CentralUnit::SetupStress(const QString& ConstPar,QList<QString> XSpann,QList<QString> YSpann, StressTest::StressVariability Xvar,StressTest::StressVariability Yvar){
	if(!Stresser){
		Stresser=new StressTest(this);
	}
	Stresser->SetConstantPar(ConstPar);
	Stresser->SetXSpann(XSpann);
	Stresser->SetYSpann(YSpann);
	Stresser->SetXVariability(Xvar);
	Stresser->SetYVariability(Yvar);
	Stresser->SetStartDate(PoolCutOff);
	Structure.SetUseCall(StressToCall);
	Stresser->SetStructure(Structure);
	for(int i=0;i<LoansCalculator.GetLoans().size();i++)
		Stresser->AddLoan(*LoansCalculator.GetLoans().at(i));
	connect(this,SIGNAL(StressLoopStarted()),Stresser,SLOT(RunStressTest()),Qt::QueuedConnection);
	connect(Stresser,SIGNAL(AllFinished()),this,SLOT(StressFinished()));
}
void CentralUnit::Calculate(){
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	emit LoopStarted();
	ComputationLoop.exec();
}
void CentralUnit::CalculateStress(){
	char *argv[] = {"NoArgumnets"};
	int argc = sizeof(argv) / sizeof(char*) - 1;
	QApplication ComputationLoop(argc,argv);
	emit StressLoopStarted();
	ComputationLoop.exec();
}
void CentralUnit::CalculationStep1(){
	QString TmpStr=LoansCalculator.ReadyToCalculate();
	if(!TmpStr.isEmpty()){
		QMessageBox::critical(0,"Invalid Input","The following Inputs are missing or invalid:\n"+TmpStr);
		return;
	}
	LoansCalculator.SetCPR(Structure.GetReinvestmentTest().GetCPRAssumption());
	LoansCalculator.SetCDR(Structure.GetReinvestmentTest().GetCDRAssumption());
	LoansCalculator.SetLS(Structure.GetReinvestmentTest().GetLSAssumption());
	LoansCalculator.SetStartDate(PoolCutOff);
	if(MtgsProgress) MtgsProgress->deleteLater();
	MtgsProgress=new ProgressWidget;
	MtgsProgress->SetValue(0);
	MtgsProgress->SetTitle("Calculating Loans");
	MtgsProgress->SetMax(LoansCalculator.Count());
	connect(&LoansCalculator,SIGNAL(BeeCalculated(int)),MtgsProgress,SLOT(SetValue(int)));
	MtgsProgress->show();
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	LoansCalculator.StartCalculation();
}
void CentralUnit::CalculationStep2(){
	MtgsProgress->SetValue(0);
	MtgsProgress->SetTitle("Calculating Tranches");
	MtgsProgress->SetMax(1);
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	Structure.ResetMtgFlows();
	Structure.AddMortgagesFlows(LoansCalculator.GetResult());
	Structure.SetUseCall(false);
	QString TmpStr=Structure.ReadyToCalculate();
	if(!TmpStr.isEmpty()){
		QMessageBox::critical(0,"Invalid Input","The following Inputs are missing or invalid:\n"+TmpStr);
		return;
	}
	if(!RunCall){
		CallStructure.ResetMtgFlows();
		CallStructure.ResetTranches();
		CallStructure.ResetSteps();
		Structure.CalculateTranchesCashFlows();
		CheckCalculationDone();
	}
	else{
		ParallWatFalls->ResetWaterfalls();
		ParallWatFalls->AddWaterfall(Structure);
		CallStructure.ResetMtgFlows();
		CallStructure=Structure;
		CallStructure.SetUseCall(true);
		TmpStr=CallStructure.ReadyToCalculate();
		if(!TmpStr.isEmpty()){
			QMessageBox::critical(0,"Invalid Input","The following Inputs are missing or invalid:\n"+TmpStr);
			return;
		}
		ParallWatFalls->AddWaterfall(CallStructure);
		ParallWatFalls->StartCalculation();
	}

}
void CentralUnit::CheckCalculationDone()
{
	MtgsProgress->SetValue(1);
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	if(MtgsProgress) MtgsProgress->deleteLater();
	MtgsProgress=NULL;
	Tranche TempTranche;
	if(RunCall){
		Structure=*(ParallWatFalls->GetWaterfalls().at(0));
		CallStructure=*(ParallWatFalls->GetWaterfalls().at(1));
	}
	#ifdef SaveLoanTape
	{
		QFile file(FolderPath+"\\.Loans.clp");
		if (file.open(QIODevice::WriteOnly)) {
			QDataStream out(&file);
			out.setVersion(QDataStream::Qt_4_8);
			out << qint32(ModelVersionNumber) << LoansCalculator;
			file.close();
			#ifdef Q_WS_WIN
				SetFileAttributes((FolderPath+"\\.Loans.clp").toStdWString().c_str(),FILE_ATTRIBUTE_HIDDEN);
			#endif
		}
	}
	#endif
	QString Filename=FolderPath+"\\.BaseCase.clo";
	QFile file(Filename);
	if (file.open(QIODevice::WriteOnly)) {
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_4_8);
		out << qint32(ModelVersionNumber) << Structure << CallStructure;
		file.close();
	}	
	if(!MtgOutputAddress.isEmpty()){
		ExcelOutput::PrintMortgagesRepLines(Structure.GetCalculatedMtgPayments(),ExcelCommons::CellOffset(MtgOutputAddress));
	}
	if(!PlotsSheet.isEmpty() && PlotIndexes[0]>0){
		ExcelOutput::PlotMortgagesFlows(Structure.GetCalculatedMtgPayments(),PlotsSheet,PlotIndexes[0]);
	}
	if(!TranchesOutputAddress.isEmpty()){
		int ClolumnCount=0;
		QList<double> TempValList;
		ExcelOutput::PrintMergedCell("Scenario To Maturity",ExcelCommons::CellOffset(TranchesOutputAddress,0,0),1,6+(6*Structure.GetTranchesCount()),QColor(118,147,60));
		for(int i=0;i<Structure.GetTranchesCount();i++)
			ExcelOutput::PrintTrancheFlow(*Structure.GetTranche(i),ExcelCommons::CellOffset(TranchesOutputAddress,1,(i>0 ? 1:0) +(6*i)),i%2==0 ? QColor(235,241,222) : QColor(216,228,188),i==0);
		TempTranche.SetTrancheName("Senior Expenses");
		TempTranche.GetCashFlow().ResetFlows();
		TempTranche.AddCashFlow(Structure.GetTotalSeniorExpenses());
		ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,(++ClolumnCount)+(6*Structure.GetTranchesCount())),Structure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		TempTranche.SetTrancheName("Senior Fees");
		TempTranche.GetCashFlow().ResetFlows();
		TempTranche.AddCashFlow(Structure.GetTotalSeniorFees());
		ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,(++ClolumnCount)+(6*Structure.GetTranchesCount())),Structure.GetTranchesCount()%2!=0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		TempTranche.SetTrancheName("Junior Fees");
		TempTranche.GetCashFlow().ResetFlows();
		TempTranche.AddCashFlow(Structure.GetTotalJuniorFees());
		ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,(++ClolumnCount)+(6*Structure.GetTranchesCount())),Structure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		for(int ResIter=0;ResIter<NumReserves;ResIter++){
			TempTranche.SetTrancheName(QString("Reserve Fund %1").arg(ResIter+1));
			TempTranche.GetCashFlow().ResetFlows();
			TempTranche.AddCashFlow(Structure.GetReserveFundFlow(ResIter));
			if(TempTranche.GetCashFlow().Count()>0)
				ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,1,(++ClolumnCount)+(6*Structure.GetTranchesCount())),(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
		}
		if(Structure.GetExcessCashFlow().Count()>0){
			TempValList.clear(); for(int i=0;i<Structure.GetExcessCashFlow().Count();i++) TempValList.append(Structure.GetExcessCashFlow().GetTotalFlow(i));
			ExcelOutput::PrintColumn("Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,2,(++ClolumnCount)+(6*Structure.GetTranchesCount())),"_-* #,##0_-;-* #,##0_-;_-* \" - \"??_-;_-@_-",(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
		}
		TempValList.clear(); for(int i=0;i<Structure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(Structure.GetAnnualizedExcess(i));
		ExcelOutput::PrintColumn("Annualized Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,2,(++ClolumnCount)+(6*Structure.GetTranchesCount())),"0.00%",(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
		TempValList.clear(); for(int i=0;i<Structure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(Structure.GetWACostOfCapital(i));
		ExcelOutput::PrintColumn("WA Cost of Funding",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,2,(++ClolumnCount)+(6*Structure.GetTranchesCount())),"0.00%",(Structure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
		TempValList.clear(); for(int i=0;i<Structure.GetTranchesCount();i++) TempValList.append(Structure.GetTranche(i)->GetLossRate());
		ExcelOutput::PrintDataColumn(TempValList,ExcelCommons::CellOffset(LossOutputAddress),"0.00%");
		TempValList.clear(); for(int i=0;i<Structure.GetTranchesCount();i++) TempValList.append(Structure.GetCreditEnhancement(i));
		ExcelOutput::PrintDataColumn(TempValList,ExcelCommons::CellOffset(CreditEnanAddress),"0.00%");
		ExcelOutput::PrintMergedCell("Scenario To Maturity",ExcelCommons::CellOffset(TranchesOutputAddress,0,0),1,(++ClolumnCount)+(6*Structure.GetTranchesCount()),QColor(118,147,60));

		if(RunCall){
			ClolumnCount=0;
			ExcelOutput::PrintMergedCell("Scenario To Call",ExcelCommons::CellOffset(TranchesOutputAddress,3+Structure.GetTranche(0)->GetCashFlow().Count(),0),1,6+(6*Structure.GetTranchesCount()),QColor(118,147,60));
			for(int i=0;i<CallStructure.GetTranchesCount();i++)
				ExcelOutput::PrintTrancheFlow(*CallStructure.GetTranche(i),ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(i)->GetCashFlow().Count(),(i>0 ? 1:0) +(6*i)),i%2==0 ? QColor(235,241,222) : QColor(216,228,188),i==0);
			TempTranche.SetTrancheName("Senior Expenses");
			TempTranche.GetCashFlow().ResetFlows();
			TempTranche.AddCashFlow(CallStructure.GetTotalSeniorExpenses());
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),CallStructure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			TempTranche.SetTrancheName("Senior Fees");
			TempTranche.GetCashFlow().ResetFlows();
			TempTranche.AddCashFlow(CallStructure.GetTotalSeniorFees());
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),CallStructure.GetTranchesCount()%2!=0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			TempTranche.SetTrancheName("Junior Fees");
			TempTranche.GetCashFlow().ResetFlows();
			TempTranche.AddCashFlow(CallStructure.GetTotalJuniorFees());
			ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),CallStructure.GetTranchesCount()%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			for(int ResIter=0;ResIter<NumReserves;ResIter++){
				TempTranche.SetTrancheName(QString("Reserve Fund").arg(ResIter+1));
				TempTranche.GetCashFlow().ResetFlows();
				TempTranche.AddCashFlow(CallStructure.GetReserveFundFlow(ResIter));
				if(TempTranche.GetCashFlow().Count()>0)
					ExcelOutput::PrintTrancheFlow(TempTranche,ExcelCommons::CellOffset(TranchesOutputAddress,4+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*Structure.GetTranchesCount())),(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188),false,false,false,false,true,false,false,false);
			}
			if(CallStructure.GetExcessCashFlow().Count()>0){
				TempValList.clear(); for(int i=0;i<CallStructure.GetExcessCashFlow().Count();i++) TempValList.append(CallStructure.GetExcessCashFlow().GetTotalFlow(i));
				ExcelOutput::PrintColumn("Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,5+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),"_-* #,##0_-;-* #,##0_-;_-* \" - \"??_-;_-@_-",(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
			}
			TempValList.clear(); for(int i=0;i<CallStructure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(CallStructure.GetAnnualizedExcess(i));
			ExcelOutput::PrintColumn("Annualized Excess Spread",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,5+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),"0.00%",(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
			TempValList.clear(); for(int i=0;i<CallStructure.GetTranche(0)->GetCashFlow().Count();i++) TempValList.append(CallStructure.GetWACostOfCapital(i));
			ExcelOutput::PrintColumn("WA Cost of Funding",TempValList,ExcelCommons::CellOffset(TranchesOutputAddress,5+Structure.GetTranche(0)->GetCashFlow().Count(),(++ClolumnCount)+(6*CallStructure.GetTranchesCount())),"0.00%",(CallStructure.GetTranchesCount()+ClolumnCount)%2==0 ? QColor(235,241,222) : QColor(216,228,188));
			TempValList.clear(); for(int i=0;i<CallStructure.GetTranchesCount();i++) TempValList.append(CallStructure.GetTranche(i)->GetLossRate());
			ExcelOutput::PrintDataColumn(TempValList,ExcelCommons::CellOffset(LossOnCallOutputAddress),"0.00%");
			ExcelOutput::PrintMergedCell("Scenario To Call",ExcelCommons::CellOffset(TranchesOutputAddress,3+Structure.GetTranche(0)->GetCashFlow().Count(),0),1,(++ClolumnCount)+(6*Structure.GetTranchesCount()),QColor(118,147,60));
		}
	}
	if(!PlotsSheet.isEmpty() && PlotIndexes[1]>0)
		ExcelOutput::PlotTranchesDynamic(Structure,PlotsSheet,PlotIndexes[1],RunCall ? CallStructure.GetCalledPeriod():QDate());
	if(!PlotsSheet.isEmpty() && PlotIndexes[2]>0)
		ExcelOutput::PlotOCTest(Structure,PlotsSheet,PlotIndexes[2],RunCall ? CallStructure.GetCalledPeriod():QDate());
	if(!PlotsSheet.isEmpty() && PlotIndexes[3]>0)
		ExcelOutput::PlotICTest(Structure,PlotsSheet,PlotIndexes[3],RunCall ? CallStructure.GetCalledPeriod():QDate());
	if(!PlotsSheet.isEmpty() && PlotIndexes[4]>0)
		ExcelOutput::PlotAnnualExcess(Structure,PlotsSheet,PlotIndexes[4],RunCall ? CallStructure.GetCalledPeriod():QDate());
	if(!PlotsSheet.isEmpty() && PlotIndexes[5]>0)
		ExcelOutput::PlotCostFunding(Structure,PlotsSheet,PlotIndexes[5],RunCall ? CallStructure.GetCalledPeriod():QDate());
	if(!PlotsSheet.isEmpty() && PlotIndexes[6]>0)
		ExcelOutput::PlotCPRLS(Structure,PlotsSheet,PlotIndexes[6]);
	if(!PlotsSheet.isEmpty() && PlotIndexes[7]>0)
		ExcelOutput::PlotEquityReturn(Structure,PlotsSheet,PlotIndexes[7]);
	if(!PlotsSheet.isEmpty() && PlotIndexes[8]>0)
		ExcelOutput::PlotCallToEquity(Structure,PlotsSheet,PlotIndexes[8],RunCall ? CallStructure.GetCalledPeriod():QDate());
	QApplication::quit();
}
void CentralUnit::StressFinished(){
	Stresser->SaveResults(FolderPath);
	QApplication::quit();
}
CentralUnit::~CentralUnit(){
	if(MtgsProgress) MtgsProgress->deleteLater();
}
void CentralUnit::CompileBaseRates(const QHash<QString,double>& Values){
	for(int i=0;i<Structure.GetTranchesCount();i++){
		Structure.GetTranche(i)->CompileReferenceRateValue(Values);
	}
}

#ifdef _DEBUG
QDataStream& operator<<(QDataStream & stream, const CentralUnit& flows){
	stream 
		<< flows.PoolCutOff
		<< flows.RunCall
		<< flows.RunStress
		<< flows.StressToCall
		<< flows.MtgOutputAddress
		<< flows.TranchesOutputAddress
		<< flows.CallTranchesOutputAddress
		<< flows.LossOutputAddress
		<< flows.LossOnCallOutputAddress
		<< flows.CreditEnanAddress
		<< flows.FolderPath
		<< flows.PlotsSheet
		<< flows.Structure
		<< flows.CallStructure
	;
	for(int i=0;i<NumberOfPlots;i++){
		stream << flows.PlotIndexes[i];
	}
	stream << flows.LoansCalculator.Count();
	for(int i=0;i<flows.LoansCalculator.Count();i++){
		stream << *(flows.LoansCalculator.GetLoans().at(i));
	}
	return stream;
}
QDataStream& operator>>(QDataStream & stream, CentralUnit& flows){
	stream 
		>> flows.PoolCutOff
		>> flows.RunCall
		>> flows.RunStress
		>> flows.StressToCall
		>> flows.MtgOutputAddress
		>> flows.TranchesOutputAddress
		>> flows.CallTranchesOutputAddress
		>> flows.LossOutputAddress
		>> flows.LossOnCallOutputAddress
		>> flows.CreditEnanAddress
		>> flows.FolderPath
		>> flows.PlotsSheet
		>> flows.Structure
		>> flows.CallStructure
		;
	for(int i=0;i<NumberOfPlots;i++){
		stream >> flows.PlotIndexes[i];
	}
	Mortgage TempMtg;
	int CountMtgs;
	stream >> CountMtgs;
	for(int i=0;i<CountMtgs;i++){
		stream >> TempMtg;
		flows.LoansCalculator.AddLoan(TempMtg);
		if(flows.Stresser)flows.Stresser->AddLoan(TempMtg);
	}
	return stream;
}
#endif