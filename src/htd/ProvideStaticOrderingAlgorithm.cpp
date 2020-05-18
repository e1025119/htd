
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
     */
    Implementation(const htd::LibraryInstance * const manager) : managementInstance_(manager)
    {

    }

    virtual ~Implementation()
    {

    }

    /**
     *  The management instance to which the current object instance belongs.
     */
    const htd::LibraryInstance * managementInstance_;
};

htd::ProvideStaticOrderingAlgorithm::ProvideStaticOrderingAlgorithm(const htd::LibraryInstance * const manager, std::string orderingArgument) : implementation_(new Implementation(manager))
{
	char *token = strtok(orderingArgument.substr(1,orderingArgument.length()-2), ",");
	while ( token != null)
	{
		std::cout << token << str::endl;
		token = strtok(NULL, ",");
		ordering = NULL; //TODO parse argument content into ordering
	}

}
            
htd::ProvideStaticOrderingAlgorithm::~ProvideStaticOrderingAlgorithm()
{
    
}

htd::IVertexOrdering * htd::ProvideStaticOrderingAlgorithm::computeOrdering(const htd::IMultiHypergraph & graph) const HTD_NOEXCEPT
{
	return ordering;
}

htd::IVertexOrdering * htd::ProvideStaticOrderingAlgorithm::computeOrdering(const htd::IMultiHypergraph & graph, const htd::IPreprocessedGraph & preprocessedGraph) const HTD_NOEXCEPT
{
 	return ordering;
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

htd::ProvideStaticOrderingAlgorithm * htd::ProvideStaticOrderingAlgorithm::clone(void) const
{
    return new htd::ProvideStaticOrderingAlgorithm(managementInstance());
}

#ifdef HTD_USE_VISUAL_STUDIO_COMPATIBILITY_MODE
htd::IOrderingAlgorithm * htd::ProvideStaticOrderingAlgorithm::cloneOrderingAlgorithm(void) const
{
    return new htd::ProvideStaticOrderingAlgorithm(implementation_->managementInstance_);
}
#endif

#endif /* HTD_HTD_PROVIDESTATICORDERINGALGORITHM_CPP */
