#include <QNetworkInterface>
#include <QHostAddress>
#include <QTimer>
#include <QByteArray>
#include <QUdpSocket>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	#include <QNetworkDatagram>
	const enum QHostAddress::SpecialAddress AnyIPv4 = QHostAddress::AnyIPv4;
#else
	const enum QHostAddress::SpecialAddress AnyIPv4 = QHostAddress::Any;
#endif
#include "fronius_udp_detector.h"
#include "compat.h"

FroniusUdpDetector::FroniusUdpDetector(QObject *parent) :
	QObject(parent),
	mTimeout(new QTimer(this)),
	mUdpSocket(new QUdpSocket(this))
{
	mTimeout->setSingleShot(true);
	connect(mTimeout, SIGNAL(timeout()), this, SIGNAL(finished()));

	// Fronius inverters respond on this port
	mUdpSocket->bind(AnyIPv4, 50050, QUdpSocket::ReuseAddressHint);
	connect(mUdpSocket, SIGNAL(readyRead()), this, SLOT(responseReceived()));
}

void FroniusUdpDetector::reset()
{
	mDevicesFound.clear();
}

void FroniusUdpDetector::start()
{
	// Probe for inverters by broadcasting on 50049
	QByteArray dgram = "{\"GetFroniusLoggerInfo\":\"all\"}";
	mUdpSocket->writeDatagram(dgram, QHostAddress::Broadcast, 50049);

	// Give the inverters 5 seconds to respond before blindly moving on
	mTimeout->start(5000);
}

void FroniusUdpDetector::responseReceived()
{
	while (mUdpSocket->hasPendingDatagrams()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		QNetworkDatagram datagram = mUdpSocket->receiveDatagram();
		mDevicesFound.insert(datagram.senderAddress());
#else
		QHostAddress addr;
		// if length is zero packet is discarded. We care only about the
		// address.
		mUdpSocket->readDatagram(0, 0, &addr);
		mDevicesFound.insert(addr);
#endif
	}
}

QList<QHostAddress> FroniusUdpDetector::devicesFound()
{
	return setToList<QHostAddress>(mDevicesFound);
}
