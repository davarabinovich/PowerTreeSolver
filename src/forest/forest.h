
#pragma once


#include <set>
#include <map>
#include <utility>


#include "lib/ciflib.h"




using std::set;
using std::map;
using std::exception;
using std::swap;




template <typename key, typename Type>
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
		void eraseAllDesces (key parentName);

		void moveSubtree (key headerName, key newParentName);
		void freeSubtree (key headerName);
		void moveAllDesces (key parentName, key newParentName);
		void freeAllDesces (key parentName);
		void moveNode (key name, key newParentName);
		void moveNode (key name, key newParentName, key newDescesParentName);
		void freeNode (key name);
		void freeNode (key name, key newDescesParentName);

		void renameNode (key oldName, key newName);

		Type & operator [] (key name);
		const Type & at (key name) const;

		bool isExsist (key name) const;



		~Forest ();




	private:

		class Node
		{
#pragma todo think about deleting of getName and name
			public:
				Node (key name, Node * parent = nullptr);
				Node (key name, typename const Type & content, Node * parent = nullptr);
				Node (Node && otherNode);

				const Type & get () const;
				Type & getToModify ();
				key getName () const;
				bool hasParent () const;
				Node * getParent () const;
				bool hasDesces () const;
				const set<Node *> & getDesces () const;
				set<Node *> * getDescesSet () const;
				void rename (key newName);
				void record (const Type & origin);
				void setParent (Node * parent);
				void disconnectFromParent ();
				void addDesc (Node * desc);
				void disconnectDesc (Node * desc);
				void disconnectAllDesces ();

				~Node ();


			private:
				Node (const Node & otherNode) = delete;
				Node operator = (const Node & otherNode) = delete;
				
				key name;
				typename Type * content;

				Node * parent_ptr;
				set<Node *> * desces_ptr;
		};



		map<key, Node *> nodes;
		set<Node *> roots;

		

		Node * createFreeNode (key name, const Type & content = 0);
		Node * createRoot (key name, const Type & content = 0);

		void deleteNode (key name);
		void deleteAllDescesSubtrees (Node * parent_ptr);

		void convertDescesToRoots (Node * node_ptr);
		void moveAllDescesTo (Node * node_ptr, key newParentName);
		void cutLinkBetween (Node * parent_ptr, Node * desc_ptr);
		void connectNodes (Node * parent_ptr, Node * desc_ptr);

		bool isInSubtree (key name, key headerName) const;



		STRING_MESSAGE_EXCEPTION(empty_name);
		STRING_MESSAGE_EXCEPTION(busy_name);
		STRING_MESSAGE_EXCEPTION(non_excistent_node);
		STRING_MESSAGE_EXCEPTION(not_root);
		STRING_MESSAGE_EXCEPTION(not_descendant);
		STRING_MESSAGE_EXCEPTION(not_leaf);
		STRING_MESSAGE_EXCEPTION(closing_motion);

};




#include "forest/forest_defs.h"
