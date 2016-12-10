/*
 * brasildriver.c
 *
 * Interface for apcctrl to the brazil driver.
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
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1335, USA.
 */

#include "apc.h"
#include "brazildriver.h"
#include <termios.h>
#include "brazilmodel.h"


#ifndef O_BINARY
#define O_BINARY 0
#endif

BrazilUpsDriver::BrazilUpsDriver(UPSINFO *ups) : UpsDriver(ups)
{
	this->model = 0;
	this->_received = time(NULL);

}
/*
 * Read UPS events. I.e. state changes.
 */
bool BrazilUpsDriver::check_state()
{
	Dmsg(50, "Check_state.\n");
	return ReadData(false) == SUCCESS ? true : false;
}
bool BrazilUpsDriver::refresh()
{
	Dmsg(50, "Refresh.\n");
	tcflush(_ups->fd, TCIFLUSH);
	return ReadData(false) == SUCCESS ? true : false;
}

char *printBuffer(unsigned char* buffer, int len, int first){
	static char out[800];
	for(int i=0 ; i<len-first && i<99 ; i++){
		sprintf (out+8*i," %02d: %03u;",i,*buffer+i+first);
	}
	return out;
}


/*
 * Open port
 */
bool BrazilUpsDriver::Open()
{
	int cmd;
	char *opendev = _ups->device;

#ifdef HAVE_MINGW
	// On Win32 add \\.\ UNC prefix to COMx in order to correctly address
	// ports >= COM10.
	char device[MAXSTRING];
	if (!strnicmp(_ups->device, "COM", 3)) {
		snprintf(device, sizeof(device), "\\\\.\\%s", _ups->device);
		opendev = device;
	}
#endif

	Dmsg(50, "Opening port %s\n", opendev);
	if ((_ups->fd = open(opendev, O_RDWR | O_NOCTTY | O_NDELAY | O_BINARY)) < 0)
	{
		Dmsg(50, "Cannot open UPS port %s: %s\n", opendev, strerror(errno));
		return false;
	}
	tcflush(_ups->fd, TCIOFLUSH);
	/* Cancel the no delay we just set */
	cmd = fcntl(_ups->fd, F_GETFL, 0);
	fcntl(_ups->fd, F_SETFL, cmd & ~O_NDELAY);

	/* Save old settings */
	tcgetattr(_ups->fd, &_oldtio);

	_newtio.c_cflag = B9600 | CS8 | CSTOPB | CLOCAL | CREAD;
	_newtio.c_iflag = IGNPAR;    /* Ignore errors, raw input */
	_newtio.c_oflag = 0;         /* Raw output */
	_newtio.c_lflag = 0;         /* No local echo */

#if defined(HAVE_OPENBSD_OS) || \
		defined(HAVE_FREEBSD_OS) || \
		defined(HAVE_NETBSD_OS) || \
		defined(HAVE_QNX_OS)
	_newtio.c_ispeed = B9600;    /* Set input speed */
	_newtio.c_ospeed = B9600;    /* Set output speed */
#endif   /* __openbsd__ || __freebsd__ || __netbsd__  */

	/* This makes a non.blocking read() with TIMER_READ (10) sec. timeout */
	_newtio.c_cc[VMIN] = 0;
	_newtio.c_cc[VTIME] = TIMER_READ * 10;

#if defined(HAVE_OSF1_OS) || \
    defined(HAVE_LINUX_OS) || defined(HAVE_DARWIN_OS)
   (void)cfsetospeed(&_newtio, B9600);
   (void)cfsetispeed(&_newtio, B9600);
#endif  /* do it the POSIX way */

   tcflush(_ups->fd, TCIOFLUSH);
	tcsetattr(_ups->fd, TCSANOW, &_newtio);
	sleep(2);  // without this usleep we can't flush the port buffer (usb-serial converter)
	tcflush(_ups->fd, TCIOFLUSH);

	this->model = 0;			// force to instantiate a model again if there is a model already instantiated
	return true;
}
bool BrazilUpsDriver::setup()
{
	if(this->model != 0){
		return true;
	}
	Dmsg(50, "Setup UPS %s to load specific model.\n",_ups->upsname);
	int whait = 0;
	do{
		sleep(1);
		whait++;
	}while(this->ReadData(false) == FAILURE && whait<60);

	if(this->model == 0){
		log_event(_ups, LOG_ERR, "Setup fail! There is no model instantiated.");
		return false;
	}else{
		if(!(hibernate_ups || shutdown_ups)){
			if(! this->model->isScheduleSet()){
				this->programmation(false,0,false,0);
			}
			if(! this->model->isLineMode()){
				this->turnLineOn(true);
			}
			if(! this->model->isOutputOn()){
				this->turnOutputOn(true);
			}
			this->turnContinueMode();
		}
		return true;
	}
}
bool BrazilUpsDriver::shutdown()
{
	Dmsg(50, "Setting default shutdown!!!\n");
	if(! this->setup()){
		log_event(_ups, LOG_ERR, "Shutdown fail! There is no model instantiated.");
		return false;
	}
	Dmsg(50, "Setting shutdown with timeout in 1 or 2 minutes!!!\n");
	this->programmation(true, 1, false, 0);
	return true;
}
bool BrazilUpsDriver::kill_power()
{
	Dmsg(50, "Kill Power!!!\n");
	if(! this->setup()){
		log_event(_ups, LOG_ERR, "Kill power fail! There is no model instantiated.");
		return false;
	}
	if(this->model->isLineMode()){
	    printf("\n\nLine is present! Turn off in 1 or 2 minutes and turn on in 2 or 3 minutes!!!\n\n");
		Dmsg(50, "Setting kill_power with timeout!!! The UPS will turn off in 1 minute and turn on in 2 minutes!\n");
		this->programmation(true, 1, true, 2);
	}else{
	    printf("\n\nSetting auto shutdown and auto turn on!!! The UPS only do this if there is NO load!!\n\n");
		Dmsg(50, "Setting kill_power with shutdownAuto!!!\n");
		this->shutdownAuto();
	}
	return true;
}

void BrazilUpsDriver::send(unsigned char *cmd, int size){
	Dmsg(50, "Buffer to send! len: %02d; data:%s.\n",size,this->strBuffer(cmd,size,0));
	tcflush(_ups->fd, TCIFLUSH);
	char byte;
	for(int i=0 ; i<size ; i++){
		byte = cmd[i];
		write(_ups->fd, &byte, 1);
	}
}

bool BrazilUpsDriver::programmation(bool turnoff, unsigned int turnoff_minutes, bool turnon, unsigned int turnon_minutes){
	Dmsg(50, "programmation(%s, %2d, %s, %2d)\n",(turnoff?"true":"false"),turnoff_minutes,(turnon?"true":"false"),turnon_minutes);
	if(! this->setup()){
		log_event(_ups, LOG_ERR, "Programmation fail! There is no model instantiated.");
		return false;
	}
	unsigned char *cmd = 0;
	int size = this->model->generateCmdScheduleSet(&cmd, turnoff, turnoff_minutes, turnon, turnon_minutes);

	if(size > 0){
		int whait = 1;
		while(whait > 0 && whait <= 5){
			this->send(cmd,size);
			this->ReadData(false);
			if(((turnoff || turnon) && this->model->isScheduleSet()) || (!(turnoff || turnon) && !this->model->isScheduleSet())){
				whait = 0;
			}else{
				whait++;
				sleep(1);
			}
		}
		if(whait<5){
			if(turnoff || turnon){
				log_event(_ups, LOG_NOTICE, "BrazilDriver: programmation set! turnoff: %s, turnoff_min: %02d, turnon: %s, turnon_min: %02d!",(turnoff?"true":"false"),turnoff_minutes,(turnon?"true":"false"),turnon_minutes);
			}else{
				log_event(_ups, LOG_NOTICE, "BrazilDriver: programmation canceled! Not turnoff or turnon!");
			}
			return true;
		}else{
			log_event(_ups, LOG_ERR, "programmation fail!!!");
		}
	}
	return false;
}

bool BrazilUpsDriver::turnLineOn(bool turnon)
{
	Dmsg(50, "turnLineOn(%s).\n",(turnon?"true":"false"));
	if(! this->setup()){
		log_event(_ups, LOG_ERR, "TurnLineOn fail! There is no model instantiated.");
		return false;
	}
	unsigned char *cmd = 0;
	int size = this->model->generateCmdTurnLineOn(&cmd, turnon);
	if(size > 0){
		int whait = 1;
		while(whait > 0 && whait <= 5){
			this->send(cmd,size);
			this->ReadData(false);
			if((turnon && this->model->isLineMode()) || (!turnon && !this->model->isLineMode())){
				whait = 0;
			}else{
				Dmsg(50, "LineOn programmation! set: %s; read_LineOn: %s.\n",(turnon?"true":"false"),(this->model->isLineMode()?"true":"false"));
				whait++;
				sleep(1);
			}
		}
		if(whait<5){
			if(turnon){
				log_event(_ups, LOG_NOTICE, "turn Line On!");
			}else{
				log_event(_ups, LOG_NOTICE, "turn Line Off!");
			}
			return true;
		}else{
			log_event(_ups, LOG_ERR, "set Line %s FAIL!!!",(turnon?"On":"Off"));
		}
	}
	return false;
}
bool BrazilUpsDriver::turnOutputOn(bool turnon)
{
	Dmsg(50, "turnOutputOn(%s).\n",(turnon?"true":"false"));
	if(! this->setup()){
		log_event(_ups, LOG_ERR, "TurnOutputOn fail! There is no model instantiated.");
		return false;
	}
	unsigned char *cmd = 0;
	int size = this->model->generateCmdTurnOutputOn(&cmd, turnon);
	if(size > 0){
		int whait = 1;
		while(whait > 0 && whait <= 10){
			this->send(cmd,size);
			this->ReadData(false);
			if((turnon && this->model->isOutputOn()) || (!turnon && !this->model->isOutputOn())){
				whait = 0;
			}else{
				Dmsg(50, "LineOutput programmation! set: %s; read_OutputOn: %s.\n",(turnon?"true":"false"),(this->model->isOutputOn()?"true":"false"));
				whait++;
				sleep(1);
			}
		}
		if(whait<10){
			if(turnon){
				log_event(_ups, LOG_NOTICE, "turn output On!");
			}else{
				log_event(_ups, LOG_NOTICE, "turn output Off!");
			}
			return true;
		}else{
			log_event(_ups, LOG_ERR, "set Output %s FAIL!!!",(turnon?"On":"Off"));
		}
	}
	return false;
}

bool BrazilUpsDriver::shutdownAuto()
{
	Dmsg(50, "shutdownAuto start.\n");
	if(! this->setup()){
		log_event(_ups, LOG_ERR, "auto-start! There is no model instanciated.");
		return false;
	}
	unsigned char *cmd = 0;
	int size = 0;
	size = this->model->generateCmdShutdownAuto(&cmd);
	if(size > 0){
		this->send(cmd,size);
		log_event(_ups, LOG_NOTICE, "shutdown auto sent.\n");
	}
	sleep(1);
	size = this->model->generateCmdScheduleSet(&cmd, false, 0 , false, 0 );
	if(size > 0){
		this->send(cmd,size);
		log_event(_ups, LOG_NOTICE, "schedule sent.\n");
	}
	return true;
}
bool BrazilUpsDriver::turnContinueMode()
{
	Dmsg(50, "continueMode start.\n");
	if(! this->setup()){
		log_event(_ups, LOG_ERR, "continue mode! There is no model instanciated.");
		return false;
	}
	unsigned char *cmd = 0;
	int size = this->model->generateCmdContinueMode(&cmd);
	if(size > 0){
		this->send(cmd,size);
		log_event(_ups, LOG_NOTICE, "continue mode sent.\n");
		return true;
	}
	return false;
}

bool BrazilUpsDriver::Close()
{
	/* Reset serial line to old values */
	if (_ups->fd >= 0) {
		Dmsg(50, "Closing port\n");
		tcflush(_ups->fd, TCIFLUSH);
		tcsetattr(_ups->fd, TCSANOW, &_oldtio);
		tcflush(_ups->fd, TCIFLUSH);
		close(_ups->fd);
	}
	_ups->fd = -1;
	this->model = 0;
	return true;
}


/*
 * Ever read a complete status MSG.
 */
int BrazilUpsDriver::ReadData(bool getevents)
{
	Dmsg(99, "ReadData(bool getevents)\n");

	/*
	 * Message structure
	 * msg[0] model number
	 * msg[1-22] paramters
	 * msg[23] checksum
	 * msg[24] end of msg. byte = 254
	 */

	unsigned int bufpos = 0;		// position on buffer to discart a wrong byte
	unsigned int buflen = 0;		// length of buffer

	bool ending = false;
	unsigned char c;
	int retval;
	time_t now = time(NULL);
	int wait;

	if (getevents) {
		wait = TIMER_FAST;   /* 1 sec, expect fast response */
	}else{
		wait = _ups->wait_time;
	}


#ifdef HAVE_MINGW
	/* Set read() timeout since we have no select() support. */
	{
		COMMTIMEOUTS ct;
		HANDLE h = (HANDLE)_get_osfhandle(_ups->fd);
		ct.ReadIntervalTimeout = MAXDWORD;
		ct.ReadTotalTimeoutMultiplier = MAXDWORD;
		ct.ReadTotalTimeoutConstant = wait * 1000;
		ct.WriteTotalTimeoutMultiplier = 0;
		ct.WriteTotalTimeoutConstant = 0;
		SetCommTimeouts(h, &ct);
	}
#endif

	while (!ending && buflen < BrazilModelAbstract::BUFFERLEN - 50) {

#if !defined(HAVE_MINGW)
		fd_set rfds;
		struct timeval tv;

		FD_ZERO(&rfds);
		FD_SET(_ups->fd, &rfds);
		tv.tv_sec = wait;
		tv.tv_usec = 0;

		errno = 0;
		retval = select((_ups->fd) + 1, &rfds, NULL, NULL, &tv);

		switch (retval) {
		case 0:					/* No chars available in TIMER seconds. */
			continue;
		case -1:
			if (errno == EINTR || errno == EAGAIN) {     /*   assume SIGCHLD */
				continue;
			} else if (errno == EBADF) {
				return FAILURE;               /* We're probably shutting down */
			}
			Error_abort("Select error on UPS FD. %s\n", strerror(errno));
			break;
		default:
			break;
		}
#endif

		do {
			retval = read(_ups->fd, &c, 1);
		} while (retval <= 0 && (errno == EAGAIN || errno == EINTR));
		if (retval <= 0) {
			/*
			 * Test if connection was losted.
			 */
			if(now - this->_received > 20){
				Dmsg(50, "Connection lost.\n");
				bool commok = false;
				if(access(_ups->device, F_OK | W_OK) != -1 ) {
					if (_ups->fd >= 0){
						this->Close();
					}
					if(this->Open()){
						this->setup();
						this->read_static_data();
						commok = true;
					}
				}
				if(! commok){
					if(! _ups->is_commlost()){
						_ups->set_commlost();
						generate_event(_ups, CMDCOMMFAILURE);
					}
				}
				return FAILURE;
			}
		}

		this->_buffer[buflen++] = c;

		if(!getevents){
			int ret = BrazilModelAbstract::testBuffer(this->_buffer+bufpos, buflen-bufpos);
			// ret == -1: error! rotate buffer
			// ret == 0: get next byte
			// ret == 1: test ok!
			if(ret == 1){
				if(this->model == 0){
					this->model = BrazilModelAbstract::newInstance(this->_buffer[bufpos],_ups->expander_ampere);
					if(this->model == 0){
						log_event(_ups, LOG_ERR, "APC Brazil. Model %u not recognized.",this->_buffer[bufpos]);
						bufpos++;
					}
				}
				if(this->model != 0){
					this->model->setBuffer(this->_buffer+bufpos, buflen-bufpos);
					time(&this->_received);
					ending = true;
				}

			}else{
				if(ret == -1){
					bufpos++;
				}
			}
		}else{
			int ret = this->model->testEvents(this->_buffer+bufpos, buflen-bufpos);
			// ret == -1: error! rotate buffer
			// ret == 0: get next byte
			// ret == 1: test ok!
			if(ret == 1){
				if(this->model->setEvents(this->_buffer+bufpos, buflen-bufpos)){
					ending = true;
				}
			}
			if(ret == -1){
				bufpos++;
			}
		}
	}
	Dmsg(99, "Buffer! len: %02d; pos: %02d; data:%s.\n",buflen,bufpos,this->strBuffer(this->_buffer,buflen,0));

	if(_ups->is_commlost() && ending){
		_ups->clear_commlost();
		generate_event(_ups, CMDCOMMOK);
	}
	if(ending){
		tcflush(_ups->fd, TCIFLUSH);
	}

	if(this->model == 0){
		return FAILURE;
	}else{
		return SUCCESS;
	}
}

/*
 * Setup capabilities structure for UPS
 */
bool BrazilUpsDriver::get_capabilities()
{
	Dmsg(50, "get_capabilities()\n");
	write_lock(_ups);

	_ups->UPS_Cap[CI_BATTLEV] = TRUE;
	_ups->UPS_Cap[CI_RUNTIM] = TRUE;
	_ups->UPS_Cap[CI_VBATT] = TRUE;
	_ups->UPS_Cap[CI_VLINE] = TRUE;
	_ups->UPS_Cap[CI_VMAX] = TRUE;
	_ups->UPS_Cap[CI_VMIN] = TRUE;
	_ups->UPS_Cap[CI_LOAD] = TRUE;
	_ups->UPS_Cap[CI_VOUT] = TRUE;
	_ups->UPS_Cap[CI_ITEMP] = TRUE;
	_ups->UPS_Cap[CI_FREQ] = TRUE;
	_ups->UPS_Cap[CI_UPSMODEL] = TRUE;
	_ups->UPS_Cap[CI_NOMINV] = TRUE;
	_ups->UPS_Cap[CI_NOMOUTV] = TRUE;
	_ups->UPS_Cap[CI_NOMPOWER] = TRUE;
	_ups->UPS_Cap[CI_NOMBATTV] = TRUE;
	_ups->UPS_Cap[CI_Overload] = TRUE;
	_ups->UPS_Cap[CI_LowBattery] = TRUE;

	write_unlock(_ups);
	return true;
}

/*
 * Read UPS info that remains unchanged -- e.g. transfer
 * voltages, shutdown delay, ...
 *
 * This routine is called once when apcctrl is starting
 */
bool BrazilUpsDriver::read_static_data()
{
	Dmsg(50, "read_static_data()\n");
	if(this->model == 0){
		return false;
	}

	this->model->setBufferLock();
	write_lock(_ups);

	/* Clear Status */
	_ups->Status = 0;

	/* model, firmware */
	strlcpy(_ups->upsmodel, this->model->getModelName(), sizeof(_ups->upsmodel));

	/* Nominal battery voltage */
	_ups->nombattv = this->model->getBatteryVoltageNom();

	/* Nominal out voltage */
	_ups->NomOutputVoltage = this->model->getOutputVoltageNom();

	_ups->NomPower = this->model->getOutputActivePowerNom();

	/* there is no information about this. Battery need to be always present, I think.  */
	_ups->set_battpresent();

	_ups->extbatts = 0;

	_ups->lastxfer = XFER_NONE;

	write_unlock(_ups);
	this->model->setBufferUnlock();

	return true;
}

/*
 * Read UPS info that changes -- e.g. Voltage, temperature, ...
 *
 * This routine is called once every 5 seconds to get
 * a current idea of what the UPS is doing.
 */
bool BrazilUpsDriver::read_volatile_data()
{
	Dmsg(50, "read_volatile_data()\n");
	if(this->model == 0)
		return false;
	this->model->setBufferLock();
	write_lock(_ups);

	/* save time stamp */
	time(&_ups->poll_time);

	_ups->set_online(this->model->isLineMode());

	/* Nominal voltage can change because the UPSs are bivolt */
	_ups->NomInputVoltage = this->model->getLineVoltageNom();
	_ups->LineMin = this->model->getLineVoltageMin();
	_ups->LineMax = this->model->getLineVoltageMax();

	/* LINE_FREQ */
	_ups->LineFreq = this->model->getLineFrequency();

	if(this->model->isLineMode()){
		_ups->OutputFreq = this->model->getLineFrequency();
	}else{
		_ups->OutputFreq = 60;
	}

	_ups->set_battlow(this->model->isBatteryCritical());

	_ups->TimeLeft = this->model->getBatteryTimeLeft();

	/* LINE_VOLTAGE */
	_ups->LineVoltage = this->model->getLineVoltage();

	/* BATT_VOLTAGE */
	_ups->BattVoltage = this->model->getBatteryVoltage();

	/* OUTPUT_VOLTAGE */
	_ups->OutputVoltage = this->model->getOutputVoltage();

	/* BATT_FULL Battery level percentage */
	double batlevel;
	if(this->model->isLineMode()){
		if(this->model->isCharging()){
			/*
			 * Para sinalizar que a bateria está sendo carregada e não sabemos o real estado dela o nível de tensão fica alternando
			 */
			if((_ups->poll_time & 0x02) == 0x02){
				batlevel = 100;
			}else{
				batlevel = 50;
			}
		}else{
			batlevel = 100;
		}
	}else{
		batlevel = this->model->getBatteryLevel();
		if(batlevel > 100) batlevel = 100;
	}
	_ups->BattChg = batlevel;

	/* UPS_LOAD */
	_ups->OutputCurrent = this->model->getOutputCurrent();

	/* UPS_LOAD */
	_ups->UPSLoad = this->model->getLoadActivePowerPercent();

	/* UPS_TEMP */
	_ups->UPSTemp = this->model->getTemperature();

	_ups->set_overload(this->model->isOverload());

	Dmsg(10, "Extra status! LineOn: %s; "
			"OutputOn: %s; "
			"220V: %s; "
			"Charging: %s; "
			"Overload: %s; "
			"Critical battery: %s; "
			"Date (yyyy-mm-dd HH:MM:SS WeekDay): %04d-%02d-%02d %02d:%02d:%02d %1d; "
			"Days of week programmed (Sunday-Saturday): %1d%1d%1d%1d%1d%1d%1d.\n",
			(this->model->isLineMode()?"true":"false"),
			(this->model->isOutputOn()?"true":"false"),
			(this->model->isLine220V()?"true":"false"),
			(this->model->isCharging()?"true":"false"),
			(this->model->isOverload()?"true":"false"),
			(this->model->isBatteryCritical()?"true":"false"),
			this->model->getYear(),
			this->model->getMonth(),
			this->model->getDayOfMonth(),
			this->model->getHour(),
			this->model->getMinute(),
			this->model->getSecond(),
			this->model->getDayOfWeek(),
			this->model->isScheduleSetDayOfWeek(Sunday),
			this->model->isScheduleSetDayOfWeek(Monday),
			this->model->isScheduleSetDayOfWeek(Tuesday),
			this->model->isScheduleSetDayOfWeek(Wednesday),
			this->model->isScheduleSetDayOfWeek(Thursday),
			this->model->isScheduleSetDayOfWeek(Friday),
			this->model->isScheduleSetDayOfWeek(Saturday)
	);


	write_unlock(_ups);
	this->model->setBufferUnlock();

	return true;
}

char *BrazilUpsDriver::strBuffer(unsigned char* buffer, int len, int first){
	static char out[900];
	for(int i=0 ; i<len-first && i<99 ; i++){
		sprintf (out+9*i," %02d(%03u);",i,*(buffer+i+first));
	}
	return out;
}

int BrazilUpsDriver::getEventsStr(char **events){
	Dmsg(50, "shutdownAuto start.\n");
	if(! this->setup()){
		log_event(_ups, LOG_ERR, "auto-start! There is no model instantiated.");
		return false;
	}
	unsigned int sizeout = 0;
	unsigned char *cmd = 0;
	int sizecmd = this->model->generateCmdGetEvents(&cmd);
	if(sizecmd > 0){
		tcflush(_ups->fd, TCIFLUSH);
		sleep(1);
		this->send(cmd,sizecmd);
		this->ReadData(true);
		sizeout = this->model->getEventsStr(events);
	}

	return sizeout;
}
