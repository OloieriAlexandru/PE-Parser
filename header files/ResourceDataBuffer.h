#pragma once

struct ResourceDataBuffer
{
public:
	ResourceDataBuffer();
	int idName, idType;
	char** type, ** name;
};