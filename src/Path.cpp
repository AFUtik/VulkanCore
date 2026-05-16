#include "Path.hpp"

fs::path exePath = fs::read_symlink("/proc/self/exe");
fs::path exeDir  = exePath.parent_path();