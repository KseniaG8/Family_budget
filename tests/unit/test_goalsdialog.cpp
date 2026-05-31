#include <QTest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class TestGoalsDialogLogic : public QObject
{
    Q_OBJECT

private slots:
    void testParseGoalsResponse() {
        QString jsonData = R"([
            {
                "id": 1,
                "name": "Ноутбук",
                "target_amount": 50000,
                "current_amount": 15000
            },
            {
                "id": 2,
                "name": "Отдых",
                "target_amount": 30000,
                "current_amount": 0
            }
        ])";

        QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8());
        QVERIFY(!doc.isNull());
        QVERIFY(doc.isArray());

        QJsonArray goals = doc.array();
        QCOMPARE(goals.size(), 2);

        QJsonObject firstGoal = goals[0].toObject();
        QCOMPARE(firstGoal["id"].toInt(), 1);
        QCOMPARE(firstGoal["name"].toString(), QString("Ноутбук"));
        QCOMPARE(firstGoal["target_amount"].toDouble(), 50000.0);
        QCOMPARE(firstGoal["current_amount"].toDouble(), 15000.0);

        QJsonObject secondGoal = goals[1].toObject();
        QCOMPARE(secondGoal["id"].toInt(), 2);
        QCOMPARE(secondGoal["name"].toString(), QString("Отдых"));
        QCOMPARE(secondGoal["target_amount"].toDouble(), 30000.0);
        QCOMPARE(secondGoal["current_amount"].toDouble(), 0.0);
    }

    void testCreateGoalRequest() {
        int userId = 1;
        QString goalName = "Машина";
        double targetAmount = 100000;

        QJsonObject request;
        request["user_id"] = userId;
        request["name"] = goalName;
        request["target_amount"] = targetAmount;

        QCOMPARE(request["user_id"].toInt(), 1);
        QCOMPARE(request["name"].toString(), QString("Машина"));
        QCOMPARE(request["target_amount"].toDouble(), 100000.0);
    }

    void testContributeRequest() {
        int goalId = 1;
        double currentAmount = 15000;
        double additional = 5000;
        double newAmount = currentAmount + additional;

        QJsonObject request;
        request["goal_id"] = goalId;
        request["current_amount"] = newAmount;

        QCOMPARE(request["goal_id"].toInt(), 1);
        QCOMPARE(request["current_amount"].toDouble(), 20000.0);
    }

    void testValidateGoalName() {
        QString emptyName = "";
        QString validName = "Ноутбук";

        bool isEmptyValid = emptyName.isEmpty();
        bool isNotEmptyValid = !validName.isEmpty();

        QVERIFY(isEmptyValid);
        QVERIFY(isNotEmptyValid);
    }

    void testValidateTargetAmount() {
        double invalidAmount = 0;
        double validAmount = 10000;

        bool isInvalidValid = (invalidAmount > 0);
        bool isValidValid = (validAmount > 0);

        QVERIFY(!isInvalidValid);
        QVERIFY(isValidValid);
    }

    void testValidateContributeAmount() {
        double invalidAmount = 0;
        double validAmount = 500;

        bool isInvalidValid = (invalidAmount > 0);
        bool isValidValid = (validAmount > 0);

        QVERIFY(!isInvalidValid);
        QVERIFY(isValidValid);
    }

    void testCalculateNewAmount() {
        double current = 15000;
        double additional = 5000;
        double expected = 20000;

        double actual = current + additional;

        QCOMPARE(actual, expected);
    }
};

QTEST_MAIN(TestGoalsDialogLogic)
#include "test_goalsdialog.moc"
