//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/26
// Time: 下午4:49
// To change this template use File | Settings | File Templates.
//

#include "epollhandler.h"

NS_HIVE_BEGIN

void onAcceptRead(Accept* pAccept, char* recvBuffer, int nread){
    Packet* pPacket;
    pPacket = pAccept->getTempReadPacket();
    //check stick message
    if( NULL != pPacket ){
    	int needLength = pPacket->getLength() - pPacket->getCursor();
    	if( needLength > nread ){
			// 数据不够，仍然继续等待后续数据
			pPacket->write(recvBuffer, nread);
			return;
    	}else{
			// 数据包已经完整，发送这个消息
			pPacket->write(recvBuffer, needLength);
			// 派发消息给对应的消息处理器
			pAccept->dispatchPacket(pPacket, pPacket->getHead()->command);
			pPacket->release();		// 对应Packet创建时的retain
			pPacket = NULL;
			// 剩下的消息需要继续解析
			if(nread == needLength){
				// 使用完临时数据之后，需要直接清除
				pAccept->setTempReadPacket(pPacket);
				return;
			}
			recvBuffer += needLength;
			nread -= needLength;
    	}
    }
    // 解析剩余的数据包
	char* recvBufferPtr;
	int packetLength;
	int writeLength;
	// 数据量太少，如果发生就直接丢了吧
	if( nread < (int)sizeof(PacketHead) ){
		return;
	}
	//这里读取的信息很可能包含多条信息，这时候需要解析出来；这几条信息因为太短，在发送时被底层socket合并了
	recvBufferPtr = recvBuffer;
	do{
		// 对头部数据进行解密
		if( pAccept->isNeedDecrypt() ){
			binary_decrypt(recvBufferPtr, 8, GlobalSetting::getInstance()->getKey());
		}
		packetLength = ((PacketHead*)(recvBufferPtr))->length;
		if( packetLength < (int)sizeof(PacketHead) || packetLength > pAccept->getMaxLength() ){
			LOG_ERROR("head length is invalid packetLength=%d", packetLength);
			break;	// 这里直接将数据丢弃
		}
		writeLength = std::min( (int)(nread-(recvBufferPtr-recvBuffer)), packetLength );
		// 创建Packet对象，并将数据写入
		pPacket = new Packet(packetLength);
		pPacket->retain();	// 如果数据没有全部收到，那么m_tempReadPacket会保持这个retain状态
		pPacket->write( recvBufferPtr, writeLength );
		recvBufferPtr += writeLength;
		if( pPacket->isCursorEnd() ){
			// 派发消息给对应的消息处理器
			pAccept->dispatchPacket(pPacket, pPacket->getHead()->command);
			pPacket->release();
			pPacket = NULL;
		}
		// 如果消息没有全部接收，那么将会放到临时包中等待下一次读数据操作
	}while(nread-(recvBufferPtr-recvBuffer) > (int)sizeof(PacketHead));
    pAccept->setTempReadPacket(pPacket);
}
void onAcceptReceivePacket(Accept* pAccept, Packet* pPacket){
	LOG_DEBUG("handle=%d packet length=%d", pAccept->getHandle(), pPacket->getLength());
	pAccept->sendPacket(pPacket);
}
void onAcceptDecrypt(Accept* pAccept, Packet* pPacket){
	LOG_DEBUG("handle=%d packet length=%d", pAccept->getHandle(), pPacket->getLength());
	if( pPacket->getLength() > 8 ){
		binary_decrypt(pPacket->getDataPtr()+8, pPacket->getLength()-8, GlobalSetting::getInstance()->getKey());
	}
}
void onAcceptEncrypt(Accept* pAccept, Packet* pPacket){
	LOG_DEBUG("handle=%d packet length=%d", pAccept->getHandle(), pPacket->getLength());
	binary_encrypt(pPacket->getDataPtr(), pPacket->getLength(), GlobalSetting::getInstance()->getKey());
}
void onReceiveHttp(Http* pHttp){
	LOG_DEBUG("handle=%d", pHttp->getHandle());
}
void onRemoveHttp(Http* pHttp){
	LOG_DEBUG("handle=%d", pHttp->getHandle());
}
void onHttpReceivePacket(Http* pHttp, Packet* pPacket){
	LOG_DEBUG("handle=%d packet length=%d", pHttp->getHandle(), pPacket->getLength());
	// here will skip the packet head
	pHttp->responseRequest(pPacket->getBody(), pPacket->getBodyLength());
}
void onCommandPing(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);

}
void onCommandPong(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);

}
void onCommandRegister(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);

}
void onCommandResponse(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);

}
void onCommandHiveRegister(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);

}
void onCommandHiveResponse(Accept* pAccept, Packet* pPacket, uint32 command){
	LOG_DEBUG("handle=%d packet length=%d command=%d", pAccept->getHandle(), pPacket->getLength(), command);

}

NS_HIVE_END
