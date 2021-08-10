#pragma once

#include "IMicroService.h"

class StartMicroServiceGuard
{

public:

	StartMicroServiceGuard(shared_ptr<IMicroService> microservice)
		: _microservice{ microservice }
	{
		_microservice->start();
	}

	~StartMicroServiceGuard()
	{
		_microservice->stop();
	}

private:

	shared_ptr<IMicroService> _microservice;

};