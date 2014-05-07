//
//  Logger.h
//  database
//
//  Created by Felix Jankowski on 07.05.14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <pthread.h>
#include <iostream>

pthread_mutex_t __log__mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef DEBUG

#define log(__msg__) \
	pthread_mutex_lock(&__log__mutex); \
	std::cout << "[" << pthread_self() << "] " << __msg__ << std::endl; \
	pthread_mutex_unlock(&__log__mutex)

#else

#define log(__msg__)

#endif
