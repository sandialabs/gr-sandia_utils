#!/bin/bash

CCOPTS="-g -I../../include -I/opt/gnuradio-sandia/include/"

function clean()
{
	rm qa_manual_rx
	rm qa_unit_test
}

function buildUnitTest()
{
	echo Building Unit Tests

	g++ -o qa_unit_test $CCOPTS \
		test_runner.cc qa_vita.cc qa_vitaheader.cc qa_vitatrailer.cc qa_vrtpacket.cc \
		vitabaseabs.cpp vitaheader.cpp vitatrailer.cpp ContextPacket.cpp cifs_defs.cpp \
		vitarx.cpp VRTPacket.cpp vitarxlistener.cpp CifValue.cpp qa_CifValue.cc \
		vitaclassid.cpp qa_vitaclassid.cc \
		-lpthread -lcppunit
}

function buildManTest()
{
	echo Building Manual Test Tools
	g++ -o qa_manual_rx $CCOPTS \
		qa_manual_rx.cpp vitabaseabs.cpp vitaheader.cpp vitatrailer.cpp \
		vitarx.cpp VRTPacket.cpp vitarxlistener.cpp ContextPacket.cpp cifs_defs.cpp \
		CifValue.cpp vitaclassid.cpp \
		-lpthread
}

#################3
# main entry
if [ "$1" = "clean" ]
then
	clean
else
	#buildUnitTest
	buildManTest
fi

