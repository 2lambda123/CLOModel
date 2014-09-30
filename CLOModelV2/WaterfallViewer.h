#ifndef WaterfallViewer_h__
#define WaterfallViewer_h__
#include <QWidget>
#include "WatFalPrior.h"
class QTableWidget;
class WatFalPrior;
class QLabel;
class Waterfall;
class WaterfallViewer : public QWidget{
	Q_OBJECT
public:
	WaterfallViewer(QWidget* parent=0);
	void ResetSteps();
	void AddStep(const WatFalPrior& a);
	void AddStep(
		WatFalPrior::WaterfallStepType Step
		, int ArgSeniorityGroup
		, int ArgSeniorityGroupLevel
		, int ArgRedemptionGroup
		, int ArgRedemptionGroupLevel
		, double ArgRedemptionShare
		, double ArgAdditionalCollateralShare
		, int ArgSourceofFunding
		, int ArgCouponIndex
		, double ArgTestTargetOverride
		, double ArgIRRtoEquityTarget
		, int ArgReserveIndex
	);
	void SetWaterfall(const Waterfall& a);
private:
	QTableWidget* InterestTable;
	QTableWidget* PrincipalTable;
	QLabel* InterestLabel;
	QLabel* PrincipalLabel;
};
#endif // WaterfallViewer_h__