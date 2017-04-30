/* 
 * File:   AdaptiveWidthMinimizingTreeDecompositionAlgorithm.hpp
 * 
 * Author: ABSEHER Michael (abseher@dbai.tuwien.ac.at)
 * 
 * Copyright 2015-2017, Michael Abseher
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

#ifndef HTD_HTD_ADAPTIVEWIDTHMINIMIZINGTREEDECOMPOSITIONALGORITHM_HPP
#define HTD_HTD_ADAPTIVEWIDTHMINIMIZINGTREEDECOMPOSITIONALGORITHM_HPP

#include <htd/Globals.hpp>
#include <htd/ICustomizedTreeDecompositionAlgorithm.hpp>

#include <htd/ILabelingFunction.hpp>
#include <htd/IMutableTreeDecomposition.hpp>
#include <htd/ITreeDecompositionManipulationOperation.hpp>
#include <htd/ITreeDecompositionFitnessFunction.hpp>

#include <vector>
#include <functional>

namespace htd
{
    /**
     *  Implementation of the ITreeDecompositionAlgorithm interface based on repeated construction of a tree decomposition
     *  of the same input graph with different base decomposition algorithms. After finishing the last iteration (or being
     *  interrupted by a call to htd::LibraryInstance::terminate()), the algorithm returns the decomposition with lowest
     *  width that was found so far. In case that this algorithm is interrupted before the first iteration is finished,
     *  the return value of computeDecomposition() will be nullptr.
     *
     *  In contrast to the class htd::CombinedWidthMinimizingTreeDecompositionAlgorithm which executes each of the provided
     *  algorithms only once, this class first performs a number of decision rounds to determine the most promising
     *  decomposition algorithm for the given instance. Then the algorithm performing best is called repeatedly in order to
     *  find decompositions of lower width.
     *
     *  @note The algorithm always returns a valid tree decomposition according to the provided base
     *  decomposition algorithms and the given manipulations (or nullptr) also if it was interrupted
     *  by a call to htd::LibraryInstance::terminate().
     *
     *  @note The base algorithms must be assigned before the optional manipulation operations!
     */
    class AdaptiveWidthMinimizingTreeDecompositionAlgorithm : public htd::ICustomizedTreeDecompositionAlgorithm
    {
        public:
            /**
             *  Constructor for a new width-improving tree decomposition algorithm.
             *
             *  @param[in] manager  The management instance to which the new algorithm belongs.
             */
            HTD_API AdaptiveWidthMinimizingTreeDecompositionAlgorithm(const htd::LibraryInstance * const manager);

            /**
             *  Copy constructor for a width-improving tree decomposition algorithm.
             *
             *  @param[in] original  The original width-improving tree decomposition algorithm.
             */
            HTD_API AdaptiveWidthMinimizingTreeDecompositionAlgorithm(const AdaptiveWidthMinimizingTreeDecompositionAlgorithm & original);

            HTD_API virtual ~AdaptiveWidthMinimizingTreeDecompositionAlgorithm();

            /**
             *  Add a new decomposition algorithm to the list of base algorithms which shall be used.
             *
             *  @param[in] algorithm    The new decomposition algorithm which will be added to the list of base algorithms.
             *
             *  @note When calling this method the control over the memory region of the provided algorithm is transferred to the
             *  decomposition algorithm. Deleting the algorithm provided to this method outside the decomposition algorithm or
             *  assigning the same base algorithm multiple times will lead to undefined behavior.
             */
            HTD_API void addDecompositionAlgorithm(htd::ITreeDecompositionAlgorithm * algorithm);

            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph) const HTD_OVERRIDE;

            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                   const std::function<void(const htd::IMultiHypergraph &, const htd::ITreeDecomposition &, const htd::FitnessEvaluation &)> & progressCallback) const HTD_OVERRIDE;

            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                   const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations) const HTD_OVERRIDE;

            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                   const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations,
                                                                   const std::function<void(const htd::IMultiHypergraph &, const htd::ITreeDecomposition &, const htd::FitnessEvaluation &)> & progressCallback) const HTD_OVERRIDE;

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

            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph, const htd::IPreprocessedGraph & preprocessedGraph) const HTD_OVERRIDE;

            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                   const htd::IPreprocessedGraph & preprocessedGraph,
                                                                   const std::function<void(const htd::IMultiHypergraph &, const htd::ITreeDecomposition &, const htd::FitnessEvaluation &)> & progressCallback) const HTD_OVERRIDE;

            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                   const htd::IPreprocessedGraph & preprocessedGraph,
                                                                   const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations) const HTD_OVERRIDE;

            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                   const htd::IPreprocessedGraph & preprocessedGraph,
                                                                   const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations,
                                                                   const std::function<void(const htd::IMultiHypergraph &, const htd::ITreeDecomposition &, const htd::FitnessEvaluation &)> & progressCallback) const HTD_OVERRIDE;

            /**
             *  Compute a decomposition of the given graph and apply the given manipulation operations to it. The manipulation operations are applied in the given order.
             *
             *  @param[in] graph                        The input graph to decompose.
             *  @param[in] preprocessedGraph            The input graph in preprocessed format.
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
            HTD_API htd::ITreeDecomposition * computeDecomposition(const htd::IMultiHypergraph & graph, const htd::IPreprocessedGraph & preprocessedGraph, int manipulationOperationCount, ...) const;

            HTD_API void setManipulationOperations(const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations) HTD_OVERRIDE;

            HTD_API void addManipulationOperation(htd::IDecompositionManipulationOperation * manipulationOperation) HTD_OVERRIDE;

            HTD_API void addManipulationOperations(const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations) HTD_OVERRIDE;

            HTD_API bool isSafelyInterruptible(void) const HTD_OVERRIDE;

            /**
             *  Check whether the edges induced by the bag contents shall be computed.
             *
             *  @return A boolean flag indicating whether the edges induced by the bag contents shall be computed.
             */
            HTD_API bool isComputeInducedEdgesEnabled(void) const;

            /**
             *  Set whether the edges induced by the bag contents shall be computed.
             *
             *  @param[in] computeInducedEdges  A boolean flag indicating whether the edges induced by the bag contents shall be computed.
             */
            HTD_API void setComputeInducedEdges(bool computeInducedEdges);

            /**
             *  Getter for the number of decision rounds which shall be performed (0=no decision round, random selection of algorithm).
             *
             *  @return The number of decision rounds which shall be performed.
             */
            HTD_API std::size_t decisionRounds(void) const;

            /**
             *  Set the number of decision rounds which shall be performed (0=no decision round, random selection of algorithm).
             *
             *  @param[in] rounds   The number of decision rounds which shall be performed.
             */
            HTD_API void setDecisionRounds(std::size_t rounds);

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
             *  Getter for the maximum number of iterations without improvement after which the most promising algorithm shall terminate (0=return after first non-improving decomposition was found).
             *
             *  @note The current non-improvement count is reset to 0 after each iteration resulting in an improved decomposition.
             *
             *  @return The maximum number of iterations without improvement after which the most promising algorithm shall terminate.
             */
            HTD_API std::size_t nonImprovementLimit(void) const;

            /**
             *  Set the maximum number of iterations without improvement after which the most promising algorithm shall terminate (0=return after first non-improving decomposition was found).
             *
             *  @note The current non-improvement count is reset to 0 after each iteration resulting in an improved decomposition.
             *
             *  @param[in] nonImprovementLimit  The maximum number of iterations without improvement after which the most promising algorithm shall terminate.
             */
            HTD_API void setNonImprovementLimit(std::size_t nonImprovementLimit);

            HTD_API const htd::LibraryInstance * managementInstance(void) const HTD_NOEXCEPT HTD_OVERRIDE;

            HTD_API void setManagementInstance(const htd::LibraryInstance * const manager) HTD_OVERRIDE;

            HTD_API AdaptiveWidthMinimizingTreeDecompositionAlgorithm * clone(void) const HTD_OVERRIDE;

        protected:
            /**
             *  Copy assignment operator for a width-improving tree decomposition algorithm.
             *
             *  @note This operator is protected to prevent assignments to an already initialized algorithm.
             */
            AdaptiveWidthMinimizingTreeDecompositionAlgorithm & operator=(const AdaptiveWidthMinimizingTreeDecompositionAlgorithm &) { return *this; }

        private:
            struct Implementation;

            std::unique_ptr<Implementation> implementation_;
    };
}

#endif /* HTD_HTD_ADAPTIVEWIDTHMINIMIZINGTREEDECOMPOSITIONALGORITHM_HPP */
