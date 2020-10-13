
#pragma once


#include <set>
#include <map>
#include <utility>


#include "lib/ciflib.h"




using std::set;
using std::map;
using std::exception;
using std::swap;





template <typename Type, typename key>
class Tree
{

	public:

		Tree () {};



		void addRoot (key name, const Type & content = 0);
		void pushFrontRoot (key name, key oldRootName, const Type & content = 0);
		void insertDesc (key name, key parentName, const Type & content = 0);
		void insertDesc (key name, key parentName, key descName, const Type & content = 0);
		void pushBackLeaf (key name, key parentName, const Type & content = 0);

		void popFrontRoot (key name);
		void eraseDesc (key name);
		void eraseDesc (key name, key newParentName);
		void popBackSubnet (key headerName);
		void popBackLeaf (key name);

		void moveSubnet (key headerName, key newParentName);
		void freeSubnet (key headerName);
		void moveNode (key name, key newParentName);
		void moveNode (key name, key newParentName, key newDescesParentName);
		void freeNode (key name);
		void freeNode (key name, key newDescesParentName);

		Type & operator [] (key name);
		const Type & at (key name) const;

		bool isExsist (key name) const;



		~Tree ();




	private:

		class Node
		{
			public:
				Node (Node * parent = nullptr);
				Node (typename const Type & content, Node * parent = nullptr);
				Node (Node && otherNode);

				const Type & get () const;
				Type & getToModify ();
				void record (const Type & origin);
				void setParent (Node * parent);
				void disconnectFromParent ();
				void addDesc (Node * desc);
				void disconnectDesc (Node * desc);
				void disconnectAllDesces ();
				bool hasParent () const;
				Node * getParent () const;
				bool hasDesces () const;
				set<Node *> * getDesces () const;

				~Node ();


			private:
				Node (const Node & otherNode) = delete;
				Node operator = (const Node & otherNode) = delete;
				
				typename Type * content;

				Node * parent_ptr;
				set<Node *> * desces_ptr;
		};



		map<key, Node *> nodes;
		set<Node *> roots;

		

		Node * createFreeNode (key name, const Type & content = 0);
		Node * createRoot (key name, const Type & content = 0);

		void deleteNode (Node * node_ptr);
		void convertDescesToRoots (Node * node_ptr);
	
};










template<typename Type, typename key>
inline Tree<Type, key>::Node::Node (Node * prnt)
	: parent_ptr(prnt) 
{
	content = new Type();
	desces_ptr = new set<Node *>();
}


template<typename Type, typename key>
inline Tree<Type, key>::Node::Node (typename const Type & cntnt, Node * prnt)
	: parent_ptr(prnt) 
{
	content = new Type();
	*content = cntnt;

	desces_ptr = new set<Node*>();
}


template<typename Type, typename key>
inline Tree<Type, key>::Node::Node (Node && otherNode)
{
#pragma todo
}




template<typename Type, typename key>
inline const Type & Tree<Type, key>::Node::get () const
{
	return *content;
}


template<typename Type, typename key>
inline Type & Tree<Type, key>::Node::getToModify ()
{
	return *content;
}


template<typename Type, typename key>
inline void Tree<Type, key>::Node::record (const Type & origin)
{
	*content = origin;
}


template<typename Type, typename key>
inline void Tree<Type, key>::Node::setParent (Node * prnt)
{
	parent_ptr = prnt;
}


template<typename Type, typename key>
inline void Tree<Type, key>::Node::disconnectFromParent ()
{
	parent_ptr = nullptr;
}


template<typename Type, typename key>
inline void Tree<Type, key>::Node::addDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	
	desces_ptr->insert(desc);
}


template<typename Type, typename key>
inline void Tree<Type, key>::Node::disconnectDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	if (desces_ptr->count(desc) == 0)    throw exception("There no such a descendant");

	desces_ptr->erase(desc);
}


template<typename Type, typename key>
inline void Tree<Type, key>::Node::disconnectAllDesces ()
{
	desces_ptr->clear();
}


template<typename Type, typename key>
inline bool Tree<Type, key>::Node::hasParent () const
{
	if (parent_ptr == nullptr)
		return false;
	return true;
}


template<typename Type, typename key>
inline typename Tree<Type, key>::Node * Tree<Type, key>::Node::getParent () const
{
	return parent_ptr;
}


template<typename Type, typename key>
inline bool Tree<Type, key>::Node::hasDesces () const
{
	bool result = desces_ptr->empty();
	return result;
}

template<typename Type, typename key>
inline set<typename Tree<Type, key>::Node *> * Tree<Type, key>::Node::getDesces () const
{
	return desces_ptr;
}







template<typename Type, typename key>
inline Tree<Type, key>::Node::~Node ()
{
	delete desces_ptr;
	delete content;
}










template<typename Type, typename key>
inline void Tree<Type, key>::addRoot (key name, const Type & content)
{
	createRoot(content);
}


template<typename Type, typename key>
inline void Tree<Type, key>::pushFrontRoot (key name, key oldRootName, const Type & content)
{
	Node * newRoot_ptr = createRoot(content);

	roots.erase(oldRootName);
	newRoot_ptr->addDesc(oldRootName);
	nodes[oldRootName] ->setParent(name);
}


template<typename Type, typename key>
inline void Tree<Type, key>::insertDesc (key name, key parentName, const Type & content)
{
	Node * newNode_ptr = createFreeNode(content);

	Node * parent_ptr = nodes[parentName];
	swap( newNode_ptr->getDesces(), parent_ptr->getDesces() );
	parent_ptr->addDesc(newNode_ptr);

	if (!parent_ptr->hasParent())
	{
		roots.erase(parent_ptr);
		roots.insert(newNode_ptr);
	}
}

template<typename Type, typename key>
inline void Tree<Type, key>::insertDesc (key name, key parentName, key descName, const Type & content)
{
	Node * newNode_ptr = createFreeNode(content);

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

template<typename Type, typename key>
inline void Tree<Type, key>::pushBackLeaf (key name, key parentName, const Type & content)
{
	Node * newLeaf_ptr = createFreeNode(content);

	Node * parent_ptr = nodes[parentName];
	parent_ptr->addDecs(newLeaf_ptr);
	newLeaf_ptr->setParent(parent_ptr);
}


template<typename Type, typename key>
inline void Tree<Type, key>::popFrontRoot (key name)
{
#pragma todo validate execution deleting function for limit cases (e. g. roots)
	Node * deletedRoot_ptr = nodes[name];
	convertDescesToRoots();

	roots.erase(deletedRoot_ptr);
	deleteNode(deletedRoot_ptr);
}


template<typename Type, typename key>
inline void Tree<Type, key>::eraseDesc (key name)
{
	Node * deletedNode_ptr = nodes[name];
	convertDescesToRoots();

	popBackLeaf(name);
}


template<typename Type, typename key>
inline void Tree<Type, key>::eraseDesc (key name, key newParentName)
{
	Node * deletedNode_ptr = nodes[name];
	
	AUTO_CONST_REF desces = *( deletedNode_ptr->getDesces() );
	for (auto & desc : desces)
	{
		nodes[newParentName] ->addDesc(desc);
	}

	popBackLeaf(name);
}


template<typename Type, typename key>
inline void Tree<Type, key>::popBackSubnet (key headerName)
{
	Node * header_ptr = nodes[headerName];
	header_ptr->getParent() ->disconnectDesc(header_ptr);

	
}


template<typename Type, typename key>
inline void Tree<Type, key>::popBackLeaf (key name)
{
	Node * deletedLeaf_ptr = nodes[name];
	deletedLeaf_ptr->getParent() ->disconnectDesc(deletedLeaf_ptr);

	deleteNode(deletedLeaf_ptr);
}


template<typename Type, typename key>
inline typename Tree<Type, key>::Node * Tree<Type, key>::createFreeNode (key name, const Type & content)
{
	Node * newNode_ptr = new Node(content);
	nodes[name] = newNode_ptr;

	return newNode_ptr;
}


template<typename Type, typename key>
inline typename Tree<Type, key>::Node * Tree<Type, key>::createRoot (key name, const Type & content)
{
	Node * newRoot_ptr = new Node(content);
	nodes[name] = newRoot_ptr;
	roots[name] = newRoot_ptr;

	return newRoot_ptr;
}


template<typename Type, typename key>
inline void Tree<Type, key>::deleteNode (Node * node_ptr)
{
	nodes.erase(node_ptr);
	delete node_ptr;
}


template<typename Type, typename key>
inline void Tree<Type, key>::convertDescesToRoots (Node * node_ptr)
{
	AUTO_CONST_REF desces = *( node_ptr->getDesces() );
	for (auto & desc : desces)
	{
		desc->disconnectFromParent();
		roots.insert();
	}
}