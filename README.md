# htd

[![License](http://img.shields.io/badge/license-GPLv3-blue.svg)](https://www.gnu.org/copyleft/gpl.html)
[![Build Status](https://travis-ci.org/mabseher/htd.svg?branch=master)](https://travis-ci.org/mabseher/htd)
[![Coverity Status](https://scan.coverity.com/projects/8163/badge.svg)](https://scan.coverity.com/projects/mabseher-htd)
[![Code Coverage Status](https://coveralls.io/repos/github/mabseher/htd/badge.svg?branch=master&update=1)](https://coveralls.io/github/mabseher/htd?branch=master)

**A small but efficient C++ library for computing (customized) tree and hypertree decompositions.**

**htd** is a library which does not only compute tree decompositions, it also allows to fully customize them via (custom) manipulations, labelings and optimize them based on a user-provided fitness function. The library provides efficient implementations of well-established techniques for computing tree decompositions (like bucket-elimination based on a vertex elimination ordering of the input graph) and it is optimized for large (hyper)graphs. At the current stage, **htd** is able to decompose graphs containing millions of vertices and several hundreds of thousands of (hyper)edges efficiently. 

For almost each class used in the library, **htd** provides an interface and a corresponding factory class allowing to directly influence the process of generating decompositions without having to re-invent the wheel at any place. That is, if one for instance develops a new heuristic for vertex elimination orderings there is absolutely no need to change anything in the remainder of the library in order to test its influence on the quality of bucket-elimination. (In fact, one does not even need to re-compile **htd** for such a modification as long as all interfaces are implemented properly by the new algorithm and as long as the algorithm is made available to **htd** via the corresponding factory class.)

## BUILD PROCESS

For instructions about the build process, please read the `INSTALL.md` file.

## USAGE

### Using htd via command-line interface

For using **htd** via the command-line interface there is the front-end application **htd_main**. **htd_main** provides access to basic functionality of **htd**.

A program call for **htd_main** is of the following form:

`./htd_main [-h] [-s <SEED>] [--type <DECOMPOSITION_TYPE>] [--input <FORMAT>] [--output <FORMAT>] [--ordering <ALGORITHM>] [--opt <CRITERION>] [--iterations <LIMIT>] [--non-improvement-limit <LIMIT>] [--print-opt-progress] < $FILE`

Options are organized in the following groups:

* General Options:
  * `--help, -h :                      Print usage information and exit.`
  * `--seed, -s <seed> :               Set the seed for the random number generator to <seed>.`

* Decomposition Options:
  * `--type <type> :                   Compute a graph decomposition of type <type>.`
    * `Permitted Values:`
      * `.) tree      : Compute a tree decomposition of the input graph. (default)`
      * `.) hypertree : Compute a hypertree decomposition of the input graph.`

  * Input-Specific Options:
    * `--input <format> :              Assume that the input graph is given in format <format>.` (See [FORMATS](https://github.com/mabseher/htd/formats.md) for information about the available input formats.)
      * `Permitted Values:`
        * `.) gr  : Use the input format 'gr'. (default)`
        * `.) lp  : Use the input format 'lp'.`
        * `.) mgr : Use the input format 'mgr'.`

* Output-Specific Options:
  * `--output <format> :               Set the output format of the decomposition to <format>.` (See [FORMATS](https://github.com/mabseher/htd/formats.md) for information about the available output formats.)
    * `Permitted Values:`
      * `.) td    : Use the output format 'td'. (default)`
      * `.) human : Provide a human-readable output of the decomposition.`
      * `.) width : Provide only the maximum bag size of the decomposition.`

* Algorithm Options:
  * `--ordering <algorithm> :          Set the ordering algorithm which shall be used to <algorithm>.`
    * `Permitted Values:`
      * `.) min-fill+       : Advanced minimum-fill ordering algorithm (default)`
      * `.) min-fill        : Standard minimum-fill ordering algorithm`
      * `.) min-degree      : Minimum-degree ordering algorithm`
      * `.) max-cardinality : Maximum cardinality search ordering algorithm`

* Optimization Options:
  * `--opt <criterion> :               Iteratively compute a decomposition which optimizes <criterion>.`
    * `Permitted Values:`
      * `.) none  : Do not perform any optimization. (default)`
      * `.) width : Minimize the maximum bag size of the computed decomposition.`
  * `--iterations <count> :            Set the number of iterations to be performed during optimization to <count> (0 = infinite). (Default: 10)`
  * `--non-improvement-limit <count> : Terminate the algorithm if more than <count> iterations did not lead to an improvement (-1 = infinite). (Default: -1)`
  * `--print-opt-progress :            Print progress whenever a new optimal decomposition is found.`

### Using htd as a developer

The following example code uses the most important features of **htd**. 

A full API documentation can be generated via `make doc` (requires [Doxygen](www.doxygen.org/)):

```cpp
#include <htd/main.hpp>

#include <csignal>
#include <memory>
#include <chrono>

using namespace std;

/**
 *  Sample fitness function which minimizes width and height of the decomposition.
 *
 *  Width is of higher priority than height, i.e., at first, the width is minimized
 *  and if two decompositions have the same width, the one of lower height is chosen.
 */
class FitnessFunction : public htd::ITreeDecompositionFitnessFunction
{
    public:
        FitnessFunction(void)
        {

        }

        ~FitnessFunction()
        {

        }

        htd::FitnessEvaluation * fitness(const htd::IMultiHypergraph & graph, 
                                         const htd::ITreeDecomposition & decomposition) const
        {
            HTD_UNUSED(graph)

            /**
              * Here we specify the fitness evaluation for a given decomposition. 
              * In this case, we select the maximum bag size and the height.
              */
            return new htd::FitnessEvaluation(2, 
                                              -(double)(decomposition.maximumBagSize()), 
                                              -(double)(decomposition.height()));
        }

        FitnessFunction * clone(void) const
        {
            return new FitnessFunction();
        }
};

/**
 * Signal handling procedure.
 */
void handleSignal(int signal)
{
    switch (signal)
    {
        case SIGINT:
        {
            htd::Library::instance().terminate();

            break;
        }
        case SIGTERM:
        {
            htd::Library::instance().terminate();

            break;
        }
        default:
        {
            break;
        }
    }

    std::signal(signal, handleSignal);
}

int main(int argc, const char * const * const argv)
{
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    std::srand(0);

    // Get a new importer for graph format 'gr'.
    htd::GrFormatImporter importer;

    // Create a new management instance to support graceful termination of algorithms.
    std::shared_ptr<htd::LibraryInstance> libraryInstance = 
        htd::Library::instance().createManagementInstance();

    // Import graph f.
    htd::IMultiGraph * graph = importer.import(argv[1]);

    // Check whether the algorithm was terminated.
    if (!libraryInstance->isTerminated())
    {
        // Create an instance of the fitness function.
        FitnessFunction fitnessFunction;

        /**
         *  This operation changes the root of a given decomposition so that the provided 
         *  fitness function is maximized. When no fitness function is provided to the
         *  constructor, the constructed optimization operation does not perform any 
         *  optimization and only applies provided manipulations.
         */
        htd::TreeDecompositionOptimizationOperation * operation = 
            new htd::TreeDecompositionOptimizationOperation(fitnessFunction);

        /**
         *  Set the previously created management instance to support graceful termination.
         */
        operation->setManagementInstance(libraryInstance);

        /**
         *  Set the vertex selections strategy (default = exhaustive).
         *
         *  In this case, we want to select (at most) 10 vertices of the input decomposition randomly.
         */
        operation->setVertexSelectionStrategy(new htd::RandomVertexSelectionStrategy(10));

        /**
         *  Set desired manipulations. In this case we want a nice (= normalized) tree decomposition.
         */
        operation->addManipulationOperation(new htd::NormalizationOperation(true, true, false, false));

        /**
          * Optionally, we can set the vertex elimination algorithm. 
          * We decide to use the min-degree heuristic in this case.
          */
        htd::OrderingAlgorithmFactory::instance()
            .setConstructionTemplate(new htd::MinDegreeOrderingAlgorithm());

        // Get the default tree decomposition algorithm. One can also choose a custom one.
        htd::ITreeDecompositionAlgorithm * baseAlgorithm = 
            htd::TreeDecompositionAlgorithmFactory::instance().getTreeDecompositionAlgorithm();

        /**
         *  Set the optimization operation as manipulation operation in order 
         *  to choose the optimal root reducing height of the tree decomposition.
         */
        baseAlgorithm->addManipulationOperation(operation);

        /**
         *  Create a new instance of htd::IterativeImprovementTreeDecompositionAlgorithm based 
         *  on the base algorithm and the fitness function. Note that the fitness function can 
         *  be an arbiraty one and can differ from the one used in the optimization operation.
         */
        htd::IterativeImprovementTreeDecompositionAlgorithm algorithm(baseAlgorithm, fitnessFunction);

        /**
         *  Set the previously created management instance to support graceful termination.
         */
        algorithm.setManagementInstance(libraryInstance);

        /**
         *  Set the maximum number of iterations after which the best decomposition with 
         *  respect to the fitness function shall be returned. Use value 1 to make the 
         *  iterative algorithm return the first decomposition found.
         */
        algorithm.setIterationCount(10);

        /**
         *  Set the maximum number of iterations without improvement after which the algorithm returns 
         *  best decomposition with respect to the fitness function found so far. A limit of 0 aborts 
         *  the algorithm after the first non-improving solution has been found, i.e. the algorithm 
         *  will perform a simple hill-climbing approach.
         */
        algorithm.setNonImprovementLimit(3);

        // Record the optimal maximal bag size of the tree decomposition to allow printing the progress.
        std::size_t optimalBagSize = (std::size_t)-1;

        /**
         *  Compute the decomposition. Note that the additional, optional parameter of the function 
         *  computeDecomposition() in case of htd::IterativeImprovementTreeDecompositionAlgorithm 
         *  can be used to intercept every new decomposition. In this case we output some 
         *  intermediate information upon perceiving an improved decompostion.
         */
        htd::ITreeDecomposition * decomposition =
            algorithm.computeDecomposition(*graph, [&](const htd::IMultiHypergraph & graph, 
                                                       const htd::ITreeDecomposition & decomposition, 
                                                       const htd::FitnessEvaluation & fitness)
        {
            // Disable warnings concerning unused variables.
            HTD_UNUSED(graph)
            HTD_UNUSED(decomposition)

            std::size_t bagSize = -fitness.at(0);

            /**
             *  After each improvement we print the current optimal 
             *  width + 1 and the time when the decomposition was found.
             */
            if (bagSize < optimalBagSize)
            {
                optimalBagSize = bagSize;

                std::chrono::milliseconds::rep msSinceEpoch =
                    std::chrono::duration_cast<std::chrono::milliseconds>
                            (std::chrono::system_clock::now().time_since_epoch()).count();

                std::cout << "c status " << optimalBagSize << " " << msSinceEpoch << std::endl;
            }
        });

        // If a decomposition was found we want to print it to stdout.
        if (decomposition != nullptr)
        {
            /**
             *  Check whether the algorithm indeed computed a valid decomposition.
             *
             *  algorithm.isSafelyInterruptible() for decomposition algorithms allows
             *  to find out if the algorithm returned by a factory class is safely
             *  interruptible. If the getter returns true, the algorithms will
             *  guarantee that the computed decomposition is indeed a valid one
             *  and that all manipulations and labelings were applied successfully.
             */
            if (!algorithm.isTerminated() || algorithm.isSafelyInterruptible())
            {
                htd::TdFormatExporter exporter;

                exporter.write(*decomposition, *graph, std::cout);
            }

            delete decomposition;
        }
    }

    delete graph;

    return 0;
}
```

## LICENSE

**htd** is released under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007.

A copy of the license should be provided with the system, otherwise see [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).
