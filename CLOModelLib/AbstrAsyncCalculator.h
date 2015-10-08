
#ifndef AbstrAsyncCalculator_h__
#define AbstrAsyncCalculator_h__
#include "BackwardCompatibilityInterface.h"
#include <QObject>
#include <memory>
#ifndef RETURN_WHEN_RUNNING
#define RETURN_WHEN_RUNNING(rvr,retval) if( ContinueCalculation() == rvr) return retval;
#endif
class AbstrAsyncCalculatorPrivate;
class CLOMODELLIB_EXPORT AbstrAsyncCalculator : public QObject, public BackwardInterface
{
	Q_OBJECT	
protected:
    inline AbstrAsyncCalculatorPrivate* d_func() { return reinterpret_cast<AbstrAsyncCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    inline const AbstrAsyncCalculatorPrivate* d_func() const { return reinterpret_cast<const AbstrAsyncCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    friend class AbstrAsyncCalculatorPrivate;
    AbstrAsyncCalculator(AbstrAsyncCalculatorPrivate *d, QObject* parent = nullptr);
    virtual QHash<qint32, std::shared_ptr<void>>& getResultVoid();
    virtual const QHash<qint32, std::shared_ptr<void>>& getResultVoid() const;
    virtual QHash<qint32, QPointer<QObject> >& getThreadPool();
    virtual const QHash<qint32, QPointer<QObject> >& getThreadPool() const;
    virtual const QSet<qint32>& getBeeSent() const;
    virtual QSet<qint32>& getBeeSent();
    virtual qint32& getBeesReturned();
    virtual const qint32& getBeesReturned() const;
    virtual const std::shared_ptr<void> getResultVoid(qint32 key)const;
    virtual void insertResult(qint32 Key, std::shared_ptr<void> val);
    virtual bool ContinueCalculation() const;
    virtual void setContinueCalculation(bool val);
    virtual int availableThreads() const;
public:
    virtual quint8 operativity() const;
    virtual void setOperativity(quint8 val);
    AbstrAsyncCalculator(QObject* parent =nullptr);
    virtual ~AbstrAsyncCalculator();
    virtual QString ReadyToCalculate() const = 0;
    virtual void SetSequentialComputation(bool a);
    virtual bool GetSequentialComputation()const;
    virtual QList<qint32> GetResultKeys() const;
    virtual int NumBees() const = 0;
signals :
	void Calculated();
	void BeeCalculated(int);
	void BeeError(int);
	void Progress(double);
	void ProgressPct(int);
	void Stopped();
private slots:
	void SendPctSignal(double a);
public slots:
	virtual void StopCalculation();
	virtual bool StartCalculation()=0;
};
#endif // AbstrAsyncCalculator_h__

