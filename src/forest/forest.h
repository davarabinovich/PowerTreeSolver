
#pragma once


#include <set>
#include <map>
#include <list>
#include <utility>
#include <iterator>


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
		void moveLeaf (key name, key newParentName);
		void freeLeaf (key name);

		void renameNode (key oldName, key newName);

		Type & operator [] (key name);
		const Type & at (key name) const;
		unsigned getNestingLevel (key name) const;

		const Type & getParent (key name) const;

		bool isExsist (key name) const;
		bool isRoot (key name) const;
		bool isParent (key name) const;
		


		~Forest ();




	private:

		class Node
		{
			public:
				Node (key name, Node * parent = nullptr);
				Node (key name, typename const Type & content, Node * parent = nullptr);
				Node (Node && otherNode);

				const Type & get () const;
				Type & getToModify ();
				key getName () const;
				unsigned getNestingLevel () const;
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

				void updateNestingLevel ();
				
				key name;
				typename Type * content;
				unsigned nestingLevel = 1;

				Node * parent_ptr;
				set<Node *> * desces_ptr;
		};




	public:

		using nodes_set_it = typename set<Node *>::iterator;

#pragma todo implement access for writing for this and the desces_group
		class iterator
		{
#pragma todo add type of iterator
			public:
				iterator ();
				iterator (const iterator & it);
				iterator (typename set<Node *>::iterator it, set<Node *> * roots);

				bool operator != (const iterator & other_it) const;
                bool operator == (const iterator & other_it) const;
                iterator operator++ ();   
                iterator operator++ (int);
                pair<key, Type> operator * () const;    
				iterator operator = (const iterator & other_it);

			private:
				list< typename set<Node *>::iterator > nodesStack;
				set<Node *> * roots_ptr;

				bool isLastDesc () const;

				CUSTOM_EXCEPTION(forest_iterator_exception, exception);
				STRING_MESSAGE_EXCEPTION(end_iterator_dereferencing, forest_iterator_exception);
		};

#pragma todo implement access for writing for this and the desces_group
		class const_iterator
		{
#pragma todo add type of iterator
		public:
			const_iterator ();
			const_iterator (const const_iterator & it);
			const_iterator (typename set<Node *>::const_iterator it, const set<Node *> * roots);

			bool operator != (const const_iterator & other_it) const;
			bool operator == (const const_iterator & other_it) const;
			const_iterator operator++ ();
			const_iterator operator++ (int);
			pair<key, Type> operator * () const;
			const_iterator operator = (const const_iterator & other_it);

		private:
			list< typename set<Node *>::const_iterator > nodesStack;
			const set<Node *> * roots_ptr;

			bool isLastDesc () const;

			CUSTOM_EXCEPTION(forest_iterator_exception, exception);
			STRING_MESSAGE_EXCEPTION(end_iterator_dereferencing, forest_iterator_exception);
		};



		class desces_group_iterator
		{
#pragma todo add type of iterator
			public:
				desces_group_iterator ();
				desces_group_iterator (const desces_group_iterator & gen_it);
				desces_group_iterator (typename nodes_set_it gen_it);

				bool operator != (const desces_group_iterator & other_it) const;
                bool operator == (const desces_group_iterator & other_it) const;
                desces_group_iterator operator++ ();   
                desces_group_iterator operator++ (int);
                pair<key, Type> operator * () const;    
				desces_group_iterator operator = (const desces_group_iterator & other_it);

			private:
				nodes_set_it it;
		};



		iterator begin ();
		iterator end ();

		const_iterator begin () const;
		const_iterator end () const;


		desces_group_iterator dgbegin (key parentName = "");
		desces_group_iterator dgend (key parentName = "");




	private:

		map<key, Node *> nodes;
		set<Node *> roots;



		static const size_t object_size = sizeof(Node *);

		

		static bool isRoot (const Node * node_ptr);



		Node * createFreeNode (key name, const Type & content = 0);
		Node * createRoot (key name, const Type & content = 0);

		void deleteNode (key name);
		void deleteLeaf (key name);
		void deleteAllDescesSubtrees (Node * parent_ptr);

		void convertDescesToRoots (Node * node_ptr);
		void moveAllDescesTo (Node * node_ptr, key newParentName);
		void cutLinkBetween (Node * parent_ptr, Node * desc_ptr);
		void connectNodes (Node * parent_ptr, Node * desc_ptr);

		bool isFirstInSubtreeOfSecond (key name, key headerName) const;



		CUSTOM_EXCEPTION(forest_exception, exception);

		STRING_MESSAGE_EXCEPTION(empty_name, forest_exception);
		STRING_MESSAGE_EXCEPTION(busy_name, forest_exception);
		STRING_MESSAGE_EXCEPTION(non_excistent_node, forest_exception);
		STRING_MESSAGE_EXCEPTION(not_root, forest_exception);
		STRING_MESSAGE_EXCEPTION(not_leaf, forest_exception);
		STRING_MESSAGE_EXCEPTION(not_descendant, forest_exception);
		STRING_MESSAGE_EXCEPTION(closing_motion, forest_exception);

};




#include "forest/forest_defs.h"
