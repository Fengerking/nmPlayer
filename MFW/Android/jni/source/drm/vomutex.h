
/* david 2009-11-28 */


class Mutex {
public:
	Mutex();
	Mutex(const char* name);
	~Mutex();

	// lock or unlock the mutex
	int    lock();
	void        unlock();

	// lock if possible; returns 0 on success, error otherwise
	int    tryLock();

	// Manages the mutex automatically. It'll be locked when Autolock is
	// constructed and released when Autolock goes out of scope.
	class Autolock {
		public:
			inline Autolock(Mutex& mutex) : mpMutex(&mutex) { mutex.lock(); }
			inline Autolock(Mutex* mutex) : mpMutex(mutex) { mutex->lock(); }
			inline ~Autolock() { mpMutex->unlock(); }
		private:
			Mutex*  mpMutex;
	};

private:
		// A mutex cannot be copied
	Mutex(const Mutex&);
	Mutex&      operator = (const Mutex&);
	void        _init();

	void*   mState;
};

