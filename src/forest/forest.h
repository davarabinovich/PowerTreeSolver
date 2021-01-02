
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





template <typename Key, typename Type>
class Forest
{

	public:

		Forest () {};



		void addRoot (Key name, const Type & content = 0);
		void pushFrontRoot (Key name, Key oldRootName, const Type & content = 0);
		void insertDesc (Key name, Key parentName, const Type & content = 0);
		void insertDesc (Key name, Key parentName, Key descName, const Type & content = 0);
		void pushBackLeaf (Key name, Key parentName, const Type & content = 0);

		void popFrontRoot (Key name);
		void eraseDesc (Key name);
		void eraseDesc (Key name, Key newDescesParentName);
		void popBackSubtree (Key headerName);
		void popBackLeaf (Key name);
		void eraseAllDesces (Key parentName);

		void moveSubtree (Key headerName, Key newParentName);
		void freeSubtree (Key headerName);
		void moveAllDesces (Key parentName, Key newParentName);
		void freeAllDesces (Key parentName);
		void moveNode (Key name, Key newParentName);
		void moveNode (Key name, Key newParentName, Key newDescesParentName);
		void freeNode (Key name);
		void freeNode (Key name, Key newDescesParentName);
		void moveLeaf (Key name, Key newParentName);
		void freeLeaf (Key name);

		void renameNode (Key oldName, Key newName);

		Type & operator [] (Key name);
		const Type & at (Key name) const;
		unsigned getNestingLevel (Key name) const;

		const Type & getParent (Key name) const;

		bool isExsist (Key name) const;
		bool isRoot (Key name) const;
		bool isParent (Key name) const;
		


		~Forest ();




	private:

		class Node
		{
			public:
				Node (Key genName, Node * genParent_ptr = nullptr);
				Node (Key genName, typename const Type & genContent, Node * genParent_ptr = nullptr);

				const Type & get () const;
				Type & getToModify ();
				Key getName () const;
				unsigned getNestingLevel () const;
				bool hasParent () const;
				Node * getParent () const;
				bool hasDesces () const;
				const set<Node *> & getDesces () const;
				set<Node *> * getDescesSet () const;
				void rename (Key newName);
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
				
				Key name;
				typename Type * content;
				unsigned nestingLevel = 1;

				Node * parent_ptr;
				set<Node *> * desces_ptr;
		};




	public:

		using nodes_set_it = typename set<Node *>::iterator;
		using const_nodes_set_it = typename set<Node *>::const_iterator;

		class iterator
		{
			public:
				using iterator_category = std::forward_iterator_tag;

				iterator ();
				iterator (const iterator & it);
				iterator (nodes_set_it it, set<Node *> * roots);

				bool operator != (const iterator & other_it) const;
                bool operator == (const iterator & other_it) const;
                iterator operator++ ();   
                iterator operator++ (int);
                pair<Key, Type &> operator * () const;  
				iterator operator = (const iterator & other_it);

			private:
				list< typename set<Node *>::iterator > nodesStack;
				set<Node *> * roots_ptr;

				typename nodes_set_it getParentsSetIt (typename set<Node *>::iterator it);
				bool isLastDesc () const;

				CUSTOM_EXCEPTION(forest_iterator_exception, exception);
				STRING_MESSAGE_EXCEPTION(end_iterator_dereferencing, forest_iterator_exception);
		};

		class const_iterator
		{
			public:
				using iterator_category = std::forward_iterator_tag;

				const_iterator ();
				const_iterator (const const_iterator & it);
				const_iterator (const_nodes_set_it it, const set<Node *> * roots);

				bool operator != (const const_iterator & other_it) const;
				bool operator == (const const_iterator & other_it) const;
				const_iterator operator++ ();
				const_iterator operator++ (int);
				pair<Key, Type> operator * () const;
				const_iterator operator = (const const_iterator & other_it);

			private:
				list< typename set<Node *>::const_iterator > nodesStack;
				const set<Node *> * roots_ptr;

				typename const_nodes_set_it getParentsSetIt (typename set<Node*>::const_iterator it);
				bool isLastDesc () const;

				CUSTOM_EXCEPTION(forest_iterator_exception, exception);
				STRING_MESSAGE_EXCEPTION(end_iterator_dereferencing, forest_iterator_exception);
		};



		class desces_group_iterator
		{
			public:
				using iterator_category = std::forward_iterator_tag;

				desces_group_iterator () = delete;
				desces_group_iterator (const desces_group_iterator & gen_it);
				desces_group_iterator (typename nodes_set_it gen_it);

				bool operator != (const desces_group_iterator & other_it) const;
                bool operator == (const desces_group_iterator & other_it) const;
                desces_group_iterator operator++ ();   
                desces_group_iterator operator++ (int);
                pair<Key, Type &> operator * () const;    
				desces_group_iterator operator = (const desces_group_iterator & other_it);

			private:
				nodes_set_it it;
		};

		class const_desces_group_iterator
		{
			public:
				using iterator_category = std::forward_iterator_tag;

				const_desces_group_iterator () = delete;
				const_desces_group_iterator (const const_desces_group_iterator & gen_it);
				const_desces_group_iterator (typename const_nodes_set_it gen_it);
	
				bool operator != (const const_desces_group_iterator & other_it) const;
				bool operator == (const const_desces_group_iterator & other_it) const;
				const_desces_group_iterator operator++ ();
				const_desces_group_iterator operator++ (int);
				pair<Key, Type> operator * () const;
				const_desces_group_iterator operator = (const const_desces_group_iterator & other_it);
	
			private:
				const_nodes_set_it it;
		};




		iterator begin ();
		iterator end ();

		const_iterator cbegin () const;
		const_iterator cend () const;

		desces_group_iterator dgbegin (Key parentName = "");
		desces_group_iterator dgend (Key parentName = "");

		const_desces_group_iterator cdgbegin (Key parentName = "") const;
		const_desces_group_iterator cdgend (Key parentName = "") const;




	private:

		map<Key, Node *> nodes;
		set<Node *> roots;



		static const size_t object_size = sizeof(Node *);

		

		static bool isRoot (const Node * node_ptr);



		Node * createFreeNode (Key name, const Type & content = 0);
		Node * createRoot (Key name, const Type & content = 0);

		void deleteNode (Key name);
		void deleteLeaf (Key name);
		void deleteAllDescesSubtrees (Node * parent_ptr);

		void convertDescesToRoots (Node * node_ptr);
		void moveAllDescesTo (Node * node_ptr, Key newParentName);
		void cutLinkBetween (Node * parent_ptr, Node * desc_ptr);
		void connectNodes (Node * parent_ptr, Node * desc_ptr);

		bool isFirstInSubtreeOfSecond (Key name, Key headerName) const;



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
