#pragma once

#include <libds/amt/abstract_memory_type.h>
#include <libds/amt/hierarchy.h>
#include <cmath>

namespace ds::amt {

	template<typename DataType, size_t K>
	class ImplicitHierarchy :
		virtual public KWayHierarchy<MemoryBlock<DataType>, K>,
		public ImplicitAMS<DataType>
	{
	public:
		ImplicitHierarchy();
		~ImplicitHierarchy() override;

		size_t level(const MemoryBlock<DataType>& node) const override;
		size_t level(size_t index) const;
		size_t degree(const MemoryBlock<DataType>& node) const override;
		size_t degree(size_t index) const;
		// Overload with one parameter hides overload with no parameter. We need to explicitly 'include' it.
		using Hierarchy<MemoryBlock<DataType>>::nodeCount;
		size_t nodeCount(const MemoryBlock<DataType>& node) const override;

		MemoryBlock<DataType>* accessRoot() const override;
		MemoryBlock<DataType>* accessParent(const MemoryBlock<DataType>& node) const override;
		MemoryBlock<DataType>* accessSon(const MemoryBlock<DataType>& node, size_t sonOrder) const override;
		MemoryBlock<DataType>* accessLastLeaf() const;

		MemoryBlock<DataType>& emplaceRoot() override; // throw(unavailable_function_call)
		void changeRoot(MemoryBlock<DataType>* newRoot) override; // throw(unavailable_function_call)

		MemoryBlock<DataType>& emplaceSon(MemoryBlock<DataType>& parent, size_t sonOrder) override; // throw(unavailable_function_call)
		void changeSon(MemoryBlock<DataType>& parent, size_t sonOrder, MemoryBlock<DataType>* newSon) override; // throw(unavailable_function_call)
		void removeSon(MemoryBlock<DataType>& parent, size_t sonOrder) override; // throw(unavailable_function_call)

		MemoryBlock<DataType>& insertLastLeaf();
		void removeLastLeaf();

		size_t indexOfParent(const MemoryBlock<DataType>& node) const;
		size_t indexOfParent(size_t index) const;
		size_t indexOfSon(const MemoryBlock<DataType>& node, size_t sonOrder) const;
		size_t indexOfSon(size_t indexOfParent, size_t sonOrder) const;
	};

	template<typename DataType, size_t K>
	using IH = ImplicitHierarchy<DataType, K>;

	//----------

	template<typename DataType>
	class BinaryImplicitHierarchy :
		public BinaryHierarchy<MemoryBlock<DataType>>,
		public ImplicitHierarchy<DataType, 2>
	{
	};

	template<typename DataType>
	using BinaryIH = BinaryImplicitHierarchy<DataType>;

	//----------

	template<typename DataType, size_t K>
    ImplicitHierarchy<DataType, K>::ImplicitHierarchy()
	{
	}

	template<typename DataType, size_t K>
    ImplicitHierarchy<DataType, K>::~ImplicitHierarchy()
	{
	}

	template<typename DataType, size_t K>
    size_t ImplicitHierarchy<DataType, K>::level(const MemoryBlock<DataType>& node) const
	{
		return this->level(this->getMemoryManager()->calculateIndex(node));
	}

	template<typename DataType, size_t K>
    size_t ImplicitHierarchy<DataType, K>::level(size_t index) const
	{
		return static_cast<size_t>(std::floor(std::log((K - 1) * (index + 1)) / std::log(K)));
	}

	template<typename DataType, size_t K>
    size_t ImplicitHierarchy<DataType, K>::degree(const MemoryBlock<DataType>& node) const
	{
		return this->degree(this->getMemoryManager()->calculateIndex(node));
	}

	template<typename DataType, size_t K>
    size_t ImplicitHierarchy<DataType, K>::degree(size_t index) const
	{
		size_t currentLevel = this->level(index);
		size_t indexOfLast = this->size() - 1;
		size_t depth = level(indexOfLast);

		if (currentLevel == depth)
		{
			return 0;
		}
		else 
		{
			if (currentLevel == depth - 1)
			{
				size_t indexOfParentOfLast = indexOfParent(indexOfLast);
				if (index < indexOfParentOfLast)
				{
					return K;
				}
				else
				{
					if (index > indexOfParentOfLast)
					{
						return 0;
					}
					else
					{
						size_t mod = (this->size() - 1) % K;
						if (mod == 0)
						{
							return K;
						}
						else
						{
							return mod;
					}
				}
			}
		}
			else
			{
				return K;
			}

		}
	}

	

	template<typename DataType, size_t K>
    size_t ImplicitHierarchy<DataType, K>::nodeCount(const MemoryBlock<DataType>& node) const
	{
		return this->getMemoryManager()->calculateIndex(node) == 0
                 ? this->size()
                 : Hierarchy<MemoryBlock<DataType>>::nodeCount(node);
	}

	template<typename DataType, size_t K>
    MemoryBlock<DataType>* ImplicitHierarchy<DataType, K>::accessRoot() const
	{
		if (this->size() > 0)
		{
			return &this->getMemoryManager()->getBlockAt(0);
		}
		else
		{
			return NULL;
		}
	}

	template<typename DataType, size_t K>
    MemoryBlock<DataType>* ImplicitHierarchy<DataType, K>::accessParent(const MemoryBlock<DataType>& node) const
	{
		size_t index = this->indexOfParent(node);
		if (INVALID_INDEX != index)
		{
			return &this->getMemoryManager()->getBlockAt(index);
		}
		else
		{
			return NULL;
		}
	}

	template<typename DataType, size_t K>
    MemoryBlock<DataType>* ImplicitHierarchy<DataType, K>::accessSon(const MemoryBlock<DataType>& node, size_t sonOrder) const
	{
		
		size_t index = this->indexOfSon(node, sonOrder);
		if (index < this->size()) {
			return &this->getMemoryManager()->getBlockAt(index);
		}
		else
		{
			return NULL;
		}
	}

	template<typename DataType, size_t K>
    MemoryBlock<DataType>* ImplicitHierarchy<DataType, K>::accessLastLeaf() const
	{
		if (this->size() > 0)
		{
			return &this->getMemoryManager()->getBlockAt(this->size() - 1);
		}
		else
		{
			return NULL;
		}
	}

	template<typename DataType, size_t K>
    MemoryBlock<DataType>& ImplicitHierarchy<DataType, K>::emplaceRoot()
	{
		throw unavailable_function_call("Method emplace_root() unavailable in implicit hierarchies!");
	}

	template<typename DataType, size_t K>
    void ImplicitHierarchy<DataType, K>::changeRoot(MemoryBlock<DataType>* newRoot)
	{
		throw unavailable_function_call("Method changeRoot() unavailable in implicit hierarchies!");
	}

	template<typename DataType, size_t K>
    MemoryBlock<DataType>& ImplicitHierarchy<DataType, K>::emplaceSon(MemoryBlock<DataType>& parent, size_t sonOrder)
	{
		throw unavailable_function_call("Method emplaceSon() unavailable in implicit hierarchies!");
	}

	template<typename DataType, size_t K>
    void ImplicitHierarchy<DataType, K>::changeSon(MemoryBlock<DataType>& parent, size_t sonOrder, MemoryBlock<DataType>* newSon)
	{
		throw unavailable_function_call("Method changeSon() unavailable in implicit hierarchies!");
	}

	template<typename DataType, size_t K>
    void ImplicitHierarchy<DataType, K>::removeSon(MemoryBlock<DataType>& parent, size_t sonOrder)
	{
		throw unavailable_function_call("Method removeSon() unavailable in implicit hierarchies!");
	}

	template<typename DataType, size_t K>
    MemoryBlock<DataType>& ImplicitHierarchy<DataType, K>::insertLastLeaf()
	{
		return *this->getMemoryManager()->allocateMemory();
	}

	template<typename DataType, size_t K>
    void ImplicitHierarchy<DataType, K>::removeLastLeaf()
	{
		this->getMemoryManager()->releaseMemory();
	}

	template<typename DataType, size_t K>
    size_t ImplicitHierarchy<DataType, K>::indexOfParent(const MemoryBlock<DataType>& node) const
	{
		return this->indexOfParent(this->getMemoryManager()->calculateIndex(node));
	}

	template<typename DataType, size_t K>
    size_t ImplicitHierarchy<DataType, K>::indexOfParent(size_t index) const
	{
		if (index == 0)
		{
			return INVALID_INDEX;
		}
		else
		{
			return (index - 1) / K;
		}
	}

	template<typename DataType, size_t K>
    size_t ImplicitHierarchy<DataType, K>::indexOfSon(const MemoryBlock<DataType>& node, size_t sonOrder) const
	{
		return this->indexOfSon(this->getMemoryManager()->calculateIndex(node), sonOrder);
	}

	template<typename DataType, size_t K>
    size_t ImplicitHierarchy<DataType, K>::indexOfSon(size_t indexOfParent, size_t sonOrder) const
	{
		return K * indexOfParent + sonOrder + 1;
	}

}