#include "ReinvestmentTest.h"
#include "CommonFunctions.h"
#include <QtGlobal>
void ReinvestmentTest::SetupReinvBond(
	const QString& IntrVec,
	const QString& CPRVec,
	const QString& CDRVec,
	const QString& LSVec,
	const QString& WALval,
	const QString& PayFreq,
	const QString& AnnuityVec,
	const QString& ReinvPric,
	const QString& ReinvDel,
	const QString& FloatingBase,
	const QString& RecoveryLag,
	const QString& Delinquency,
	const QString& DelinquencyLag
){
	ReinvestmentBond.SetAnnuity(AnnuityVec);
	ReinvestmentBond.SetFloatingRateBase(FloatingBase);
	ReinvestmentBond.SetInterest(IntrVec);
	ReinvestmentBond.SetPaymentFreq(PayFreq);
	ReinvestmentDelay = ReinvDel;
	ReinvestmentPrice = ReinvPric;
	CDRAssumption=CDRVec;
	CPRAssumption=CPRVec;
	LSAssumption=LSVec;
	WALAssumption=WALval;
	m_RecoveryLag = RecoveryLag;
	m_Delinquency = Delinquency;
	m_DelinquencyLag = DelinquencyLag;
}
void ReinvestmentTest::SetupTest(
	const QDate& ReinvPer
	,double TstLvl
	,double IIshare
	,double IRshare
	,double OIshare
	,double ORshare
){
	ReinvestmentPeriod=ReinvPer;
	ReinvestmentTestLevel=TstLvl;
	if(IIshare>=0.0 && IIshare<=1.0) ReinvestmentShare[InReinvShare]=IIshare;
	if(IRshare>=0.0 && IRshare<=1.0) ReinvestmentShare[InRedempShare]=IRshare;
	if(OIshare>=0.0 && OIshare<=1.0) ReinvestmentShare[OutReinvShare]=OIshare;
	if(ORshare>=0.0 && ORshare<=1.0) ReinvestmentShare[OutRedempShare]=ORshare;
}
ReinvestmentTest::ReinvestmentTest()
	:CDRAssumption("0")
	,CPRAssumption("0")
	,LSAssumption("100")
	,WALAssumption("1")
	, ReinvestmentDelay("0")
	, ReinvestmentPrice("100")
	, m_RecoveryLag("0")
	, m_Delinquency("0")
	, m_DelinquencyLag("0")
{
	ReinvestmentShare[InReinvShare]=0.0;
	ReinvestmentShare[InRedempShare]=0.0;
	ReinvestmentShare[OutReinvShare]=0.0;
	ReinvestmentShare[OutRedempShare]=0.0;
	WALAssumption.SetDivisor(1.0);
}
ReinvestmentTest::ReinvestmentTest(const ReinvestmentTest& a)
	:CDRAssumption(a.CDRAssumption)
	,CPRAssumption(a.CPRAssumption)
	,LSAssumption(a.LSAssumption)
	,WALAssumption(a.WALAssumption)
	,ReinvestmentBond(a.ReinvestmentBond)
	,ReinvestmentPeriod(a.ReinvestmentPeriod)
	,ReinvestmentTestLevel(a.ReinvestmentTestLevel)
	, ReinvestmentDelay(a.ReinvestmentDelay)
	, ReinvestmentPrice(a.ReinvestmentPrice)
	, m_RecoveryLag(a.m_RecoveryLag)
	, m_Delinquency(a.m_Delinquency)
	, m_DelinquencyLag(a.m_DelinquencyLag)
{
		ReinvestmentShare[InReinvShare]=a.ReinvestmentShare[InReinvShare];
		ReinvestmentShare[InRedempShare]=a.ReinvestmentShare[InRedempShare];
		ReinvestmentShare[OutReinvShare]=a.ReinvestmentShare[OutReinvShare];
		ReinvestmentShare[OutRedempShare]=a.ReinvestmentShare[OutRedempShare];
}
ReinvestmentTest& ReinvestmentTest::operator=(const ReinvestmentTest& a){
	ReinvestmentShare[InReinvShare]=a.ReinvestmentShare[InReinvShare];
	ReinvestmentShare[InRedempShare]=a.ReinvestmentShare[InRedempShare];
	ReinvestmentShare[OutReinvShare]=a.ReinvestmentShare[OutReinvShare];
	ReinvestmentShare[OutRedempShare]=a.ReinvestmentShare[OutRedempShare];
	CDRAssumption=a.CDRAssumption;
	CPRAssumption=a.CPRAssumption;
	LSAssumption=a.LSAssumption;
	WALAssumption=a.WALAssumption;
	ReinvestmentBond=a.ReinvestmentBond;
	ReinvestmentPeriod=a.ReinvestmentPeriod;
	ReinvestmentTestLevel=a.ReinvestmentTestLevel;
	ReinvestmentDelay = a.ReinvestmentDelay;
	ReinvestmentPrice = a.ReinvestmentPrice;
	m_RecoveryLag = a.m_RecoveryLag;
	m_Delinquency = a.m_Delinquency;
	m_DelinquencyLag = a.m_DelinquencyLag;
	return *this;
}
void ReinvestmentTest::CalculateBondCashFlows(double Size, QDate StartDate, int Period){
	ReinvestmentBond.ResetFlows();
	bool NullDates[] = {
		CPRAssumption.GetAnchorDate().isNull()
		, CDRAssumption.GetAnchorDate().isNull()
		, LSAssumption.GetAnchorDate().isNull()
	};
	if (NullDates[0]) CPRAssumption.SetAnchorDate(StartDate.addMonths(-Period));
	if (NullDates[1]) CDRAssumption.SetAnchorDate(StartDate.addMonths(-Period));
	if (NullDates[2]) LSAssumption.SetAnchorDate(StartDate.addMonths(-Period));
	double CurrentPrice;
	int CurrentDelay = ReinvestmentDelay.GetValue(StartDate);
	if (ReinvestmentPrice.GetAnchorDate().isNull()) CurrentPrice = ReinvestmentPrice.GetValue(Period + CurrentDelay);
	else CurrentPrice = ReinvestmentPrice.GetValue(StartDate.addMonths(CurrentDelay));
	ReinvestmentBond.SetSize(Size / qMax(CurrentPrice,0.01));
	if (WALAssumption.GetAnchorDate().isNull()) ReinvestmentBond.SetMaturityDate(StartDate.addDays(RoundUp(365.25*WALAssumption.GetValue(Period + CurrentDelay))));
	else ReinvestmentBond.SetMaturityDate(StartDate.addDays(RoundUp(365.25*WALAssumption.GetValue(StartDate.addMonths(CurrentDelay)))));
	ReinvestmentBond.CalculateCashFlows(
		StartDate.addMonths(CurrentDelay)
		, CPRAssumption
		, CDRAssumption
		, LSAssumption
		, m_RecoveryLag
		, m_Delinquency
		, m_DelinquencyLag
	);
	for (int DelayIter = 0; DelayIter < CurrentDelay; DelayIter++) {
		ReinvestmentBond.AddCashFlow(StartDate.addMonths(DelayIter), Size, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
	}
	if (NullDates[0]) CPRAssumption.RemoveAnchorDate();
	if (NullDates[1]) CDRAssumption.RemoveAnchorDate();
	if (NullDates[2]) LSAssumption.RemoveAnchorDate();
}
const MtgCashFlow& ReinvestmentTest::GetBondCashFlow() const{
	return ReinvestmentBond.GetCashFlow();
}
void ReinvestmentTest::SetCPR(const QString& a){CPRAssumption=a;}
void ReinvestmentTest::SetCDR(const QString& a){CDRAssumption=a;}
void ReinvestmentTest::SetLS(const QString& a){LSAssumption=a;}
QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows){
	stream 
		<< flows.ReinvestmentPeriod
		<< flows.ReinvestmentTestLevel
		<< flows.ReinvestmentShare[0]
		<< flows.ReinvestmentShare[1]
		<< flows.ReinvestmentShare[2]
		<< flows.ReinvestmentShare[3]
		<< flows.ReinvestmentBond
		<< flows.CPRAssumption
		<< flows.CDRAssumption
		<< flows.LSAssumption
		<< flows.WALAssumption
		<< flows.ReinvestmentDelay
		<< flows.ReinvestmentPrice
		<< flows.m_RecoveryLag
		<< flows.m_Delinquency
		<< flows.m_DelinquencyLag
	;
	return stream;
}
QDataStream& ReinvestmentTest::LoadOldVersion(QDataStream& stream) {
	stream
		>> ReinvestmentPeriod
		>> ReinvestmentTestLevel
		>> ReinvestmentShare[0]
		>> ReinvestmentShare[1]
		>> ReinvestmentShare[2]
		>> ReinvestmentShare[3];
	ReinvestmentBond.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> ReinvestmentBond;
	CPRAssumption.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> CPRAssumption;
	CDRAssumption.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> CDRAssumption;
	LSAssumption.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> LSAssumption;
	WALAssumption.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> WALAssumption;
	ReinvestmentDelay.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> ReinvestmentDelay;
	ReinvestmentPrice.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> ReinvestmentPrice;
	m_RecoveryLag.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_RecoveryLag;
	m_Delinquency.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_Delinquency;
	m_DelinquencyLag.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_DelinquencyLag;
	ResetProtocolVersion();
	return stream;
}
QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows){
	return flows.LoadOldVersion(stream);
}