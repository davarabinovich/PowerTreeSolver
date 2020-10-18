
#include "forest/forest.h"





template <typename Type, typename key>
inline Forest<Type, key>::Node::Node (key nm, Node * prnt)
	: name(nm), parent_ptr(prnt) 
{
	content = new Type();
	desces_ptr = new set<Node *>();
}


template <typename Type, typename key>
inline Forest<Type, key>::Node::Node (key nm, typename const Type & cntnt, Node * prnt)
	: name(nm),  parent_ptr(prnt) 
{
	content = new Type();
	*content = cntnt;

	desces_ptr = new set<Node*>();
}


template <typename Type, typename key>
inline Forest<Type, key>::Node::Node (Node && otherNode)
{
#pragma todo
}




template <typename Type, typename key>
inline const Type & Forest<Type, key>::Node::get () const
{
	return *content;
}


template <typename Type, typename key>
inline Type & Forest<Type, key>::Node::getToModify ()
{
	return *content;
}


template <typename Type, typename key>
inline key Forest<Type, key>::Node::getName () const
{
	return name;
}


template <typename Type, typename key>
inline void Forest<Type, key>::Node::record (const Type & origin)
{
	*content = origin;
}


template <typename Type, typename key>
inline void Forest<Type, key>::Node::setParent (Node * prnt)
{
	parent_ptr = prnt;
}


template <typename Type, typename key>
inline void Forest<Type, key>::Node::disconnectFromParent ()
{
	parent_ptr = nullptr;
}


template <typename Type, typename key>
inline void Forest<Type, key>::Node::addDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	
	desces_ptr->insert(desc);
}


template <typename Type, typename key>
inline void Forest<Type, key>::Node::disconnectDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	if (desces_ptr->count(desc) == 0)    throw exception("There no such a descendant");

	desces_ptr->erase(desc);
}


template <typename Type, typename key>
inline void Forest<Type, key>::Node::disconnectAllDesces ()
{
	desces_ptr->clear();
}


template <typename Type, typename key>
inline bool Forest<Type, key>::Node::hasParent () const
{
	if (parent_ptr == nullptr)
		return false;
	return true;
}


template <typename Type, typename key>
inline typename Forest<Type, key>::Node * Forest<Type, key>::Node::getParent () const
{
	return parent_ptr;
}


template <typename Type, typename key>
inline bool Forest<Type, key>::Node::hasDesces () const
{
	bool result = desces_ptr->empty();
	return result;
}


template <typename Type, typename key>
inline const typename set<typename Forest<Type, key>::Node *> & Forest<Type, key>::Node::getDesces () const
{
	return *desces_ptr;
}


template <typename Type, typename key>
inline set<typename Forest<Type, key>::Node *> * Forest<Type, key>::Node::getDescesSet () const
{
	return desces_ptr;
}




template <typename Type, typename key>
inline Forest<Type, key>::Node::~Node ()
{
	delete desces_ptr;
	delete content;
}










#pragma todo refactor auxiliary functions
template <typename Type, typename key>
inline void Forest<Type, key>::addRoot (key name, const Type & content)
{
	createRoot(name, content);
}


template <typename Type, typename key>
inline void Forest<Type, key>::pushFrontRoot (key name, key oldRootName, const Type & content)
{
	auto newRoot_ptr = createRoot(name, content);
	auto oldRoot_ptr = nodes[oldRootName];

	roots.erase(oldRoot_ptr);
	newRoot_ptr->addDesc(oldRoot_ptr);
	oldRoot_ptr->setParent(newRoot_ptr);
}


template <typename Type, typename key>
inline void Forest<Type, key>::insertDesc (key name, key parentName, const Type & content)
{
	auto newNode_ptr = createFreeNode(name, content);
	auto parent_ptr = nodes[parentName];

	auto newNodeDesces_ptr = newNode_ptr->getDescesSet();
	auto parentDesces_ptr = parent_ptr->getDescesSet();
	swap(newNodeDesces_ptr, parentDesces_ptr);
	parent_ptr->addDesc(newNode_ptr);

	for (auto & desc : *newNodeDesces_ptr)
		desc->setParent(newNode_ptr);
}


template <typename Type, typename key>
inline void Forest<Type, key>::insertDesc (key name, key parentName, key descName, const Type & content)
{
	auto newNode_ptr = createFreeNode(name, content);

	auto parent_ptr = nodes[parentName];
	auto desc_ptr   = nodes[descName];
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


template <typename Type, typename key>
inline void Forest<Type, key>::pushBackLeaf (key name, key parentName, const Type & content)
{
	auto newLeaf_ptr = createFreeNode(name, content);

	auto parent_ptr = nodes[parentName];
	parent_ptr->addDesc(newLeaf_ptr);
	newLeaf_ptr->setParent(parent_ptr);
}


template <typename Type, typename key>
inline void Forest<Type, key>::popFrontRoot (key name)
{
#pragma todo validate execution deleting function for limit cases (e. g. roots)
	Node * deletedRoot_ptr = nodes[name];
	convertDescesToRoots(deletedRoot_ptr);

	roots.erase(deletedRoot_ptr);
	deleteNode(name);
}


template <typename Type, typename key>
inline void Forest<Type, key>::eraseDesc (key name)
{
	Node * deletedNode_ptr = nodes[name];
	convertDescesToRoots(deletedNode_ptr);

	popBackLeaf(name);
}


template <typename Type, typename key>
inline void Forest<Type, key>::eraseDesc (key name, key newDescesParentName)
{
	Node * deletedNode_ptr = nodes[name];
	moveAllDescesTo(deletedNode_ptr, newDescesParentName);

	popBackLeaf(name);
}


template <typename Type, typename key>
inline void Forest<Type, key>::popBackSubtree (key headerName)
{
	Node * header_ptr = nodes[headerName];
	header_ptr->getParent() ->disconnectDesc(header_ptr);

	deleteAllDescesSubtrees(header_ptr);
	deleteNode(headerName);
}


template <typename Type, typename key>
inline void Forest<Type, key>::popBackLeaf (key name)
{
	Node * deletedLeaf_ptr = nodes[name];
	deletedLeaf_ptr->getParent() ->disconnectDesc(deletedLeaf_ptr);

	deleteNode(name);
}


template <typename Type, typename key>
inline void Forest<Type, key>::moveSubtree (key headerName, key newParentName)
{
	Node * header_ptr = nodes[headerName];
	Node * parent_ptr = header_ptr->getParent();
	cutLinkBetween(parent_ptr, header_ptr);

	Node * newParent_ptr = nodes[newParentName];
	connectNodes(newParent_ptr, header_ptr);
}


template <typename Type, typename key>
inline void Forest<Type, key>::freeSubtree (key headerName)
{
	Node * header_ptr = nodes[headerName];
	Node * parent_ptr = header_ptr->getParent();
	cutLinkBetween(parent_ptr, header_ptr);

	roots.insert(header_ptr);
}


template <typename Type, typename key>
inline void Forest<Type, key>::moveNode (key name, key newParentName)
{
	Node * node_ptr = nodes[name];
	Node * parent_ptr = node_ptr->getParent();
	convertDescesToRoots(node_ptr);
	cutLinkBetween(parent_ptr, node_ptr);
	
	Node * newParent_ptr = nodes[newParentName];
	connectNodes(newParent_ptr, node_ptr);
}


template <typename Type, typename key>
inline void Forest<Type, key>::moveNode (key name, key newParentName, key newDescesParentName)
{
	Node * node_ptr = nodes[name];
	Node * parent_ptr = node_ptr->getParent();
	cutLinkBetween(parent_ptr, node_ptr);

	moveAllDescesTo(node_ptr, newDescesParentName);

	Node * newParent_ptr = nodes[newParentName];
	connectNodes(newParent_ptr, node_ptr);
}


template <typename Type, typename key>
inline void Forest<Type, key>::freeNode (key name)
{
	Node * node_ptr = nodes[name];
	Node * parent_ptr = node_ptr->getParent();
	cutLinkBetween(parent_ptr, node_ptr);

	convertDescesToRoots(node_ptr);
	roots.insert(node_ptr);
}


template <typename Type, typename key>
inline void Forest<Type, key>::freeNode (key name, key newDescesParentName)
{
	Node * node_ptr = nodes[name];
	moveAllDescesTo(node_ptr, newDescesParentName);

	roots.insert(node_ptr);
}


template <typename Type, typename key>
inline Type & Forest<Type, key>::operator [] (key name)
{
	auto content = nodes[name] ->getToModify();
	return content;
}


template <typename Type, typename key>
inline const Type & Forest<Type, key>::at (key name) const
{
	auto content = nodes.at(name) ->get();
	return content;
}


template <typename Type, typename key>
inline bool Forest<Type, key>::isExsist (key name) const
{
	bool result = (nodes.count(name) > 0);
	return result;
}


template <typename Type, typename key>
inline key Forest<Type, key>::getParentKey (key name) const
{
	key parentKey = nodes.at(name) ->getParent() ->getName();
	return parentKey;
}




template <typename Type, typename key>
inline Forest<Type, key>::~Forest<Type, key> ()
{
	for (auto node_rec : nodes)
	{
		auto node = node_rec.second;
		delete ( node->getDescesSet() );
		delete node;
	}
}




template <typename Type, typename key>
inline typename Forest<Type, key>::Node * Forest<Type, key>::createFreeNode (key name, const Type & content)
{
	auto newNode_ptr = new Node(name, content);
	nodes[name] = newNode_ptr;

	return newNode_ptr;
}


template <typename Type, typename key>
inline typename Forest<Type, key>::Node * Forest<Type, key>::createRoot (key name, const Type & content)
{
	auto newRoot_ptr = new Node(name, content);
	nodes[name] = newRoot_ptr;
	roots.insert(newRoot_ptr);

	return newRoot_ptr;
}


template <typename Type, typename key>
inline void Forest<Type, key>::deleteNode (key name)
{
	auto node_ptr = nodes[name];
	nodes.erase(name);
	delete node_ptr;
}


template <typename Type, typename key>
inline void Forest<Type, key>::deleteAllDescesSubtrees (Node * parent_ptr)
{
	AUTO_CONST_REF desces = parent_ptr->getDesces();
	for (auto & desc : desces)
		if ( desc->hasDesces() )
			deleteAllDescesSubtrees(desc);

	deleteNode( parent_ptr->getName() );
}


template <typename Type, typename key>
inline void Forest<Type, key>::convertDescesToRoots (Node * node_ptr)
{
	AUTO_CONST_REF desces = node_ptr->getDesces();
	for (auto & desc : desces)
	{
		desc->disconnectFromParent();
		roots.insert(desc);
	}
}


template <typename Type, typename key>
inline void Forest<Type, key>::moveAllDescesTo (Node * node_ptr, key newParentName)
{
	AUTO_CONST_REF desces = node_ptr->getDesces();
	for (auto & desc : desces)
		nodes[newParentName] ->addDesc(desc);
}


template <typename Type, typename key>
inline void Forest<Type, key>::cutLinkBetween (Node * parent_ptr, Node * desc_ptr)
{
	parent_ptr->disconnectDesc(desc_ptr);
	desc_ptr->disconnectFromParent();
}


template <typename Type, typename key>
inline void Forest<Type, key>::connectNodes (Node * parent_ptr, Node * desc_ptr)
{
	parent_ptr->addDesc(desc_ptr);
	desc_ptr->setParent(parent_ptr);
}
