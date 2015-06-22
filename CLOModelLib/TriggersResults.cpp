#include "TriggersResults.h"
#include "TriggersResults_p.h"
#include <QDataStream>
DEFINE_PUBLIC_COMMONS(TriggersResults)
DEFINE_PUBLIC_COMMONS_COPY(TriggersResults)

TriggersResultsPrivate::TriggersResultsPrivate(TriggersResults *q)
	:BackwardInterfacePrivate(q)
{}
TriggersResults::TriggersResults(TriggersResultsPrivate *d, const TriggersResults& other)
	:BackwardInterface(d,other)
{
    for (auto i = other.d_func()->m_Results.constBegin(); i != other.d_func()->m_Results.constEnd(); ++i) {
        d->m_Results.insert(i.key(), new QMap<QDate, bool>(*(i.value())));
    }
}
TriggersResults& TriggersResults::operator=(const TriggersResults& other){
    Q_D(TriggersResults);
	BackwardInterface::operator=(other);
    ClearResults();
    for (auto i = other.d_func()->m_Results.constBegin(); i != other.d_func()->m_Results.constEnd(); ++i) {
        d->m_Results.insert(i.key(), new QMap<QDate, bool>(*(i.value())));
    }
	return *this;
}
TriggersResults::TriggersResults(TriggersResultsPrivate *d)
	:BackwardInterface(d)
{}

TriggersResults::~TriggersResults()
{
    ClearResults();
}

void TriggersResults::ClearResults()
{
    Q_D(TriggersResults);
    for (auto i = d->m_Results.begin(); i != d->m_Results.end(); ++i)
		delete i.value();
    d->m_Results.clear();
}

TriggersResults::TrigRes TriggersResults::GetResult(quint32 TrigType, const QDate& RefDate)const {
    Q_D(const TriggersResults);
    const QMap<QDate, bool>* Tempmap = d->m_Results.value(TrigType, nullptr);
	if (Tempmap) {
		auto MapIter = Tempmap->constFind(RefDate);
		if (MapIter == Tempmap->constEnd()) return TrigRes::trNA;
		return (MapIter.value() ? TrigRes::trTrue : TrigRes::trFalse);
	}
	return TrigRes::trNA;
}

void TriggersResults::SetResult(quint32 TrigType, const QDate& RefDate, bool Res) {
    Q_D(TriggersResults);
    auto hashIter = d->m_Results.find(TrigType);
    if (hashIter == d->m_Results.end()) {
        hashIter = d->m_Results.insert(TrigType, new QMap<QDate, bool>());
	}
	hashIter.value()->operator[](RefDate) = Res;
}

QDataStream& TriggersResults::LoadOldVersion(QDataStream& stream) {
    Q_D(TriggersResults);
	ClearResults();
	qint32 tempInt, tempKey;
	QMap<QDate, bool> Tempmap;
	stream >> tempInt;
	for (qint32 i = 0; i < tempInt; i++) {
		Tempmap.clear();
		stream >> tempKey >> Tempmap;
        d->m_Results.insert(tempKey, new QMap<QDate, bool>(Tempmap));
	}
	ResetProtocolVersion();
	return stream;
}

QDataStream& operator<<(QDataStream & stream, const TriggersResults& flows) {
	stream << static_cast<qint32>(flows.d_func()->m_Results.size());
    for (auto i = flows.d_func()->m_Results.constBegin(); i != flows.d_func()->m_Results.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
	return stream;
}

QDataStream& operator>>(QDataStream & stream, TriggersResults& flows) {
	return flows.LoadOldVersion(stream);
}
