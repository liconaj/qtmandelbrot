#include "algorithms.h"
#include "types.h"

#include <QTest>

namespace {
constexpr int defaultBailoutRadius = 256;
constexpr int defaultMaxIterations = 100;
} // namespace

class tst_EscapeTime : public QObject
{
    Q_OBJECT
private slots:
    void knownPoints_data();
    void knownPoints();
    void zeroIterations_data();
    void zeroIterations();
    void smoothIsContinuous_data();
    void smoothIsContinuous();
};

void tst_EscapeTime::knownPoints_data()
{
    QTest::addColumn<mb::Real>("cRe");
    QTest::addColumn<mb::Real>("cIm");
    QTest::addColumn<int>("iterations");
    QTest::addColumn<mb::Real>("magnitudeSquared");

    // clang-format off
    QTest::newRow("c =  0") <<  0.0 << 0.0 << defaultMaxIterations << 0.0;
    QTest::newRow("c = -1") << -1.0 << 0.0 << defaultMaxIterations << 0.0;
    QTest::newRow("c =  i") <<  0.0 << 1.0 << defaultMaxIterations << 2.0;
    QTest::newRow("c = -2") << -2.0 << 0.0 << defaultMaxIterations << 4.0;
    QTest::newRow("c =  1") <<  1.0 << 0.0 <<                    5 << 458329.0;
    QTest::newRow("c =  2") <<  2.0 << 0.0 <<                    4 << 2090916.0;
    // clang-format on
}

void tst_EscapeTime::knownPoints()
{
    QFETCH(mb::Real, cRe);
    QFETCH(mb::Real, cIm);
    QFETCH(int, iterations);
    QFETCH(mb::Real, magnitudeSquared);

    mb::EscapeTimeResult result{mb::computeEscapeTime(cRe, cIm, defaultBailoutRadius, defaultMaxIterations)};

    QCOMPARE(result.iterations, iterations);
    QCOMPARE(result.magnitudeSquared, magnitudeSquared);
}

void tst_EscapeTime::zeroIterations_data()
{
    QTest::addColumn<mb::Real>("cRe");
    QTest::addColumn<mb::Real>("cIm");

    // clang-format off
    QTest::newRow("c = 0   ") <<     0.0 << 0.0;
    QTest::newRow("c = i   ") <<     0.0 << 1.0;
    QTest::newRow("c = 1000") <<  1000.0 << 0.0;
    // clang-format on
}

void tst_EscapeTime::zeroIterations()
{
    QFETCH(mb::Real, cRe);
    QFETCH(mb::Real, cIm);

    constexpr int maxIterations = 0;

    mb::EscapeTimeResult result{mb::computeEscapeTime(cRe, cIm, defaultBailoutRadius, maxIterations)};

    QCOMPARE(result.iterations, 0);
    QCOMPARE(result.magnitudeSquared, 0);
}

void tst_EscapeTime::smoothIsContinuous_data()
{
    QTest::addColumn<mb::Real>("cRe1");
    QTest::addColumn<mb::Real>("cIm1");
    QTest::addColumn<mb::Real>("cRe2");
    QTest::addColumn<mb::Real>("cIm2");

    QTest::newRow("c1 = 1.51246, c2 = 1.51247") << 1.51246 << 0.0 << 1.51247 << 0.0;
}

void tst_EscapeTime::smoothIsContinuous()
{
    QFETCH(mb::Real, cRe1);
    QFETCH(mb::Real, cIm1);
    QFETCH(mb::Real, cRe2);
    QFETCH(mb::Real, cIm2);

    mb::EscapeTimeResult escapeTime1{mb::computeEscapeTime(cRe1, cIm1, defaultBailoutRadius, defaultMaxIterations)};
    mb::EscapeTimeResult escapeTime2{mb::computeEscapeTime(cRe2, cIm2, defaultBailoutRadius, defaultMaxIterations)};
    mb::Real smooth1{mb::smoothIterationCount(escapeTime1.iterations, escapeTime1.magnitudeSquared)};
    mb::Real smooth2{mb::smoothIterationCount(escapeTime2.iterations, escapeTime2.magnitudeSquared)};

    // Escape times must have different iteration for the test make sense
    QCOMPARE_GE(qAbs(escapeTime2.iterations - escapeTime1.iterations), 1);
    QCOMPARE_LT(qAbs(smooth2 - smooth1), 1e-3);
}

QTEST_MAIN(tst_EscapeTime)
#include "tst_escapetime.moc"
