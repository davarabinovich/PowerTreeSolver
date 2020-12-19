
#pragma once


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
	bool result = !( desces_ptr->empty() );
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


template<typename key, typename Type>
inline void Forest<key, Type>::Node::rename (key newName)
{
	name = newName;
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
	desces_ptr->insert(desc);
}


template <typename key, typename Type>
inline void Forest<key, Type>::Node::disconnectDesc (Node * desc)
{
	desces_ptr->erase(desc);
}


template <typename key, typename Type>
inline void Forest<key, Type>::Node::disconnectAllDesces ()
{
	desces_ptr->clear();
}




template <typename key, typename Type>
inline Forest<key, Type>::Node::~Node ()
{
	delete desces_ptr;
	delete content;
}









template <typename key, typename Type>
inline Forest<key, Type>::iterator::iterator ()
{;}


template <typename key, typename Type>
inline Forest<key, Type>::iterator::iterator (const iterator & it)
	: nodesStack(it.nodesStack), end_it(it.end_it) {;}


template <typename key, typename Type>
inline Forest<key, Type>::iterator::iterator (typename set<Node*>::iterator genBegin_it, typename set<Node*>::iterator genEnd_it)
	: end_it(genEnd_it)
{
	nodesStack.push_front(genBegin_it);
}




template <typename key, typename Type>
inline bool Forest<key, Type>::iterator::operator != (const iterator & other_it) const
{
	auto setIt = nodesStack.back();
	auto other_setIt = other_it.nodesStack.back();

	bool isEnd = (setIt == end_it);
	bool isOtherEnd = (other_setIt == other_it.end_it);

	if (isEnd != isOtherEnd)
		return true;
	if (isEnd)
		return false;

	bool result = (*setIt != *other_setIt);
	return result;
}


template <typename key, typename Type>
inline bool Forest<key, Type>::iterator::operator == (const iterator & other_it) const
{
	bool result = !(*this != other_it);
}


template <typename key, typename Type>
inline typename Forest<key, Type>::iterator & Forest<key, Type>::iterator::operator++ ()
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
			while (isLastDesc() && !isRoot(*it))
				nodesStack.pop_back();

			if (!isLastDesc())
			{
				nodesStack.back()++;
			}
			else
			{
				nodesStack.clear();
				nodesStack.push_front(end_it);
			}
		}

	}
	else
	{
		AUTO_CONST_REF descesSet = (*it)->getDesces();
		nodesStack.push_back(descesSet.begin());
	}

	return *this;
}


template <typename key, typename Type>
inline typename Forest<key, Type>::iterator & Forest<key, Type>::iterator::operator++ (int)
{
	auto temp_it = *this;
	(*this)++;
	return temp_it;
}


template <typename key, typename Type>
inline pair<key, Type> Forest<key, Type>::iterator::operator * () const
{
	auto node = *nodesStack.back();
	auto content = make_pair(node->getName(), node->getToModify());
	return content;
}


template <typename key, typename Type>
inline typename Forest<key, Type>::iterator Forest<key, Type>::iterator::operator = (const iterator & other_it)
{
	nodesStack = other_it.nodesStack;
	end_it = other_it.end_it;
	return *this;
}


template <typename key, typename Type>
inline bool Forest<key, Type>::iterator::isLastDesc () const
{
	auto temp_it = nodesStack.back();
	AUTO_CONST_REF parentsDescesSet = (*temp_it)->getParent()->getDesces();
	bool result = (next(temp_it) == parentsDescesSet.end());
	return result;
}










#pragma todo refactor auxiliary functions
template <typename key, typename Type>
inline void Forest<key, Type>::addRoot (key name, const Type & content)
{
	if (name == "")    throw empty_name("addRoot");
	if (nodes.count(name) == 1)    throw busy_name("addRoot");

	createRoot(name, content);
}


template <typename key, typename Type>
inline void Forest<key, Type>::pushFrontRoot (key name, key oldRootName, const Type & content)
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


template <typename key, typename Type>
inline void Forest<key, Type>::insertDesc (key name, key parentName, const Type & content)
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


template <typename key, typename Type>
inline void Forest<key, Type>::insertDesc (key name, key parentName, key descName, const Type & content)
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


template <typename key, typename Type>
inline void Forest<key, Type>::pushBackLeaf (key name, key parentName, const Type & content)
{
	if (name == "")    throw empty_name("insertDesc; name");
	if (nodes.count(name) == 1)    throw busy_name("insertDesc; name");

	if (nodes.count(parentName) == 0)    throw non_excistent_node("pushBackLeaf; parentName");


	auto newLeaf_ptr = createFreeNode(name, content);

	auto parent_ptr = nodes[parentName];
	parent_ptr->addDesc(newLeaf_ptr);
	newLeaf_ptr->setParent(parent_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::popFrontRoot (key name)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("popFrontRoot");
	if ( roots.count(nodes[name]) == 0 )    throw not_root("popFrontRoot");


	auto deletedRoot_ptr = nodes[name];
	convertDescesToRoots(deletedRoot_ptr);

	roots.erase(deletedRoot_ptr);
	deleteNode(name);
}


template <typename key, typename Type>
inline void Forest<key, Type>::eraseDesc (key name)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("eraseDesc");


	auto deletedNode_ptr = nodes[name];
	convertDescesToRoots(deletedNode_ptr);

	deleteLeaf(name);
}


template <typename key, typename Type>
inline void Forest<key, Type>::eraseDesc (key name, key newDescesParentName)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("eraseDesc; name");
	if (nodes.count(name) == 0)    throw non_excistent_node("eraseDesc; newDescesParentName");


	auto deletedNode_ptr = nodes[name];
	moveAllDescesTo(deletedNode_ptr, newDescesParentName);

	deleteLeaf(name);
}


template <typename key, typename Type>
inline void Forest<key, Type>::popBackSubtree (key headerName)
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


template <typename key, typename Type>
inline void Forest<key, Type>::popBackLeaf (key name)
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


template<typename key, typename Type>
inline void Forest<key, Type>::eraseAllDesces (key parentName)
{
	if (nodes.count(parentName) == 0)    throw non_excistent_node("eraseAllDesces");


	auto parent_ptr = nodes[parentName];
	deleteAllDescesSubtrees(parent_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::moveSubtree (key headerName, key newParentName)
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


template <typename key, typename Type>
inline void Forest<key, Type>::freeSubtree (key headerName)
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


template<typename key, typename Type>
inline void Forest<key, Type>::moveAllDesces (key parentName, key newParentName)
{
	if (nodes.count(parentName)    == 0)    throw non_excistent_node("moveAllDesces; parentName");
	if (nodes.count(newParentName) == 0)    throw non_excistent_node("moveAllDesces; newParentName");

	if ( isFirstInSubtreeOfSecond(newParentName, parentName) )    throw closing_motion("moveAllDesces");


	auto parent_ptr = nodes[parentName];
	moveAllDescesTo(parent_ptr, newParentName);
}


template<typename key, typename Type>
inline void Forest<key, Type>::freeAllDesces (key parentName)
{
	if (nodes.count(parentName) == 0)    throw non_excistent_node("freeAllDesces");

	auto parent_ptr = nodes[parentName];
	convertDescesToRoots(parent_ptr);
}


template <typename key, typename Type>
inline void Forest<key, Type>::moveNode (key name, key newParentName)
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


template <typename key, typename Type>
inline void Forest<key, Type>::moveNode (key name, key newParentName, key newDescesParentName)
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


template <typename key, typename Type>
inline void Forest<key, Type>::freeNode (key name)
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


template <typename key, typename Type>
inline void Forest<key, Type>::freeNode (key name, key newDescesParentName)
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


template <typename key, typename Type>
inline void Forest<key, Type>::moveLeaf (key name, key newParentName)
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


template <typename key, typename Type>
inline void Forest<key, Type>::freeLeaf (key name)
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


template<typename key, typename Type>
inline void Forest<key, Type>::renameNode (key oldName, key newName)
{
	if (nodes.count(oldName) == 0)    throw non_excistent_node("renameNode; oldName");
	
	if (newName == "")    throw empty_name("renameNode");
	if (nodes.count(newName) > 0)    throw busy_name("renameNode");


	auto node_ptr = nodes[oldName];
	
	node_ptr->rename(newName);

	nodes.erase(oldName);
	nodes[newName] = node_ptr;
}


template <typename key, typename Type>
inline Type & Forest<key, Type>::operator [] (key name)
{
	if (nodes.count(name) == 0)    throw non_excistent_node("operator []");

	auto & content = nodes[name] ->getToModify();
	return content;
}


template <typename key, typename Type>
inline const Type & Forest<key, Type>::at (key name) const
{
	if (nodes.count(name) == 0)    throw non_excistent_node("at");

	AUTO_CONST_REF content = nodes.at(name) ->get();
	return content;
}


template <typename key, typename Type>
inline bool Forest<key, Type>::isExsist (key name) const
{
	bool result = (nodes.count(name) == 1);
	return result;
}


template <typename key, typename Type>
inline bool Forest<key, Type>::isRoot (key name) const
{
	bool result = !( nodes.at(name) ->hasParent() );
	return result;
}


template <typename key, typename Type>
inline typename Forest<key, Type>::iterator Forest<key, Type>::begin ()
{
	iterator it(roots.begin(), roots.end());
	return it;
}


template <typename key, typename Type>
inline typename Forest<key, Type>::iterator Forest<key, Type>::end ()
{
	iterator it(roots.end(), roots.end());
	return it;
}




template <typename key, typename Type>
inline Forest<key, Type>::~Forest<key, Type> ()
{
	for (auto node_rec : nodes)
	{
		auto node = node_rec.second;
		delete node;
	}
}




template <typename key, typename Type>
inline bool Forest<key, Type>::isRoot (const Node * node_ptr)
{
	auto result = !(node_ptr->hasParent());
	return result;
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
void Forest<key, Type>::deleteLeaf (key name)
{
	auto deletedLeaf_ptr = nodes[name];

	if ( deletedLeaf_ptr->hasParent() )
		deletedLeaf_ptr->getParent() ->disconnectDesc(deletedLeaf_ptr);
	else
		roots.erase(deletedLeaf_ptr);

	deleteNode(name);
}


template <typename key, typename Type>
inline void Forest<key, Type>::deleteAllDescesSubtrees (Node * parent_ptr)
{
	AUTO_CONST_REF desces = parent_ptr->getDesces();
	for (auto & desc : desces)
	{
		if ( desc->hasDesces() )
			deleteAllDescesSubtrees(desc);
		deleteNode( parent_ptr->getName() );
	}
}


template <typename key, typename Type>
inline void Forest<key, Type>::convertDescesToRoots (Node * node_ptr)
{
	auto desces = node_ptr->getDescesSet();

	for (auto & desc : *desces)
	{
		desc->disconnectFromParent();
		roots.insert(desc);
	}
	desces->clear();
}


template <typename key, typename Type>
inline void Forest<key, Type>::moveAllDescesTo (Node * node_ptr, key newParentName)
{
	auto parent_ptr = nodes[newParentName];

	AUTO_CONST_REF desces = node_ptr->getDesces();
	for (auto desc_ptr : desces)
		connectNodes(parent_ptr, desc_ptr);
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


template <typename key, typename Type>
inline bool Forest<key, Type>::isFirstInSubtreeOfSecond (key first, key second) const
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
