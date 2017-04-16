//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/31
// Time: 下午9:40
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__client__
#define __hive__client__

#include "accept.h"
#include "task.h"

NS_HIVE_BEGIN

class Client : public Accept
{
public:
	Client(void);
	virtual ~Client(void);

	static EpollObject* createObject(EpollObject::index_type index){
		return new Client();
	}
	static void releaseObject(EpollObject* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

	// from EpollObject
	virtual bool epollActive(uint32 events);
	virtual void epollRemove(void);

	bool connectServer(void);
};

NS_HIVE_END

#endif
