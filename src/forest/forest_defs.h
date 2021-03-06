
#pragma once


#include "forest/forest.h"




template <typename Key, typename Type>
inline Forest<Key, Type>::Node::Node (Key genName, Node * genParent_ptr)
	: name(genName), parent_ptr(genParent_ptr)
{
	if (parent_ptr != nullptr)
		updateNestingLevel();

	content = new Type();
	desces_ptr = new set<Node *>();
}


template <typename Key, typename Type>
inline Forest<Key, Type>::Node::Node (Key genName, typename const Type & genContent, Node * genParent_ptr)
	: name(genName)
{
	if (parent_ptr != nullptr)
		updateNestingLevel();

	content = new Type();
	*content = genContent;

	desces_ptr = new set<Node*>();
}




template <typename Key, typename Type>
inline const Type & Forest<Key, Type>::Node::get () const
{
	return *content;
}


template <typename Key, typename Type>
inline Type & Forest<Key, Type>::Node::getToModify ()
{
	return *content;
}


template <typename Key, typename Type>
inline Key Forest<Key, Type>::Node::getName () const
{
	return name;
}


template <typename Key, typename Type>
unsigned Forest<Key, Type>::Node::getNestingLevel () const
{
	return nestingLevel;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::Node::hasParent () const
{
	if (parent_ptr == nullptr)
		return false;
	return true;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::Node * Forest<Key, Type>::Node::getParent () const
{
	return parent_ptr;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::Node::hasDesces () const
{
	bool result = !( desces_ptr->empty() );
	return result;
}


template <typename Key, typename Type>
inline const typename set<typename Forest<Key, Type>::Node *> & Forest<Key, Type>::Node::getDesces () const
{
	return *desces_ptr;
}


template <typename Key, typename Type>
inline set<typename Forest<Key, Type>::Node *> * Forest<Key, Type>::Node::getDescesSet () const
{
	return desces_ptr;
}


template<typename Key, typename Type>
inline void Forest<Key, Type>::Node::rename (Key newName)
{
	name = newName;
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::Node::record (const Type & origin)
{
	*content = origin;
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::Node::setParent (Node * prnt)
{
	parent_ptr = prnt;
	updateNestingLevel();
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::Node::disconnectFromParent ()
{
	parent_ptr = nullptr;
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::Node::addDesc (Node * desc)
{
	desces_ptr->insert(desc);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::Node::disconnectDesc (Node * desc)
{
	desces_ptr->erase(desc);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::Node::disconnectAllDesces ()
{
	desces_ptr->clear();
}




template <typename Key, typename Type>
void Forest<Key, Type>::Node::updateNestingLevel ()
{
	nestingLevel = getParent()->nestingLevel + 1;
}




template <typename Key, typename Type>
inline Forest<Key, Type>::Node::~Node ()
{
	delete desces_ptr;
	delete content;
}









template <typename Key, typename Type>
inline Forest<Key, Type>::iterator::iterator ()
{;}


template <typename Key, typename Type>
inline Forest<Key, Type>::iterator::iterator (const iterator & it)
	: nodesStack(it.nodesStack), roots_ptr(it.roots_ptr) {;}


template <typename Key, typename Type>
inline Forest<Key, Type>::iterator::iterator (nodes_set_it it, set<Node *> * roots)
	: roots_ptr(roots)
{
	if (it == roots_ptr->end())    return;

	nodesStack.push_front(it);
	while ((*it)->hasParent())
	{
		auto parent_it = getParentsSetIt(it);
		nodesStack.push_front(parent_it);
	}
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::iterator::operator != (const iterator & other_it) const
{
	bool isEmpty = nodesStack.empty();
	bool isOtherEmpty = other_it.nodesStack.empty();

	if (isEmpty != isOtherEmpty)
		return true;
	else if (isEmpty)
		return false;
		

	bool result = (*nodesStack.back() == *other_it.nodesStack.back());
	return result;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::iterator::operator == (const iterator & other_it) const
{
	bool result = !(*this != other_it);
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::iterator Forest<Key, Type>::iterator::operator++ ()
{
	auto & it = nodesStack.back();

	if ( !(*it)->hasDesces() )
	{

		if (!isLastDesc())
		{
			it++;
		}
		else
		{
			while (isLastDesc()  &&  ( (*nodesStack.back()) ->hasParent() ))
				nodesStack.pop_back();

			if (!isLastDesc())
				nodesStack.back()++;
			else
				nodesStack.clear(); // This iterator points to the end of the Forest now
		}

	}
	else
	{
		AUTO_CONST_REF descesSet = (*it)->getDesces();
		nodesStack.push_back(descesSet.begin());
	}

	return *this;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::iterator Forest<Key, Type>::iterator::operator++ (int)
{
	auto temp_it = *this;
	++(*this);
	return temp_it;
}


template <typename Key, typename Type>
inline pair<Key, Type &> Forest<Key, Type>::iterator::operator * () const
{
	if (nodesStack.empty())    throw end_iterator_dereferencing("dereferencing operator *");

	auto * node_ptr = *nodesStack.back();
	pair<Key, Type &> nodeWithName = { node_ptr->getName(), node_ptr->getToModify() };
	return nodeWithName;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::iterator Forest<Key, Type>::iterator::operator = (const iterator & other_it)
{
	nodesStack = other_it.nodesStack;
	return *this;
}


template <typename Key, typename Type>
typename Forest<Key, Type>::nodes_set_it Forest<Key, Type>::iterator::getParentsSetIt (typename set<Node *>::iterator it)
{
	nodes_set_it result_it;
	auto parent_ptr = (*it)->getParent();
	const set<Node *> * setWithParent;

	if (!parent_ptr->hasDesces())
		setWithParent = &(parent_ptr->getParent()->getDesces());
	else
		setWithParent = roots_ptr;

	result_it = find(setWithParent->begin(), setWithParent->end(), parent_ptr);
	return result_it;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::iterator::isLastDesc () const
{
	if (nodesStack.empty())    throw end_iterator_dereferencing("isLastDesc");


	bool result;
	auto temp_it = nodesStack.back();

	if ((*temp_it)->hasParent())
	{
		AUTO_CONST_REF parentsDescesSet = (*temp_it)->getParent()->getDesces();
		result = (next(temp_it) == parentsDescesSet.end());
	}
	else
	{
		result = (next(temp_it) == roots_ptr->end());
	}

	return result;
}










template <typename Key, typename Type>
inline Forest<Key, Type>::const_iterator::const_iterator ()
{;}


template <typename Key, typename Type>
inline Forest<Key, Type>::const_iterator::const_iterator (const const_iterator & it)
	: nodesStack(it.nodesStack), roots_ptr(it.roots_ptr) {;}


template <typename Key, typename Type>
inline Forest<Key, Type>::const_iterator::const_iterator (const_nodes_set_it it, const set<Node *> * roots)
	: roots_ptr(roots)
{
	if (it == roots_ptr->cend())    return;

	nodesStack.push_front(it);
	while ((*it)->hasParent())
	{
		auto parent_it = getParentsSetIt(it);
		nodesStack.push_front(parent_it);
	}
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::const_iterator::operator != (const const_iterator & other_it) const
{
	bool isEmpty = nodesStack.empty();
	bool isOtherEmpty = other_it.nodesStack.empty();

	if (isEmpty != isOtherEmpty)
		return true;
	else if (isEmpty)
		return false;


	bool result = (*nodesStack.back() == *other_it.nodesStack.back());
	return result;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::const_iterator::operator == (const const_iterator & other_it) const
{
	bool result = !(*this != other_it);
	return result;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::const_iterator Forest<Key, Type>::const_iterator::operator++ ()
{
	auto & it = nodesStack.back();

	if (!(*it)->hasDesces())
	{

		if (!isLastDesc())
		{
			it++;
		}
		else
		{
			while (isLastDesc() && ((*nodesStack.back())->hasParent()))
				nodesStack.pop_back();

			if (!isLastDesc())
				nodesStack.back()++;
			else
				nodesStack.clear(); // This iterator points to the end of the Forest now
		}

	}
	else
	{
		AUTO_CONST_REF descesSet = (*it)->getDesces();
		nodesStack.push_back(descesSet.begin());
	}

	return *this;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::const_iterator Forest<Key, Type>::const_iterator::operator++ (int)
{
	auto temp_it = *this;
	++(*this);
	return temp_it;
}


template <typename Key, typename Type>
inline pair<Key, Type> Forest<Key, Type>::const_iterator::operator * () const
{
	if (nodesStack.empty())    throw end_iterator_dereferencing("dereferencing operator *");

	auto node = *nodesStack.back();
	auto content = make_pair(node->getName(), node->get());
	return content;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::const_iterator Forest<Key, Type>::const_iterator::operator = (const const_iterator & other_it)
{
	nodesStack = other_it.nodesStack;
	return *this;
}


template <typename Key, typename Type>
typename Forest<Key, Type>::const_nodes_set_it Forest<Key, Type>::const_iterator::getParentsSetIt (typename set<Node *>::const_iterator it)
{
	const_nodes_set_it result_it;
	auto parent_ptr = (*it)->getParent();
	const set<Node *> * setWithParent;

	if (!parent_ptr->hasDesces())
		setWithParent = &(parent_ptr->getParent()->getDesces());
	else
		setWithParent = roots_ptr;

	result_it = find(setWithParent->cbegin(), setWithParent->cend(), parent_ptr);
	return result_it;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::const_iterator::isLastDesc() const
{
	if (nodesStack.empty())    throw end_iterator_dereferencing("isLastDesc");


	bool result;
	auto temp_it = nodesStack.back();

	if ((*temp_it)->hasParent())
	{
		AUTO_CONST_REF parentsDescesSet = (*temp_it)->getParent()->getDesces();
		result = (next(temp_it) == parentsDescesSet.end());
	}
	else
	{
		result = (next(temp_it) == roots_ptr->end());
	}

	return result;
}










template <typename Key, typename Type>
inline Forest<Key, Type>::desces_group_iterator::desces_group_iterator (const desces_group_iterator & gen_it)
	: it(gen_it.it)    {;}


template <typename Key, typename Type>
inline Forest<Key, Type>::desces_group_iterator::desces_group_iterator (typename nodes_set_it gen_it)
	: it(gen_it)    {;}




template <typename Key, typename Type>
inline bool Forest<Key, Type>::desces_group_iterator::operator != (const desces_group_iterator & other_it) const
{
	bool result = (it != other_it.it);
	return result;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::desces_group_iterator::operator == (const desces_group_iterator & other_it) const
{
	bool result = !(*this != other_it);
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::desces_group_iterator Forest<Key, Type>::desces_group_iterator::operator++ ()
{
	it++;
	return *this;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::desces_group_iterator Forest<Key, Type>::desces_group_iterator::operator++ (int)
{
	auto temp_it = *this;
	++(*this);
	return temp_it;
}


template <typename Key, typename Type>
inline pair<Key, Type &> Forest<Key, Type>::desces_group_iterator::operator * () const
{
	auto node_ptr = *it;
	pair<Key, Type &> nodeWithName = { node_ptr->getName(), node_ptr->getToModify() };
	return nodeWithName;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::desces_group_iterator Forest<Key, Type>::desces_group_iterator::operator = (const desces_group_iterator & other_it)
{
	it = other_it.it;
}










template <typename Key, typename Type>
inline Forest<Key, Type>::const_desces_group_iterator::const_desces_group_iterator (const const_desces_group_iterator & gen_it)
	: it(gen_it.it) {;}


template <typename Key, typename Type>
inline Forest<Key, Type>::const_desces_group_iterator::const_desces_group_iterator (typename const_nodes_set_it gen_it)
	: it(gen_it) {;}




template <typename Key, typename Type>
inline bool Forest<Key, Type>::const_desces_group_iterator::operator != (const const_desces_group_iterator & other_it) const
{
	bool result = (it != other_it.it);
	return result;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::const_desces_group_iterator::operator == (const const_desces_group_iterator & other_it) const
{
	bool result = !(*this != other_it);
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::const_desces_group_iterator Forest<Key, Type>::const_desces_group_iterator::operator++ ()
{
	it++;
	return *this;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::const_desces_group_iterator Forest<Key, Type>::const_desces_group_iterator::operator++ (int)
{
	auto temp_it = *this;
	++(*this);
	return temp_it;
}


template <typename Key, typename Type>
inline pair<Key, Type> Forest<Key, Type>::const_desces_group_iterator::operator * () const
{
	auto node_ptr = *it;
	auto content = make_pair(node_ptr->getName(), node_ptr->get());
	return content;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::const_desces_group_iterator Forest<Key, Type>::const_desces_group_iterator::operator = 
       (const const_desces_group_iterator & other_it)
{
	it = other_it.it;
}










template <typename Key, typename Type>
inline void Forest<Key, Type>::addRoot (Key name, const Type & content)
{
	if (name == "")    throw empty_name("addRoot");
	if (nodes.count(name) == 1)    throw busy_name("addRoot");

	createRoot(name, content);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::pushFrontRoot (Key name, Key oldRootName, const Type & content)
{
	if (name == "")    throw empty_name("pushFrontRoot; name");
	if (nodes.count(name) == 1)    throw busy_name("pushFrontRoo; name");

	if (nodes.count(oldRootName) == 0)    throw non_excistent_node("pushFrontRoot; oldRootName");


	auto newRoot_ptr = createRoot(name, content);
	auto oldRoot_ptr = nodes[oldRootName];

	roots.erase(oldRoot_ptr);
	newRoot_ptr->addDesc(oldRoot_ptr);
	oldRoot_ptr->setParent(newRoot_ptr);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::insertDesc (Key name, Key parentName, const Type & content)
{
	if (name == "")    throw empty_name("insertDesc; name");
	if (nodes.count(name) == 1)    throw busy_name("insertDesc; name");

	if (nodes.count(parentName) == 0)    throw non_excistent_node("insertDesc; parentName");


	auto newNode_ptr = createFreeNode(name, content);
	auto parent_ptr = nodes[parentName];

	auto newNodeDesces_ptr = newNode_ptr->getDescesSet();
	auto parentDesces_ptr = parent_ptr->getDescesSet();
	swap(newNodeDesces_ptr, parentDesces_ptr);
	parent_ptr->addDesc(newNode_ptr);

	for (auto & desc : *newNodeDesces_ptr)
		desc->setParent(newNode_ptr);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::insertDesc (Key name, Key parentName, Key descName, const Type & content)
{
	if (name == "")    throw empty_name("insertDesc; name");
	if (nodes.count(name) == 1)    throw busy_name("insertDesc; name");

	if (nodes.count(parentName) == 0)    throw non_excistent_node("insertDesc; parentName");
	if (nodes.count(descName)   == 0)    throw non_excistent_node("insertDesc; descName");


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


template <typename Key, typename Type>
inline void Forest<Key, Type>::pushBackLeaf (Key name, Key parentName, const Type & content)
{
	if (name == "")    throw empty_name("pushBackLeaf; name");
	if (nodes.count(name) == 1)    throw busy_name("pushBackLeaf; name");

	if (nodes.count(parentName) == 0)    throw non_excistent_node("pushBackLeaf; parentName");


	auto newLeaf_ptr = createFreeNode(name, content);

	auto parent_ptr = nodes[parentName];
	parent_ptr->addDesc(newLeaf_ptr);
	newLeaf_ptr->setParent(parent_ptr);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::popFrontRoot (Key name)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("popFrontRoot");
	if ( roots.count(nodes[name]) == 0 )    throw not_root("popFrontRoot");


	auto deletedRoot_ptr = nodes[name];
	convertDescesToRoots(deletedRoot_ptr);

	roots.erase(deletedRoot_ptr);
	deleteNode(name);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::eraseDesc (Key name)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("eraseDesc");


	auto deletedNode_ptr = nodes[name];
	convertDescesToRoots(deletedNode_ptr);

	deleteLeaf(name);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::eraseDesc (Key name, Key newDescesParentName)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("eraseDesc; name");
	if (nodes.count(name) == 0)    throw non_excistent_node("eraseDesc; newDescesParentName");


	auto deletedNode_ptr = nodes[name];
	moveAllDescesTo(deletedNode_ptr, newDescesParentName);

	deleteLeaf(name);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::popBackSubtree (Key headerName)
{
	if (nodes.count(headerName) == 0)    throw non_excistent_node("popBackSubtree");


	auto header_ptr = nodes[headerName];
	if ( header_ptr->hasParent() )
		header_ptr->getParent() ->disconnectDesc(header_ptr);
	else
		roots.erase(header_ptr);

	deleteAllDescesSubtrees(header_ptr);
	deleteNode(headerName);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::popBackLeaf (Key name)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("popBackLeaf");
	if ( nodes[name] ->hasDesces() )    throw not_leaf("popBackLeaf");


	auto deletedLeaf_ptr = nodes[name];

	if ( deletedLeaf_ptr->hasParent() )
		deletedLeaf_ptr->getParent() ->disconnectDesc(deletedLeaf_ptr);
	else
		roots.erase(deletedLeaf_ptr);

	deleteNode(name);
}


template<typename Key, typename Type>
inline void Forest<Key, Type>::eraseAllDesces (Key parentName)
{
	if (nodes.count(parentName) == 0)    throw non_excistent_node("eraseAllDesces");


	auto parent_ptr = nodes[parentName];
	deleteAllDescesSubtrees(parent_ptr);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::moveSubtree (Key headerName, Key newParentName)
{
	if (nodes.count(headerName)    == 0)    throw non_excistent_node("moveSubtree; headerName");
	if (nodes.count(newParentName) == 0)    throw non_excistent_node("moveSubtree; newParentName");
	
	if ( isFirstInSubtreeOfSecond(newParentName, headerName) )    throw closing_motion("moveSubtree");


	auto header_ptr = nodes[headerName];
	
	if ( header_ptr->hasParent() )
	{
		auto parent_ptr = header_ptr->getParent();
		cutLinkBetween(parent_ptr, header_ptr);
	}
	else
		roots.erase(header_ptr);

	auto newParent_ptr = nodes[newParentName];
	connectNodes(newParent_ptr, header_ptr);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::freeSubtree (Key headerName)
{
	if (nodes.count(headerName) == 0)    throw non_excistent_node("freeSubtree");


	auto header_ptr = nodes[headerName];
	
	if ( header_ptr->hasParent() )
	{
		auto parent_ptr = header_ptr->getParent();
		cutLinkBetween(parent_ptr, header_ptr);
		
		roots.insert(header_ptr);
	}
}


template<typename Key, typename Type>
inline void Forest<Key, Type>::moveAllDesces (Key parentName, Key newParentName)
{
	if (nodes.count(parentName)    == 0)    throw non_excistent_node("moveAllDesces; parentName");
	if (nodes.count(newParentName) == 0)    throw non_excistent_node("moveAllDesces; newParentName");

	if ( isFirstInSubtreeOfSecond(newParentName, parentName) )    throw closing_motion("moveAllDesces");


	auto parent_ptr = nodes[parentName];
	moveAllDescesTo(parent_ptr, newParentName);
}


template<typename Key, typename Type>
inline void Forest<Key, Type>::freeAllDesces (Key parentName)
{
	if (nodes.count(parentName) == 0)    throw non_excistent_node("freeAllDesces");

	auto parent_ptr = nodes[parentName];
	convertDescesToRoots(parent_ptr);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::moveNode (Key name, Key newParentName)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("moveNode; name");
	if (nodes.count(newParentName) == 0)    throw non_excistent_node("moveNode; newParentName");

	if ( isFirstInSubtreeOfSecond(newParentName, name) )    throw closing_motion("moveNode");


	auto node_ptr = nodes[name];
	convertDescesToRoots(node_ptr);

	if ( node_ptr->hasParent() )
	{
		auto parent_ptr = node_ptr->getParent();
		cutLinkBetween(parent_ptr, node_ptr);
	}
	else
		roots.erase(node_ptr);
		
	auto newParent_ptr = nodes[newParentName];
	connectNodes(newParent_ptr, node_ptr);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::moveNode (Key name, Key newParentName, Key newDescesParentName)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("moveNode; name");
	if (nodes.count(newParentName) == 0)    throw non_excistent_node("moveNode; newParentName");
	if (nodes.count(newDescesParentName) == 0)    throw non_excistent_node("moveNode; newDescesParentName");

	if ( isFirstInSubtreeOfSecond(newParentName, name) )    throw closing_motion("moveNode, newParentName");
	if ( isFirstInSubtreeOfSecond(newDescesParentName, name) )    throw closing_motion("moveNode, newDescesParentName");


	auto node_ptr = nodes[name];
	moveAllDescesTo(node_ptr, newDescesParentName);

	if ( node_ptr->hasParent() )
	{
		auto parent_ptr = node_ptr->getParent();
		cutLinkBetween(parent_ptr, node_ptr);
	}
	else
		roots.erase(node_ptr);

	auto newParent_ptr = nodes[newParentName];
	connectNodes(newParent_ptr, node_ptr);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::freeNode (Key name)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("freeNode");


	auto node_ptr = nodes[name];
	convertDescesToRoots(node_ptr);

	if ( node_ptr->hasParent() )
	{
		auto parent_ptr = node_ptr->getParent();
		cutLinkBetween(parent_ptr, node_ptr);
		
		roots.insert(node_ptr);
	}
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::freeNode (Key name, Key newDescesParentName)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("freeNode; name");
	if (nodes.count(newDescesParentName) == 0)    throw non_excistent_node("freeNode; newDescesParentName");


	auto node_ptr = nodes[name];
	moveAllDescesTo(node_ptr, newDescesParentName);

	if ( node_ptr->hasParent() )
	{
		auto parent_ptr = node_ptr->getParent();
		cutLinkBetween(parent_ptr, node_ptr);
		roots.insert(node_ptr);
	}
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::moveLeaf (Key name, Key newParentName)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("moveLeaf; name");
	if (nodes.count(newParentName) == 0)    throw non_excistent_node("moveLeaf; newParentName");


	auto leaf_ptr = nodes[name];

	if ( leaf_ptr->hasParent() )
	{
		auto parent_ptr = leaf_ptr->getParent();
		cutLinkBetween(parent_ptr, leaf_ptr);
	}
	else
		roots.erase(leaf_ptr);
		
	auto newParent_ptr = nodes[newParentName];
	connectNodes(newParent_ptr, leaf_ptr);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::freeLeaf (Key name)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("freeLeaf; name");


	auto leaf_ptr = nodes[name];

	if ( leaf_ptr->hasParent() )
	{
		auto parent_ptr = leaf_ptr->getParent();
		cutLinkBetween(parent_ptr, leaf_ptr);

		roots.insert(leaf_ptr);
	}
}


template<typename Key, typename Type>
inline void Forest<Key, Type>::renameNode (Key oldName, Key newName)
{
	if (nodes.count(oldName) == 0)    throw non_excistent_node("renameNode; oldName");
	
	if (newName == "")    throw empty_name("renameNode");
	if (nodes.count(newName) > 0)    throw busy_name("renameNode");


	auto node_ptr = nodes[oldName];
	
	node_ptr->rename(newName);

	nodes.erase(oldName);
	nodes[newName] = node_ptr;
}


template <typename Key, typename Type>
inline Type & Forest<Key, Type>::operator [] (Key name)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("operator []");

	auto & content = nodes[name] ->getToModify();
	return content;
}


template <typename Key, typename Type>
inline const Type & Forest<Key, Type>::at (Key name) const
{
	if (nodes.count(name) == 0)    throw non_excistent_node("at");

	AUTO_CONST_REF content = nodes.at(name) ->get();
	return content;
}


template <typename Key, typename Type>
unsigned Forest<Key, Type>::getNestingLevel (Key name) const
{
	unsigned result = nodes.at(name)->getNestingLevel();
	return result;
}


template <typename Key, typename Type>
const Type & Forest<Key, Type>::getParent (Key name) const
{
	if ( !(nodes.at(name) ->hasParent()) )    throw not_descendant("getParent");

	AUTO_CONST_PTR parent_ptr = (nodes.at(name)) ->getParent();
	AUTO_CONST_REF parentContent = parent_ptr->get();
	return parentContent;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::isExsist (Key name) const
{
	bool result = (nodes.count(name) == 1);
	return result;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::isRoot (Key name) const
{
	bool result = !(nodes.at(name) ->hasParent());
	return result;
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::isParent (Key name) const
{
	bool result = (nodes.at(name) ->hasDesces());
	return result;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::iterator Forest<Key, Type>::begin ()
{
	iterator it(roots.begin(), &roots);
	return it;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::iterator Forest<Key, Type>::end ()
{
	return iterator();
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::const_iterator Forest<Key, Type>::cbegin () const
{
	const_iterator it(roots.cbegin(), &roots);
	return it;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::const_iterator Forest<Key, Type>::cend () const
{
	return const_iterator();
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::desces_group_iterator Forest<Key, Type>::dgbegin (Key parentName)
{
	set<Node *> * desces_ptr;
	if (!parentName.empty())
		desces_ptr = nodes.at(parentName) ->getDescesSet();
	else
		desces_ptr = &roots;

	desces_group_iterator it(desces_ptr->begin());
	return it;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::desces_group_iterator Forest<Key, Type>::dgend (Key parentName)
{
	set<Node *> * desces_ptr;
	if (!parentName.empty())
		desces_ptr = nodes.at(parentName) ->getDescesSet();
	else
		desces_ptr = &roots;

	desces_group_iterator it(desces_ptr->end());
	return it;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::const_desces_group_iterator Forest<Key, Type>::cdgbegin (Key parentName) const
{
	const set<Node *> * desces_ptr;
	if (!parentName.empty())
		desces_ptr = nodes.at(parentName)->getDescesSet();
	else
		desces_ptr = &roots;

	const_desces_group_iterator it(desces_ptr->cbegin());
	return it;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::const_desces_group_iterator Forest<Key, Type>::cdgend (Key parentName) const
{
	const set<Node *> * desces_ptr;
	if (!parentName.empty())
		desces_ptr = nodes.at(parentName)->getDescesSet();
	else
		desces_ptr = &roots;

	const_desces_group_iterator it(desces_ptr->cend());
	return it;
}




template <typename Key, typename Type>
inline Forest<Key, Type>::~Forest<Key, Type> ()
{
	for (auto node_rec : nodes)
	{
		auto node = node_rec.second;
		delete node;
	}
}




template <typename Key, typename Type>
inline bool Forest<Key, Type>::isRoot (const Node * node_ptr)
{
	auto result = !(node_ptr->hasParent());
	return result;
}




template <typename Key, typename Type>
inline typename Forest<Key, Type>::Node * Forest<Key, Type>::createFreeNode (Key name, const Type & content)
{
	auto newNode_ptr = new Node(name, content);
	nodes[name] = newNode_ptr;

	return newNode_ptr;
}


template <typename Key, typename Type>
inline typename Forest<Key, Type>::Node * Forest<Key, Type>::createRoot (Key name, const Type & content)
{
	auto newRoot_ptr = new Node(name, content);
	nodes[name] = newRoot_ptr;
	roots.insert(newRoot_ptr);

	return newRoot_ptr;
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::deleteNode (Key name)
{
	auto node_ptr = nodes[name];
	nodes.erase(name);
	delete node_ptr;
}


template <typename Key, typename Type>
void Forest<Key, Type>::deleteLeaf (Key name)
{
	auto deletedLeaf_ptr = nodes[name];

	if ( deletedLeaf_ptr->hasParent() )
		deletedLeaf_ptr->getParent() ->disconnectDesc(deletedLeaf_ptr);
	else
		roots.erase(deletedLeaf_ptr);

	deleteNode(name);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::deleteAllDescesSubtrees (Node * parent_ptr)
{
	AUTO_CONST_REF desces = parent_ptr->getDesces();
	for (auto & desc : desces)
	{
		if ( desc->hasDesces() )
			deleteAllDescesSubtrees(desc);
		deleteNode( parent_ptr->getName() );
	}
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::convertDescesToRoots (Node * node_ptr)
{
	auto desces = node_ptr->getDescesSet();

	for (auto & desc : *desces)
	{
		desc->disconnectFromParent();
		roots.insert(desc);
	}
	desces->clear();
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::moveAllDescesTo (Node * node_ptr, Key newParentName)
{
	auto parent_ptr = nodes[newParentName];

	AUTO_CONST_REF desces = node_ptr->getDesces();
	for (auto desc_ptr : desces)
		connectNodes(parent_ptr, desc_ptr);
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::cutLinkBetween (Node * parent_ptr, Node * desc_ptr)
{
	parent_ptr->disconnectDesc(desc_ptr);
	desc_ptr->disconnectFromParent();
}


template <typename Key, typename Type>
inline void Forest<Key, Type>::connectNodes (Node * parent_ptr, Node * desc_ptr)
{
	parent_ptr->addDesc(desc_ptr);
	desc_ptr->setParent(parent_ptr);
}


template <typename Key, typename Type>
inline bool Forest<Key, Type>::isFirstInSubtreeOfSecond (Key first, Key second) const
{
	AUTO_CONST_REF desces = nodes.at(second) ->getDesces();
	for (auto & desc : desces)
	{
		if ( desc->hasDesces() )
			if ( isFirstInSubtreeOfSecond( first, desc->getName() ) )
				return true;

		if ( first == desc->getName() )
			return true;
	}

	return false;
}
