
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
class Forest
{

	public:

		Forest () {};


#pragma todo all chekcing
		void addRoot (key name, const Type & content = 0);
		void pushFrontRoot (key name, key oldRootName, const Type & content = 0);
		void insertDesc (key name, key parentName, const Type & content = 0);
		void insertDesc (key name, key parentName, key descName, const Type & content = 0);
		void pushBackLeaf (key name, key parentName, const Type & content = 0);

		void popFrontRoot (key name);
		void eraseDesc (key name);
		void eraseDesc (key name, key newDescesParentName);
		void popBackSubtree (key headerName);
		void popBackLeaf (key name);

		void moveSubtree (key headerName, key newParentName);
		void freeSubtree (key headerName);
		void moveNode (key name, key newParentName);
		void moveNode (key name, key newParentName, key newDescesParentName);
		void freeNode (key name);
		void freeNode (key name, key newDescesParentName);

		Type & operator [] (key name);
		const Type & at (key name) const;

		bool isExsist (key name) const;



		~Forest ();




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
				const set<Node *> & getDesces () const;

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
		void deleteAllDescesSubtrees (Node * parent_ptr);

		void convertDescesToRoots (Node * node_ptr);
		void moveAllDescesTo (Node * node_ptr, key newParentName);
		void cutLinkBetween (Node * parent_ptr, Node * desc_ptr);
		void connectNodes (Node * parent_ptr, Node * desc_ptr);

};










template<typename Type, typename key>
inline Forest<Type, key>::Node::Node (Node * prnt)
	: parent_ptr(prnt) 
{
	content = new Type();
	desces_ptr = new set<Node *>();
}


template<typename Type, typename key>
inline Forest<Type, key>::Node::Node (typename const Type & cntnt, Node * prnt)
	: parent_ptr(prnt) 
{
	content = new Type();
	*content = cntnt;

	desces_ptr = new set<Node*>();
}


template<typename Type, typename key>
inline Forest<Type, key>::Node::Node (Node && otherNode)
{
#pragma todo
}




template<typename Type, typename key>
inline const Type & Forest<Type, key>::Node::get () const
{
	return *content;
}


template<typename Type, typename key>
inline Type & Forest<Type, key>::Node::getToModify ()
{
	return *content;
}


template<typename Type, typename key>
inline void Forest<Type, key>::Node::record (const Type & origin)
{
	*content = origin;
}


template<typename Type, typename key>
inline void Forest<Type, key>::Node::setParent (Node * prnt)
{
	parent_ptr = prnt;
}


template<typename Type, typename key>
inline void Forest<Type, key>::Node::disconnectFromParent ()
{
	parent_ptr = nullptr;
}


template<typename Type, typename key>
inline void Forest<Type, key>::Node::addDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	
	desces_ptr->insert(desc);
}


template<typename Type, typename key>
inline void Forest<Type, key>::Node::disconnectDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	if (desces_ptr->count(desc) == 0)    throw exception("There no such a descendant");

	desces_ptr->erase(desc);
}


template<typename Type, typename key>
inline void Forest<Type, key>::Node::disconnectAllDesces ()
{
	desces_ptr->clear();
}


template<typename Type, typename key>
inline bool Forest<Type, key>::Node::hasParent () const
{
	if (parent_ptr == nullptr)
		return false;
	return true;
}


template<typename Type, typename key>
inline typename Forest<Type, key>::Node * Forest<Type, key>::Node::getParent () const
{
	return parent_ptr;
}


template<typename Type, typename key>
inline bool Forest<Type, key>::Node::hasDesces () const
{
	bool result = desces_ptr->empty();
	return result;
}

template<typename Type, typename key>
inline const set<typename Forest<Type, key>::Node *> & Forest<Type, key>::Node::getDesces () const
{
	return *desces_ptr;
}







template<typename Type, typename key>
inline Forest<Type, key>::Node::~Node ()
{
	delete desces_ptr;
	delete content;
}










template<typename Type, typename key>
inline void Forest<Type, key>::addRoot (key name, const Type & content)
{
	createRoot(content);
}


template<typename Type, typename key>
inline void Forest<Type, key>::pushFrontRoot (key name, key oldRootName, const Type & content)
{
	Node * newRoot_ptr = createRoot(content);

	roots.erase(oldRootName);
	newRoot_ptr->addDesc(oldRootName);
	nodes[oldRootName] ->setParent(name);
}


template<typename Type, typename key>
inline void Forest<Type, key>::insertDesc (key name, key parentName, const Type & content)
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
inline void Forest<Type, key>::insertDesc (key name, key parentName, key descName, const Type & content)
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
inline void Forest<Type, key>::pushBackLeaf (key name, key parentName, const Type & content)
{
	Node * newLeaf_ptr = createFreeNode(content);

	Node * parent_ptr = nodes[parentName];
	parent_ptr->addDecs(newLeaf_ptr);
	newLeaf_ptr->setParent(parent_ptr);
}


template<typename Type, typename key>
inline void Forest<Type, key>::popFrontRoot (key name)
{
#pragma todo validate execution deleting function for limit cases (e. g. roots)
	Node * deletedRoot_ptr = nodes[name];
	convertDescesToRoots();

	roots.erase(deletedRoot_ptr);
	deleteNode(deletedRoot_ptr);
}


template<typename Type, typename key>
inline void Forest<Type, key>::eraseDesc (key name)
{
	Node * deletedNode_ptr = nodes[name];
	convertDescesToRoots(deletedNode_ptr);

	popBackLeaf(name);
}


template<typename Type, typename key>
inline void Forest<Type, key>::eraseDesc (key name, key newDescesParentName)
{
	Node * deletedNode_ptr = nodes[name];
	moveAllDescesTo(deletedNode_ptr, newDescesParentName);

	popBackLeaf(name);
}


template<typename Type, typename key>
inline void Forest<Type, key>::popBackSubtree (key headerName)
{
	Node * header_ptr = nodes[headerName];
	header_ptr->getParent() ->disconnectDesc(header_ptr);

	deleteAllDescesSubtrees(header_ptr);
	deleteNode(header_ptr);
}


template<typename Type, typename key>
inline void Forest<Type, key>::popBackLeaf (key name)
{
	Node * deletedLeaf_ptr = nodes[name];
	deletedLeaf_ptr->getParent() ->disconnectDesc(deletedLeaf_ptr);

	deleteNode(deletedLeaf_ptr);
}


template<typename Type, typename key>
inline void Forest<Type, key>::moveSubtree (key headerName, key newParentName)
{
	Node * header_ptr = nodes[headerName];
	Node * parent_ptr = header_ptr->getParent();
	parent_ptr->disconnectDesc(header_ptr);
	header_ptr->disconnectFromParent();

	Node * newParent_ptr = nodes[newParentName];
	newParentName->addDesc(header_ptr);
	header_ptr->setParent(newParent_ptr);
}


template<typename Type, typename key>
inline void Forest<Type, key>::freeSubtree (key headerName)
{
	Node * header_ptr = nodes[headerName];
	Node * parent_ptr = header_ptr->getParent();
	parent_ptr->disconnectDesc(header_ptr);
	header_ptr->disconnectFromParent();

	roots.insert(header_ptr);
}


template<typename Type, typename key>
inline void Forest<Type, key>::moveNode (key name, key newParentName)
{
	Node * node_ptr = nodes[name];
	convertDescesToRoots(node_ptr);

	popBackLeaf(name);
}


template<typename Type, typename key>
inline void Forest<Type, key>::moveNode (key name, key newParentName, key newDescesParentName)
{

}


template<typename Type, typename key>
inline void Forest<Type, key>::freeNode (key name)
{
}


template<typename Type, typename key>
inline void Forest<Type, key>::freeNode (key name, key newDescesParentName)
{
	Node * node_ptr = nodes[name];
	moveAllDescesTo(node_ptr, newDescesParentName);

	roots.insert(node_ptr);
}


template<typename Type, typename key>
inline typename Forest<Type, key>::Node * Forest<Type, key>::createFreeNode (key name, const Type & content)
{
	Node * newNode_ptr = new Node(content);
	nodes[name] = newNode_ptr;

	return newNode_ptr;
}


template<typename Type, typename key>
inline typename Forest<Type, key>::Node * Forest<Type, key>::createRoot (key name, const Type & content)
{
	Node * newRoot_ptr = new Node(content);
	nodes[name] = newRoot_ptr;
	roots[name] = newRoot_ptr;

	return newRoot_ptr;
}


template<typename Type, typename key>
inline void Forest<Type, key>::deleteNode (Node * node_ptr)
{
	nodes.erase(node_ptr);
	delete node_ptr;
}


template<typename Type, typename key>
inline void Forest<Type, key>::deleteAllDescesSubtrees (Node * parent_ptr)
{
	AUTO_CONST_REF desces = parent_ptr->getDesces();
	for (auto & desc : desces)
		if ( desc->hasDesces() )
			deleteAllDescesSubtrees(desc);

	deleteNode(parent_ptr);
}


template<typename Type, typename key>
inline void Forest<Type, key>::convertDescesToRoots (Node * node_ptr)
{
	AUTO_CONST_REF desces = node_ptr->getDesces();
	for (auto & desc : desces)
	{
		desc->disconnectFromParent();
		roots.insert();
	}
}


template<typename Type, typename key>
inline void Forest<Type, key>::moveAllDescesTo (Node * node_ptr, key newParentName)
{
	AUTO_CONST_REF desces = node_ptr->getDesces();
	for (auto & desc : desces)
		nodes[newParentName] ->addDesc(desc);
}


template<typename Type, typename key>
inline void Forest<Type, key>::cutLinkBetween (Node * parent_ptr, Node * desc_ptr)
{
	parent_ptr->disconnectDesc(desc_ptr);
	desc_ptr->discoonectFromParetn();
}

template<typename Type, typename key>
inline void Forest<Type, key>::connectNodes(Node* parent_ptr, Node* desc_ptr)
{
}



