
#pragma once


#include <set>
#include <map>




using std::set;
using std::map;
using std::exception;





template <typename Type, typename key>
class Tree
{

	public:

		Tree () {};



		void addRoot(key name, const Type & content = 0);
		void pushFrontRoot(key name, key oldRootName, const Type & content = 0);
		void insertDesc(key name, key parentName, const Type & content = 0);
		void insertDesc(key name, key parentName, key descName, const Type & content = 0);
		void pushBackLeaf(key name, key parentName, const Type & content = 0);

		void popFrontRoot (key name);
		void eraseDesc (key name);
		void eraseDesc (key name, key newParentName);
		void popBackSubnet (key headerName);
		void popBackLeaf (key name);

		void moveSubnet(key headerName, key newParentName);
		void freeSubnet(key headerName);
		void moveNode(key name, key newParentName);
		void moveNode(key name, key newParentName, key newDescesParentName);
		void freeNode(key name);
		void freeNode(key name, key newDescesParentName);

		Type & operator [] (key name);
		const Type & at (key name) const;




	private:

		class Node
		{
			public:
				Node(Node * parent = nullptr);
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
				set<Node *> & getDesces () const;

				~Node ();


			private:
				Node (const Node & otherNode) = delete;
				Node operator = (const Node & otherNode) = delete;
				
				typename Type * content;

				Node * parent_ptr = nullptr;
				set<Node *> desces;
		};



		map<key, Node *> nodes;
		set<Node *> roots;



		Node * createFreeNode (const Type & content = 0);
		Node * createRoot (const Type & content = 0);
	
};





template<typename Type, typename key>
inline Tree<Type, key>::Node::Node (Node * prnt)
	: parent_ptr(prnt) 
{
	content = new Type();
}


template<typename Type, typename key>
inline Tree<Type, key>::Node::Node (typename const Type & cntnt, Node * prnt)
	: parent_ptr(prnt) 
{
	content = new Type();
	*content = cntnt;
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
	
	desces.insert(desc);
}


template<typename Type, typename key>
inline void Tree<Type, key>::Node::disconnectDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	if (desces.count(desc) == 0)    throw exception("There no such a descendant");

	desces.erase(desc);
}


template<typename Type, typename key>
inline void Tree<Type, key>::Node::disconnectAllDesces ()
{
	desces.clear();
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
	bool result = desces.empty();
	return result;
}


template<typename Type, typename key>
inline set<typename Tree<Type, key>::Node *> & Tree<Type, key>::Node::getDesces () const
{
	return desces;
}




template<typename Type, typename key>
inline Tree<Type, key>::Node::~Node ()
{
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
	parent_ptr->disconnectAllDesces();
	parent_ptr->addDesc(newNode_ptr);


}

template<typename Type, typename key>
inline void Tree<Type, key>::insertDesc (key name, key parentName, key descName, const Type& content)
{
}

template<typename Type, typename key>
inline void Tree<Type, key>::pushBackLeaf(key name, key parentName, const Type& content)
{
}


template<typename Type, typename key>
inline typename Tree<Type, key>::Node * Tree<Type, key>::createFreeNode (const Type & content)
{
	Node * newNode_ptr = new Node(content);
	nodes[key] = newNode_ptr;

	return newNode_ptr;
}


template<typename Type, typename key>
inline typename Tree<Type, key>::Node * Tree<Type, key>::createRoot (const Type & content)
{
	Node * newRoot_ptr = new Node(content);
	nodes[key] = newRoot_ptr;
	roots[key] = newRoot_ptr;

	return newRoot_ptr;
}
