#ifndef LOANASSMATCHERTHREAD_H
#define LOANASSMATCHERTHREAD_H
#include "TemplAsyncThread.h"
#include "LoanAssMatcherResult.h"
#include "LoanAssumption.h"
#include <QHash>
class LoanAssMatcherThread : public TemplAsyncThread<LoanAssMatcherResult>
{
	Q_OBJECT
public:
	LoanAssMatcherThread(int Identity, QObject *parent = nullptr);
	const QString& GetModelToScan() const { return m_ModelToScan; }
	void SetModelToScan(const QString& val) { m_ModelToScan = val; }
	void SetAvailableAssumptions(const QHash<QString, QSharedPointer<LoanAssumption> >& a) { m_AvailableAssumptions = &a; }
protected:
	QString m_ModelToScan;
	LoanAssMatcherResult Result;
	const QHash<QString, QSharedPointer<LoanAssumption> >* m_AvailableAssumptions;
signals:
	void Calculated(int, const LoanAssMatcherResult&);
protected slots:
	virtual void EmitFromAnonim()override {
		emit Calculated(Identifier, Result);
	}
public slots :
	virtual void run() override;
private:
	void ReturnError();
};

#endif // LOANASSMATCHERTHREAD_H
