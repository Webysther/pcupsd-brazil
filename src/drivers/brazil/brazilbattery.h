/*
 * brazilbattery.h
 *
 *  Created on: Jun 10, 2016
 *      Author: wagner
 */

#ifndef SRC_DRIVERS_BRAZIL_BRAZILBATTERY_H_
#define SRC_DRIVERS_BRAZIL_BRAZILBATTERY_H_

class BrazilBattery
{
public:
	BrazilBattery();
	virtual ~BrazilBattery();

	void setBatteryCount(int battery_12v_count);
	double getBatteryVoltageNom();

	double calcBatteryLoadC1(double act_power);
	double calcVoltageMax(double load);
	double calcVoltageMin(double load);
	double calcTimeLeft(double load, double voltage);
	double calcTimeLeftPeukert(double load);
	double calcLevel(double load, double voltage);

	static const double VOLTAGE_12V_REF = 12.0;
	static const double VOLTAGE_12V_C1_MIN = 10.4;
	static const double VOLTAGE_12V_C1_MAX = 12.2;
	static const double AMPER_HOUR_C1 = 4.8;
	static const double LOAD_MIN = 0.6;
	static const double LOAD_MAX = 5.0;
	static const double CST_PEUKERT = 1.2;

protected:
	double calcTimeLeftC1(double voltage);

	static const int COEF_C1_SIZE = 4;
	static const int COEF_VI_SIZE = 4;
	static const int COEF_VF_SIZE = 4;
	static const double COEF_C1[];
	static const double COEF_VI[];
	static const double COEF_VF[];

	int battery_12v_count;
private:

};

#endif /* SRC_DRIVERS_BRAZIL_BRAZILBATTERY_H_ */