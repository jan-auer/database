//
//  Mutex.cpp
//  database
//
//  Created by Felix Jankowski on 28.04.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

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
	
	
	int Mutex::lock() {
		return pthread_mutex_lock(&m);
	}
	
	int Mutex::unlock() {
		return pthread_mutex_unlock(&m);
	}

}