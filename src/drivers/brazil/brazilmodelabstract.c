/*
 * brazilmodels.c
 *
 *  Created on: Nov 8, 2015
 *      Author: wagner popov dos santos
 */

/*
 * Copyright (C) 2001-2006 Kern Sibbald
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General
 * Public License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */


/* ############## Considerações sobre baterias de chumbo-ácido ########### */

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

/*discharging lead acid battery with CONSTANT POWER discharge:
   Voltage\Time       5MIN       10MIN      15MIN      30MIN       1HR        2HR        3HR        4HR        5HR        8HR       10HR       20HR
       9.60V          2828       2102       1677       1052        601        367        256        212        179        122        101       54.3
       10.0V          2772       2073       1650       1039        599        365        256        212        178        122        101       53.3
       10.2V          2741       1984       1632       1032        595        362        255        211        178        121        100       52.4
       10.5V          2495       1847       1556       1025        589        360        254        209        175        120        99        51.4
       10.8V          2272       1703       1439       1008        579        355        247        205        172        118        98        50.4
       11.1V          1996       1539       1295        950        554        342        236        195        165        113        95        47.4
 *
 */




/* Microsol codes
 *	int SOLIS = 1;
 *	int RHINO = 2;
 *	int STAY = 3;
 *	int SOLIS_LI_700 = 169;
 *	int SOLIS_M11 = 171;
 *	int SOLIS_M15 = 175;
 *	int SOLIS_M14 = 174;
 *	int SOLIS_M13 = 173;
 *	int SOLISDC_M14 = 201;
 *	int SOLISDC_M13 = 206;
 *	int SOLISDC_M15 = 207;
 *	int CABECALHO_RHINO = 194;
 *	int PS800 = 185;
 *	int STAY1200_USB = 186;
 *	int PS350_CII = 184;
 *	int PS2200 = 187;
 *	int PS2200_22 = 188;
 *	int STAY700_USB = 189;
 *	int BZ1500 = 190;
 *
 */

#include "apc.h"
#include "brazilmodel.h"
#include "brazilbattery.h"

BrazilModelAbstract::BrazilModelAbstract(unsigned char model){
	this->_eventssize = 0;
	this->vmodel = model;
	this->lock = false;
	this->regulating_relay = 0;
	this->bat = new BrazilBattery();

	memset (this->_timeleft,0, sizeof this->_timeleft);

	this->_load[0] = 0.4; // 0.4C (fator de descarga de 0.4C/hora)
	this->_load[1] = 0.6; // ...
	this->_load[2] = 1.0; // ...
	this->_load[3] = 2.0; // ...
	this->_load[4] = 3.0; // ...

	this->_voltage[0] =  9.00;
	this->_voltage[1] =  9.25;
	this->_voltage[2] =  9.50;
	this->_voltage[3] =  9.75;
	this->_voltage[4] =  10.00;
	this->_voltage[5] =  10.25;
	this->_voltage[6] =  10.50;
	this->_voltage[7] =  10.75;
	this->_voltage[8] =  11.00;
	this->_voltage[9] =  11.25;
	this->_voltage[10] = 11.50;
	this->_voltage[11] = 11.75;
	this->_voltage[12] = 12.00;
	this->_voltage[13] = 12.25;
	this->_voltage[14] = 12.50;

	this->_timeleft[0][0] = 0;
	this->_timeleft[0][1] = 0;
	this->_timeleft[0][2] = 0;
	this->_timeleft[0][3] = 0;
	this->_timeleft[0][4] = 0;
	this->_timeleft[0][5] = 0;
	this->_timeleft[0][6] = 0;
	this->_timeleft[0][7] = 0;
	this->_timeleft[0][8] = 0;
	this->_timeleft[0][9] = 0;
	this->_timeleft[0][10] = 0;
	this->_timeleft[0][11] = 1.19;
	this->_timeleft[0][12] = 22.69;
	this->_timeleft[0][13] = 44.19;
	this->_timeleft[0][14] = 65.69;

	this->_timeleft[1][0] = 0;
	this->_timeleft[1][1] = 0;
	this->_timeleft[1][2] = 0;
	this->_timeleft[1][3] = 0;
	this->_timeleft[1][4] = 0;
	this->_timeleft[1][5] = 0;
	this->_timeleft[1][6] = 0;
	this->_timeleft[1][7] = 0;
	this->_timeleft[1][8] = 6.29;
	this->_timeleft[1][9] = 13.21;
	this->_timeleft[1][10] = 20.13;
	this->_timeleft[1][11] = 27.04;
	this->_timeleft[1][12] = 33.96;
	this->_timeleft[1][13] = 40.88;
	this->_timeleft[1][14] = 47.8;

	this->_timeleft[2][0] = 0;
	this->_timeleft[2][1] = 0;
	this->_timeleft[2][2] = 0;
	this->_timeleft[2][3] = 0;
	this->_timeleft[2][4] = 1.44;
	this->_timeleft[2][5] = 3.77;
	this->_timeleft[2][6] = 6.1;
	this->_timeleft[2][7] = 8.43;
	this->_timeleft[2][8] = 10.76;
	this->_timeleft[2][9] = 13.09;
	this->_timeleft[2][10] = 15.42;
	this->_timeleft[2][11] = 17.75;
	this->_timeleft[2][12] = 20.08;
	this->_timeleft[2][13] = 22.41;
	this->_timeleft[2][14] = 24.74;

	this->_timeleft[3][0] = 0;
	this->_timeleft[3][1] = 0;
	this->_timeleft[3][2] = 0;
	this->_timeleft[3][3] = 0;
	this->_timeleft[3][4] = 0;
	this->_timeleft[3][5] = 0.83;
	this->_timeleft[3][6] = 2;
	this->_timeleft[3][7] = 3.18;
	this->_timeleft[3][8] = 4.35;
	this->_timeleft[3][9] = 5.52;
	this->_timeleft[3][10] = 6.69;
	this->_timeleft[3][11] = 7.86;
	this->_timeleft[3][12] = 9.04;
	this->_timeleft[3][13] = 10.21;
	this->_timeleft[3][14] = 11.38;

	this->_timeleft[4][0] = 0.28;
	this->_timeleft[4][1] = 0.78;
	this->_timeleft[4][2] = 1.28;
	this->_timeleft[4][3] = 1.78;
	this->_timeleft[4][4] = 2.29;
	this->_timeleft[4][5] = 2.79;
	this->_timeleft[4][6] = 3.29;
	this->_timeleft[4][7] = 3.79;
	this->_timeleft[4][8] = 4.3;
	this->_timeleft[4][9] = 4.8;
	this->_timeleft[4][10] = 5.3;
	this->_timeleft[4][11] = 5.8;
	this->_timeleft[4][12] = 6.31;
	this->_timeleft[4][13] = 6.81;
	this->_timeleft[4][14] = 7.31;
}
BrazilModelAbstract::~BrazilModelAbstract(){
	delete this->bat;
}
BrazilModelAbstract *BrazilModelAbstract::newInstance(unsigned char model){
	Dmsg(50, "Instancing specific model, number %03u.\n",model);
	BrazilModelAbstract *value = 0;
	switch(model){
	case 185:
		value = new BrazilModelBackUPS800(model);
		break;
	case 186:
		value = new BrazilModelBackUPS1200(model);
		break;
	case 187:
		value = new BrazilModelBackUPS2200(model);
		break;
	case 188:
		value = new BrazilModelBackUPS2200_22(model);
		break;
	case 189:
		value = new BrazilModelBackUPS700(model);
		break;
	case 190:
		value = new BrazilModelBackUPS1500(model);
		break;
	default:
		value = 0;
		break;
	}
	value->bat->setBatteryCount(value->getBatteryCount());
	return value;
}
int BrazilModelAbstract::testBuffer(unsigned char *buffer, unsigned int datasize){
	unsigned int checksum = 0;

	switch(buffer[0]){
	case 185:
	case 186:
	case 187:
	case 188:
	case 189:
	case 190:
		if(datasize == BrazilModelBackUPS::MSGLEN){
			for(int i=0 ; i < 23 ; i++){
				checksum += buffer[i];
			}
			if(checksum > 0 && buffer[23] == checksum % 256 && buffer[24] == 254){
				return 1;
			}else{
				return -1;
			}
		}else{
			return 0;
		}
		break;
	default:
		return -1;
	}



	return 0;
}

double BrazilModelAbstract::getBatteryVoltageNom(){
	return this->bat->getBatteryVoltageNom();
}
double BrazilModelAbstract::getBatteryLevel(){
	if(this->isLineOn()){
		return 100;
	}else{
		return this->bat->calcLevel(this->getBatteryLoad(), this->getBatteryVoltage());
	}
}

double BrazilModelAbstract::getBatteryTimeLeft(){
	if(this->isLineOn()){
		/*
		 * Caso o nobreak esteja alimentado, o único parâmetro que podemos utilizar é a corrente. Já que a tensão na bateria
		 * é a tensão de flutuação (carregamento constante. Nessa condição vamos utilizar a "Peukert's law". Cabe uma observação
		 * de que a Peukert's law só é uma boa aproximação com descarregamento constante.
		 *
		 */
		return bat->calcTimeLeftPeukert(this->getBatteryLoad());
	}else{
		return bat->calcTimeLeft(this->getBatteryLoad(),this->getBatteryVoltage());
	}
}
double BrazilModelAbstract::getBatteryVoltageExpectedInitial(){
	return this->bat->calcVoltageMax(this->getBatteryLoad());
}
double BrazilModelAbstract::getBatteryVoltageExpectedFinal(){
	return this->bat->calcVoltageMin(this->getBatteryLoad());
}
double BrazilModelAbstract::getBatteryLoad(){
	return this->bat->calcBatteryLoadC1(this->getOutputActivePower());
}

double BrazilModelAbstract::getBatteryVoltageExpected(double minutes){
	double load = this->getBatteryLoad();

	double voltage_max = bat->calcVoltageMax(load);
	double voltage_min = bat->calcVoltageMin(load);
	double timeleft_full = bat->calcTimeLeft(load, voltage_max);
	double timeleft_target = timeleft_full - minutes;
	double voltage=voltage_max;
	while((bat->calcTimeLeft(load, voltage) > timeleft_target) && (voltage > voltage_min)){
		voltage -= 0.1;
	}
	return voltage;
}

void BrazilModelAbstract::setBufferLock(){
	this->lock = true;
}

void BrazilModelAbstract::setBufferUnlock(){
	this->lock = false;
}

