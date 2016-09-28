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

	void setBattery(double voltage,double current,double current_expander);
	double getBatteryVoltageNom();
	double getBatteryCurrentNom();
	double getBatteryCurrentC1Nom();
	double getBatteryPowerC1Nom();

	double calcBatteryLoadC1(double act_power);
	double calcVoltageMax(double battery_load);
	double calcVoltageMin(double battery_load);
	double calcTimeLeft(double battery_load, double voltage);
	double calcTimeLeftPeukert(double battery_load);
	double calcLevel(double battery_load, double voltage);


protected:
	double calcTimeLeftC1(double voltage);

	static const double VOLTAGE_12V_REF = 12.0;
	static const double VOLTAGE_12V_C1_MIN = 10.4;
	static const double VOLTAGE_12V_C1_MAX = 12.2;
	static const double LOAD_MIN = 0.6;
	static const double LOAD_MAX = 5.0;
	static const double PEUKERT_POW = 1.578;
	static const double PEUKERT_MUL = 1.24;
	static const double TIMELEFT_POW = 1.58;
	static const double TIMELEFT_MUL = 1.2;

	static const double CURRENT_RATE_C1_C20 = 0.74;

	static const int COEF_1207_C1_SIZE = 4;
	static const int COEF_1207_VI_SIZE = 4;
	static const int COEF_1207_VF_SIZE = 4;
	static const double COEF_1207_C1[];
	static const double COEF_1207_VI[];
	static const double COEF_1207_VF[];

	double battery_voltage_nom;
	double battery_current_nom;
	double battery_current_expander_nom;
private:

};

#endif /* SRC_DRIVERS_BRAZIL_BRAZILBATTERY_H_ */
