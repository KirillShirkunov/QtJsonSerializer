#include "typeconvertertestbase.h"
#include <QtJsonSerializer>
#include <QtTest>
#include <typeinfo>

#define private public
#include <QtJsonSerializer/private/qjsonserializer_p.h>
#undef private

#define SELF_COMPARE(type, actual, expected) \
do {\
	if (!compare(type, actual, expected, #actual, #expected, __FILE__, __LINE__))\
		return;\
} while (false)

TypeConverterTestBase::TypeConverterTestBase(QObject *parent) :
	QObject{parent},
	helper{new DummySerializationHelper{this}}
{}

void TypeConverterTestBase::initTest() {}

void TypeConverterTestBase::cleanupTest() {}

void TypeConverterTestBase::addSerData() {}

void TypeConverterTestBase::addDeserData() {}

bool TypeConverterTestBase::compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line)
{
	Q_UNUSED(type)
	return QTest::qCompare(actual, expected, aName, eName, file, line);
}

void TypeConverterTestBase::initTestCase()
{
	initTest();
}

void TypeConverterTestBase::cleanupTestCase()
{
	cleanupTest();
}

void TypeConverterTestBase::testConverterIsRegistered_data() {}

void TypeConverterTestBase::testConverterIsRegistered()
{
	const auto cOwn = converter();
	for(const auto &factory : qAsConst(QJsonSerializerPrivate::typeConverterFactories)) {
		const auto conv = factory->createConverter();
		const auto cReg = conv.data();
		if(typeid(*cOwn).hash_code() == typeid(*cReg).hash_code())
			return;
	}
	QFAIL(QByteArrayLiteral("TypeConverter \"") + typeid(*cOwn).name() + QByteArrayLiteral("\" is not registerd within QJsonSerializer"));
}

void TypeConverterTestBase::testConverterMeta_data()
{
	QTest::addColumn<int>("priority");
	QTest::addColumn<QList<QJsonValue::Type>>("jsonTypes");

	addConverterData();
}

void TypeConverterTestBase::testConverterMeta()
{
	QFETCH(int, priority);
	QFETCH(QList<QJsonValue::Type>, jsonTypes);

	QCOMPARE(converter()->priority(), priority);
	QCOMPARE(converter()->jsonTypes(), jsonTypes);
}

void TypeConverterTestBase::testMetaTypeDetection_data()
{
	QTest::addColumn<int>("metatype");
	QTest::addColumn<bool>("matches");

	addMetaData();
}

void TypeConverterTestBase::testMetaTypeDetection()
{
	QFETCH(int, metatype);
	QFETCH(bool, matches);

	QCOMPARE(converter()->canConvert(metatype), matches);
}

void TypeConverterTestBase::testSerialization_data()
{
	QTest::addColumn<QVariantHash>("properties");
	QTest::addColumn<QList<DummySerializationHelper::SerInfo>>("serData");
	QTest::addColumn<QObject*>("parent");
	QTest::addColumn<int>("type");
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QJsonValue>("result");

	addCommonSerData();
	addSerData();
}

void TypeConverterTestBase::testSerialization()
{
	QFETCH(QVariantHash, properties);
	QFETCH(QList<DummySerializationHelper::SerInfo>, serData);
	QFETCH(int, type);
	QFETCH(QVariant, data);
	QFETCH(QJsonValue, result);

	helper->properties = properties;
	helper->serData = serData;

	try {
		if(result.isUndefined())
			QVERIFY_EXCEPTION_THROWN(converter()->serialize(type, data, helper), QJsonSerializationException);
		else {
			auto res = converter()->serialize(type, data, helper);
			QCOMPARE(res, result);
		}
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

void TypeConverterTestBase::testDeserialization_data()
{
	QTest::addColumn<QVariantHash>("properties");
	QTest::addColumn<QList<DummySerializationHelper::SerInfo>>("deserData");
	QTest::addColumn<QObject*>("parent");
	QTest::addColumn<int>("type");
	QTest::addColumn<QVariant>("result");
	QTest::addColumn<QJsonValue>("data");

	addCommonSerData();
	addDeserData();
}

void TypeConverterTestBase::testDeserialization()
{
	QFETCH(QVariantHash, properties);
	QFETCH(QList<DummySerializationHelper::SerInfo>, deserData);
	QFETCH(QObject*, parent);
	QFETCH(int, type);
	QFETCH(QJsonValue, data);
	QFETCH(QVariant, result);

	helper->properties = properties;
	helper->deserData = deserData;
	helper->expectedParent = parent;

	try {
		if(!result.isValid())
			QVERIFY_EXCEPTION_THROWN(converter()->deserialize(type, data, this, helper), QJsonDeserializationException);
		else {
			auto res = converter()->deserialize(type, data, this, helper);
			QVERIFY(res.convert(type));
			SELF_COMPARE(type, res, result);
		}
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}
