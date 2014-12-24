#include "MtgCalculator.h"
#include "Mortgage.h"
#include <QDir>
#include "Waterfall.h"
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#endif
#include "LoanAssumption.h"
#include <QCache>
MtgCalculator::MtgCalculator(QObject* parent)
	:TemplAsyncCalculator <MtgCalculatorThread, MtgCashFlow>(parent)
	,m_UseStoredCashFlows(true)
	, m_DownloadScenario(false)
{}
void MtgCalculator::AddLoan(const Mortgage& a, qint32 Index) {
	auto FoundLn = Loans.find(Index);
	if (FoundLn != Loans.end()) {
		delete FoundLn.value();
		Loans.erase(FoundLn);
	}
	Loans.insert(Index, new Mortgage(a));
}
bool MtgCalculator::StartCalculation() {
	if (m_ContinueCalculation) return false;
	if (!ReadyToCalculate().isEmpty()) return false;
	{//Check if all base rates are valid
		bool CheckAgain = false;
		ConstantBaseRateTable TempTable;
		for (auto i = Loans.constBegin(); i != Loans.constEnd(); ++i) {
			if (i.value()->GetFloatingRateValue().IsEmpty()) {
				i.value()->CompileReferenceRateValue(TempTable);
				CheckAgain = true;
			}
		}
		for (auto i = Loans.constBegin(); CheckAgain && i != Loans.constEnd(); ++i) {
			if (i.value()->GetFloatingRateValue().IsEmpty()) return false;
		}
	}

	/*if (m_UseStoredCashFlows){//Check if CF are available
		QString DealName,TrancheName;
		bool CashFound=false;
#ifndef NO_DATABASE
		bool DBAvailable;
		ConfigIni.beginGroup("Database");
		QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
		if (!db.isValid()) {
			db = QSqlDatabase::addDatabase(ConfigIni.value("DBtype", "QODBC").toString(), "TwentyFourDB");
			db.setDatabaseName(
				"Driver={" + ConfigIni.value("Driver", "SQL Server").toString()
				+ "}; "
				+ ConfigIni.value("DataSource", "Server=SYNSERVER2\\SQLExpress;Initial Catalog=ABSDB;Integrated Security=SSPI;Trusted_Connection=Yes;").toString()
				);
		}
		DBAvailable = db.open();
		ConfigIni.endGroup();
#endif
		MtgCashFlow ResultingFlow;
		ConfigIni.beginGroup("Folders");
		QDir CLOMsFolder(ConfigIni.value("UnifiedResultsFolder", "Z:/24AM/Monitoring/Model Results").toString());
		ConfigIni.endGroup();
		for (auto i = Loans.constBegin(); i != Loans.constEnd(); ++i) {
			ResultingFlow.Clear();
			if (CLOMsFolder.exists()) {
				DealName = i.value()->GetProperty("Issuer");
				TrancheName = i.value()->GetProperty("Facility");
				if (DealName.isEmpty()) DealName = TrancheName;
				if (DealName.isEmpty()) continue;
				if (CLOMsFolder.exists(DealName + ".clom")) {
					QFile SingleClom(CLOMsFolder.absoluteFilePath(DealName + ".clom"));
					if (SingleClom.open(QIODevice::ReadOnly)) {
						QDataStream out(&SingleClom);
						qint32 TempInt;
						out.setVersion(QDataStream::Qt_5_3);
						out >> TempInt;
						if (TempInt >= MinimumSupportedVersion && TempInt <= ModelVersionNumber) {
								{QDate Junk;  out >> Junk; } //drop LiborUpdateDate
							bool BaseToCall;
							out >> BaseToCall; //drop UseForwardCurve
							out >> BaseToCall;
							Waterfall TargetDeal;
							out >> TargetDeal;
							if (BaseToCall) out >> TargetDeal;
							const Tranche* TargetTranche = TargetDeal.GetTranche(TrancheName);
							TrancheCashFlow ScaledFlows = TargetTranche->GetCashFlow().ScaledCashFlows(i.value()->GetSize());
							for (int j = 0; j < ScaledFlows.Count(); ++j) {
								ResultingFlow.AddFlow(ScaledFlows.GetDate(j), ScaledFlows.GetInterest(j), MtgCashFlow::MtgFlowType::InterestFlow);
								ResultingFlow.AddFlow(ScaledFlows.GetDate(j), ScaledFlows.GetPrincipal(j), MtgCashFlow::MtgFlowType::PrincipalFlow);
								double TempOut = ScaledFlows.GetAmountOutstanding(j);
								ResultingFlow.AddFlow(ScaledFlows.GetDate(j), TempOut, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
								ResultingFlow.AddFlow(ScaledFlows.GetDate(j), TempOut*TargetTranche->GetTotalCoupon(TargetTranche->GetCashFlow().GetDate(j)), MtgCashFlow::MtgFlowType::WACouponFlow);
							}
							CashFound = true;
						}
						SingleClom.close();
					}
				}
			}
			#ifndef NO_DATABASE
			if (!CashFound && DBAvailable) {
				ConfigIni.beginGroup("Database");
				QSqlQuery query(db);
				query.prepare("exec " + ConfigIni.value("CashFlowsStoredProc", "getCashFlows").toString() + " ?");
				ConfigIni.endGroup();
				query.setForwardOnly(true);
				query.bindValue(0, TrancheName);
				bool FirstTime = true;
				if (query.exec()) {
					TrancheCashFlow ScaledFlows;
					while (query.next()) {
						if (FirstTime) {
							ScaledFlows.SetInitialOutstanding(query.value(5).toDouble() + query.value(4).toDouble());
							ScaledFlows.SetStartingDeferredInterest(0.0);
							FirstTime = false;
						}
						ScaledFlows.AddFlow(query.value(0).toDate(), query.value(3).toDouble(), TrancheCashFlow::TrancheFlowType::InterestFlow);
						ScaledFlows.AddFlow(query.value(0).toDate(), query.value(4).toDouble(), TrancheCashFlow::TrancheFlowType::PrincipalFlow);
						ScaledFlows.AddFlow(query.value(0).toDate(), query.value(1).toDouble(), static_cast<qint32>(MtgCashFlow::MtgFlowType::WACouponFlow));
					}
					if (!FirstTime) {
						ScaledFlows = ScaledFlows.ScaledCashFlows(i.value()->GetSize());
						for (int j = 0; j < ScaledFlows.Count(); ++j) {
							ResultingFlow.AddFlow(ScaledFlows.GetDate(j), ScaledFlows.GetInterest(j), MtgCashFlow::MtgFlowType::InterestFlow);
							ResultingFlow.AddFlow(ScaledFlows.GetDate(j), ScaledFlows.GetPrincipal(j), MtgCashFlow::MtgFlowType::PrincipalFlow);
							double TempOut = ScaledFlows.GetAmountOutstanding(j);
							ResultingFlow.AddFlow(ScaledFlows.GetDate(j), TempOut, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
							ResultingFlow.AddFlow(ScaledFlows.GetDate(j), TempOut*ScaledFlows.GetFlow(j, static_cast<qint32>(MtgCashFlow::MtgFlowType::WACouponFlow)), MtgCashFlow::MtgFlowType::WACouponFlow);
						}
					}
				}
				CashFound = !FirstTime;	
			}
			#endif
			if (CashFound) {
				BeesSent.append(i.key());
				BeeReturned(i.key(), ResultingFlow);
			}
		}
		#ifndef NO_DATABASE
		if (DBAvailable) db.close();
		#endif
	}*/


	BeesReturned = 0;
	BeesSent.clear();
	m_ContinueCalculation = true;
	int NumberOfThreads = QThread::idealThreadCount();
	if (m_SequentialComputation || NumberOfThreads < 1) NumberOfThreads = 1;
	int NumofSent = 0;
	MtgCalculatorThread* CurrentThread;
	for (auto SingleLoan = Loans.constBegin(); SingleLoan != Loans.constEnd(); ++SingleLoan) {
		if (NumofSent >= NumberOfThreads) break;
		if (BeesSent.contains(SingleLoan.key())) continue;
		CurrentThread = AddThread(SingleLoan.key());
		CurrentThread->SetLoan(*(SingleLoan.value()));
		CurrentThread->SetCPR(m_CPRass);
		CurrentThread->SetCDR(m_CDRass);
		CurrentThread->SetLS(m_LSass);
		CurrentThread->SetRecoveryLag(m_RecoveryLag);
		CurrentThread->SetDelinquency(m_Delinquency);
		CurrentThread->SetDelinquencyLag(m_DelinquencyLag);
		CurrentThread->SetOverrideAssumptions(m_OverrideAssumptions);
		CurrentThread->SetStartDate(StartDate);
		CurrentThread->SetDownloadScenario(m_DownloadScenario);
		CurrentThread->start();
		++NumofSent;
	}
	return true;
}
void MtgCalculator::BeeReturned(int Ident, const MtgCashFlow& a) {
	m_AggregatedRes+=a;
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::BeeReturned(Ident, a);
	if (!m_ContinueCalculation)return;
	MtgCalculatorThread* CurrentThread;
	for (auto SingleLoan = Loans.constBegin(); SingleLoan != Loans.constEnd(); ++SingleLoan) {
		if (BeesSent.contains(SingleLoan.key())) continue;
		CurrentThread = AddThread(SingleLoan.key());
		CurrentThread->SetLoan(*(SingleLoan.value()));
		CurrentThread->SetCPR(m_CPRass);
		CurrentThread->SetCDR(m_CDRass);
		CurrentThread->SetLS(m_LSass);
		CurrentThread->SetRecoveryLag(m_RecoveryLag);
		CurrentThread->SetDelinquency(m_Delinquency);
		CurrentThread->SetDelinquencyLag(m_DelinquencyLag);
		CurrentThread->SetOverrideAssumptions(m_OverrideAssumptions);
		CurrentThread->SetStartDate(StartDate);
		CurrentThread->SetDownloadScenario(m_DownloadScenario);
		CurrentThread->start();
		return;
	}
}

void MtgCalculator::ClearLoans() {
	for (auto i = Loans.begin(); i != Loans.end(); i++) {
		delete i.value();
	}
	Loans.clear();
}
void MtgCalculator::Reset(){
	ClearLoans();
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::Reset();
}
QString MtgCalculator::ReadyToCalculate()const{
	QString Result;
	QString TempStr;
	if (StartDate.isNull()) Result += "Invalid Start Date\n";
	for (auto i=Loans.constBegin();i!=Loans.constEnd();i++){
		TempStr = i.value()->ReadyToCalculate();
		if(!TempStr.isEmpty()) Result+=TempStr+'\n';
	}
	if (BloombergVector(m_CPRass).IsEmpty(0.0,1.0)) Result += "CPR Vector: " + m_CPRass + '\n';
	if (BloombergVector(m_CDRass).IsEmpty(0.0, 1.0)) Result += "CDR Vector: " + m_CDRass + '\n';
	if (BloombergVector(m_LSass).IsEmpty()) Result += "LS Vector: " + m_LSass + '\n';
	if (IntegerVector(m_RecoveryLag).IsEmpty()) Result += "Recovery Lag Vector: " + m_RecoveryLag + '\n';
	if (BloombergVector(m_Delinquency).IsEmpty(0, 1.0)) Result += "Delinquency Vector Vector: " + m_Delinquency + '\n';
	if (IntegerVector(m_DelinquencyLag).IsEmpty()) Result += "Delinquency Lag Vector: " + m_DelinquencyLag + '\n';
	if(!Result.isEmpty()) return Result.left(Result.size()-1);
	return Result;
}

QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows) {
	stream
		<< static_cast<qint32>(flows.Loans.size())
		<< flows.m_UseStoredCashFlows
		<< flows.m_CPRass
		<< flows.m_CDRass
		<< flows.m_LSass
		<< flows.m_RecoveryLag
		<< flows.m_Delinquency
		<< flows.m_DelinquencyLag
		<< flows.StartDate
		;
	for (auto i = flows.Loans.begin(); i != flows.Loans.end(); i++) {
		stream << i.key() << *(i.value());
	}
	return flows.SaveToStream(stream);
}
QDataStream& MtgCalculator::LoadOldVersion(QDataStream& stream) {
	Reset();
	qint32 tempInt, TempKey;
	stream >> tempInt;
	stream >> m_UseStoredCashFlows;
	stream >> m_CPRass;
	stream >> m_CDRass;
	stream >> m_LSass;
	stream >> m_RecoveryLag;
	stream >> m_Delinquency;
	stream >> m_DelinquencyLag;
	stream >> StartDate;
	Mortgage TmpMtg;
	for (qint32 i = 0; i < tempInt; i++) {
		stream >> TempKey;
		TmpMtg.SetLoadProtocolVersion(m_LoadProtocolVersion);
		stream >> TmpMtg;
		AddLoan(TmpMtg, TempKey);
	}
	return TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::LoadOldVersion(stream);
}

void MtgCalculator::CompileReferenceRateValue(ForwardBaseRateTable& Values) {
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->CompileReferenceRateValue(Values);
}
void MtgCalculator::CompileReferenceRateValue(ConstantBaseRateTable& Values) {
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->CompileReferenceRateValue(Values);
}
#ifndef NO_DATABASE
void MtgCalculator::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll) {
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->GetBaseRatesDatabase(Values, DownloadAll);
}
void MtgCalculator::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll) {
	for (auto i = Loans.begin(); i != Loans.end(); i++)
		i.value()->GetBaseRatesDatabase(Values, DownloadAll);
}
#endif
QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows) {
	return flows.LoadOldVersion(stream);
}
void MtgCalculator::SetLoans(const QHash<qint32, Mortgage*>& a) {
	ClearLoans();
	for (auto i = a.constBegin(); i != a.constEnd(); ++i) {
		Loans.insert(i.key(), new Mortgage(*(i.value())));
	}
}
void MtgCalculator::ClearResults() {
	m_AggregatedRes.Clear();
	TemplAsyncCalculator<MtgCalculatorThread, MtgCashFlow>::ClearResults();
}

void MtgCalculator::DownloadScenarios() {
	QCache<QString, LoanAssumption> AssumptionCache;
	foreach(Mortgage* i,Loans){
		if (i->HasProperty("Scenario") && i->HasProperty("Mezzanine")) {
			if (!AssumptionCache.contains(i->GetProperty("Scenario"))) {
				LoanAssumption* DownloadedAssumption = new LoanAssumption(i->GetProperty("Scenario"));
				Db_Mutex.lock();
				QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
				if (!db.isValid()) {
					if (!db.isValid()) {
						db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype", "QODBC"), "TwentyFourDB");
						db.setDatabaseName(
							"Driver={" + GetFromConfig("Database", "Driver", "SQL Server")
							+ "}; "
							+ GetFromConfig("Database", "DataSource", R"(Server=SYNSERVER2\SQLExpress; Initial Catalog = ABSDB; Integrated Security = SSPI; Trusted_Connection = Yes;)")
							);
					}
				}
				bool DbOpen = db.isOpen();
				if (!DbOpen) DbOpen = db.open();
				if (DbOpen) {
					QSqlQuery LoanAssQuerry(db);
					LoanAssQuerry.setForwardOnly(true);
					LoanAssQuerry.prepare("{CALL " + GetFromConfig("Database", "GetLoanAssumptionStoredProc", "getLoanAssumption(:scenarioName,:isSenior)") + "}");
					LoanAssQuerry.bindValue(":scenarioName", i->GetProperty("Scenario"));
					LoanAssQuerry.bindValue(":isSenior", true);
					if (LoanAssQuerry.exec()) {
						if (LoanAssQuerry.next()) {
							auto DbgRecord = LoanAssQuerry.record();
							int FieldCount = 0;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorLastUpdate(DbgRecord.value(FieldCount).toDate()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorMaturityExtension(QString::number(DbgRecord.value(FieldCount).toInt())); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorInitialHaircut(QString::number(DbgRecord.value(FieldCount).toDouble(),'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorPrepaymentFee(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorDayCount(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorCPR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorCDR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorLS(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorRecoveryLag ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorDelinquency (DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorDelinquencyLag (DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorPrice(QString::number(DbgRecord.value(FieldCount).toDouble(), 'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorHaircut ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorPrepayMultiplier ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetSeniorLossMultiplier ( DbgRecord.value(FieldCount).toString()); ++FieldCount;
						}
					}
					LoanAssQuerry.bindValue(":isSenior", false);
					if (LoanAssQuerry.exec()) {
						if (LoanAssQuerry.next()) {
							auto DbgRecord = LoanAssQuerry.record();
							int FieldCount = 0;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzLastUpdate(DbgRecord.value(FieldCount).toDate()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzMaturityExtension(QString::number(DbgRecord.value(FieldCount).toInt())); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzInitialHaircut(QString::number(DbgRecord.value(FieldCount).toDouble(), 'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzPrepaymentFee(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzDayCount(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzCPR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzCDR(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzLS(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzRecoveryLag(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzDelinquency(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzDelinquencyLag(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzPrice(QString::number(DbgRecord.value(FieldCount).toDouble(), 'f')); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzHaircut(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzPrepayMultiplier(DbgRecord.value(FieldCount).toString()); ++FieldCount;
							if (!DbgRecord.isNull(FieldCount)) DownloadedAssumption->SetMezzLossMultiplier(DbgRecord.value(FieldCount).toString()); ++FieldCount;
						}
					}
				}
				Db_Mutex.unlock();
				AssumptionCache.insert(DownloadedAssumption->GetScenarioName(), DownloadedAssumption);
			}
			if (i->GetProperty("Mezzanine").compare("Yes", Qt::CaseInsensitive) == 0) {
				auto CurrentAss = AssumptionCache.object(i->GetProperty("Scenario"));
				if (!i->HasProperty("MaturityExtension") && !CurrentAss->GetRawMezzMaturityExtension().isEmpty()) i->SetProperty("MaturityExtension", CurrentAss->GetMezzMaturityExtension());
				if (!i->HasProperty("StartingHaircut") && !CurrentAss->GetRawMezzInitialHaircut().isEmpty())  i->SetProperty("StartingHaircut", CurrentAss->GetMezzHaircut());
				if (!i->HasProperty("PrepaymentFee") && !CurrentAss->GetRawMezzPrepaymentFee().isEmpty())i->SetProperty("PrepaymentFee", CurrentAss->GetMezzPrepaymentFee());
				if (!i->HasProperty("DayCount") && !CurrentAss->GetRawMezzDayCount().isEmpty()) i->SetProperty("DayCount", CurrentAss->GetMezzDayCount());
				if (!i->HasProperty("Haircut") && !CurrentAss->GetRawMezzHaircut().isEmpty()) i->SetProperty("Haircut", CurrentAss->GetMezzHaircut());
				if (!i->HasProperty("PrepayMultiplier") && !CurrentAss->GetRawMezzPrepayMultiplier().isEmpty())i->SetProperty("PrepayMultiplier", CurrentAss->GetMezzPrepayMultiplier());
				if (!i->HasProperty("LossMultiplier") && !CurrentAss->GetRawMezzLossMultiplier().isEmpty()) i->SetProperty("LossMultiplier", CurrentAss->GetMezzLossMultiplier());
				if (!i->HasProperty("CPR") && !CurrentAss->GetRawMezzCPR().isEmpty()) i->SetProperty("CPR", CurrentAss->GetMezzCPR());
				if (!i->HasProperty("CDR") && !CurrentAss->GetRawMezzCDR().isEmpty()) i->SetProperty("CDR", CurrentAss->GetMezzCDR());
				if (!i->HasProperty("LS") && !CurrentAss->GetRawMezzLS().isEmpty()) i->SetProperty("LS", CurrentAss->GetMezzLS());
				if (!i->HasProperty("RecoveryLag") && !CurrentAss->GetRawMezzRecoveryLag().isEmpty()) i->SetProperty("RecoveryLag", CurrentAss->GetMezzRecoveryLag());
				if (!i->HasProperty("Delinquency") && !CurrentAss->GetRawMezzDelinquency().isEmpty())  i->SetProperty("Delinquency", CurrentAss->GetMezzDelinquency());
				if (!i->HasProperty("DelinquencyLag") && !CurrentAss->GetRawMezzDelinquencyLag().isEmpty())  i->SetProperty("DelinquencyLag", CurrentAss->GetMezzDelinquencyLag());
				if (!i->HasProperty("Price") && !CurrentAss->GetRawMezzPrice().isEmpty())  i->SetProperty("Price", CurrentAss->GetMezzPrice());
			}
			else {
				auto CurrentAss = AssumptionCache.object(i->GetProperty("Scenario"));
				if (!i->HasProperty("MaturityExtension") && !CurrentAss->GetRawSeniorMaturityExtension().isEmpty()) i->SetProperty("MaturityExtension", CurrentAss->GetSeniorMaturityExtension());
				if (!i->HasProperty("StartingHaircut") && !CurrentAss->GetRawSeniorInitialHaircut().isEmpty())  i->SetProperty("StartingHaircut", CurrentAss->GetSeniorHaircut());
				if (!i->HasProperty("PrepaymentFee") && !CurrentAss->GetRawSeniorPrepaymentFee().isEmpty())i->SetProperty("PrepaymentFee", CurrentAss->GetSeniorPrepaymentFee());
				if (!i->HasProperty("DayCount") && !CurrentAss->GetRawSeniorDayCount().isEmpty()) i->SetProperty("DayCount", CurrentAss->GetSeniorDayCount());
				if (!i->HasProperty("Haircut") && !CurrentAss->GetRawSeniorHaircut().isEmpty()) i->SetProperty("Haircut", CurrentAss->GetSeniorHaircut());
				if (!i->HasProperty("PrepayMultiplier") && !CurrentAss->GetRawSeniorPrepayMultiplier().isEmpty())i->SetProperty("PrepayMultiplier", CurrentAss->GetSeniorPrepayMultiplier());
				if (!i->HasProperty("LossMultiplier") && !CurrentAss->GetRawSeniorLossMultiplier().isEmpty()) i->SetProperty("LossMultiplier", CurrentAss->GetSeniorLossMultiplier());
				if (!i->HasProperty("CPR") && !CurrentAss->GetRawSeniorCPR().isEmpty()) i->SetProperty("CPR", CurrentAss->GetSeniorCPR());
				if (!i->HasProperty("CDR") && !CurrentAss->GetRawSeniorCDR().isEmpty()) i->SetProperty("CDR", CurrentAss->GetSeniorCDR());
				if (!i->HasProperty("LS") && !CurrentAss->GetRawSeniorLS().isEmpty()) i->SetProperty("LS", CurrentAss->GetSeniorLS());
				if (!i->HasProperty("RecoveryLag") && !CurrentAss->GetRawSeniorRecoveryLag().isEmpty()) i->SetProperty("RecoveryLag", CurrentAss->GetSeniorRecoveryLag());
				if (!i->HasProperty("Delinquency") && !CurrentAss->GetRawSeniorDelinquency().isEmpty())  i->SetProperty("Delinquency", CurrentAss->GetSeniorDelinquency());
				if (!i->HasProperty("DelinquencyLag") && !CurrentAss->GetRawSeniorDelinquencyLag().isEmpty())  i->SetProperty("DelinquencyLag", CurrentAss->GetSeniorDelinquencyLag());
				if (!i->HasProperty("Price") && !CurrentAss->GetRawSeniorPrice().isEmpty())  i->SetProperty("Price", CurrentAss->GetSeniorPrice());
			}
		}
	}
}
