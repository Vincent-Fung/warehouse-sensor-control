#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "warehouse/Article.hpp"
#include "warehouse/StorageLocation.hpp"
#include "warehouse/Warehouse.hpp"
#include "warehouse/FirstFitStrategy.hpp"
#include "warehouse/ConsolidationStrategy.hpp"

namespace py = pybind11;

PYBIND11_MODULE(warehouse_py, m) {
    py::class_<Article>(m, "Article")
        .def(py::init<int, const std::string&, double>())
        .def("get_id", &Article::getId)
        .def("get_name", &Article::getName);

    py::class_<StorageLocation>(m, "StorageLocation")
        .def(py::init<int, double, double, std::optional<int>>())
        .def("get_id", &StorageLocation::getId);

    py::class_<Warehouse>(m, "Warehouse")
        .def(py::init([](std::vector<StorageLocation> locs, std::vector<Article> arts) {
            return std::make_unique<Warehouse>(locs, arts, std::make_unique<ConsolidationStrategy>());
        }))
        .def("store_article", &Warehouse::storeArticle);
}