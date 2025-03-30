#pragma once

class IManager
{
public:
	IManager() = default;

	virtual void Init() = 0;
	virtual void Start() = 0;

protected:

private:
};