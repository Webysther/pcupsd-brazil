/*
 * brazildriver.h
 *
 * Public header file for the test driver.
 */

/*
 * Copyright (C) 2015-20XX Wagner Popov
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

#ifndef _BRAZILDRIVER_H
#define _BRAZILDRIVER_H

#include "brazilmodel.h"

class BrazilUpsDriver: public UpsDriver
{
public:
	BrazilUpsDriver(UPSINFO *ups);
	virtual ~BrazilUpsDriver() {}

	static UpsDriver *Factory(UPSINFO *ups)
	{ return new BrazilUpsDriver(ups); }

	virtual bool get_capabilities();
	virtual bool read_volatile_data();
	virtual bool read_static_data();
	virtual bool check_state();
	virtual bool refresh();
	virtual bool Open();
	virtual bool Close();
	virtual bool setup();
	virtual bool shutdown();
	virtual bool kill_power();
	virtual int getEventsStr(char **events);

	virtual bool programmation(bool turnoff, unsigned int turnoff_minutes, bool turnon, unsigned int turnon_minutes);
	virtual bool turnLineOn(bool turnon);
	virtual bool turnOutputOn(bool turnon);
	virtual bool shutdownAuto();
	virtual bool turnContinueMode();

	BrazilModelAbstract *model;

protected:

	void send(unsigned char *cmd, int size);
	char *strBuffer(unsigned char* buffer, int len, int first);
private:

	int ReadData(bool getevents);

	struct termios _oldtio;
	struct termios _newtio;
	time_t _received;
	unsigned char _buffer[BrazilModelAbstract::BUFFERLEN];  // Buffer

};


#endif   /* _BRAZIL_DRIVER_H */
