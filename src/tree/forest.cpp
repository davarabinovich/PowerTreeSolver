
#include "tree/tree.h"





Forest::Node::Node (Node * prnt)
	: parent_ptr(prnt) 
{
	content = new Type();
	desces_ptr = new set<Node *>();
}


Forest::Node::Node (typename const Type & cntnt, Node * prnt)
	: parent_ptr(prnt) 
{
	content = new Type();
	*content = cntnt;

	desces_ptr = new set<Node*>();
}


Forest::Node::Node (Node && otherNode)
{
#pragma todo
}




inline const Type & Forest::Node::get () const
{
	return *content;
}



inline Type & Forest::Node::getToModify ()
{
	return *content;
}



inline void Forest::Node::record (const Type & origin)
{
	*content = origin;
}



inline void Forest::Node::setParent (Node * prnt)
{
	parent_ptr = prnt;
}



inline void Forest::Node::disconnectFromParent ()
{
	parent_ptr = nullptr;
}



inline void Forest::Node::addDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	
	desces_ptr->insert(desc);
}



inline void Forest::Node::disconnectDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	if (desces_ptr->count(desc) == 0)    throw exception("There no such a descendant");

	desces_ptr->erase(desc);
}



inline void Forest::Node::disconnectAllDesces ()
{
	desces_ptr->clear();
}



inline bool Forest::Node::hasParent () const
{
	if (parent_ptr == nullptr)
		return false;
	return true;
}



inline typename Forest::Node * Forest::Node::getParent () const
{
	return parent_ptr;
}



inline bool Forest::Node::hasDesces () const
{
	bool result = desces_ptr->empty();
	return result;
}


inline const set<typename Forest::Node *> & Forest::Node::getDesces () const
{
	return *desces_ptr;
}


set<Forest::Node *> * Forest::Node::getDescesSet () const
{
	return desces_ptr;
}






bool Forest::Node:: operator < (const Node & second) const
{
	return false;
}

inline Forest::Node::~Node ()
{
	delete desces_ptr;
	delete content;
}











inline void Forest::addRoot (key name, const Type & content)
{
	createRoot(name, content);
}



inline void Forest::pushFrontRoot (key name, key oldRootName, const Type & content)
{
	Node * newRoot_ptr = createRoot(name, content);
	Node * oldRoot_ptr = nodes[oldRootName];

	roots.erase(oldRoot_ptr);
	newRoot_ptr->addDesc(oldRoot_ptr);
	oldRoot_ptr->setParent(newRoot_ptr);
}



inline void Forest::insertDesc (key name, key parentName, const Type & content)
{
	//Node * newNode_ptr = createFreeNode(name, content);

	//Node * parent_ptr = nodes[parentName];
	//swap( newNode_ptr->getDescesSet(), parent_ptr->getDescesSet() );
	//parent_ptr->addDesc(newNode_ptr);

	//if (!parent_ptr->hasParent())
	//{
	//	roots.erase(parent_ptr);
	//	roots.insert(newNode_ptr);
	//}
}


inline void Forest::insertDesc (key name, key parentName, key descName, const Type & content)
{
	Node * newNode_ptr = createFreeNode(name, content);

	Node * parent_ptr = nodes[parentName];
	Node * desc_ptr   = nodes[descName];
	parent_ptr->addDesc(newNode_ptr);
	parent_ptr->disconnectDesc(desc_ptr);
	desc_ptr->setParent(newNode_ptr);
	newNode_ptr->addDesc(desc_ptr);

	if (!parent_ptr->hasParent())
	{
		roots.erase(parent_ptr);
		roots.insert(newNode_ptr);
	}
}


inline void Forest::pushBackLeaf (key name, key parentName, const Type & content)
{
	Node * newLeaf_ptr = createFreeNode(name, content);

	Node * parent_ptr = nodes[parentName];
	parent_ptr->addDecs(newLeaf_ptr);
	newLeaf_ptr->setParent(parent_ptr);
}



inline void Forest::popFrontRoot (key name)
{
#pragma todo validate execution deleting function for limit cases (e. g. roots)
	Node * deletedRoot_ptr = nodes[name];
	convertDescesToRoots();

	roots.erase(deletedRoot_ptr);
	deleteNode(deletedRoot_ptr);
}



inline void Forest::eraseDesc (key name)
{
	Node * deletedNode_ptr = nodes[name];
	convertDescesToRoots(deletedNode_ptr);

	popBackLeaf(name);
}



inline void Forest::eraseDesc (key name, key newDescesParentName)
{
	Node * deletedNode_ptr = nodes[name];
	moveAllDescesTo(deletedNode_ptr, newDescesParentName);

	popBackLeaf(name);
}



inline void Forest::popBackSubtree (key headerName)
{
	Node * header_ptr = nodes[headerName];
	header_ptr->getParent() ->disconnectDesc(header_ptr);

	deleteAllDescesSubtrees(header_ptr);
	deleteNode(header_ptr);
}



inline void Forest::popBackLeaf (key name)
{
	Node * deletedLeaf_ptr = nodes[name];
	deletedLeaf_ptr->getParent() ->disconnectDesc(deletedLeaf_ptr);

	deleteNode(deletedLeaf_ptr);
}



inline void Forest::moveSubtree (key headerName, key newParentName)
{
	Node * header_ptr = nodes[headerName];
	Node * parent_ptr = header_ptr->getParent();
	parent_ptr->disconnectDesc(header_ptr);
	header_ptr->disconnectFromParent();

	Node * newParent_ptr = nodes[newParentName];
	newParentName->addDesc(header_ptr);
	header_ptr->setParent(newParent_ptr);
}



inline void Forest::freeSubtree (key headerName)
{
	Node * header_ptr = nodes[headerName];
	Node * parent_ptr = header_ptr->getParent();
	parent_ptr->disconnectDesc(header_ptr);
	header_ptr->disconnectFromParent();

	roots.insert(header_ptr);
}



inline void Forest::moveNode (key name, key newParentName)
{
	Node * node_ptr = nodes[name];
	convertDescesToRoots(node_ptr);

	popBackLeaf(name);
}



inline void Forest::moveNode (key name, key newParentName, key newDescesParentName)
{

}



inline void Forest::freeNode (key name)
{
}



inline void Forest::freeNode (key name, key newDescesParentName)
{
	Node * node_ptr = nodes[name];
	moveAllDescesTo(node_ptr, newDescesParentName);

	roots.insert(node_ptr);
}



inline typename Forest::Node * Forest::createFreeNode (key name, const Type & content)
{
	Node * newNode_ptr = new Node(content);
	nodes[name] = newNode_ptr;

	return newNode_ptr;
}



inline typename Forest::Node * Forest::createRoot (key name, const Type & content)
{
	Node * newRoot_ptr = new Node(content);
	nodes[name] = newRoot_ptr;
	roots.insert(newRoot_ptr);

	return newRoot_ptr;
}



inline void Forest::deleteNode (Node * node_ptr)
{
	nodes.erase(node_ptr);
	delete node_ptr;
}



inline void Forest::deleteAllDescesSubtrees (Node * parent_ptr)
{
	AUTO_CONST_REF desces = parent_ptr->getDesces();
	for (auto & desc : desces)
		if ( desc->hasDesces() )
			deleteAllDescesSubtrees(desc);

	deleteNode(parent_ptr);
}



inline void Forest::convertDescesToRoots (Node * node_ptr)
{
	AUTO_CONST_REF desces = node_ptr->getDesces();
	for (auto & desc : desces)
	{
		desc->disconnectFromParent();
		roots.insert();
	}
}



inline void Forest::moveAllDescesTo (Node * node_ptr, key newParentName)
{
	AUTO_CONST_REF desces = node_ptr->getDesces();
	for (auto & desc : desces)
		nodes[newParentName] ->addDesc(desc);
}



inline void Forest::cutLinkBetween (Node * parent_ptr, Node * desc_ptr)
{
	parent_ptr->disconnectDesc(desc_ptr);
	desc_ptr->discoonectFromParetn();
}


inline void Forest::connectNodes(Node* parent_ptr, Node* desc_ptr)
{
}



