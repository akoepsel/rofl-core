/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crofcore.h"

using namespace rofl;

/*static*/std::set<crofcore*>	 crofcore::rofcores;
/*static*/PthreadRwLock          crofcore::rofcores_rwlock;
/*static*/bool                   crofcore::initialized = false;
/*static*/unsigned int           crofcore::next_worker_id = 0;
/*static*/std::vector<pthread_t> crofcore::workers;
/*static*/PthreadRwLock          crofcore::workers_rwlock;

/*static*/
pthread_t
crofcore::get_next_worker_tid()
{
	RwLock(workers_rwlock, RwLock::RWLOCK_READ);
	next_worker_id =
			(next_worker_id == (workers.size() - 1)) ?
					0 : next_worker_id + 1;
	return workers[next_worker_id];
}


/*static*/
void
crofcore::initialize(
		unsigned int workers_num)
{
	workers_num = (0 == workers_num) ? 1 : workers_num;

	RwLock(workers_rwlock, RwLock::RWLOCK_WRITE);

	if (crofcore::initialized && (workers_num < workers.size())) {
		return;
	}

	unsigned int old_workers_num = workers.size();
	for (unsigned int i = old_workers_num; i < workers_num; i++) {
		workers.push_back(cioloop::add_thread());
	}
	next_worker_id = old_workers_num;
	crofcore::initialized = true;
}



/*static*/
void
crofcore::terminate()
{
	RwLock(workers_rwlock, RwLock::RWLOCK_WRITE);

	if (not crofcore::initialized) {
		return;
	}

	for (std::vector<pthread_t>::iterator
			it = workers.begin(); it != workers.end(); ++it) {
		cioloop::drop_thread(*it);
	}
	workers.clear();
	crofcore::initialized = false;

	rofl::cioloop::get_loop().shutdown();

	rofl::cioloop::get_loop().cleanup_on_exit();
}


