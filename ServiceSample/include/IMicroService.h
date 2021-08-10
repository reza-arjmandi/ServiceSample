#pragma once

class IMicroService
{

public:

	virtual ~IMicroService() = default;

	virtual void start() = 0;
	virtual void stop() = 0;

};