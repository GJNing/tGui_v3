#include "TObject/TObject.h"


//-----------------------------OBJECT-------------------------
TObject::TObject(const char* n , TObject* obj )
{
	name = NULL;
	setName(n);
	parents = obj;
	if (obj)
		obj->addChild(this);
	childList = NULL;
	TConnections = NULL;
}

TObject::~TObject()
{
	destroyChild(this);
	delete childList;
	if (TConnections)
	{
		TConnections->setAutoDelete(true);
		TConnections->clear();
	}
	if (name)
		delete name;
}

void TObject::setName(const char* str) 
{ 
	if(!name)
		name = new TString(str);
	else
		name->setStr(str);
}

void TObject::regist(TObject* obj)
{
	obj->addChild(this);
	this->setParent(obj);
}
void TObject::logout(TObject* obj)
{
	obj->remChild(this);
	this->setParent(NULL);
}

tLNode<TObject*> * TObject::unlink(TObject* obj)
{
	if (getChildList())
		return getChildList()->unlink(obj);
	return NULL;
}

void TObject::relink(tLNode<TObject*> * node)
{
	if(getChildList())
		getChildList()->relink(node);
}

void TObject::addChild(TObject* child)
{
	if (!childList)
		childList = new TObjList();
	//???
	if((childList->find(child)) < 0)
		childList->append(child);
}

void TObject::remChild(TObject* child)
{
	if (!childList)
		return;
	//???--????к???
	destroyChild(child);
	//???
	childList->remove(child);
}

void TObject::destroyChild(TObject* obj)
{
	if (obj->childList)
	{	
		destroyChild(obj->childList->getLast());
		delete(obj->childList->getLast());
		obj->childList->removeLast();
	}
}



bool TObject::connect(func  sig, TObject* receiver, func  slot)
{
	if (!TConnections)
		TConnections = new TConnectList();
	int32 num = TConnections->find(sig);//??????????????????????
	if (num >= 0)//????
	{//??????????????????в?
		TFuncList* funclist = TConnections->at(num)->slotList;
		TObjList* objlist = TConnections->at(num)->recvList;
		if (funclist->find(slot) < 0)
		{
			funclist->append(slot);
			objlist->append(receiver);
		}
	}
	else//??????
	{
		TConnection* con = new TConnection(sig, receiver, slot);
		if (!con)
			return false;
		TConnections->append(con);
	}
	return true;
}

bool TObject::disconnect(func  sig, TObject* receiver, func  slot)
{
	if (!TConnections)
		return true;
	int32 num = TConnections->find(sig);
	if (num >= 0)//????
	{
		TFuncList* funclist = TConnections->at(num)->slotList;
		TObjList* objlist = TConnections->at(num)->recvList;
		funclist->remove(slot);
		objlist->remove(receiver);
	}
	return true;
}

void TObject::callSlot(func sig, int32 d1, int32 d2)
{
	if (getConnections())
	{
		int num = getConnections()->find(sig);
		if (num >= 0)
			getConnections()->at(num)->active(d1,d2);
	}
}
