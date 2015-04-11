/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */



#include "crofcore.h"

using namespace rofl;

/*static*/bool                   crofcore::rofcore_initialized = false;
/*static*/unsigned int           crofcore::next_worker_id = 0;
/*static*/unsigned int           crofcore::workers_num = 1;
/*static*/std::vector<pthread_t> crofcore::workers;
/*static*/std::set<crofcore*>    crofcore::rofcores;
/*static*/PthreadRwLock          crofcore::rofcores_rwlock;

/*static*/
pthread_t
crofcore::get_next_worker_tid()
{
	crofcore::rofcore_init();
	RwLock(rofcores_rwlock, RwLock::RWLOCK_READ);
	next_worker_id =
			(next_worker_id == (workers.size() - 1)) ?
					0 : next_worker_id + 1;
	return workers[next_worker_id];
}


/*static*/
void
crofcore::rofcore_init()
{
	if (crofcore::rofcore_initialized) {
		return;
	}

	RwLock(rofcores_rwlock, RwLock::RWLOCK_WRITE);
	for (unsigned int i = 0; i < workers_num; i++) {
		workers.push_back(cioloop::add_thread());
	}
	crofcore::rofcore_initialized = true;
}



/*static*/
void
crofcore::rofcore_term()
{
	if (not crofcore::rofcore_initialized) {
		return;
	}

	RwLock(rofcores_rwlock, RwLock::RWLOCK_WRITE);
	for (std::vector<pthread_t>::iterator
			it = workers.begin(); it != workers.end(); ++it) {
		cioloop::drop_thread(*it);
	}
	workers.clear();
	crofcore::rofcore_initialized = false;
}


