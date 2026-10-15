#include "algorithms.h"
#include "types.h"

#include <QTest>

namespace {
constexpr int bailoutRadius = 256;
constexpr int maxIterations = 100;
} // namespace

class tst_EscapeTime : public QObject
{
    Q_OBJECT
private slots:
    void knownPoints_data();
    void knownPoints();
};

void tst_EscapeTime::knownPoints_data()
{
    QTest::addColumn<mb::Real>("cRe");
    QTest::addColumn<mb::Real>("cIm");
    QTest::addColumn<int>("iterations");
    QTest::addColumn<mb::Real>("magnitudeSquared");

    // clang-format off
    QTest::newRow("c =  0") <<  0.0 << 0.0 << maxIterations << 0.0;
    QTest::newRow("c = -1") << -1.0 << 0.0 << maxIterations << 0.0;
    QTest::newRow("c =  i") <<  0.0 << 1.0 << maxIterations << 2.0;
    QTest::newRow("c = -2") << -2.0 << 0.0 << maxIterations << 4.0;
    QTest::newRow("c =  1") <<  1.0 << 0.0 <<             5 << 458329.0;
    QTest::newRow("c =  2") <<  2.0 << 0.0 <<             4 << 2090916.0;
    // clang-format on
}

void tst_EscapeTime::knownPoints()
{
    QFETCH(mb::Real, cRe);
    QFETCH(mb::Real, cIm);
    QFETCH(int, iterations);
    QFETCH(mb::Real, magnitudeSquared);

    mb::EscapeTimeResult result{mb::computeEscapeTime(cRe, cIm, bailoutRadius, maxIterations)};
    QCOMPARE(result.iterations, iterations);
    QCOMPARE(result.magnitudeSquared, magnitudeSquared);
}

QTEST_MAIN(tst_EscapeTime)
#include "tst_escapetime.moc"
