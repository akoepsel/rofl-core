/*
 * cworkflow_test.h
 *
 *  Created on: 12.04.2015
 *      Author: andreas
 */

#ifndef CWORKFLOW_TEST_H_
#define CWORKFLOW_TEST_H_

#include <vector>
#include "rofl/common/crofbase.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

void*
run_thread(void* arg) {

	pthread_t tid = pthread_self();

	std::cout << (int)tid << std::endl;

	return NULL;
};


class controller_t : public rofl::crofbase {
public:
	controller_t()
	{};
	virtual
	~controller_t()
	{};
};

class datapath_t : public rofl::crofbase {
public:
	datapath_t()
	{};
	virtual
	~datapath_t()
	{};
};


class cworkflow_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cworkflow_test );
	CPPUNIT_TEST( testRoflImpl );
	CPPUNIT_TEST_SUITE_END();


public:
	void setUp();
	void tearDown();

	void testRoflImpl();

private:

	std::map<unsigned int, controller_t> ctls;
	std::map<unsigned int, datapath_t>   dpts;
};

#endif /* CWORKFLOW_TEST_H_ */
