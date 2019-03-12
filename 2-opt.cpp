#include "TourModifier.h"
#include "fileio.h"
#include "lateral.h"
#include "vopt/lateral.h"
#include "solver.h"

#include <iostream>

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::cout << "Arguments: point_set_file_path optional_tour_file_path" << std::endl;
        return 0;
    }

    // Read input files.
    const auto coordinates {fileio::read_coordinates(argv[1])};
    const auto& x {coordinates[0]};
    const auto& y {coordinates[1]};
    const auto initial_tour = fileio::initial_tour(argc, argv, x.size());

    // Initialize tour modifier.
    TourModifier tour(initial_tour, x, y);
    const auto initial_tour_length {tour.length()};
    std::cout << "Initial tour length: " << initial_tour_length << std::endl;

    solver::multi_climb(tour);

    // Save result.
    const auto final_length {tour.length()};
    if (initial_tour_length > final_length)
    {
        auto save_file_prefix {fileio::extract_filename(argv[1])};
        fileio::write_ordered_points(tour.order()
            , "saves/" + save_file_prefix + "_" + std::to_string(final_length) + ".txt");
    }

    // Perturbation hill-climbing.
    auto best_tour {tour};
    bool improving {false};
    do
    {
        improving = false;
        auto new_tour = vopt::lateral::perturbation_climb(best_tour);
        auto new_length = new_tour.length();
        if (new_length < best_tour.length())
        {
            best_tour = new_tour;
            improving = true;
        }
        std::cout << "v-opt perturbation improvement: " << new_length << std::endl;
        new_tour = lateral::perturbation_climb(best_tour);
        new_length = new_tour.length();
        if (new_length < best_tour.length())
        {
            best_tour = new_tour;
            improving = true;
        }
        std::cout << "2-opt perturbation improvement: " << new_length << std::endl;
    } while (improving);
    std::cout << "final length: " << best_tour.length() << std::endl;
    return 0;
}
