#include <QTest>

class tst_Sanity : public QObject
{
    Q_OBJECT
private slots:
    void sanity();
};

void tst_Sanity::sanity()
{
    QVERIFY(true);
}

QTEST_MAIN(tst_Sanity)
#include "tst_sanity.moc"
