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

BrazilModelAbstract::BrazilModelAbstract(unsigned char model){
	this->_eventssize = 0;
	this->vmodel = model;
	this->lock = false;
	this->regulating_relay = 0;


	memset (this->_minutes,0, sizeof this->_minutes);
	/*
	 * Curve from file bateria-chumbo-acido-curva-descarregamento.jpg
	 */

	// fatores de descarga. Para uma bateria de 12V 7A, o fator = 1 representa 84W por 1 hora.
	this->_load[2] = 0.4; // 0.4C (fator de descarga de 0.4 A/hora
	this->_load[3] = 0.6; // ...
	this->_load[4] = 1.0; // ...
	this->_load[5] = 2.0; // ...
	this->_load[6] = 3.0; // ...

	this->_low[0] = 11.75;
	this->_low[1] = 10.8;
	this->_low[2] = 10.0;
	this->_low[3] = 10.0;
	this->_low[4] = 9.0;

	this->_battery_v0[0] = 12.60;
	this->_battery_v0[1] = 12.35;
	this->_battery_v0[2] = 12.20;
	this->_battery_v0[3] = 11.75;
	this->_battery_v0[4] = 11.20;

	this->_curve[0][0] = 12.52;
	this->_curve[0][1] = 12.48;
	this->_curve[0][2] = 12.42;
	this->_curve[0][3] = 12.35;
	this->_curve[0][4] = 12.30;
	this->_curve[0][5] = 12.20;
	this->_curve[0][6] = 12.10;
	this->_curve[0][7] = 11.75;

	this->_curve[1][0] = 12.35;
	this->_curve[1][1] = 12.25;
	this->_curve[1][2] = 12.15;
	this->_curve[1][3] = 12.10;
	this->_curve[1][4] = 11.92;
	this->_curve[1][5] = 11.85;
	this->_curve[1][6] = 11.65;
	this->_curve[1][7] = 11.50;
	this->_curve[1][8] = 11.25;
	this->_curve[1][9] = 10.8;

	this->_curve[2][0] = 12.10;
	this->_curve[2][1] = 11.90;
	this->_curve[2][2] = 11.70;
	this->_curve[2][3] = 11.40;
	this->_curve[2][4] = 10.00;

	this->_curve[3][0] = 11.70;
	this->_curve[3][1] = 11.70;
	this->_curve[3][2] = 11.65;
	this->_curve[3][3] = 11.60;
	this->_curve[3][4] = 11.55;
	this->_curve[3][5] = 11.50;
	this->_curve[3][6] = 11.45;
	this->_curve[3][7] = 11.40;
	this->_curve[3][8] = 11.30;
	this->_curve[3][9] = 11.20;
	this->_curve[3][10] = 10.0;

	this->_curve[4][0] = 11.20;
	this->_curve[4][1] = 11.10;
	this->_curve[4][2] = 10.95;
	this->_curve[4][3] = 10.80;
	this->_curve[4][4] = 10.50;
	this->_curve[4][5] = 9.0;

	this->_minutes[0][0] = 80;
	this->_minutes[0][1] = 70;
	this->_minutes[0][2] = 60;
	this->_minutes[0][3] = 50;
	this->_minutes[0][4] = 40;
	this->_minutes[0][5] = 30;
	this->_minutes[0][6] = 20;
	this->_minutes[0][7] = 10;

	this->_minutes[1][0] = 50;
	this->_minutes[1][1] = 45;
	this->_minutes[1][2] = 40;
	this->_minutes[1][3] = 35;
	this->_minutes[1][4] = 30;
	this->_minutes[1][5] = 25;
	this->_minutes[1][6] = 20;
	this->_minutes[1][7] = 15;
	this->_minutes[1][8] = 10;
	this->_minutes[1][9] = 5;

	this->_minutes[2][0] = 25;
	this->_minutes[2][1] = 20;
	this->_minutes[2][2] = 15;
	this->_minutes[2][3] = 10;
	this->_minutes[2][4] = 5;

	this->_minutes[3][0] = 11;
	this->_minutes[3][1] = 10;
	this->_minutes[3][2] = 9;
	this->_minutes[3][3] = 8;
	this->_minutes[3][4] = 7;
	this->_minutes[3][5] = 6;
	this->_minutes[3][6] = 5;
	this->_minutes[3][7] = 4;
	this->_minutes[3][8] = 3;
	this->_minutes[3][9] = 2;
	this->_minutes[3][10] = 1;

	this->_minutes[4][0] = 6;
	this->_minutes[4][1] = 5;
	this->_minutes[4][2] = 4;
	this->_minutes[4][3] = 3;
	this->_minutes[4][4] = 2;
	this->_minutes[4][5] = 1;
}
BrazilModelAbstract::~BrazilModelAbstract(){
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
	return this->getBatteryCount() * 12;
}
double BrazilModelAbstract::getBatteryLevel(){
	if(this->isLineOn()){
		if(this->isCharging()){
			return 0.7;
		}else{
			return 1.0;
		}
	}
	double level = this->getBatteryTimeLeft() / this->_minutes[this->getBatteryLoadIndex()][0];
	return (level > 1 ? 1 : level);
}
int BrazilModelAbstract::getBatteryLoadIndex(){
	double batload = this->getOutputActivePower() / (12 * 7 * this->getBatteryCount());
	int iload = 0;
	for(int i=0 ; i<4 ; i++){
		if(batload > this->_load[i]){
			iload = i;
		}
	}
	Dmsg(50, "GetBatteryLoad: %03.2f; Index: %1d.\n", batload, iload);
	return iload;
}

double BrazilModelAbstract::getBatteryVoltageExpectedAfter(double minutes){
	Dmsg(50, "getBatteryVoltage(minutes %03.2d)!\n",minutes);

	int iload = this->getBatteryLoadIndex();

	int minleft = this->_minutes[iload][0] - minutes;
	if(minleft <= 0){
		return this->_low[iload] * this->getBatteryCount();
	}

	for(int i=0 ; i<10 ; i++){
		if(i > 10){
			return this->_curve[iload][i] * this->getBatteryCount();
		}else{
			if(minleft >= this->_minutes[iload][i+1]){
				double volt = this->_curve[iload][i+1];
				/*
				 * Calculate linear proportion of volt with 2 values of voltage:
				 */
				if((this->_minutes[iload][i] - this->_minutes[iload][i+1]) > 0){
					volt += ((this->_curve[iload][i] - this->_curve[iload][i+1]) * (minleft - this->_minutes[iload][i+1]) / (this->_minutes[iload][i] - this->_minutes[iload][i+1]));
					return volt * this->getBatteryCount();
				}else{
					return volt * this->getBatteryCount();
				}
			}
		}
	}
	return this->_low[iload];
}


double BrazilModelAbstract::getBatteryTimeLeft(){
	Dmsg(50, "getBatteryTimeLeft!\n");

	int iload = this->getBatteryLoadIndex();

	double bat12v = this->getBatteryVoltage() / this->getBatteryCount();   // voltage of one 12V battery
	double voltage0 = this->_curve[iload][0];
	double voltage1 = this->_curve[iload][1];
	double timeleft0 = this->_minutes[iload][0];
	double timeleft1 = this->_minutes[iload][1];
	if(bat12v > this->_low[iload]){
		for(int i=0 ; i<10 ; i++){
			if(bat12v < this->_curve[iload][i]){
				voltage0 = this->_curve[iload][i];
				voltage1 = this->_curve[iload][i+1];
				timeleft0 = this->_minutes[iload][i];
				timeleft1 = this->_minutes[iload][i+1];
			}
		}
		Dmsg(50, "bat12v: %3.2f; "
				"voltage0: %3.2f; "
				"voltage1: %3.2f; "
				"timeleft0: %3.2f; "
				"timeleft1: %3.2f; "
				"iload: %1d.\n", bat12v, voltage0, voltage1, timeleft0, timeleft1, iload);

		double timeleft = timeleft1;
		if((voltage0>voltage1) && (bat12v < voltage0) && (bat12v > voltage1)){
			timeleft = timeleft1 + (timeleft0 - timeleft1) * ((bat12v - voltage1) / (voltage0 - voltage1));
		}
		Dmsg(50, "timeleft: %3.2f.\n", timeleft);
		return timeleft;

	}else{
		return 0;
	}
}



void BrazilModelAbstract::setBufferLock(){
	this->lock = true;
}

void BrazilModelAbstract::setBufferUnlock(){
	this->lock = false;
}

