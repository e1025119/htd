
#ifndef HTD_HTD_PROVIDESTATICORDERINGALGORITHM_HPP
#define HTD_HTD_PROVIDESTATICORDERINGALGORITHM_HPP

#include <htd/Globals.hpp>
#include <htd/IOrderingAlgorithm.hpp>
#include <htd/VertexOrdering.hpp>

namespace htd
{
    /**
     *  Implementation of the IOrderingAlgorithm interface, bypassing the ordering algorithms via a static CLI argument ordering.
     */
    class ProvideStaticOrderingAlgorithm : public virtual htd::IOrderingAlgorithm
    {
        public:
            /**
             *  Constructor for a new ordering algorithm of type ProvideStaticOrderingAlgorithm.
             *
             *  @param[in] manager   			The management instance to which the new algorithm belongs.
             *  @param[in] orderingArgument   	Parsed from the string argument from --provideOrdering/-p, contains the static ordering.
             */
            
			HTD_API ProvideStaticOrderingAlgorithm(const htd::LibraryInstance * const manager, htd::VertexOrdering * ordering);
            
            HTD_API virtual ~ProvideStaticOrderingAlgorithm();
            
            HTD_API htd::IVertexOrdering * computeOrdering(void) const HTD_NOEXCEPT;
            
			HTD_API htd::IVertexOrdering * computeOrdering(const htd::IMultiHypergraph & graph) const HTD_NOEXCEPT HTD_OVERRIDE;

            HTD_API htd::IVertexOrdering * computeOrdering(const htd::IMultiHypergraph & graph, const htd::IPreprocessedGraph & preprocessedGraph) const HTD_NOEXCEPT HTD_OVERRIDE;

            HTD_API const htd::LibraryInstance * managementInstance(void) const HTD_NOEXCEPT HTD_OVERRIDE;

            HTD_API void setManagementInstance(const htd::LibraryInstance * const manager) HTD_OVERRIDE;

			HTD_API htd::VertexOrdering * getOrdering(void) const HTD_NOEXCEPT;

			HTD_API void setOrdering(htd::VertexOrdering * const ordering);

#ifndef HTD_USE_VISUAL_STUDIO_COMPATIBILITY_MODE
            HTD_API ProvideStaticOrderingAlgorithm * clone(void) const HTD_OVERRIDE;
#else
            HTD_API ProvideStaticOrderingAlgorithm * clone(void) const;

            HTD_API htd::IOrderingAlgorithm * cloneOrderingAlgorithm(void) const HTD_OVERRIDE;
#endif

        protected:
            /**
             *  Copy assignment operator for an ordering algorithm.
             *
             *  @note This operator is protected to prevent assignments to an already initialized algorithm.
             */
            ProvideStaticOrderingAlgorithm & operator=(const ProvideStaticOrderingAlgorithm &) { return *this; }

        private:
            struct Implementation;

            std::unique_ptr<Implementation> implementation_;
	};
}

#endif /* HTD_HTD_PROVIDESTATICORDERINGALGORITHM_HPP */
