
#pragma once


#include <memory>
#include <set>
#include <map>




using std::shared_ptr;
using std::set;
using std::map;
using std::exception;





template <typename Type, typename key>
class Tree
{

	public:

		Tree () {};



		void addRoot (key name);
		void pushFrontRoot (key name, key oldRootName);
		void insertDesc (key name, key parentName);
		void pushBackLeaf (key name, key parentName);

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

		shared_ptr<Type> get (key name);
		const shared_ptr<Type> at (key name) const;




	private:

		class Node
		{
			public:
				Node(Node * parent = nullptr);
				Node (typename const Type & content, Node * parent = nullptr);
				Node (Node && otherNode);

				const Type & get () const;
				Type & getToModify ();
				void record (const Type& origin);
				void setParent (Node * parent);
				void removeParent ();
				void addDesc (Node * desc);
				void removeDesc (Node * desc);
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



		map<key, Node *> roots;
	
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
inline void Tree<Type, key>::Node::removeParent ()
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
inline void Tree<Type, key>::Node::removeDesc (Node * desc)
{
	if (desc == nullptr)    throw exception("Null pointer as descendant is not allowed");
	if (desces.count(desc) == 0)    throw exception("There no such a descendant");

	desces.erase(desc);
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
