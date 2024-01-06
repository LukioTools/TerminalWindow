#pragma once

#include <fstream>
auto clog = std::ofstream("/dev/pts/1");
auto clog2 = std::ofstream("/dev/pts/6");

auto& cerr_redirect = clog;