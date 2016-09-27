topdir:=../..

SUBDIRS =
include $(topdir)/autoconf/targets.mak

all-install: install-debian
all-uninstall: uninstall-debian

install-debian:
	$(call DISTINST,debian)
	
	$(if $(DESTDIR),,$(V)if test -x /usr/sbin/update-rc.d ; then \
        /usr/sbin/update-rc.d apcupsd remove ; \
    fi )
    
	$(call MKDIR,/etc/init.d)
	$(call INSTDATA,744,apcupsd,/etc/init.d/apcupsd)
	 
	$(if $(DESTDIR),,$(V)if test -x /usr/sbin/update-rc.d ; then \
        /usr/sbin/update-rc.d apcupsd defaults ; \
        echo "        rc.d updated with update-rc.d"; \
    else \
        echo "        update-rc.d not located"; \
    fi )
	
	$(if $(DESTDIR),, $(V)if test -x /sbin/insserv ; then \
        /sbin/insserv apcupsd ; \
        echo "        rc.d updated with insserv"; \
    else \
        echo "        insserv not located. Don't worry about."; \
    fi )
	
	$(call INSTORIG,744,ups-monitor,/etc/init.d/ups-monitor)
	@echo "---------------------------------------------------------------------"
	@echo " NOTE: APCUPSD will almost certainly not work \"out of the box.\""
	@echo "       You MUST properly edit $(sysconfdir)/apcctrl.conf or apcupsd"
	@echo "       will fail on startup.  Also, please see that your script"
	@echo "       at /etc/init.d/halt properly calls /etc/init.d/ups-monitor"
	@echo "       as shown in platforms/debian/examples/halt.ex."
	@echo "---------------------------------------------------------------------"

uninstall-debian:
	$(call DISTUNINST,debian)
	-$(call UNINST,/etc/init.d/apcupsd)
	-$(if $(DESTDIR),,$(V)/usr/sbin/update-rc.d apcupsd remove)
	-$(call UNINST,/etc/init.d/ups-monitor)

