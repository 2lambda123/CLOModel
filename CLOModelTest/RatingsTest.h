#ifndef RATINGSTEST_H
#define RATINGSTEST_H

#include <QObject>

class RatingsTest : public QObject
{
    Q_OBJECT
private slots:
    void averageRating_data();
    void averageRating();
    void getBucket();
    void rankedRatings_data();
    void rankedRatings();
};

#endif // RATINGSTEST_H