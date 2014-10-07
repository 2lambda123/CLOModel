#ifndef TestTable_h__
#define TestTable_h__
#include <QWidget>
#include <QList>
#include "Waterfall.h"
#include "StressTest.h"
class TrancheViewer;
class QTableWidget;
class Mortgage;
class QTabWidget;
class QStackedWidget;
class QListWidget;
class QLineEdit;
class ChartsWidget;
class WaterfallViewer;
class SummaryView : public QWidget{
	Q_OBJECT
private:
	QTableWidget* MtgTable;
	QList<TrancheViewer*> TrancheTables;
	QList<TrancheViewer*> CallTrancheTables;
	QTabWidget* MainWidget;
	Waterfall Structure;
	Waterfall CallStructure;
	QStackedWidget* CallTranchesArea;
	QListWidget* CallTranchesList;
	QStackedWidget* TranchesArea;
	QListWidget* TranchesList;
	QTableWidget* ExpensesTable;
	QTableWidget* ReinvestmentsTable;
	QTableWidget* GICTable;
	QTableWidget* StructureTable;
	QList<QLineEdit*> PricesLabel;
	ChartsWidget* ChartPlotter;
	WaterfallViewer* WatStructView;
	void ResetCharts();
	void DisplayStructure();
	void ResetTranches();
	void ResetPricesLabel();
public:
	void ShowCallStructure(bool a=true);
	void SetStructure(const Waterfall& a,const Waterfall& ca=Waterfall());
	SummaryView(QWidget* parent=0);
private slots:
	void PriceChanged();
};
#endif // TestTable_h__