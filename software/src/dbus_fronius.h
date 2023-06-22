#ifndef DBUS_TEST2_H
#define DBUS_TEST2_H

#include "gateway_interface.h"
#include "ve_service.h"

class InverterGateway;
class InverterMediator;
class Settings;
class VeQItem;

struct DeviceInfo;

/*!
 * Ties all functionality together in the dbus-fronius process.
 *
 * It acts as a composite GatewayInterface, as a D-Bus publisher of the
 * 'com.victronenergy.fronius` service, and as createor of the `InverterMediator` classes.
 */
class DBusFronius : public VeService, public GatewayInterface
{
	Q_OBJECT
public:
	DBusFronius(QObject *parent = 0);

	void startDetection() override;

	int handleSetValue(VeQItem *item, const QVariant &variant) override;

private slots:
	void onSettingsInitialized();

	void onInverterFound(const DeviceInfo &deviceInfo);

	void onScanProgressChanged();

	void onAutoDetectChanged();

private:
	QList<InverterMediator *> mMediators;
	Settings *mSettings;
	VeQItem *mAutoDetect;
	VeQItem *mScanProgress;
	InverterGateway *mGateway;
};

#endif // DBUS_TEST2_H
