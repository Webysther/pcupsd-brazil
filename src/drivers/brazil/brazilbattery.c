/*
 * brazilbattery.c
 *
 *  Created on: Jun 10, 2016
 *      Author: wagner
 */

#include <math.h>
#include "apc.h"
#include "brazilbattery.h"

/*
 * Para calcular o timeleft da bateria foi feita uma interpolação cubica para descobrir
 * quais são os coeficientes que geraria uma curva similar aos datasheets disponíveis
 * na web. Esses coeficientes são os COEF_C1[]. A função calcTimeLeftC1() calcula o timeleft
 * para uma taxa de descarga de 1C. Para aproximar o timeleft com outras cargas é feita
 * a corelação entre as tensões com uma carga diferente em calcTimeLeft().
 *
 * Em resumo:
 * 1) As tensões inícial e final da bateria é uma função da carga na calcTimeLeft()
 * 2) O timeleft resultante não é diretamente proporcional a carga. Por esse motivo
 *    o timeleft da curva C1 é dividido pela carga (load) elevada por um fator de 1.2.
 */

const double BrazilBattery::COEF_C1[4] = {-5976.6571433071,1695.1547620352,-160.3571428695,5.0595238099};
const double BrazilBattery::COEF_VI[4] = {12.760511,-0.669034,0.121307,-0.012784};
const double BrazilBattery::COEF_VF[4] = {10.127273,0.768182,-0.563636,0.068182};

BrazilBattery::BrazilBattery(){
	if(battery_12v_count < 1)battery_12v_count = 1;
	this->battery_12v_count = battery_12v_count;
}
BrazilBattery::~BrazilBattery(){
}
void BrazilBattery::setBatteryCount(int battery_12v_count){
	if(battery_12v_count < 1)battery_12v_count = 1;
	this->battery_12v_count = battery_12v_count;
}
double BrazilBattery::getBatteryVoltageNom(){
	return BrazilBattery::VOLTAGE_12V_REF * this->battery_12v_count;
}
double BrazilBattery::calcBatteryLoadC1(double act_power){
	return act_power / (this->getBatteryVoltageNom() * BrazilBattery::AMPER_HOUR_C1);
}

double BrazilBattery::calcTimeLeft(double load, double voltage){
	double vload_max = this->calcVoltageMax(load);
	double vload_min = this->calcVoltageMin(load);

	double vc1_max = this->calcVoltageMax(1);
	double vc1_min = this->calcVoltageMin(1);

	if(voltage < vload_min) voltage = vload_min;
	if(voltage > vload_max) voltage = vload_max;
	double prop = (voltage - vload_min) / (vload_max - vload_min);
	double voltage_c1 = vc1_min + (vc1_max - vc1_min) * prop;

	double timeleft = BrazilBattery::TIMELEFT_MUL * this->calcTimeLeftC1(voltage_c1) / pow(load,BrazilBattery::TIMELEFT_POW);
	return timeleft;
}
double BrazilBattery::calcTimeLeftPeukert(double load){
	if(load < BrazilBattery::LOAD_MIN) load = BrazilBattery::LOAD_MIN;
	double timeleft = BrazilBattery::PEUKERT_MUL * (BrazilBattery::AMPER_HOUR_C1/pow(load*BrazilBattery::AMPER_HOUR_C1,BrazilBattery::PEUKERT_POW)) * 60;
	return timeleft;
}
double BrazilBattery::calcLevel(double load, double voltage){
	double voltage_i = this->calcVoltageMax(load);
	double voltage_f = this->calcVoltageMin(load);
	return ((voltage - voltage_f) / (voltage_i - voltage_f)) * 100;
}

double BrazilBattery::calcTimeLeftC1(double voltage){
	double voltage_12v = voltage / this->battery_12v_count;
	if(voltage_12v < BrazilBattery::VOLTAGE_12V_C1_MIN) voltage_12v = BrazilBattery::VOLTAGE_12V_C1_MIN;
	if(voltage_12v > BrazilBattery::VOLTAGE_12V_C1_MAX) voltage_12v = BrazilBattery::VOLTAGE_12V_C1_MAX;

	double res = 0;
	for(int i=0 ; i<BrazilBattery::COEF_C1_SIZE ; i++){
			res += BrazilBattery::COEF_C1[i]*pow(voltage_12v,i);
	}
	if(res < 0) res = 0;
	return res;
}

double BrazilBattery::calcVoltageMax(double load){
	if(load < BrazilBattery::LOAD_MIN) load = BrazilBattery::LOAD_MIN;
	if(load > BrazilBattery::LOAD_MAX) load = BrazilBattery::LOAD_MAX;

	double res = 0;
	for(int i=0 ; i<BrazilBattery::COEF_VI_SIZE ; i++){
			res += BrazilBattery::COEF_VI[i]*pow(load,i);
	}
	res *= this->battery_12v_count;
	if(res < 0) res = 0;
	return res;
}
double BrazilBattery::calcVoltageMin(double load){
	if(load < BrazilBattery::LOAD_MIN) load = BrazilBattery::LOAD_MIN;
	if(load > BrazilBattery::LOAD_MAX) load = BrazilBattery::LOAD_MAX;

	double res = 0;
	for(int i=0 ; i<BrazilBattery::COEF_VF_SIZE ; i++){
			res += BrazilBattery::COEF_VF[i]*pow(load,i);
	}
	res *= this->battery_12v_count;
	if(res < 0) res = 0;
	return res;
}



