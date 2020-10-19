
#include "forest/forest.h"





template <typename key, typename Type>
inline Forest<key, Type>::Node::Node (key nm, Node * prnt)
	: name(nm), parent_ptr(prnt) 
{
	content = new Type();
	desces_ptr = new set<Node *>();
}


template <typename key, typename Type>
inline Forest<key, Type>::Node::Node (key nm, typename const Type & cntnt, Node * prnt)
	: name(nm),  parent_ptr(prnt) 
{
	content = new Type();
	*content = cntnt;

	desces_ptr = new set<Node*>();
}


template <typename key, typename Type>
inline Forest<key, Type>::Node::Node (Node && otherNode)
{
#pragma todo
}




template <typename key, typename Type>
inline const Type & Forest<key, Type>::Node::get () const
{
	return *content;
}


template <typename key, typename Type>
inline Type & Forest<key, Type>::Node::getToModify ()
{
	return *content;
}


template <typename key, typename Type>
inline key Forest<key, Type>::Node::getName () const
{
	return name;
}


template <typename key, typename Type>
inline void Forest<key, Type>::Node::record (const Type & origin)
{
	*content = origin;
}


template <typename key, typename Type>
inline void Forest<key, Type>::Node::setParent (Node * prnt)
{
	parent_ptr = prnt;
}


template <typename key, typename Type>
inline void Forest<key, Type>::Node::disconnectFromParent ()
{
	parent_ptr = nullptr;
}


template <typename key, typename Type>
inline void Forest<key, Type>::Node::addDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	
	desces_ptr->insert(desc);
}


template <typename key, typename Type>
inline void Forest<key, Type>::Node::disconnectDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	if (desces_ptr->count(desc) == 0)    throw exception("There no such a descendant");

	desces_ptr->erase(desc);
}


template <typename key, typename Type>
inline void Forest<key, Type>::Node::disconnectAllDesces ()
{
	desces_ptr->clear();
}


template <typename key, typename Type>
inline bool Forest<key, Type>::Node::hasParent () const
{
	if (parent_ptr == nullptr)
		return false;
	return true;
}


template <typename key, typename Type>
inline typename Forest<key, Type>::Node * Forest<key, Type>::Node::getParent () const
{
	return parent_ptr;
}


template <typename key, typename Type>
inline bool Forest<key, Type>::Node::hasDesces () const
{
	bool result = desces_ptr->empty();
	return result;
}


template <typename key, typename Type>
inline const typename set<typename Forest<key, Type>::Node *> & Forest<key, Type>::Node::getDesces () const
{
	return *desces_ptr;
}


template <typename key, typename Type>
inline set<typename Forest<key, Type>::Node *> * Forest<key, Type>::Node::getDescesSet () const
{
	return desces_ptr;
}




template <typename key, typename Type>
inline Forest<key, Type>::Node::~Node ()
{
	delete desces_ptr;
	delete content;
}










#pragma todo refactor auxiliary functions
template <typename key, typename Type>
inline void Forest<key, Type>::addRoot (key name, const Type & content)
{
	createRoot(name, content);
}


template <typename key, typename Type>
inline void Forest<key, Type>::pushFrontRoot (key name, key oldRootName, const Type & content)
{
	auto newRoot_ptr = createRoot(name, content);
	auto oldRoot_ptr = nodes[oldRootName];

	roots.erase(oldRoot_ptr);
	newRoot_ptr->addDesc(oldRoot_ptr);
	oldRoot_ptr->setParent(newRoot_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::insertDesc (key name, key parentName, const Type & content)
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


template <typename key, typename Type>
inline void Forest<key, Type>::insertDesc (key name, key parentName, key descName, const Type & content)
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


template <typename key, typename Type>
inline void Forest<key, Type>::pushBackLeaf (key name, key parentName, const Type & content)
{
	auto newLeaf_ptr = createFreeNode(name, content);

	auto parent_ptr = nodes[parentName];
	parent_ptr->addDesc(newLeaf_ptr);
	newLeaf_ptr->setParent(parent_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::popFrontRoot (key name)
{
#pragma todo validate execution deleting function for limit cases (e. g. roots)
	Node * deletedRoot_ptr = nodes[name];
	convertDescesToRoots(deletedRoot_ptr);

	roots.erase(deletedRoot_ptr);
	deleteNode(name);
}


template <typename key, typename Type>
inline void Forest<key, Type>::eraseDesc (key name)
{
	Node * deletedNode_ptr = nodes[name];
	convertDescesToRoots(deletedNode_ptr);

	popBackLeaf(name);
}


template <typename key, typename Type>
inline void Forest<key, Type>::eraseDesc (key name, key newDescesParentName)
{
	Node * deletedNode_ptr = nodes[name];
	moveAllDescesTo(deletedNode_ptr, newDescesParentName);

	popBackLeaf(name);
}


template <typename key, typename Type>
inline void Forest<key, Type>::popBackSubtree (key headerName)
{
	Node * header_ptr = nodes[headerName];
	header_ptr->getParent() ->disconnectDesc(header_ptr);

	deleteAllDescesSubtrees(header_ptr);
	deleteNode(headerName);
}


template <typename key, typename Type>
inline void Forest<key, Type>::popBackLeaf (key name)
{
	auto deletedLeaf_ptr = nodes[name];
	deletedLeaf_ptr->getParent() ->disconnectDesc(deletedLeaf_ptr);

	deleteNode(name);
}


template<typename key, typename Type>
inline void Forest<key, Type>::eraseAllDesces (key parentName)
{
	auto parent_ptr = nodes[parentName];
	deleteAllDescesSubtrees(parent_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::moveSubtree (key headerName, key newParentName)
{
	Node * header_ptr = nodes[headerName];
	Node * parent_ptr = header_ptr->getParent();
	cutLinkBetween(parent_ptr, header_ptr);

	Node * newParent_ptr = nodes[newParentName];
	connectNodes(newParent_ptr, header_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::freeSubtree (key headerName)
{
	auto header_ptr = nodes[headerName];
	auto parent_ptr = header_ptr->getParent();
	cutLinkBetween(parent_ptr, header_ptr);

	roots.insert(header_ptr);
}


template<typename key, typename Type>
inline void Forest<key, Type>::moveAllDesces (key parentName, key newParentName)
{
	auto parent_ptr = nodes[parentName];
	moveAllDescesTo(parent_ptr, newParentName);
}


template<typename key, typename Type>
inline void Forest<key, Type>::freeAllDesces (key parentName)
{
	auto parent_ptr = nodes[parentName];
	convertDescesToRoots(parent_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::moveNode (key name, key newParentName)
{
	Node * node_ptr = nodes[name];
	Node * parent_ptr = node_ptr->getParent();
	convertDescesToRoots(node_ptr);
	cutLinkBetween(parent_ptr, node_ptr);
	
	Node * newParent_ptr = nodes[newParentName];
	connectNodes(newParent_ptr, node_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::moveNode (key name, key newParentName, key newDescesParentName)
{
	Node * node_ptr = nodes[name];
	Node * parent_ptr = node_ptr->getParent();
	cutLinkBetween(parent_ptr, node_ptr);

	moveAllDescesTo(node_ptr, newDescesParentName);

	Node * newParent_ptr = nodes[newParentName];
	connectNodes(newParent_ptr, node_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::freeNode (key name)
{
	Node * node_ptr = nodes[name];
	Node * parent_ptr = node_ptr->getParent();
	cutLinkBetween(parent_ptr, node_ptr);

	convertDescesToRoots(node_ptr);
	roots.insert(node_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::freeNode (key name, key newDescesParentName)
{
	Node * node_ptr = nodes[name];
	moveAllDescesTo(node_ptr, newDescesParentName);

	roots.insert(node_ptr);
}


template <typename key, typename Type>
inline Type & Forest<key, Type>::operator [] (key name)
{
	auto content = nodes[name] ->getToModify();
	return content;
}


template <typename key, typename Type>
inline const Type & Forest<key, Type>::at (key name) const
{
	auto content = nodes.at(name) ->get();
	return content;
}


template <typename key, typename Type>
inline bool Forest<key, Type>::isExsist (key name) const
{
	bool result = (nodes.count(name) > 0);
	return result;
}


template <typename key, typename Type>
inline key Forest<key, Type>::getParentKey (key name) const
{
	key parentKey = nodes.at(name) ->getParent() ->getName();
	return parentKey;
}




template <typename key, typename Type>
inline Forest<key, Type>::~Forest<key, Type> ()
{
	for (auto node_rec : nodes)
	{
		auto node = node_rec.second;
		delete ( node->getDescesSet() );
		delete node;
	}
}




template <typename key, typename Type>
inline typename Forest<key, Type>::Node * Forest<key, Type>::createFreeNode (key name, const Type & content)
{
	auto newNode_ptr = new Node(name, content);
	nodes[name] = newNode_ptr;

	return newNode_ptr;
}


template <typename key, typename Type>
inline typename Forest<key, Type>::Node * Forest<key, Type>::createRoot (key name, const Type & content)
{
	auto newRoot_ptr = new Node(name, content);
	nodes[name] = newRoot_ptr;
	roots.insert(newRoot_ptr);

	return newRoot_ptr;
}


template <typename key, typename Type>
inline void Forest<key, Type>::deleteNode (key name)
{
	auto node_ptr = nodes[name];
	nodes.erase(name);
	delete node_ptr;
}


template <typename key, typename Type>
inline void Forest<key, Type>::deleteAllDescesSubtrees (Node * parent_ptr)
{
	AUTO_CONST_REF desces = parent_ptr->getDesces();
	for (auto & desc : desces)
		if ( desc->hasDesces() )
			deleteAllDescesSubtrees(desc);

	deleteNode( parent_ptr->getName() );
}


template <typename key, typename Type>
inline void Forest<key, Type>::convertDescesToRoots (Node * node_ptr)
{
	AUTO_CONST_REF desces = node_ptr->getDesces();
	for (auto & desc : desces)
	{
		desc->disconnectFromParent();
		roots.insert(desc);
	}
}


template <typename key, typename Type>
inline void Forest<key, Type>::moveAllDescesTo (Node * node_ptr, key newParentName)
{
	AUTO_CONST_REF desces = node_ptr->getDesces();
	for (auto & desc : desces)
		nodes[newParentName] ->addDesc(desc);
}


template <typename key, typename Type>
inline void Forest<key, Type>::cutLinkBetween (Node * parent_ptr, Node * desc_ptr)
{
	parent_ptr->disconnectDesc(desc_ptr);
	desc_ptr->disconnectFromParent();
}


template <typename key, typename Type>
inline void Forest<key, Type>::connectNodes (Node * parent_ptr, Node * desc_ptr)
{
	parent_ptr->addDesc(desc_ptr);
	desc_ptr->setParent(parent_ptr);
}
