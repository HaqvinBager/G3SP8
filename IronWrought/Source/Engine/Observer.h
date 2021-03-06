#pragma once
#include "Message.h"

class IObserver
{
public:
	virtual ~IObserver() { }
	virtual void Receive(const SMessage& aMessage) = 0;
};

class IStringObserver
{
public:
	virtual ~IStringObserver() { }
	virtual void Receive(const SStringMessage& aMessage) = 0;
};

class IMessageObserver
{
public:
	virtual ~IMessageObserver() {}
	virtual void Receive(const SIDMessage& aMessage) = 0;
};