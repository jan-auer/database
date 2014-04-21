//
//  Locking.cpp
//  database
//
//  Created by Jan Michael Auer on 19/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "Locking.h"

#define __lock(target, exclusive, section) {\
          Lock* __l = &(target).lock; \
          lsql::lock(__l, exclusive); \
          { section } \
          lsql::unlock(__l); \
        }

namespace lsql {

	int lock(Lock* lock, bool exclusive) {
		if (exclusive) {
			return pthread_rwlock_wrlock(lock);
		} else {
			return pthread_rwlock_rdlock(lock);
		}
	}
	
	int unlock(Lock* lock) {
		return pthread_rwlock_unlock(lock);
	}
	
}
