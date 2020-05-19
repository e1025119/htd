
#ifndef HTD_HTD_PROVIDESTATICORDERINGALGORITHM_CPP
#define HTD_HTD_PROVIDESTATICORDERINGALGORITHM_CPP

#include <htd/Globals.hpp>
#include <htd/Helpers.hpp>
#include <htd/ProvideStaticOrderingAlgorithm.hpp>
#include <htd/GraphPreprocessorFactory.hpp>
#include <htd/IGraphPreprocessor.hpp>
#include <htd/VertexOrdering.hpp>

#include <algorithm>
#include <deque>
#include <unordered_set>

/**
 *  Private implementation details of class htd::ProvideStaticOrderingAlgorithm.
 */
struct htd::ProvideStaticOrderingAlgorithm::Implementation
{
    /**
     *  Constructor for the implementation details structure.
     *
     *  @param[in] manager   The management instance to which the current object instance belongs.
     *  @param[in] ordering   The static ordering parsed via commandline argument.
     */
    Implementation(const htd::LibraryInstance * const manager, htd::VertexOrdering * const ordering) : managementInstance_(manager), ordering_(ordering)
    {

    }

    virtual ~Implementation()
    {

    }

    /**
     *  The management instance to which the current object instance belongs.
     */
    const htd::LibraryInstance * managementInstance_;
	
	/**
	 * The static ordering parsed via commandline argument.
	 */
	htd::VertexOrdering * ordering_;
};


htd::ProvideStaticOrderingAlgorithm::ProvideStaticOrderingAlgorithm(const htd::LibraryInstance * const manager, htd::VertexOrdering * const ordering) 
: implementation_(new Implementation(manager, ordering))
{
	
}
            
htd::ProvideStaticOrderingAlgorithm::~ProvideStaticOrderingAlgorithm()
{
    
}

htd::IVertexOrdering * htd::ProvideStaticOrderingAlgorithm::computeOrdering(void) const HTD_NOEXCEPT
{
	return getOrdering();
}

htd::IVertexOrdering * htd::ProvideStaticOrderingAlgorithm::computeOrdering(const htd::IMultiHypergraph & graph) const HTD_NOEXCEPT
{
	return computeOrdering();
}

htd::IVertexOrdering * htd::ProvideStaticOrderingAlgorithm::computeOrdering(const htd::IMultiHypergraph & graph, const htd::IPreprocessedGraph & preprocessedGraph) const HTD_NOEXCEPT
{
	return computeOrdering();
}

const htd::LibraryInstance * htd::ProvideStaticOrderingAlgorithm::managementInstance(void) const HTD_NOEXCEPT
{
    return implementation_->managementInstance_;
}

void htd::ProvideStaticOrderingAlgorithm::setManagementInstance(const htd::LibraryInstance * const manager)
{
    HTD_ASSERT(manager != nullptr)

    implementation_->managementInstance_ = manager;
}

htd::VertexOrdering * htd::ProvideStaticOrderingAlgorithm::getOrdering(void) const HTD_NOEXCEPT
{
    return implementation_->ordering_;
}

void htd::ProvideStaticOrderingAlgorithm::setOrdering(htd::VertexOrdering * const ordering)
{
    HTD_ASSERT(ordering != nullptr)
	
	implementation_->ordering_ = ordering;
}

htd::ProvideStaticOrderingAlgorithm * htd::ProvideStaticOrderingAlgorithm::clone(void) const
{
    return new htd::ProvideStaticOrderingAlgorithm(managementInstance(), getOrdering());
}

#ifdef HTD_USE_VISUAL_STUDIO_COMPATIBILITY_MODE
htd::IOrderingAlgorithm * htd::ProvideStaticOrderingAlgorithm::cloneOrderingAlgorithm(void) const
{
    return new htd::ProvideStaticOrderingAlgorithm(implementation_->managementInstance_, getOrdering());
}
#endif

#endif /* HTD_HTD_PROVIDESTATICORDERINGALGORITHM_CPP */
