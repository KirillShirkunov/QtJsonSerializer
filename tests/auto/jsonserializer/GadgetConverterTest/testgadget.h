#ifndef TESTGADGET_H
#define TESTGADGET_H

#include <QtCore/QObject>

class TestGadget
{
	Q_GADGET

	Q_PROPERTY(int key MEMBER key)
	Q_PROPERTY(double value MEMBER value)
	Q_PROPERTY(int hidden MEMBER hidden STORED false)

public:
	TestGadget(int key = 0, double value = 0.0, int hidden = 0);

	int key;
	double value;
	int hidden;

	bool operator==(const TestGadget &other) const;
};

Q_DECLARE_METATYPE(TestGadget)
Q_DECLARE_METATYPE(TestGadget*)

#endif // TESTGADGET_H
