#ifndef NO_BLOOMBERG
#ifndef BloombergResult_h__
#define BloombergResult_h__
#include <QStringList>
#include <QDate>
#include "BackwardCompatibilityInterface.h"
class SingleBbgRequest;
class BloombergResult :public BackwardInterface  {
public:
	BloombergResult();
	BloombergResult(const BloombergResult& a);
	BloombergResult& operator= (const BloombergResult& a);
	~BloombergResult();
	bool IsEmpty() const { return !HasString() && !HasTable(); }
	bool HasString() const { return !m_StringValue; }
	bool HasDate() const { return !m_DateValue; }
	bool HasDouble() const { return !m_DoubleValue; }
	bool HasTable() const { return (!m_TableResultRows.isEmpty()) && TableResultCols > 0; }
	const QString& GetHeader()const { return m_Header; }
	QString GetString()const { if (m_StringValue) return *m_StringValue; else return QString(); }
	QDate GetDate() const { if (m_DateValue) return *m_DateValue; else return QDate(); }
	double GetDouble()const { if (m_DoubleValue) return *m_DoubleValue; else return 0.0; }
private:
	QString m_Header;
	QString* m_StringValue;
	QDate* m_DateValue;
	double* m_DoubleValue;
	QList<BloombergResult*> m_TableResultRows;
	int TableResultCols;
protected:
	void Clear();
	void SetValue(const QString& val, const QString& Header="");
	void AddValueRow(const QStringList& val, const QStringList& Headers = QStringList());
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
	friend QDataStream& operator<<(QDataStream & stream, const BloombergResult& flows);
	friend QDataStream& operator>>(QDataStream & stream, BloombergResult& flows);
	friend SingleBbgRequest;
};
QDataStream& operator<<(QDataStream & stream, const BloombergResult& flows);
QDataStream& operator>>(QDataStream & stream, BloombergResult& flows);
#endif // BloombergResult_h__
#endif
