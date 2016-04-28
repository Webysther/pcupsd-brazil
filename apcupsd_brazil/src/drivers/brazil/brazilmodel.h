/*
 * brazilmodels.h
 *
 *  Created on: Nov 6, 2015
 *      Author: Wagner Popov dos Santos
 *
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-400VA-115V-Brazil/P-BZ400-BR">BZ400-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-600VA-115V-Brazil/P-BZ600-BR">BZ600-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-700VA-115V-Brazil/P-BZ700-BR">BZ700-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-400VA-115V-220V-Brazil/P-BZ400BI-BR">BZ400BI-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-600VA-115V-220V-Brazil/P-BZ600BI-BR">BZ600BI-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-700VA-115V-220V-Brazil/P-BZ700BI-BR">BZ700BI-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-700-VA-Bivolt-115V-4-Outlet-Voltage-Regulation-with-Isolation-iPower-Standard/P-IPOWER700STDBI">IPOWER700STDBI</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-1200-VA-Bivolt-115V-6-Outlet-Voltage-Regulation-Staytion-1200/P-STAYTION1200BI">STAYTION1200BI</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-1200-115V-220V-Brazil/P-BZ1200-BR">BZ1200-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-1400-VA-Bivolt-115V-6-Outlet-Voltage-Regulation-Telephone-Protection-Stay-1400/P-STAY1400BI">STAY1400BI</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-1500VA-115V-220V-Brazil/P-BZ1500BI-BR">BZ1500BI-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-1500VA-115V-220V-Brazil/P-BZ1500BI-BR">BZ1500BI-BR + (1)BZ24BP-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-1500VA-115V-220V-Brazil/P-BZ1500PBI-BR">BZ1500PBI-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-1500VA-115V-220V-Brazil/P-BZ1500PBI-BR">BZ1500PBI-BR + (1)BZ24BP-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-2200VA-115V-220V/P-BZ2200BI-BR">BZ2200BI-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-2200VA-115V-220V/P-BZ2200BI-BR">BZ2200BI-BR + (1)BZ24BP-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-2200VA-220V/P-BZ2200I-BR">BZ2200I-BR</a>
 *		<a href="http://www.apc.com/shop/br/bz/products/APC-Back-UPS-2200VA-220V/P-BZ2200I-BR">BZ2200I-BR + (1)BZ24BP-BR</a>
 *
 */

#ifndef SRC_DRIVERS_BRAZIL_BRAZILMODEL_H_
#define SRC_DRIVERS_BRAZIL_BRAZILMODEL_H_

enum DAYS_OF_WEEK{
	Sunday = 0,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday
};

class BrazilModelAbstract
{
public:
	BrazilModelAbstract(unsigned char model);
	virtual ~BrazilModelAbstract();

	static BrazilModelAbstract *newInstance(unsigned char model);

	static int testBuffer(unsigned char *buffer, unsigned int datasize);

	void setBufferLock();
	void setBufferUnlock();
	void setDischarging(bool discharging, time_t* start);
	bool getDischarging();

	virtual int testEvents(unsigned char *buffer, unsigned int datasize) = 0;
	virtual bool setBuffer(unsigned char *buffer, unsigned int datasize) = 0;
	virtual bool setEvents(unsigned char *buffer, unsigned int datasize) = 0;
	virtual int getEventsStr(char **out) = 0;

	virtual double getTemperature() = 0;
	virtual double getBatteryVoltage() = 0;
	virtual double getLineVoltage() = 0;
	virtual double getLineFrequency() = 0;
	virtual double getOutputVoltage() = 0;
	virtual double getOutputCurrent() = 0;
	virtual double getOutputActivePower() = 0;

	virtual bool isProgrammationSet() = 0;

	virtual bool isProgrammationDayOfWeek(DAYS_OF_WEEK day) = 0;
	virtual unsigned char getTurnOffHour() = 0;
	virtual unsigned char getTurnOffMinute() = 0;
	virtual unsigned char getTurnOnHour() = 0;
	virtual unsigned char getTurnOnMinute() = 0;
	virtual unsigned int getYear() = 0;
	virtual unsigned char getMonth() = 0;
	virtual unsigned char getDayOfMonth() = 0;
	virtual unsigned char getDayOfWeek() = 0;
	virtual unsigned char getHour() = 0;
	virtual unsigned char getMinute() = 0;
	virtual unsigned char getSecond() = 0;

	double getBatteryVoltageNom();
	double getBatteryLevel();
	double getBatteryTimeLeft();
	double getBatteryVoltageExpectedAfter(double minutes);
	virtual int getBatteryCount() = 0;

	virtual double getOutputPowerNom() = 0;
	virtual double getOutputActivePowerNom() = 0;
	virtual int getOutputVoltageNom() = 0;
	virtual int getLineVoltageNom() = 0;
	virtual int getLineVoltageMin() = 0;
	virtual int getLineVoltageMax() = 0;

	virtual int generateCmdTurnLineOn(unsigned char **cmd, bool turnon) = 0;
	virtual int generateCmdTurnOutputOn(unsigned char **cmd, bool turnon) = 0;
	virtual int generateCmdProgrammation(unsigned char **cmd, bool turnoff, unsigned int turnoff_minutes, bool turnon, unsigned int turnon_minutes) = 0;
	virtual int generateCmdShutdown(unsigned char **cmd) = 0;
	virtual int generateCmdGetEvents(unsigned char **cmd) = 0;

	virtual bool hasShutdownAuto() = 0;
	virtual char *getModelName() = 0;
	virtual bool isLineOn() = 0;
	virtual bool isOverload() = 0;
	virtual bool isCharging() = 0;
	virtual bool isLine220V() = 0;
	virtual bool isOutputOn() = 0;
	virtual bool isBatteryCritical() = 0;

	bool isBatteryVoltageLow(){
		return this->getBatteryVoltage() <= (this->getBatteryCount() * 10);
	}
	double getOutputPower(){
		return this->getOutputCurrent() * this->getOutputVoltage();
	}

	double getLoadPercent(){
		double load = (this->getOutputPower() / this->getOutputPowerNom())*100;
		return load;
	}
	double getLoadActivePowerPercent(){
		return (this->getOutputActivePower() / this->getOutputActivePowerNom())*100;
	}
	unsigned char getModelNumber(){
		return this->vmodel;
	}

	static const unsigned int BUFFERLEN = 200;

protected:
	int getBatteryLoadIndex();

	unsigned char _buffer[BrazilModelAbstract::BUFFERLEN];
	unsigned char _events[BrazilModelAbstract::BUFFERLEN];
	unsigned int _eventssize;
	unsigned char vmodel;
	bool lock;
	int regulating_relay;

	double _curve[5][12];
	unsigned char _minutes[5][12];
	double _load[5];
	double _low[5];
	double _battery_v0[5];


private:
};

class BrazilModelBackUPS: public BrazilModelAbstract
{
public:
	BrazilModelBackUPS(unsigned char model);
	virtual ~BrazilModelBackUPS();

	static BrazilModelBackUPS *Factory();

	virtual int testEvents(unsigned char *buffer, unsigned int datasize);
	virtual bool setBuffer(unsigned char *buffer, unsigned int datasize);
	virtual bool setEvents(unsigned char *buffer, unsigned int datasize);
	virtual int getEventsStr(char **out);

	virtual double getTemperature();
	virtual double getBatteryVoltage();
	virtual double getLineVoltage();
	virtual double getLineFrequency();
	virtual double getOutputVoltage();
	virtual double getOutputCurrent();
	virtual double getOutputActivePower();

	virtual bool isProgrammationSet();

	virtual bool isProgrammationDayOfWeek(DAYS_OF_WEEK day);
	virtual unsigned char getTurnOffHour();
	virtual unsigned char getTurnOffMinute();
	virtual unsigned char getTurnOnHour();
	virtual unsigned char getTurnOnMinute();
	virtual unsigned int getYear();
	virtual unsigned char getMonth();
	virtual unsigned char getDayOfMonth();
	virtual unsigned char getDayOfWeek();
	virtual unsigned char getHour();
	virtual unsigned char getMinute();
	virtual unsigned char getSecond();

	virtual bool isLineOn();
	virtual bool isOverload();
	virtual bool isCharging();
	virtual bool isLine220V();
	virtual bool isOutputOn();
	virtual bool isBatteryCritical();

	virtual int generateCmdTurnLineOn(unsigned char **cmd, bool turnon);
	virtual int generateCmdTurnOutputOn(unsigned char **cmd, bool turnon);
	virtual int generateCmdProgrammation(unsigned char **cmd, bool turnoff, unsigned int turnoff_minutes, bool turnon, unsigned int turnon_minutes);
	virtual int generateCmdShutdown(unsigned char **cmd);
	virtual int generateCmdGetEvents(unsigned char **cmd);

	static const unsigned char SUBTENSAO_ENTRADA = 1;
	static const unsigned char SOBRETENSAO_ENTRADA = 2;
	static const unsigned char RETORNO_REDE_ELETRICA = 3;
	static const unsigned char SOBRECARGA = 4;
	static const unsigned char BATERIAS_DESCARREGADAS = 5;
	static const unsigned char CHAVE_LIGADA = 6;
	static const unsigned char CHAVE_DESLIGADA = 7;
	static const unsigned char DESLIGAMENTO = 8;
	static const unsigned char INICIALIZACAO = 9;
	static const unsigned char DESLIGAMENTO_REMOTO = 10;
	static const unsigned char SUPER_AQUECIMENTO = 11;
	static const unsigned char DESLIGAMENTO_PROGAMADO = 12;
	static const unsigned char ACIONAMENTO_PROGRAMADO = 13;
	static const unsigned char SAIDA_SEM_CONSUMO = 14;
	static const unsigned char BATERIA_BAIXO = 15;
	static const unsigned char SAIDA_LIGADA = 16;
	static const unsigned char BATERIAS_CARREGADAS = 17;
	static const unsigned char SHUTDOWN = 18;
	static const unsigned char FALHA_NA_COMUNICACAO = 19;
	static const unsigned char RETORNO_DE_COMUNICACAO = 20;
	static const unsigned char FALHA_NA_REDE = 21;
	static const unsigned char FALHA_AUTO_TEST = 22;
	static const unsigned char FALHA_INVERSOR = 23;
	static const unsigned char ATIVA_BYPASS = 24;
	static const unsigned char DESATIVA_BYPASS = 25;
	static const unsigned char CARREGADOR_FALHO = 26;
	static const unsigned char BATERIA_FALHA = 27;
	static const unsigned char AUTO_DIAGNOSTICO_OK = 28;
	static const unsigned char AUTO_TESTE_BAT_OK = 29;
	static const unsigned char AUTO_TESTE_BAT_BOM = 30;
	static const unsigned char AUTO_TESTE_BAT_RUIM = 31;

	static const unsigned int MSGLEN = 25;
protected:
	virtual const unsigned int getRegulatingRelay();

	double TENSAO_ENTRADA_F1;
	double TENSAO_ENTRADA_F2;
	double TENSAO_BATERIA_F1;
	double TENSAO_BATERIA_F2;
	double CORRENTE_SAIDA_F1_MR;
	double CORRENTE_SAIDA_F2_MR;
	double CORRENTE_SAIDA_F1_MI;
	double CORRENTE_SAIDA_F2_MI;
	double TENSAO_SAIDA_F1_MR[8];
	double TENSAO_SAIDA_F2_MR[8];


private:
};


class BrazilModelBackUPS1200: public BrazilModelBackUPS
{
public:
	BrazilModelBackUPS1200(unsigned char model);

	static BrazilModelBackUPS1200 *Factory();

	double getOutputPowerNom();
	double getOutputActivePowerNom();
	int getOutputVoltageNom();
	int getLineVoltageNom();
	int getLineVoltageMin();
	int getLineVoltageMax();
	int getBatteryCount();

	bool hasShutdownAuto();
	char *getModelName();

protected:

private:
};


class BrazilModelBackUPS1500: public BrazilModelBackUPS
{
public:
	BrazilModelBackUPS1500(unsigned char model);

	static BrazilModelBackUPS1500 *Factory();

	double getOutputPowerNom();
	double getOutputActivePowerNom();
	int getOutputVoltageNom();
	int getLineVoltageNom();
	int getLineVoltageMin();
	int getLineVoltageMax();
	int getBatteryCount();

	bool hasShutdownAuto();
	char *getModelName();

protected:

private:
};

class BrazilModelBackUPS2200: public BrazilModelBackUPS
{
public:
	BrazilModelBackUPS2200(unsigned char model);

	static BrazilModelBackUPS2200 *Factory();

	double getOutputPowerNom();
	double getOutputActivePowerNom();
	int getOutputVoltageNom();
	int getLineVoltageNom();
	int getLineVoltageMin();
	int getLineVoltageMax();
	int getBatteryCount();

	bool hasShutdownAuto();
	char *getModelName();

protected:

private:
};

class BrazilModelBackUPS2200_22: public BrazilModelBackUPS
{
public:
	BrazilModelBackUPS2200_22(unsigned char model);

	static BrazilModelBackUPS2200_22 *Factory();

	double getOutputPowerNom();
	double getOutputActivePowerNom();
	int getOutputVoltageNom();
	int getLineVoltageNom();
	int getLineVoltageMin();
	int getLineVoltageMax();
	int getBatteryCount();

	bool hasShutdownAuto();
	char *getModelName();

protected:

private:
};

class BrazilModelBackUPS700: public BrazilModelBackUPS
{
public:
	BrazilModelBackUPS700(unsigned char model);

	static BrazilModelBackUPS700 *Factory();

	const unsigned int getRegulatingRelay();

	double getOutputPowerNom();
	double getOutputActivePowerNom();
	int getOutputVoltageNom();
	int getLineVoltageNom();
	int getLineVoltageMin();
	int getLineVoltageMax();
	int getBatteryCount();

	bool hasShutdownAuto();
	char *getModelName();

protected:

private:
};

class BrazilModelBackUPS800: public BrazilModelBackUPS
{
public:
	BrazilModelBackUPS800(unsigned char model);

	static BrazilModelBackUPS800 *Factory();

	double getOutputPowerNom();
	double getOutputActivePowerNom();
	int getOutputVoltageNom();
	int getLineVoltageNom();
	int getLineVoltageMin();
	int getLineVoltageMax();
	int getBatteryCount();

	bool hasShutdownAuto();
	char *getModelName();

protected:

private:
};


#endif /* SRC_DRIVERS_BRAZIL_BRAZILMODEL_H_ */
