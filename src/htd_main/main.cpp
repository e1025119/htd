/*
 * File:   main.cpp
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

#include <htd/main.hpp>
#include <htd_cli/main.hpp>

#include <htd_main/GrFormatImporter.hpp>
#include <htd_main/LpFormatImporter.hpp>
#include <htd_main/HgrFormatImporter.hpp>
#include <htd_main/ITreeDecompositionExporter.hpp>
#include <htd_main/IHypertreeDecompositionExporter.hpp>
#include <htd_main/WidthExporter.hpp>
#include <htd_main/TdFormatExporter.hpp>
#include <htd_main/HumanReadableExporter.hpp>

#include <csignal>
#include <cstring>

htd::LibraryInstance * const libraryInstance = htd::createManagementInstance(htd::Id::FIRST);

htd_cli::OptionManager * createOptionManager(void)
{
    htd_cli::OptionManager * manager = new htd_cli::OptionManager();

    htd_cli::Option * helpOption = new htd_cli::Option("help", "Print usage information and exit.", 'h');

    manager->registerOption(helpOption, "General Options");

    htd_cli::SingleValueOption * seedOption = new htd_cli::SingleValueOption("seed", "Set the seed for the random number generator to <seed>.", "seed", 's');

    manager->registerOption(seedOption, "General Options");

    htd_cli::Choice * decompositionTypeChoice = new htd_cli::Choice("type", "Compute a graph decomposition of type <type>.", "type");

    decompositionTypeChoice->addPossibility("tree", "Compute a tree decomposition of the input graph.");
    decompositionTypeChoice->addPossibility("hypertree", "Compute a hypertree decomposition of the input graph.");

    decompositionTypeChoice->setDefaultValue("tree");

    manager->registerOption(decompositionTypeChoice, "Decomposition Options");

    htd_cli::Choice * inputFormatChoice = new htd_cli::Choice("input", "Assume that the input graph is given in format <format>.\n  (See https://github.com/mabseher/htd/blob/master/FORMATS.md for information about the available input formats.)", "format");

    inputFormatChoice->addPossibility("gr", "Use the input format 'gr'.");
    //inputFormatChoice->addPossibility("hg", "Use the input format 'hg'.");
    inputFormatChoice->addPossibility("lp", "Use the input format 'lp'.");
    inputFormatChoice->addPossibility("hgr", "Use the input format 'hgr'.");

    inputFormatChoice->setDefaultValue("gr");

    manager->registerOption(inputFormatChoice, "Input-Specific Options");

    htd_cli::Choice * outputFormatChoice = new htd_cli::Choice("output", "Set the output format of the decomposition to <format>.\n  (See https://github.com/mabseher/htd/blob/master/FORMATS.md for information about the available output formats.)", "format");

    outputFormatChoice->addPossibility("td", "Use the output format 'td'.");
    //outputFormatChoice->addPossibility("graphml", "Use the output format 'graphml'.");
    outputFormatChoice->addPossibility("human", "Provide a human-readable output of the decomposition.");
    outputFormatChoice->addPossibility("width", "Provide only the maximum bag size of the decomposition.");

    outputFormatChoice->setDefaultValue("td");

    manager->registerOption(outputFormatChoice, "Output-Specific Options");

    htd_cli::Choice * orderingAlgorithmChoice = new htd_cli::Choice("ordering", "Set the ordering algorithm which shall be used to <algorithm>.", "algorithm");

    orderingAlgorithmChoice->addPossibility("min-fill+", "Advanced minimum-fill ordering algorithm");
    orderingAlgorithmChoice->addPossibility("min-fill", "Standard minimum-fill ordering algorithm");
    orderingAlgorithmChoice->addPossibility("min-degree", "Minimum-degree ordering algorithm");
    orderingAlgorithmChoice->addPossibility("max-cardinality", "Maximum cardinality search ordering algorithm");

    orderingAlgorithmChoice->setDefaultValue("min-fill+");

    manager->registerOption(orderingAlgorithmChoice, "Algorithm Options");

    htd_cli::Choice * optimizationChoice = new htd_cli::Choice("opt", "Iteratively compute a decomposition which optimizes <criterion>.", "criterion");

    optimizationChoice->addPossibility("none", "Do not perform any optimization.");
    optimizationChoice->addPossibility("width", "Minimize the maximum bag size of the computed decomposition.");

    optimizationChoice->setDefaultValue("none");

    manager->registerOption(optimizationChoice, "Optimization Options");

    htd_cli::SingleValueOption * iterationOption = new htd_cli::SingleValueOption("iterations", "Set the number of iterations to be performed during optimization to <count> (0 = infinite). (Default: 10)", "count");

    manager->registerOption(iterationOption, "Optimization Options");

    htd_cli::SingleValueOption * nonImprovementLimitOption = new htd_cli::SingleValueOption("non-improvement-limit", "Terminate the algorithm if more than <count> iterations did not lead to an improvement (-1 = infinite). (Default: -1)", "count");

    manager->registerOption(nonImprovementLimitOption, "Optimization Options");

    htd_cli::Option * printProgressOption = new htd_cli::Option("print-opt-progress", "Print progress whenever a new optimal decomposition is found.");

    manager->registerOption(printProgressOption, "Optimization Options");

    return manager;
}

bool handleOptions(int argc, const char * const * const argv, htd_cli::OptionManager & optionManager, htd::LibraryInstance * const manager)
{
    bool ret = true;

    try
    {
        optionManager.parse(argc, argv);
    }
    catch (const std::runtime_error & exception)
    {
        std::cout << "INVALID PROGRAM CALL: " << exception.what() << std::endl;

        ret = false;
    }

    const htd_cli::Option & helpOption = optionManager.accessOption("help");

    const htd_cli::Choice & outputFormatChoice = optionManager.accessChoice("output");

    const htd_cli::Choice & decompositionTypeChoice = optionManager.accessChoice("type");

    const htd_cli::Choice & orderingAlgorithmChoice = optionManager.accessChoice("ordering");

    const htd_cli::SingleValueOption & seedOption = optionManager.accessSingleValueOption("seed");

    const htd_cli::Choice & optimizationChoice = optionManager.accessChoice("opt");

    const htd_cli::SingleValueOption & iterationOption = optionManager.accessSingleValueOption("iterations");

    const htd_cli::SingleValueOption & nonImprovementLimitOption = optionManager.accessSingleValueOption("non-improvement-limit");

    const htd_cli::Option & printProgressOption = optionManager.accessOption("print-opt-progress");

    if (ret && helpOption.used())
    {
        std::cout << "USAGE: " << argv[0] << " [options...] < input" << std::endl << std::endl;

        optionManager.printHelp();

        ret = false;
    }

    if (ret)
    {
        if (seedOption.used())
        {
            std::size_t index = 0;

            const std::string & value = seedOption.value();

            if (value.empty() || value.find_first_not_of("01234567890") != std::string::npos)
            {
                std::cerr << "INVALID SEED: " << seedOption.value() << std::endl;

                ret = false;
            }

            if (ret)
            {
                unsigned long seed = std::stoul(value, &index, 10);

                if (index != value.length())
                {
                    std::cerr << "INVALID SEED: " << value << std::endl;

                    ret = false;
                }
                else
                {
                    srand(seed);
                }
            }
        }
        else
        {
            srand(time(NULL));
        }
    }

    if (ret && orderingAlgorithmChoice.used())
    {
        const std::string & value = orderingAlgorithmChoice.value();

        if (value == "min-fill+")
        {
            manager->orderingAlgorithmFactory().setConstructionTemplate(new htd::AdvancedMinFillOrderingAlgorithm(manager));
        }
        else if (value == "min-fill")
        {
            manager->orderingAlgorithmFactory().setConstructionTemplate(new htd::MinFillOrderingAlgorithm(manager));
        }
        else if (value == "min-degree")
        {
            manager->orderingAlgorithmFactory().setConstructionTemplate(new htd::MinDegreeOrderingAlgorithm(manager));
        }
        else if (value == "max-cardinality")
        {
            manager->orderingAlgorithmFactory().setConstructionTemplate(new htd::MaximumCardinalitySearchOrderingAlgorithm(manager));
        }
        else
        {
            std::cerr << "INVALID ORDERING ALGORITHM: " << orderingAlgorithmChoice.value() << std::endl;

            ret = false;
        }
    }

    if (ret && decompositionTypeChoice.used() && std::string(decompositionTypeChoice.value()) == "hypertree")
    {
        if (outputFormatChoice.used() && std::string(outputFormatChoice.value()) == "td")
        {
            std::cerr << "INVALID OUTPUT FORMAT: Format 'td' only supports tree decompositions!" << std::endl;

            ret = false;
        }

        if (optimizationChoice.used() && std::string(optimizationChoice.value()) == "width")
        {
            std::cerr << "INVALID PROGRAM CALL: Currently, optimization is supported only for tree decompositions!" << std::endl;

            ret = false;
        }
    }

    if (ret)
    {
        if (iterationOption.used())
        {
            if (optimizationChoice.used() && std::string(optimizationChoice.value()) == "width")
            {
                std::size_t index = 0;

                const std::string & value = iterationOption.value();

                if (value.empty() || value.find_first_not_of("01234567890") != std::string::npos)
                {
                    std::cerr << "INVALID NUMBER OF ITERATIONS: " << iterationOption.value() << std::endl;

                    ret = false;
                }

                if (ret)
                {
                    std::stoul(value, &index, 10);

                    if (index != value.length())
                    {
                        std::cerr << "INVALID NUMBER OF ITERATIONS: " << value << std::endl;

                        ret = false;
                    }
                }
            }
            else
            {
                std::cerr << "INVALID PROGRAM CALL: Option --iterations may only be used when option --opt is set to \"width\"!" << std::endl;

                ret = false;
            }
        }
    }

    if (ret)
    {
        if (nonImprovementLimitOption.used())
        {
            if (optimizationChoice.used() && std::string(optimizationChoice.value()) == "width")
            {
                std::size_t index = 0;

                const std::string & value = nonImprovementLimitOption.value();

                if (value.empty() || (value != "-1" && value.find_first_not_of("01234567890") != std::string::npos))
                {
                    std::cerr << "INVALID MAXIMUM NUMBER OF NON-IMPROVED DECOMPOSITIONS: " << nonImprovementLimitOption.value() << std::endl;

                    ret = false;
                }

                if (ret && value != "-1")
                {
                    std::stoul(value, &index, 10);

                    if (index != value.length())
                    {
                        std::cerr << "INVALID MAXIMUM NUMBER OF NON-IMPROVED DECOMPOSITIONS: " << value << std::endl;

                        ret = false;
                    }
                }
            }
            else
            {
                std::cerr << "INVALID PROGRAM CALL: Option --non-improvement-limit may only be used when option --opt is set to \"width\"!" << std::endl;

                ret = false;
            }
        }
    }

    if (ret)
    {
        if (printProgressOption.used() && !optimizationChoice.used())
        {
            std::cerr << "INVALID PROGRAM CALL: Option --print-opt-progress may only be used when option --opt is set to \"width\"!" << std::endl;

            ret = false;
        }
    }

    return ret;
}

template <typename DecompositionAlgorithm, typename Importer, typename Exporter>
void decompose(const htd::LibraryInstance & instance, const DecompositionAlgorithm & algorithm, const Importer & importer, const Exporter & exporter)
{
    auto * graph = importer.import(std::cin);

    if (graph != nullptr && !instance.isTerminated())
    {
        auto * decomposition = algorithm.computeDecomposition(*graph);

        if (decomposition != nullptr)
        {
            if (!instance.isTerminated() || algorithm.isSafelyInterruptible())
            {
                exporter.write(*decomposition, *graph, std::cout);
            }
            else
            {
                std::cerr << "Program was terminated successfully!" << std::endl;
            }

            delete decomposition;
        }
        else
        {
            if (instance.isTerminated())
            {
                std::cerr << "Program was terminated successfully!" << std::endl;
            }
            else
            {
                std::cerr << "NO TREE DECOMPOSITION COMPUTED!" << std::endl;
            }
        }

        delete graph;
    }
    else
    {
        if (instance.isTerminated())
        {
            std::cerr << "Program was terminated successfully!" << std::endl;
        }
        else
        {
            std::cerr << "NO VALID INSTANCE PROVIDED!" << std::endl;
        }
    }
}

template <typename DecompositionAlgorithm, typename Importer, typename Exporter>
void decomposeNamed(const htd::LibraryInstance & instance, const DecompositionAlgorithm & algorithm, const Importer & importer, const Exporter & exporter)
{
    auto * graph = importer.import(std::cin);

    if (graph != nullptr && !instance.isTerminated())
    {
        auto * decomposition = algorithm.computeDecomposition(graph->internalGraph());

        if (decomposition != nullptr)
        {
            if (!instance.isTerminated() || algorithm.isSafelyInterruptible())
            {
                exporter.write(*decomposition, *graph, std::cout);
            }
            else
            {
                std::cerr << "Program was terminated successfully!" << std::endl;
            }

            delete decomposition;
        }
        else
        {
            if (instance.isTerminated())
            {
                std::cerr << "Program was terminated successfully!" << std::endl;
            }
            else
            {
                std::cerr << "NO TREE DECOMPOSITION COMPUTED!" << std::endl;
            }
        }

        delete graph;
    }
    else
    {
        if (instance.isTerminated())
        {
            std::cerr << "Program was terminated successfully!" << std::endl;
        }
        else
        {
            std::cerr << "NO VALID INSTANCE PROVIDED!" << std::endl;
        }
    }
}

template <typename Importer, typename Exporter>
void optimize(const htd::LibraryInstance & instance, const htd::IterativeImprovementTreeDecompositionAlgorithm & algorithm, const Importer & importer, const Exporter & exporter, bool printProgress, const std::string & outputFormat)
{
    auto * graph = importer.import(std::cin);

    if (graph != nullptr)
    {
        if (!instance.isTerminated())
        {
            std::size_t optimalBagSize = (std::size_t)-1;

            htd::ITreeDecomposition * decomposition =
                algorithm.computeDecomposition(*graph, [&](const htd::IMultiHypergraph & graph, const htd::ITreeDecomposition & decomposition, const htd::FitnessEvaluation & fitness)
                {
                    HTD_UNUSED(graph)
                    HTD_UNUSED(decomposition)

                    if (printProgress)
                    {
                        std::size_t bagSize = -fitness.at(0);

                        if (bagSize < optimalBagSize)
                        {
                            optimalBagSize = bagSize;

                            std::chrono::milliseconds::rep msSinceEpoch =
                                std::chrono::duration_cast<std::chrono::milliseconds>
                                    (std::chrono::system_clock::now().time_since_epoch()).count();

                            if (outputFormat == "td")
                            {
                                std::cout << "c status " << optimalBagSize << " " << msSinceEpoch << std::endl;
                            }
                            else
                            {
                                std::cerr << "New optimal bag size: " << optimalBagSize << std::endl;
                            }
                        }
                    }
                });

            if (decomposition != nullptr)
            {
                if (!instance.isTerminated() || algorithm.isSafelyInterruptible())
                {
                    exporter.write(*decomposition, *graph, std::cout);
                }
                else
                {
                    std::cerr << "Program was terminated successfully!" << std::endl;
                }

                delete decomposition;
            }
            else
            {
                if (instance.isTerminated())
                {
                    std::cerr << "Program was terminated successfully!" << std::endl;
                }
                else
                {
                    std::cerr << "NO TREE DECOMPOSITION COMPUTED!" << std::endl;
                }
            }
        }
        else
        {
            std::cerr << "Program was terminated successfully!" << std::endl;
        }

        delete graph;
    }
}

template <typename Importer, typename Exporter>
void optimizeNamed(const htd::LibraryInstance & instance, const htd::IterativeImprovementTreeDecompositionAlgorithm & algorithm, const Importer & importer, const Exporter & exporter, bool printProgress, const std::string & outputFormat)
{
    auto * graph = importer.import(std::cin);

    if (graph != nullptr && !instance.isTerminated())
    {
        std::size_t optimalBagSize = (std::size_t)-1;

        htd::ITreeDecomposition * decomposition =
            algorithm.computeDecomposition(graph->internalGraph(), [&](const htd::IMultiHypergraph & graph, const htd::ITreeDecomposition & decomposition, const htd::FitnessEvaluation & fitness)
            {
                HTD_UNUSED(graph)
                HTD_UNUSED(decomposition)

                if (printProgress)
                {
                    std::size_t bagSize = -fitness.at(0);

                    if (bagSize < optimalBagSize)
                    {
                        optimalBagSize = bagSize;

                        std::chrono::milliseconds::rep msSinceEpoch =
                            std::chrono::duration_cast<std::chrono::milliseconds>
                                (std::chrono::system_clock::now().time_since_epoch()).count();

                        if (outputFormat == "td")
                        {
                            std::cout << "c status " << optimalBagSize << " " << msSinceEpoch << std::endl;
                        }
                        else
                        {
                            std::cerr << "New optimal bag size: " << optimalBagSize << std::endl;
                        }
                    }
                }
            });

        if (decomposition != nullptr)
        {
            if (!instance.isTerminated() || algorithm.isSafelyInterruptible())
            {
                exporter.write(*decomposition, *graph, std::cout);
            }
            else
            {
                std::cerr << "Program was terminated successfully!" << std::endl;
            }

            delete decomposition;
        }
        else
        {
            if (instance.isTerminated())
            {
                std::cerr << "Program was terminated successfully!" << std::endl;
            }
            else
            {
                std::cerr << "NO TREE DECOMPOSITION COMPUTED!" << std::endl;
            }
        }

        delete graph;
    }
}

template <typename DecompositionAlgorithm, typename Exporter>
void run(const DecompositionAlgorithm & algorithm, const Exporter & exporter, const std::string & inputFormat, const htd::LibraryInstance * const manager)
{
    if (inputFormat == "gr")
    {
        htd_main::GrFormatImporter importer(manager);

        decompose(*manager, algorithm, importer, exporter);
    }
    else if (inputFormat == "lp")
    {
        htd_main::LpFormatImporter importer(manager);

        decomposeNamed(*manager, algorithm, importer, exporter);
    }
    else if (inputFormat == "hgr")
    {
        htd_main::HgrFormatImporter importer(manager);

        decompose(*manager, algorithm, importer, exporter);
    }
}

void handleSignal(int signal)
{
    switch (signal)
    {
        case SIGINT:
        {
            libraryInstance->terminate();

            break;
        }
        case SIGTERM:
        {
            libraryInstance->terminate();

            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 *  Implementation of the htd::ITreeDecompositionFitnessFunction interface which prefers decompositions of minimal width.
 */
class WidthMinimizingFitnessFunction : public htd::ITreeDecompositionFitnessFunction
{
    public:
        /**
         *  Constructor of class WidthMinimizingFitnessFunction.
         */
        WidthMinimizingFitnessFunction(void)
        {

        }

        /**
         *  Destructor of class WidthMinimizingFitnessFunction.
         */
        virtual ~WidthMinimizingFitnessFunction()
        {

        }

        htd::FitnessEvaluation * fitness(const htd::IMultiHypergraph & graph, const htd::ITreeDecomposition & decomposition) const HTD_OVERRIDE
        {
            HTD_UNUSED(graph)

            return new htd::FitnessEvaluation(1, -(double)(decomposition.maximumBagSize()));
        }

        WidthMinimizingFitnessFunction * clone(void) const HTD_OVERRIDE
        {
            return new WidthMinimizingFitnessFunction();
        }
};

int main(int argc, const char * const * const argv)
{
    int ret = 0;

    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    htd_cli::OptionManager * optionManager = createOptionManager();

    if (handleOptions(argc, argv, *optionManager, libraryInstance))
    {
        const htd_cli::Choice & inputFormatChoice = optionManager->accessChoice("input");

        const htd_cli::Choice & outputFormatChoice = optionManager->accessChoice("output");

        const htd_cli::Choice & decompositionTypeChoice = optionManager->accessChoice("type");

        const htd_cli::Choice & optimizationChoice = optionManager->accessChoice("opt");

        const htd_cli::SingleValueOption & iterationOption = optionManager->accessSingleValueOption("iterations");

        const htd_cli::SingleValueOption & nonImprovementLimitOption = optionManager->accessSingleValueOption("non-improvement-limit");

        const htd_cli::Option & printProgressOption = optionManager->accessOption("print-opt-progress");

        const std::string & outputFormat = outputFormatChoice.value();

        bool hypertreeDecompositionRequested = decompositionTypeChoice.used() && std::string(decompositionTypeChoice.value()) == "hypertree";

        if (hypertreeDecompositionRequested)
        {
            htd::IHypertreeDecompositionAlgorithm * algorithm = libraryInstance->hypertreeDecompositionAlgorithmFactory().getHypertreeDecompositionAlgorithm(libraryInstance);

            htd_main::IHypertreeDecompositionExporter * exporter = nullptr;

            if (outputFormat == "human")
            {
                exporter = new htd_main::HumanReadableExporter();
            }
            else if (outputFormat == "width")
            {
                exporter = new htd_main::WidthExporter();
            }

            run(*algorithm, *exporter, inputFormatChoice.value(), libraryInstance);

            delete algorithm;
            delete exporter;
        }
        else
        {
            htd_main::ITreeDecompositionExporter * exporter = nullptr;

            if (outputFormat == "td")
            {
                exporter = new htd_main::TdFormatExporter();
            }
            else if (outputFormat == "human")
            {
                exporter = new htd_main::HumanReadableExporter();
            }
            else if (outputFormat == "width")
            {
                exporter = new htd_main::WidthExporter();
            }

            if (optimizationChoice.used() && std::string(optimizationChoice.value()) == "width")
            {
                htd::IterativeImprovementTreeDecompositionAlgorithm algorithm(libraryInstance, libraryInstance->treeDecompositionAlgorithmFactory().getTreeDecompositionAlgorithm(libraryInstance), WidthMinimizingFitnessFunction());

                if (iterationOption.used())
                {
                    algorithm.setIterationCount(std::stoul(iterationOption.value(), nullptr, 10));
                }

                if (nonImprovementLimitOption.used())
                {
                    if (std::string(nonImprovementLimitOption.value()) == "-1")
                    {
                        algorithm.setNonImprovementLimit((std::size_t)-1);
                    }
                    else
                    {
                        algorithm.setNonImprovementLimit(std::stoul(nonImprovementLimitOption.value(), nullptr, 10));
                    }
                }

                const std::string & inputFormat = inputFormatChoice.value();

                if (inputFormat == "gr")
                {
                    htd_main::GrFormatImporter importer(libraryInstance);

                    optimize(*libraryInstance, algorithm, importer, *exporter, printProgressOption.used(), outputFormat);
                }
                else if (inputFormat == "lp")
                {
                    htd_main::LpFormatImporter importer(libraryInstance);

                    optimizeNamed(*libraryInstance, algorithm, importer, *exporter, printProgressOption.used(), outputFormat);
                }
                else if (inputFormat == "hgr")
                {
                    htd_main::HgrFormatImporter importer(libraryInstance);

                    optimize(*libraryInstance, algorithm, importer, *exporter, printProgressOption.used(), outputFormat);
                }
            }
            else
            {
                htd::ITreeDecompositionAlgorithm * algorithm = libraryInstance->treeDecompositionAlgorithmFactory().getTreeDecompositionAlgorithm(libraryInstance);

                run(*algorithm, *exporter, inputFormatChoice.value(), libraryInstance);

                delete algorithm;
            }

            delete exporter;
        }
    }

    delete libraryInstance;

    delete optionManager;

    return ret;
}