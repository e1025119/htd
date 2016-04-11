/* 
 * File:   MinFillOrderingAlgorithm.cpp
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

#ifndef HTD_HTD_MINFILLORDERINGALGORITHM_CPP
#define	HTD_HTD_MINFILLORDERINGALGORITHM_CPP

#include <htd/Globals.hpp>
#include <htd/Helpers.hpp>
#include <htd/MinFillOrderingAlgorithm.hpp>
#include <htd/VectorAdapter.hpp>

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

htd::MinFillOrderingAlgorithm::MinFillOrderingAlgorithm(void)
{
    
}
            
htd::MinFillOrderingAlgorithm::~MinFillOrderingAlgorithm()
{
    
}

htd::ConstCollection<htd::vertex_t> htd::MinFillOrderingAlgorithm::computeOrdering(const htd::IMultiHypergraph & graph) const
{
    htd::VectorAdapter<htd::vertex_t> ret;

    writeOrderingTo(graph, ret.container());

    return htd::ConstCollection<htd::vertex_t>::getInstance(ret);
}

void htd::MinFillOrderingAlgorithm::writeOrderingTo(const htd::IMultiHypergraph & graph, std::vector<htd::vertex_t> & target) const
{
    std::size_t size = graph.vertexCount();

    std::size_t minFill = (std::size_t)-1;

    std::unordered_set<htd::vertex_t> pool(size);

    std::vector<htd::vertex_t> minDegreePool;

    std::unordered_set<htd::vertex_t> vertices(graph.vertices().begin(), graph.vertices().end());

    std::unordered_map<htd::vertex_t, htd::state_t> updateStatus(size);

    std::unordered_map<htd::vertex_t, std::size_t> requiredFillAmount(size);
    
    std::unordered_map<htd::vertex_t, htd::vertex_container> neighborhood(size);

    std::unordered_map<htd::vertex_t, std::vector<htd::vertex_t>> existingNeighbors(size);
    std::unordered_map<htd::vertex_t, std::vector<htd::vertex_t>> additionalNeighbors(size);
    std::unordered_map<htd::vertex_t, std::vector<htd::vertex_t>> unaffectedNeighbors(size);
    
    htd::vertex_container newNeighborhood;
    
    htd::vertex_container affectedVertices;
    affectedVertices.reserve(size);

    for (htd::vertex_t vertex : graph.vertices())
    {
        auto & currentNeighborhood = neighborhood[vertex];

        graph.copyNeighborsTo(currentNeighborhood, vertex);

        auto position = std::lower_bound(currentNeighborhood.begin(), currentNeighborhood.end(), vertex);
        
        if (position == currentNeighborhood.end() || *position != vertex)
        {
            currentNeighborhood.insert(position, vertex);
        }

        updateStatus[vertex] = htd::State::UNKNOWN;
    }

    for (htd::vertex_t vertex : vertices)
    {
        auto & currentNeighborhood = neighborhood.at(vertex);
        
        std::size_t tmp = ((currentNeighborhood.size() * (currentNeighborhood.size() - 1)) / 2) - computeEdgeCount(neighborhood, currentNeighborhood);
        
        if (tmp <= minFill)
        {
            if (tmp < minFill)
            {
                minFill = tmp;

                pool.clear();
            }
            
            pool.insert(vertex);
        }
        
        requiredFillAmount[vertex] = tmp;

        DEBUGGING_CODE_LEVEL2(
        std::cout << "Vertex " << vertex << ":" << std::endl;
        htd::print(currentNeighborhood, std::cout, false);
        std::cout << std::endl;
        std::size_t neighborhoodSize = currentNeighborhood.size();
        std::cout << "   INITIAL EDGE COUNT " << vertex << ": " << computeEdgeCount(neighborhood, neighborhood[vertex]) << std::endl;
        std::cout << "   MAXIMUM EDGE COUNT " << vertex << ": " << (neighborhoodSize * (neighborhoodSize - 1)) / 2 << std::endl;
        std::cout << "   INITIAL FILL VALUE " << vertex << ": " << requiredFillAmount[vertex] << std::endl;
        )
    }
    
    while (size > 0 && !htd::Library::instance().isAborted())
    {
        if (pool.size() == 0)
        {
            minFill = (std::size_t)-1;
    
            for (htd::vertex_t vertex : vertices)
            {
                std::size_t tmp = requiredFillAmount.at(vertex);

                if (tmp <= minFill)
                {
                    if (tmp < minFill)
                    {
                        minFill = tmp;

                        pool.clear();
                    }

                    pool.insert(vertex);
                } 
            }
        }
        
        DEBUGGING_CODE_LEVEL2(
        std::cout << "POOL (FILL=" << min << "): ";
        htd::print(pool, false);
        std::cout << std::endl;
        )

        std::size_t minDegree = (std::size_t)-1;
        
        for (htd::vertex_t vertex : pool)
        {
            std::size_t currentDegree = neighborhood.at(vertex).size() - 1;
            
            if (currentDegree <= minDegree)
            {
                if (currentDegree < minDegree)
                {
                    minDegreePool.clear();

                    minDegree = currentDegree;
                }
                
                minDegreePool.push_back(vertex);
            }
        }

        /* Coverity complains about std::rand() being not safe for security related operations. We are happy with a pseudo-random number here. */
        // coverity[dont_call]
        htd::vertex_t selectedVertex = minDegreePool[std::rand() % minDegreePool.size()];

        auto & selectedNeighborhood = neighborhood.at(selectedVertex);
        
        pool.erase(selectedVertex);
        
        updateStatus.at(selectedVertex) = 4;
        
        affectedVertices.clear();
        
        if (requiredFillAmount.at(selectedVertex) == 0)
        {
            for (auto vertex : selectedNeighborhood)
            {
                if (vertex != selectedVertex)
                {
                    auto & currentNeighborhood = neighborhood.at(vertex);

                    requiredFillAmount[vertex] -= htd::set_difference_size(currentNeighborhood.begin(), currentNeighborhood.end(), selectedNeighborhood.begin(), selectedNeighborhood.end());

                    /* Because 'vertex' is a neighbor of 'selectedVertex', std::lower_bound will always find 'selectedVertex' in 'currentNeighborhood'. */
                    // coverity[use_iterator]
                    currentNeighborhood.erase(std::lower_bound(currentNeighborhood.begin(), currentNeighborhood.end(), selectedVertex));
                }
            }
        }
        else
        {
            for (auto neighbor : selectedNeighborhood)
            {
                if (neighbor != selectedVertex)
                {
                    if (updateStatus.at(neighbor) == 0)
                    {
                        decompose_sets(selectedNeighborhood, neighborhood.at(neighbor), selectedVertex,
                                       additionalNeighbors[neighbor],
                                       unaffectedNeighbors[neighbor],
                                       existingNeighbors[neighbor]);
                    }

                    updateStatus.at(neighbor) |= 1;

                    for (auto affectedVertex : neighborhood.at(neighbor))
                    {
                        htd::state_t currentUpdateStatus = updateStatus.at(affectedVertex);

                        if (currentUpdateStatus < 2)
                        {
                            if (currentUpdateStatus == 0)
                            {
                                decompose_sets(selectedNeighborhood, neighborhood.at(affectedVertex), selectedVertex,
                                               additionalNeighbors[affectedVertex],
                                               unaffectedNeighbors[affectedVertex],
                                               existingNeighbors[affectedVertex]);
                            }

                            affectedVertices.push_back(affectedVertex);
                            
                            updateStatus.at(affectedVertex) |= 2;
                        }
                    }
                }
            }

            for (htd::vertex_t vertex : selectedNeighborhood)
            {
                if (vertex != selectedVertex)
                {
                    auto & currentNeighborhood = neighborhood.at(vertex);
                    auto & currentUnaffectedNeighborhood = unaffectedNeighbors.at(vertex);
                    auto & currentAdditionalNeighborhood = additionalNeighbors.at(vertex);

                    std::size_t additionalNeighborCount = currentAdditionalNeighborhood.size();

                    if (additionalNeighborCount > 0)
                    {
                        if (additionalNeighborCount == 1)
                        {
                            auto first = currentNeighborhood.begin();
                            auto last = currentNeighborhood.end();
                            
                            htd::vertex_t newVertex = currentAdditionalNeighborhood[0];

                            if (newVertex < selectedVertex)
                            {
                                if (selectedVertex - newVertex == 1)
                                {
                                    *std::lower_bound(first, last, selectedVertex) = newVertex;
                                }
                                else
                                {
                                    htd::index_t position1 = std::distance(first, std::lower_bound(first, last, newVertex));
                                    htd::index_t position2 = std::distance(first, std::lower_bound(first + position1, last, selectedVertex));
                                    
                                    currentNeighborhood.erase(first + position2);
                                    currentNeighborhood.insert(currentNeighborhood.begin() + position1, newVertex);
                                }
                            }
                            else
                            {
                                if (newVertex - selectedVertex == 1)
                                {
                                    *std::lower_bound(first, last, selectedVertex) = newVertex;
                                }
                                else
                                {
                                    htd::index_t position1 = std::distance(first, std::lower_bound(first, last, selectedVertex));
                                    htd::index_t position2 = std::distance(first, std::lower_bound(first + position1, last, newVertex));
                                    
                                    currentNeighborhood.erase(first + position1);
                                    currentNeighborhood.insert(currentNeighborhood.begin() + position2 - 1, newVertex);
                                }
                            }
                        }
                        else
                        {
                            htd::set_union(currentNeighborhood, currentAdditionalNeighborhood, selectedVertex, newNeighborhood);
                            
                            std::swap(currentNeighborhood, newNeighborhood);

                            newNeighborhood.clear();
                        }
                    }
                    else
                    {
                        currentNeighborhood.erase(std::lower_bound(currentNeighborhood.begin(), currentNeighborhood.end(), selectedVertex));
                    }

                    std::size_t tmp = requiredFillAmount.at(vertex);

                    if (additionalNeighborCount > 0 || tmp > 0)
                    {
                        std::size_t unaffectedNeighborCount = currentUnaffectedNeighborhood.size();

                        if (unaffectedNeighborCount > 0)
                        {
                            if (additionalNeighborCount == 0)
                            {
                                auto & relevantNeighborhood = existingNeighbors.at(vertex);

                                auto last = relevantNeighborhood.end();

                                std::size_t count = relevantNeighborhood.size();
                                htd::index_t index = 0;
    
                                for (auto it = relevantNeighborhood.begin(); index < count && tmp > unaffectedNeighborCount; index++)
                                {
                                    htd::vertex_t vertex2 = *it;

                                    auto & currentAdditionalNeighborhood2 = additionalNeighbors.at(vertex2);

                                    it++;

                                    tmp -= htd::set_intersection_size(it, last, std::upper_bound(currentAdditionalNeighborhood2.begin(), currentAdditionalNeighborhood2.end(), vertex2), currentAdditionalNeighborhood2.end());
                                }

                                tmp -= unaffectedNeighborCount;

                                //TODO
                                updateStatus.at(vertex) = 0;

                                if (tmp <= minFill)
                                {
                                    if (tmp < minFill)
                                    {
                                        minFill = tmp;

                                        pool.clear();
                                    }

                                    pool.insert(vertex);
                                }
                            }
                            else
                            {
                                auto first = currentAdditionalNeighborhood.begin();
                                auto last = currentAdditionalNeighborhood.end();

                                for (htd::vertex_t unaffectedVertex : currentUnaffectedNeighborhood)
                                {
                                    auto & affectedVertices = existingNeighbors.at(unaffectedVertex);

                                    tmp += htd::set_difference_size(first, last, affectedVertices.begin(), affectedVertices.end());

                                    tmp--;
                                }

                                updateStatus.at(vertex) &= ~1;

                                if (updateStatus.at(vertex) == 0)
                                {
                                    if (tmp <= minFill)
                                    {
                                        if (tmp < minFill)
                                        {
                                            minFill = tmp;

                                            pool.clear();
                                        }

                                        pool.insert(vertex);
                                    }
                                }
                            }
                        }
                        else
                        {
                            tmp = 0;

                            //TODO
                            updateStatus.at(vertex) = 0;

                            if (tmp < minFill)
                            {
                                minFill = tmp;

                                pool.clear();
                            }

                            pool.insert(vertex);
                        }

                        requiredFillAmount.at(vertex) = tmp;
                    }
                    else
                    {
                        updateStatus.at(vertex) = 0;
                    }
                }
            }

    #ifdef TESTOUTPUT
            std::cout << "SELECTED VERTEX: " << selectedVertex << std::endl;
            std::cout << "   DIRECT NEIGHBORS:  ";
            htd::print(selectedNeighborhood, false);
            std::cout << std::endl;
            for (auto vertex : selectedNeighborhood)
            {
                if (vertex != selectedVertex)
                {
                    std::cout << "      NEIGHBORHOOD " << vertex << ": ";
                    htd::print(neighborhood[vertex], false);
                    std::cout << std::endl;
                    std::cout << "         EXISTING:   ";
                    htd::print(existingNeighbors[vertex], false);
                    std::cout << std::endl;
                    std::cout << "         ADDITIONAL: ";
                    htd::print(additionalNeighbors[vertex], false);
                    std::cout << std::endl;
                    std::cout << "         UNAFFECTED: ";
                    htd::print(unaffectedNeighbors[vertex], false);
                    std::cout << std::endl;
                }
            }
            std::cout << "   ----- ----- -----" << std::endl;
            std::cout << "   AFFECTED VERTICES: ";
            htd::print(affectedVertices, false);
            std::cout << std::endl;
            for (auto vertex : affectedVertices)
            {
                std::cout << "      NEIGHBORHOOD " << vertex << ": ";
                htd::print(neighborhood[vertex], false);
                std::cout << std::endl;
                std::cout << "         EXISTING:   ";
                htd::print(existingNeighbors[vertex], false);
                std::cout << std::endl;
                std::cout << "         UNAFFECTED: ";
                htd::print(unaffectedNeighbors[vertex], false);
                std::cout << std::endl;
            }
            std::cout << std::endl;

            std::cout << "STATUS: ";
            std::vector<int> statusBefore;
            std::copy(updateStatus.begin(), updateStatus.end(), std::back_inserter(statusBefore));
            htd::print(statusBefore, false);
            std::cout << std::endl;
    #endif

            for (auto vertex : affectedVertices)
            {
                if (updateStatus.at(vertex) == 2)
                {
                    std::size_t tmp = requiredFillAmount.at(vertex);

                    if (unaffectedNeighbors.at(vertex).size() > 0 && tmp > 0)
                    {
                        auto & relevantNeighborhood = existingNeighbors.at(vertex);

                        auto last = relevantNeighborhood.end();

                        std::size_t count = relevantNeighborhood.size();
                        htd::index_t index = 0;
                        
                        for (auto it = relevantNeighborhood.begin(); index < count && tmp > 0;)
                        {
                            htd::vertex_t vertex2 = *it;

                            auto & currentAdditionalNeighborhood2 = additionalNeighbors.at(vertex2);

                            it++;
                            index++;

                            tmp -= htd::set_intersection_size(it, last, std::upper_bound(currentAdditionalNeighborhood2.begin(), currentAdditionalNeighborhood2.end(), vertex2), currentAdditionalNeighborhood2.end());
                        }

                        if (tmp <= minFill)
                        {
                            if (tmp < minFill)
                            {
                                minFill = tmp;

                                pool.clear();
                            }

                            pool.insert(vertex);
                        }
                    }
                    else
                    {    
                        tmp = 0;

                        if (tmp < minFill)
                        {
                            minFill = tmp;

                            pool.clear();
                        }

                        pool.insert(vertex);
                    }

                    requiredFillAmount.at(vertex) = tmp;
                }
            }

            for (auto vertex : selectedNeighborhood)
            {
                additionalNeighbors[vertex].clear();
                unaffectedNeighbors[vertex].clear();
                existingNeighbors[vertex].clear();
            }

            for (auto vertex : affectedVertices)
            {
                if (updateStatus.at(vertex) == 2)
                {
                    additionalNeighbors[vertex].clear();
                    unaffectedNeighbors[vertex].clear();
                    existingNeighbors[vertex].clear();

                    updateStatus.at(vertex) = 0;
                }
            }
        }
        
        selectedNeighborhood.clear();
        
        vertices.erase(selectedVertex);
        
        size--;

        target.push_back(selectedVertex);

        //TODO Remove
        /*
        std::cout << "SELECTED: " << selectedVertex << std::endl;
        for (htd::vertex_t vertex : vertices)
        {
            std::cout << "Vertex " << vertex << ": FILL=" << requiredFillAmount.at(vertex) << std::endl << "   ";
            htd::print(neighborhood.at(vertex), std::cout, false);
            std::cout << std::endl;
        }
        std::cout << std::endl;
        */

#ifdef TESTOUTPUT
        std::cout << "STATUS AFTER: ";
        std::vector<int> statusAfter;
        std::copy(updateStatus.begin(), updateStatus.end(), std::back_inserter(statusAfter));
        htd::print(statusAfter, false);
        std::cout << "   AVAILABLE: " << std::count(updateStatus.begin(), updateStatus.end(), 0) << std::endl;
#endif
        
//#define VERIFY
#ifdef VERIFY
        long size = 0;
        
        //TODO
        //std::cout << "CHECK (ELIMINATED=" << selectedVertex << "): " << std::endl;
        
        for (htd::vertex_t vertex : vertices)
        {
            if (updateStatus[vertex] == 0)
            {
                auto & currentNeighborhood = neighborhood[vertex];

                size = currentNeighborhood.size();
                
                long actual = requiredFillAmount[vertex];
                
                long maximumEdges = (size * (size - 1)) / 2;
                long existingEdges = computeEdgeCount(neighborhood, currentNeighborhood);
                
                long expected = maximumEdges - existingEdges;
                
                if (actual != expected)
                {
                    std::cout << "ERROR!!! Vertex " << vertex << " (Expected: " << expected << ", Actual: " << actual << ")" << std::endl;

                    std::cout << "VERTEX " << vertex << ":" << std::endl;
                    std::cout << "   NEIGHBORHOOD:   ";
                    htd::print(currentNeighborhood, false);
                    std::cout << std::endl;
                    for (auto vertex2 : currentNeighborhood)
                    {
                        if (vertex2 != vertex)
                        {
                            std::cout << "   NEIGHBORHOOD " << vertex2 << ": ";
                            htd::print(neighborhood[vertex2], false);
                            std::cout << std::endl;
                        }
                    }
                    std::cout << "EDGES " << vertex << ": " << existingEdges << "/" << maximumEdges << std::endl;
                
                    return;
                }
            }
        }
            
        //TODO
        //std::cout << std::endl;
#endif
    }
    
    DEBUGGING_CODE_LEVEL2(std::cout << std::endl;)
}

std::size_t htd::MinFillOrderingAlgorithm::computeEdgeCount(const std::unordered_map<htd::vertex_t, htd::vertex_container> & availableNeighborhoods, const htd::vertex_container & vertices) const
{
    std::size_t ret = 0;

    htd::vertex_t vertex = htd::Vertex::UNKNOWN;

    DEBUGGING_CODE_LEVEL2(
    std::cout << "COMPUTE EDGE COUNT:" << std::endl << "   ";
    htd::print(vertices, false);
    std::cout << std::endl;
    )

    auto last = vertices.end();

    std::size_t count = vertices.size();
    htd::index_t index = 0;

    for (auto it = vertices.begin(); index < count; index++)
    {
        vertex = *it;

        auto & currentNeighborhood = availableNeighborhoods.at(vertex);

        it++;

        ret += htd::set_intersection_size(it, last, std::upper_bound(currentNeighborhood.begin(), currentNeighborhood.end(), vertex), currentNeighborhood.end());
    }
    
    return ret;
}
            
void htd::MinFillOrderingAlgorithm::decompose_sets(const std::vector<htd::vertex_t> & set1,
                                                   const std::vector<htd::vertex_t> & set2,
                                                   htd::vertex_t ignoredVertex,
                                                   std::vector<htd::vertex_t> & resultOnlySet1,
                                                   std::vector<htd::vertex_t> & resultOnlySet2,
                                                   std::vector<htd::vertex_t> & resultIntersection) const
{
    auto first1 = set1.begin();
    auto first2 = set2.begin();
    
    std::size_t count1 = set1.size();
    std::size_t count2 = set2.size();

    htd::index_t index1 = 0;
    htd::index_t index2 = 0;
    
    while (index1 < count1 && index2 < count2)
    {
        auto value1 = *first1;
        auto value2 = *first2;

        if (value1 < value2) 
        {
            if (value1 != ignoredVertex) 
            {
                resultOnlySet1.push_back(value1);
            }

            index1++;
            ++first1;
        }
        else if (value2 < value1) 
        {
            if (value2 != ignoredVertex) 
            {
                resultOnlySet2.push_back(value2);
            }

            index2++;
            ++first2; 
        }
        else 
        {
            if (value1 != ignoredVertex) 
            {
                resultIntersection.push_back(value1);
            }

            index1++;
            ++first1;

            //Skip common value in set 2.
            index2++;
            ++first2;
        }
    }

    if (index1 < count1)
    {
        if (*first1 <= ignoredVertex)
        {
            std::copy_if(first1, set1.end(), std::back_inserter(resultOnlySet1), [&](const htd::vertex_t vertex) { return vertex != ignoredVertex; });
        }
        else
        {
            std::copy(first1, set1.end(), std::back_inserter(resultOnlySet1));
        }
    }
    else if (index2 < count2)
    {
        if (*first2 <= ignoredVertex)
        {
            std::copy_if(first2, set2.end(), std::back_inserter(resultOnlySet2), [&](const htd::vertex_t vertex) { return vertex != ignoredVertex; });
        }
        else
        {
            std::copy(first2, set2.end(), std::back_inserter(resultOnlySet2));
        }
    }
}

htd::MinFillOrderingAlgorithm * htd::MinFillOrderingAlgorithm::clone(void) const
{
    return new htd::MinFillOrderingAlgorithm();
}

#endif /* HTD_HTD_MINFILLORDERINGALGORITHM_CPP */
