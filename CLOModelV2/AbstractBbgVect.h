#ifndef AbstarctBbgVect_h__
#define AbstarctBbgVect_h__
#include <QString>
#include <QDate>
#include "BackwardCompatibilityInterface.h"
#include <QDataStream>
#include <QMetaType>
class AbstractBbgVect : public BackwardInterface
{
protected:
	QString m_Vector;
	QDate m_AnchorDate;
	virtual bool IsValid() const =0;
	virtual bool IsValid(const QString& ValidInputs, bool AllowRamps) const;
	virtual void UnpackVector() = 0;
	virtual bool ValidAnchorDate() const;
	virtual bool ExtractAnchorDate();
	template<class T> T GetValueTemplate(const QList<T>& VecVal, const QDate& index, const T& DefaultValue) const {
		QDate ValidDate(m_AnchorDate);
		if (m_AnchorDate.isNull()) {
			ValidDate = QDate::currentDate();
			//LOGDEBUG("Anchor defaulted to today\n");
		}
		if (index < m_AnchorDate) {
			//LOGDEBUG("Requested date before Anchor\n");
			return VecVal.first();
		}
		return GetValueTemplate(VecVal, MonthDiff(index, ValidDate), DefaultValue);
	}
	template<class T> T GetValueTemplate(const QList<T>& VecVal, int index, const T& DefaultValue)const {
		if (VecVal.isEmpty() || index < 0) return DefaultValue;
		return VecVal.at(qMin(index, VecVal.size() - 1));
	}
public:
	virtual void Clear();
	virtual bool SetVector(const QString& Vec);
	AbstractBbgVect(){}
	AbstractBbgVect(const QString& Vec);
	virtual void SetAnchorDate(const QDate& Anchor){m_AnchorDate=Anchor;}
	virtual void RemoveAnchorDate(){m_AnchorDate=QDate();}
	virtual QString GetVector() const;
	virtual const QDate& GetAnchorDate() const{return m_AnchorDate;}
	AbstractBbgVect& operator=(const QString& a){SetVector(a); return *this;}
	virtual bool IsEmpty() const{return m_Vector.isEmpty();}
};



#endif // AbstarctBbgVect_h__

