/* 
 * File:   WidthMinimizingTreeDecompositionAlgorithm.hpp
 * 
 * Author: ABSEHER Michael (abseher@dbai.tuwien.ac.at)
 * 
 * Copyright 2015-2016, Michael Abseher
 *    E-Mail: <abseher@dbai.tuwien.ac.at>
 * 
 * This file is part of htd.
 * 
 * htd is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation, either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * htd is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public 
 * License for more details.

 * You should have received a copy of the GNU General Public License
 * along with htd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HTD_HTD_WIDTHMINIMIZINGTREEDECOMPOSITIONALGORITHM_HPP
#define HTD_HTD_WIDTHMINIMIZINGTREEDECOMPOSITIONALGORITHM_HPP

#include <htd/Globals.hpp>
#include <htd/ITreeDecompositionAlgorithm.hpp>

#include <htd/ILabelingFunction.hpp>
#include <htd/IMutableTreeDecomposition.hpp>
#include <htd/ITreeDecompositionManipulationOperation.hpp>
#include <htd/ITreeDecompositionFitnessFunction.hpp>

#include <vector>
#include <functional>

namespace htd
{
    /**
     *  Implementation of the ITreeDecompositionAlgorithm interface based on repeated construction of a tree
     *  decomposition of the same input graph. After finishing the last iteration (or being interrupted by a
     *  call to htd::LibraryInstance::terminate()), the algorithm returns the decomposition with lowest
     *  width that was found so far. In case that this algorithm is interrupted before the first iteration
     *  is finished, the return value of computeDecomposition() will be nullptr.
     *
     *  Per default, the first decomposition is returned, i.e. only a single iteration is performed.
     *
     *  @note The algorithm always returns a valid tree decomposition according to the provided base
     *  decomposition algorithm and the given manipulations (or nullptr) also if it was interrupted
     *  by a call to htd::LibraryInstance::terminate().
     */
    class WidthMinimizingTreeDecompositionAlgorithm : public htd::ITreeDecompositionAlgorithm
    {
        public:
            /**
             *  Constructor for a new width-improving tree decomposition algorithm.
             *
             *  @param[in] manager  The management instance to which the new algorithm belongs.
             */
            HTD_API WidthMinimizingTreeDecompositionAlgorithm(const htd::LibraryInstance * const manager);

            /**
             *  Constructor for a new width-improving tree decomposition algorithm.
             *
             *  @note When calling this method the control over the memory regions of the manipulation operations is transferred to the
             *  decomposition algorithm. Deleting a manipulation operation provided to this method outside the decomposition algorithm
             *  or assigning the same manipulation operation multiple times will lead to undefined behavior.
             *
             *  @param[in] manager                  The management instance to which the new algorithm belongs.
             *  @param[in] manipulationOperations   The manipulation operations which shall be applied globally to each decomposition generated by the algorithm.
             */
            HTD_API WidthMinimizingTreeDecompositionAlgorithm(const htd::LibraryInstance * const manager, const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations);

            /**
             *  Copy constructor for a width-improving tree decomposition algorithm.
             *
             *  @param[in] original  The original width-improving tree decomposition algorithm.
             */
            HTD_API WidthMinimizingTreeDecompositionAlgorithm(const WidthMinimizingTreeDecompositionAlgorithm & original);

            HTD_API virtual ~WidthMinimizingTreeDecompositionAlgorithm();

            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph) const HTD_OVERRIDE;

            /**
             *  Compute a decomposition of the given graph.
             *
             *  @param[in] graph            The input graph to decompose.
             *  @param[in] progressCallback A callback function which is invoked in each iteration after the new decomposition was computed.
             *
             *  @return A new ITreeDecomposition object representing the decomposition of the given graph.
             */
            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                   const std::function<void(const htd::IMultiHypergraph &, const htd::ITreeDecomposition &, std::size_t)> & progressCallback) const;

            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                   const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations) const HTD_OVERRIDE;

            /**
             *  Compute a decomposition of the given graph and apply the given manipulation operations to it.
             *
             *  @param[in] graph                    The input graph to decompose.
             *  @param[in] manipulationOperations   The manipulation operations which shall be applied.
             *  @param[in] progressCallback         A callback function which is invoked in each iteration after the new decomposition was computed.
             *
             *  @note The manipulation operations provided to this function are applied right after the manipulation operations defined globally for the algorithm.
             *
             *  @note When calling this method the control over the memory regions of the manipulation operations is transferred to the
             *  decomposition algorithm. Deleting a manipulation operation provided to this method outside the decomposition algorithm
             *  or assigning the same manipulation operation multiple times will lead to undefined behavior.
             *
             *  @return A new ITreeDecomposition object representing the decomposition of the given graph.
             */
            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                   const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations,
                                                                   const std::function<void(const htd::IMultiHypergraph &, const htd::ITreeDecomposition &, std::size_t)> & progressCallback) const;

            /**
             *  Compute a decomposition of the given graph and apply the given manipulation operations to it. The manipulation operations are applied in the given order.
             *
             *  @param[in] graph                        The input graph to decompose.
             *  @param[in] manipulationOperationCount   The number of manipulation operations which are provided to this function.
             *
             *  @note The manipulation operations provided to this function are applied right after the manipulation operations defined globally for the algorithm.
             *
             *  @note When calling this method the control over the memory regions of the manipulation operations is transferred to the
             *  decomposition algorithm. Deleting a manipulation operation provided to this method outside the decomposition algorithm
             *  or assigning the same manipulation operation multiple times will lead to undefined behavior.
             *
             *  @return A new ITreeDecomposition object representing the decomposition of the given graph.
             */
            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph, int manipulationOperationCount, ...) const;

            HTD_API void setManipulationOperations(const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations) HTD_OVERRIDE;

            HTD_API void addManipulationOperation(htd::IDecompositionManipulationOperation * manipulationOperation) HTD_OVERRIDE;

            HTD_API void addManipulationOperations(const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations) HTD_OVERRIDE;

            HTD_API bool isSafelyInterruptible(void) const HTD_OVERRIDE;

            /**
             *  Getter for the number of iterations which shall be performed (0=infinite).
             *
             *  @return The number of iterations which shall be performed.
             */
            HTD_API std::size_t iterationCount(void) const;

            /**
             *  Set the number of iterations which shall be performed (0=infinite).
             *
             *  @param[in] iterationCount   The number of iterations which shall be performed.
             */
            HTD_API void setIterationCount(std::size_t iterationCount);

            /**
             *  Check whether the computed decompositions shall contain only subset-maximal bags.
             *
             *  @return A boolean flag indicating whether the computed decompositions shall contain only subset-maximal bags.
             */
            HTD_API bool isCompressionEnabled(void) const;

            /**
             *  Set whether the computed decompositions shall contain only subset-maximal bags.
             *
             *  @param[in] compressionEnabled   A boolean flag indicating whether the computed decompositions shall contain only subset-maximal bags.
             */
            HTD_API void setCompressionEnabled(bool compressionEnabled);

            /**
             *  Check whether the edges induced by the bag contents shall be computed.
             *
             *  @return A boolean flag indicating whether the edges induced by the bag contents shall be computed.
             */
            HTD_API bool isComputeInducedEdgesEnabled(void) const;

            /**
             *  Set whether the edges induced by the bag contents shall be computed.
             *
             *  @param[in] computeInducedEdgesEnabled   A boolean flag indicating whether the edges induced by the bag contents shall be computed.
             */
            HTD_API void setComputeInducedEdgesEnabled(bool computeInducedEdgesEnabled);

            HTD_API const htd::LibraryInstance * managementInstance(void) const HTD_NOEXCEPT HTD_OVERRIDE;

            HTD_API void setManagementInstance(const htd::LibraryInstance * const manager) HTD_OVERRIDE;

            HTD_API WidthMinimizingTreeDecompositionAlgorithm * clone(void) const HTD_OVERRIDE;

        protected:
            /**
             *  Copy assignment operator for a width-improving tree decomposition algorithm.
             *
             *  @note This operator is protected to prevent assignments to an already initialized algorithm.
             */
            WidthMinimizingTreeDecompositionAlgorithm & operator=(const WidthMinimizingTreeDecompositionAlgorithm &) { return *this; }

        private:
            struct Implementation;

            std::unique_ptr<Implementation> implementation_;
    };
}

#endif /* HTD_HTD_WIDTHMINIMIZINGTREEDECOMPOSITIONALGORITHM_HPP */
