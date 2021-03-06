// SaveSkpAs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include "SketchUpAPI/sketchup.h"


// https://stackoverflow.com/a/40633672/486990
template <typename TF>
void for_files_with_extension(const std::string_view directory,
  const std::string_view extension, TF&& f)
{
  for(const auto& entry : std::filesystem::directory_iterator(directory))
  {
    if(entry.path().extension() == extension)
    { 
      f(entry);
    }
  }
}

bool OverwriteFilePrompt(const std::filesystem::path& file)
{
  char type = 0;
  do
  {
    std::cout << "  Destination: " << file.string() << std::endl;
    std::cout << "  File already exist. Overwrite? [y/n]" << std::endl;
    std::cin >> type;
  }
  while(!std::cin.fail() && type != 'y' && type != 'n');
  return type == 'y';
}

bool SaveSkpAs(const std::filesystem::path& input_file,
  const std::filesystem::path& output_path, SUModelVersion version,
  bool force_overwrite = false)
{
  const auto basename = input_file.filename();
  const auto output_file = output_path / basename;

  //std::cout << "  Base: " << basename << "\n";
  //std::cout << " Input: " << input_file << "\n";
  //std::cout << "Output: " << output_file << "\n";

  std::cout << "\nProcessing: " << basename << "\n";

  if (std::filesystem::exists(output_file))
  {
    if (!force_overwrite && !OverwriteFilePrompt(output_file))
    {
      std::cout << "  Skipped\n";
      return false;
    }
  }

  SUModelRef model = SU_INVALID;
  auto result = SUModelCreateFromFile(&model, input_file.string().c_str());
  if (result != SU_ERROR_NONE)
  {
    std::cerr << "  Unable to open file. Error code: " << result << "\n";
    return false;
  }
  result = SUModelSaveToFileWithVersion(model, output_file.string().c_str(), version);
  if (result != SU_ERROR_NONE)
  {
    std::cerr << "  Unable to save file. Error code: " << result << "\n";
    return false;
  }
  SUModelRelease(&model);
  std::cout << "  Saved\n";
  return true;
}

class ArgumentError : public std::runtime_error
{
public:
  explicit ArgumentError(const char* message) : std::runtime_error(message) {}
  explicit ArgumentError(const std::string& message) : std::runtime_error(message) {}
};

SUModelVersion ParseVersion(const std::string& version_string)
{
  const auto version_code = std::stoi(version_string);
  switch (version_code)
  {
  case  3: return SUModelVersion_SU3;
  case  4: return SUModelVersion_SU4;
  case  5: return SUModelVersion_SU5;
  case  6: return SUModelVersion_SU6;
  case  7: return SUModelVersion_SU7;
  case  8: return SUModelVersion_SU8;
  case 13: return SUModelVersion_SU2013;
  case 14: return SUModelVersion_SU2014;
  case 15: return SUModelVersion_SU2015;
  case 16: return SUModelVersion_SU2016;
  case 17: return SUModelVersion_SU2017;
  case 18: return SUModelVersion_SU2018;
  default:
    throw ArgumentError("Unknown SketchUp file format version: " + version_string);
  }
}

std::filesystem::path ParseDirectory(std::string_view path_string)
{
  std::filesystem::path path(path_string);
  if (!std::filesystem::is_directory(path))
  {
    throw ArgumentError("Invalid path: " + path.string());
  }
  return path;
}

bool ParseOptionalBool(int argc, char* argv[], size_t index,
  const std::string_view flag, bool default_value)
{
  if (index >= argc)
    return default_value;
  const std::string_view argument(argv[index]);
  return argument == flag;
}

void CheckArgumentCount(size_t required, size_t optional, int argc, char* argv[])
{
  if (argc < required + 1 || argc > required + optional + 1) {
    std::stringstream message;
    message << "Wrong number of arguments: " << argc <<
      " (Expected " << required << " to " << required + optional << ")";
    throw ArgumentError(message.str());
  }
}


int main(int argc, char* argv[]) try
{
  CheckArgumentCount(3, 1, argc, argv);
  const auto version = ParseVersion(argv[1]);
  const auto input_path = ParseDirectory(argv[2]);
  const auto output_path = ParseDirectory(argv[3]);
  const auto force = ParseOptionalBool(argc, argv, 4, "-f", false);

  std::cout << "    Version: " << argv[1] << "\n";
  std::cout << "     Source: " << input_path.string() << "\n";
  std::cout << "Destination: " << output_path.string() << "\n";

  for_files_with_extension(input_path.string(), ".skp", 
    [&output_path, &version, &force](const auto& file)
  {
    SaveSkpAs(file, output_path, version, force);
  });

  return 0;
}
catch(ArgumentError& error)
{
  std::cerr << "Error: " << error.what() << std::endl;
  const auto program = std::filesystem::path(argv[0]).stem().string();
  std::cerr << "Usage: " << program << " SKPVERSION SOURCE DESTINATION [-f]" << std::endl;
  return 1;
}
