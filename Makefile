#TODO: autodetect
#ARDUINO_DIR:=/home/hubbe/lib/arduino-1.8.3
ARDUINO_DIR:=/Applications/Arduino.app/Contents/Java

BOARD_TAG=Proffieboard-L433CC
#BOARD_TAG=teensy31

ifeq ($(strip $(BOARD_TAG)),Proffieboard-L433CC)
  #TODO: autodetect
  #ALTERNATE_CORE_PATH=/home/hubbe/lib/arduino-STM32L4
  ALTERNATE_CORE_PATH=/Users/hubbe/Library/Arduino15/packages/proffieboard/hardware/stm32l4/0.1.7
  USB_TYPE=USB_TYPE_CDC_MSC
  ARDUINO_LIBS=SPI Wire
  include Proffieboard.mk
else
  USB_TYPE=USB_SERIAL
  ARDUINO_LIB_PATH=FOOBAR
  ARDUINO_LIBS=FastLED SD SPI SerialFlash Snooze i2c_t3
  include Teensy.mk
endif

CPPFLAGS += $(TESTFLAGS)
DIAGNOSTICS_COLOR_WHEN = auto

style-test:
	(cd styles && $(MAKE) test)

common-test:
	(cd common && $(MAKE) test)

test1:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_proffieboard_config.h\\\" BOARD_TAG=Proffieboard-L433CC OBJDIR=test-proffieboard-default

test1V:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/proffieboard_v1_verification_config.h\\\" BOARD_TAG=Proffieboard-L433CC OBJDIR=test-proffieboard-v1-verification

test2:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_v3_config.h\\\" BOARD_TAG=teensy36 OBJDIR=test-teensy36-default-v3

test3:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_v3_config.h\\\" BOARD_TAG=teensy35 OBJDIR=test-teensy35-default-v3

test4:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/graflex_v1_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-graflex-v1

test5:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/prop_shield_fastled_v1_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-fastled-v1

test6:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/owk_v2_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-owk-v2

test7:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/crossguard_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-crossguard-v3

test8:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/test_bench_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-test-v3

test9:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/toy_saber_config.h\\\" BOARD_TAG=teensy31  OBJDIR=test-teensy31-toy-v3

testA:
	$(MAKE) all TESTFLAGS=-DCONFIG_FILE_TEST=\\\"config/default_v3_config.h\\\" BOARD_TAG=teensy31 OBJDIR=test-teensy31-default-v3


test: style-test common-test test1 test2 test3 test4 test5 test6 test7 test8 test9 testA test1V
	@echo Tests pass

# Check that there are no uncommitted changes
up-to-date-test:
	if [ -d CVS ]; then cvs diff >/dev/null 2>&1; fi
	if [ -d .git ]; then git diff-index --quiet HEAD -- ; fi

export: up-to-date-test test
	if [ -d CVS ]; then cd .. && zip -9 lightsaber/lightsaber-`sed <lightsaber/lightsaber.ino -n 's@.*lightsaber.ino,v \([^ ]*\) .*$$@\1@gp'`.zip `cd lightsaber && cvs status 2>/dev/null | sed -n 's@.*Repository.*/cvs/lightsaber/\(.*\),v@\1@gp' | grep -v 'lightsaber/doc' | grep -v '.cvsignore'` ; fi
	if [ -d .git ]; then DIR=`pwd` ; VERSION=`git describe --tags --always --dirty --broken` ; echo "Exporting $$VERSION" ; rm -rvf /tmp/exporttmp || : ; mkdir -p /tmp/exporttmp/lightsaber && cp -r . /tmp/exporttmp/lightsaber/ && sed <lightsaber.ino "s@const char version.*@const char version[] = \"$$VERSION\";@g" >/tmp/exporttmp/lightsaber/lightsaber.ino && ( cd /tmp/exporttmp && zip -MM -9 $$DIR/lightsaber-$$VERSION.zip lightsaber `cd $$DIR && git ls-files | grep -v doc/ | sed 's@^@lightsaber/@'` ) && echo "Ok, finished exporting $$VERSION" ; fi

export-dirty:
	if [ -d CVS ]; then cd .. && zip -9 lightsaber/lightsaber-`sed <lightsaber/lightsaber.ino -n 's@.*lightsaber.ino,v \([^ ]*\) .*$$@\1@gp'`.zip `cd lightsaber && cvs status 2>/dev/null | sed -n 's@.*Repository.*/cvs/lightsaber/\(.*\),v@\1@gp' | grep -v 'lightsaber/doc' | grep -v '.cvsignore'` ; fi
	if [ -d .git ]; then DIR=`pwd` ; VERSION=`git describe --tags --always --dirty --broken` ; echo "Exporting $$VERSION" ; rm -rvf /tmp/exporttmp || : ; mkdir -p /tmp/exporttmp/lightsaber && cp -r . /tmp/exporttmp/lightsaber/ && sed <lightsaber.ino "s@const char version.*@const char version[] = \"$$VERSION\";@g" >/tmp/exporttmp/lightsaber/lightsaber.ino && ( cd /tmp/exporttmp && zip -MM -9 $$DIR/lightsaber-$$VERSION.zip lightsaber `cd $$DIR && git ls-files | grep -v doc/ | sed 's@^@lightsaber/@'` ) && echo "Ok, finished exporting $$VERSION" ; fi
