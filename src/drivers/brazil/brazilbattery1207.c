/*
 * brazilbattery.c
 *
 *  Created on: Jun 10, 2016
 *      Author: wagner
 */

#include "brazilbattery1207.h"

#include <math.h>
#include "apc.h"

/* ############## Considerações sobre baterias de chumbo-ácido ########### */

/*
 * Para calcular o timeleft da bateria foi feita uma interpolação cubica para descobrir
 * quais são os coeficientes que geraria uma curva similar aos datasheets disponíveis
 * na web. Esses coeficientes são os COEF_C1[]. A função calcTimeLeftC1() calcula o timeleft
 * para uma taxa de descarga de 1C. Para aproximar o timeleft com outras cargas é feita
 * a corelação entre as tensões com uma carga diferente em calcTimeLeft().
 *
 * Nem todos os modelos de nobreak utilizam essa bateria. Para gerar uma compatibilidade entre
 * os modelos e uma bateria 1207 deve-se alterar o número de bateria em série (altera a tensão)
 * e o número de baterias em paralelo (altera a corrente da bateria). Todos os modelos
 * utilizam baterias de chumbo-ácido, portanto, há uma equivalência entre os modelos.
 *
 * Em resumo:
 * 1) As tensões inícial e final da bateria é uma função da carga na calcTimeLeft()
 * 2) O timeleft resultante não é diretamente proporcional a carga. Por esse motivo
 *    o timeleft da curva C1 é dividido pela carga (load) elevada por um fator de 1.2.
 *
 *
 *
 */

/* From: https://www.robocore.net/upload/ManualTecnicoBateriaUnipower.pdf, data 2016-04-13
 *
 * >>> Conceitos de capacidade e de utilização:
 *
 * A capacidade de armazenamento de energia de uma bateria é medida através da multiplicação da corrente de
 * descarga pelo tempo de autonomia, sendo dado em Ampére-hora (Ah).
 *
 * Exemplo: uma bateria que, submetida a uma corrente de descarga de 5A permitir autonomia de 20 horas, será uma
 * bateria de:100Ah.
 *
 * Devemos observar que, ao contrário das baterias primárias (não recarregáveis), as baterias recarregáveis não podem
 * ser descarregadas até 0V pois isto leva ao final prematuro da vida da bateria. Na verdade elas tem um limite até onde
 * podem ser descarregadas, chamado de tensão de corte. Descarregar a bateria abaixo deste limite reduz a vida útil da
 * bateria e provoca o cancelamento da garantia.
 *
 * As baterias ditas 12V, por exemplo, devem operar de 13,8V (tensão a plena carga), até 10,5V (tensão de corte),
 * quando 100% de sua capacidade terá sido utilizada, e é este o tempo que deve ser medido como autonomia da
 * bateria.
 *
 * Como o comportamento das baterias não é linear, isto é, quando maior a corrente de descarga menor será a
 * autonomia e a capacidade, não é correto falar em uma bateria de 100Ah. Devemos falar, por exemplo, em uma bateria
 * 100Ah padrão de descarga 20 horas, com tensão de corte 10,5V, o que também pode ser escrito como 100Ah C20
 * Vcorte=10,5V. Esta bateria permitirá descarga de 100 / 20 = 5A durante 20 horas, quando a bateria irá atingir 10,5V.
 * Outro fator importante é a temperatura de operação da bateria, pois sua capacidade e vida útil dependem dela.
 * Usualmente as infomações são fornecidas supondo T=25°C ou T=20°C, que é a temperatura ideal para maximizar a
 * vida útil.
 *
 * Muitas vezes estes parâmetros são omitidos, dizendo-se apenas bateria de 100Ah, no entanto para fazer uma
 * comparação criteriosa entre diferentes baterias, deve-se certificar-se que a autonomia exibida foi dada considerando
 * os mesmos parâmetros de tensão de corte, temperatura e padrão de descarga em horas.
 * Da mesma forma que se expressa a capacidade de uma bateria em Ampére hora (Ah), podemos expressar em Watt
 * hora (Wh), que é a potência de descarga x tempo. Neste manual se encontram os dados de descarga das baterias
 * Unipower tanto em Ah como em Wh.
 *
 * As baterias chumbo-ácidas seladas Unipower são compostas através de elementos ou células de 2V, formando um
 * monobloco. Isto é, as baterias de 2V são compostas por um elemento, as de 6V por 3 elementos e as de 12V por 6
 * elementos. Observar que na tabela de descarga em Wh está descrita a capacidade de descarga de cada elemento da
 * bateria portanto, para se obter a capacidade Wh da bateria, deve-se multiplicar pelo número de elementos da bateria.
 * Já as tabelas em Ah exibem a capacidade da bateria como um todo.
 *
 * >>> Carga por Tensão Constante:
 *
 * A carga por tensão constante é o modo mais apropriado e comum para carregar a bateria Unipower.
 * Em flutuação: 2,25 ~ 2,30 V/elemento a 25 C
 * Para aplicações cíclicas: 2,40 ~ 2,45 V/elemento a 25 C
 * Corrente inicial limitada de 0,1 ~ 0,25C
 * Ao atingir a tensão de 2,30 V/elemento se obtém uma corrente residual de 5 mA/Ah.
 *
 */


const double BrazilBattery1207::COEF_C1[4] = {-5976.6571433071,1695.1547620352,-160.3571428695,5.0595238099};
const double BrazilBattery1207::COEF_VI[4] = {12.760511,-0.669034,0.121307,-0.012784};
const double BrazilBattery1207::COEF_VF[4] = {10.127273,0.768182,-0.563636,0.068182};

BrazilBattery1207::BrazilBattery1207(){
	this->battery_1207_serie = 0;
	this->battery_1207_parallel = 0;
	this->battery_expander = 0;
}
BrazilBattery1207::~BrazilBattery1207(){
}
void BrazilBattery1207::setBatteryCount(double battery_1207_serie, double battery_1207_parallel){
	if(battery_1207_serie < 1)battery_1207_serie = 1;
	this->battery_1207_serie = battery_1207_serie;
	if(battery_1207_parallel < 1)battery_1207_parallel = 1;
	this->battery_1207_parallel = battery_1207_parallel;
}
void BrazilBattery1207::setExpander(double ampers_per_hour_nominal){
	if(ampers_per_hour_nominal < 0)ampers_per_hour_nominal = 0;
	this->battery_expander = ampers_per_hour_nominal;
}
double BrazilBattery1207::getBatteryVoltageNom(){
	return BrazilBattery1207::VOLTAGE_12V_REF * this->battery_1207_serie;
}
double BrazilBattery1207::calcBatteryLoadC1(double act_power){
	double batpower = this->getBatteryVoltageNom() * BrazilBattery1207::AMPER_HOUR_C1 * this->battery_1207_parallel;
	batpower += this->getBatteryVoltageNom() * this->battery_expander / (BrazilBattery1207::AMPER_HOUR_C1 / 7);
	return act_power / batpower;
}

double BrazilBattery1207::calcTimeLeft(double load, double voltage){
	double vload_max = this->calcVoltageMax(load);
	double vload_min = this->calcVoltageMin(load);

	double vc1_max = this->calcVoltageMax(1);
	double vc1_min = this->calcVoltageMin(1);

	if(voltage < vload_min) voltage = vload_min;
	if(voltage > vload_max) voltage = vload_max;
	double prop = (voltage - vload_min) / (vload_max - vload_min);
	double voltage_c1 = vc1_min + (vc1_max - vc1_min) * prop;

	double timeleft = BrazilBattery1207::TIMELEFT_MUL * this->calcTimeLeftC1(voltage_c1) / pow(load,BrazilBattery1207::TIMELEFT_POW);
	return timeleft;
}
double BrazilBattery1207::calcTimeLeftPeukert(double load){
	if(load < BrazilBattery1207::LOAD_MIN) load = BrazilBattery1207::LOAD_MIN;
	double timeleft = BrazilBattery1207::PEUKERT_MUL * (BrazilBattery1207::AMPER_HOUR_C1/pow(load*BrazilBattery1207::AMPER_HOUR_C1,BrazilBattery1207::PEUKERT_POW)) * 60;
	return timeleft;
}
double BrazilBattery1207::calcLevel(double load, double voltage){
	double timeleft = this->calcTimeLeft(load, voltage);
	double voltage_max = this->calcVoltageMax(load);
	double timeleft_max = this->calcTimeLeft(load, voltage_max);
	return 100 * (timeleft / timeleft_max);
}

double BrazilBattery1207::calcTimeLeftC1(double voltage){
	double voltage_12v = voltage / this->battery_1207_serie;
	if(voltage_12v < BrazilBattery1207::VOLTAGE_12V_C1_MIN) voltage_12v = BrazilBattery1207::VOLTAGE_12V_C1_MIN;
	if(voltage_12v > BrazilBattery1207::VOLTAGE_12V_C1_MAX) voltage_12v = BrazilBattery1207::VOLTAGE_12V_C1_MAX;

	double res = 0;
	for(int i=0 ; i<BrazilBattery1207::COEF_C1_SIZE ; i++){
			res += BrazilBattery1207::COEF_C1[i]*pow(voltage_12v,i);
	}
	if(res < 0) res = 0;
	return res;
}

double BrazilBattery1207::calcVoltageMax(double load){
	if(load < BrazilBattery1207::LOAD_MIN) load = BrazilBattery1207::LOAD_MIN;
	if(load > BrazilBattery1207::LOAD_MAX) load = BrazilBattery1207::LOAD_MAX;

	double res = 0;
	for(int i=0 ; i<BrazilBattery1207::COEF_VI_SIZE ; i++){
			res += BrazilBattery1207::COEF_VI[i]*pow(load,i);
	}
	res *= this->battery_1207_serie;
	if(res < 0) res = 0;
	return res;
}
double BrazilBattery1207::calcVoltageMin(double load){
	if(load < BrazilBattery1207::LOAD_MIN) load = BrazilBattery1207::LOAD_MIN;
	if(load > BrazilBattery1207::LOAD_MAX) load = BrazilBattery1207::LOAD_MAX;

	double res = 0;
	for(int i=0 ; i<BrazilBattery1207::COEF_VF_SIZE ; i++){
			res += BrazilBattery1207::COEF_VF[i]*pow(load,i);
	}
	res *= this->battery_1207_serie;
	if(res < 0) res = 0;
	return res;
}



