//
//  Mutex.cpp
//  database
//
//  Created by Felix Jankowski on 07.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include "Mutex.h"

namespace lsql {

	Mutex::Mutex() {
		int rc = pthread_mutex_init(&m, nullptr);
		assert(rc == 0);
	}

	Mutex::~Mutex() {
		int rc = pthread_mutex_destroy(&m);
		assert(rc == 0);
	}

	bool Mutex::lock() {
		return pthread_mutex_lock(&m) == 0;
	}

	bool Mutex::tryLock() {
		return pthread_mutex_trylock(&m) == 0;
	}

	bool Mutex::unlock() {
		return pthread_mutex_unlock(&m) == 0;
	}

	pthread_mutex_t* Mutex::object() {
		return &m;
	}

}
