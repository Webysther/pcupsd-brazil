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
		return bat->calcTimeLeftPeukert(this->getBatteryLoad())*this->getInverterEfficiency();
	}else{
		return bat->calcTimeLeft(this->getBatteryLoad(),this->getBatteryVoltage())*this->getInverterEfficiency();
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

void BrazilModelAbstract::setBufferLock(){
	this->lock = true;
}

void BrazilModelAbstract::setBufferUnlock(){
	this->lock = false;
}

