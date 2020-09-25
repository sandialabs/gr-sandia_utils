/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <cppunit/TextTestRunner.h>
#include <cppunit/XmlOutputter.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include "qa_vita.h"

int main( int argc, char **argv )
{
  CppUnit::TextTestRunner runner;

  //std::ofstream xmlfile( "vita_tests.xml" );
  //CppUnit::XmlOutputter *xmlout = new CppUnit::XmlOutputter( &runner.result(), xmlfile );

  printf("Adding tests\n");
  runner.addTest( qa_vita::suite() );
  //runner.setOutputter( xmlout );

  printf("Runing tests\n");
  bool was_successful = runner.run( "", false );

  return was_successful ? 0 : 1;
}
