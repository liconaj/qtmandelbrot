#include "viewport.h"

#include <QTest>

namespace {
constexpr mb::PlanePoint center{-0.5, 0.0};
constexpr mb::Viewport viewport{center, 0.05, 800, 600};

constexpr double epsilon{1e-9};
} // namespace

class tst_Viewport : public QObject
{
    Q_OBJECT
private slots:
    void pixelToPlane_data();
    void pixelToPlane();

    void planeToPixel_data();
    void planeToPixel();

    void pixelToPlaneRoundTrip_data();
    void pixelToPlaneRoundTrip();

    void planeToPixelRoundTrip_data();
    void planeToPixelRoundTrip();

    void isValid_data();
    void isValid();
};

void tst_Viewport::pixelToPlane_data()
{
    QTest::addColumn<mb::Real>("x");
    QTest::addColumn<mb::Real>("y");
    QTest::addColumn<mb::Real>("re");
    QTest::addColumn<mb::Real>("im");

    // clang-format off
    QTest::newRow("center")          << 399.5 << 299.5 <<    -0.5 <<     0.0;
    QTest::newRow("one pixel right") << 400.5 << 299.5 <<   -0.45 <<     0.0;
    QTest::newRow("one pixel down")  << 399.5 << 300.5 <<    -0.5 <<   -0.05;
    QTest::newRow("top left")        << 0.0   <<   0.0 << -20.475 <<  14.975;
    QTest::newRow("bottom right")    << 799.0 << 599.0 <<  19.475 << -14.975;
    // clang-format on
}

void tst_Viewport::pixelToPlane()
{
    QFETCH(mb::Real, x);
    QFETCH(mb::Real, y);
    QFETCH(mb::Real, re);
    QFETCH(mb::Real, im);

    mb::PixelPoint pixelPoint{x, y};
    mb::PlanePoint planePoint{mb::pixelToPlane(viewport, pixelPoint)};

    QCOMPARE_LT(qAbs(planePoint.re - re), epsilon * viewport.scale);
    QCOMPARE_LT(qAbs(planePoint.im - im), epsilon * viewport.scale);
}

void tst_Viewport::planeToPixel_data()
{
    QTest::addColumn<mb::Real>("re");
    QTest::addColumn<mb::Real>("im");
    QTest::addColumn<mb::Real>("x");
    QTest::addColumn<mb::Real>("y");

    // clang-format off
    QTest::newRow("center")          <<    -0.5 <<     0.0 << 399.5 << 299.5;
    QTest::newRow("one pixel right") <<   -0.45 <<     0.0 << 400.5 << 299.5;
    QTest::newRow("one pixel down")  <<    -0.5 <<   -0.05 << 399.5 << 300.5;
    QTest::newRow("top left")        << -20.475 <<  14.975 << 0.0   <<   0.0;
    QTest::newRow("bottom right")    <<  19.475 << -14.975 << 799.0 << 599.0;
    // clang-format on
}

void tst_Viewport::planeToPixel()
{
    QFETCH(mb::Real, re);
    QFETCH(mb::Real, im);
    QFETCH(mb::Real, x);
    QFETCH(mb::Real, y);

    mb::PlanePoint planePoint{re, im};
    mb::PixelPoint pixelPoint{mb::planeToPixel(viewport, planePoint)};

    QCOMPARE_LT(qAbs(pixelPoint.x - x), epsilon);
    QCOMPARE_LT(qAbs(pixelPoint.y - y), epsilon);
}

void tst_Viewport::pixelToPlaneRoundTrip_data()
{
    QTest::addColumn<mb::Real>("x");
    QTest::addColumn<mb::Real>("y");

    // clang-format off
    QTest::newRow("zero")          <<    0.0  <<    0.0;
    QTest::newRow("center")        <<  399.5  <<  299.5;
    QTest::newRow("bottom right")  <<  799.0  <<  599.0;
    QTest::newRow("fractional")    <<  123.25 <<  456.75;
    QTest::newRow("outside image") <<  -10.0  << 1000.0;
    // clang-format on
}

void tst_Viewport::pixelToPlaneRoundTrip()
{
    QFETCH(mb::Real, x);
    QFETCH(mb::Real, y);

    mb::PixelPoint original{x, y};
    mb::PixelPoint result{mb::planeToPixel(viewport, mb::pixelToPlane(viewport, original))};

    QCOMPARE_LT(qAbs(result.x - original.x), epsilon);
    QCOMPARE_LT(qAbs(result.y - original.y), epsilon);
}

void tst_Viewport::planeToPixelRoundTrip_data()
{
    QTest::addColumn<mb::Real>("re");
    QTest::addColumn<mb::Real>("im");

    // clang-format off
    QTest::newRow("zero")     <<   0.0 <<  0.0;
    QTest::newRow("center")   <<  -0.5 <<  0.0;
    QTest::newRow("left tip") <<  -2.0 <<  0.0;
    QTest::newRow("off axis") <<   0.3 << -0.7;
    QTest::newRow("far away") << -30.0 << 25.0;
    // clang-format on
}

void tst_Viewport::planeToPixelRoundTrip()
{
    QFETCH(mb::Real, re);
    QFETCH(mb::Real, im);

    mb::PlanePoint original{re, im};
    mb::PlanePoint result{mb::pixelToPlane(viewport, mb::planeToPixel(viewport, original))};

    QCOMPARE_LT(qAbs(result.re - original.re), epsilon * viewport.scale);
    QCOMPARE_LT(qAbs(result.im - original.im), epsilon * viewport.scale);
}

void tst_Viewport::isValid_data()
{
    QTest::addColumn<mb::Viewport>("viewport");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("default viewport is valid") << viewport << true;
    QTest::newRow("viewport with scale equal to zero is invalid")
        << mb::Viewport{viewport.center, 0.0, viewport.width, viewport.height} << false;
    QTest::newRow("viewport with negative scale is invalid")
        << mb::Viewport{viewport.center, -1.0, viewport.width, viewport.height} << false;
    QTest::newRow("viewport with width equal to zero is invalid")
        << mb::Viewport{viewport.center, viewport.scale, 0, viewport.height} << false;
    QTest::newRow("viewport with negative width is invalid")
        << mb::Viewport{viewport.center, viewport.scale, -viewport.width, viewport.height} << false;
    QTest::newRow("viewport with height equal to zero is invalid")
        << mb::Viewport{viewport.center, viewport.scale, viewport.width, 0} << false;
    QTest::newRow("viewport with negative height is invalid")
        << mb::Viewport{viewport.center, viewport.scale, viewport.width, -viewport.height} << false;
}

void tst_Viewport::isValid()
{
    QFETCH(mb::Viewport, viewport);
    QFETCH(bool, isValid);

    bool result{mb::isValid(viewport)};
    QCOMPARE(result, isValid);
}

QTEST_MAIN(tst_Viewport)
#include "tst_viewport.moc"
