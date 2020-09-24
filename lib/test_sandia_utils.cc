/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cppunit/TextTestRunner.h>
#include <cppunit/XmlOutputter.h>

#include <gnuradio/unittests.h>
#include "qa_sandia_utils.h"
#include "vita/qa_vita.h"
#include <iostream>
#include <fstream>

int
main (int argc, char **argv)
{
  CppUnit::TextTestRunner runner;
  std::ofstream xmlfile(get_unittest_path("sandia_utils.xml").c_str());
  CppUnit::XmlOutputter *xmlout = new CppUnit::XmlOutputter(&runner.result(), xmlfile);

  runner.addTest(qa_sandia_utils::suite());
  runner.addTest( qa_vita::suite() );
  runner.setOutputter(xmlout);

  bool was_successful = runner.run("", false);

  return was_successful ? 0 : 1;
}
