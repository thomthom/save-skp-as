// SaveSkpAs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

#include "SketchUpAPI/sketchup.h"


// https://stackoverflow.com/a/40633672/486990
template <typename TF>
void for_files_with_extension(std::string_view dir, std::string_view ext, TF&& f)
{
  for(const auto& p : std::filesystem::directory_iterator(dir))
  {
    if(p.path().extension() == ext)
    { 
      f(p);
    }
  }
}


bool SaveSkpAs(const std::filesystem::path& input_file, const std::filesystem::path& output_path, SUModelVersion version)
{
  const auto basename = input_file.filename();
  const auto output_file = output_path / basename;

  //std::cout << "  Base: " << basename << "\n";
  //std::cout << " Input: " << input_file << "\n";
  //std::cout << "Output: " << output_file << "\n";

  std::cout << "Processing: " << basename;

  SUModelRef model = SU_INVALID;
  auto result = SUModelCreateFromFile(&model, input_file.string().c_str());
  if (result != SU_ERROR_NONE)
  {
    std::cout << "\n";
    std::cerr << "\nUnable to open file. Error code: " << result << "\n";
    return false;
  }
  result = SUModelSaveToFileWithVersion(model, output_file.string().c_str(), version);
  if (result != SU_ERROR_NONE)
  {
    std::cout << "\n";
    std::cerr << "\nUnable to save file. Error code: " << result << "\n";
    return false;
  }
  SUModelRelease(&model);
  std::cout << " Done!\n";
  return true;
}

class SkpVersionParseError : public std::runtime_error
{
public:
  SkpVersionParseError(const char* message) : std::runtime_error(message) {}
  SkpVersionParseError(std::string message) : std::runtime_error(message) {}
};

SUModelVersion ParseVersion(std::string version_string)
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
    throw SkpVersionParseError("Unknown version");
  }
}


int main(int argc, char* argv[])
{
  // PROGRAM SKPVERSION SOURCE DESTINATION
  if (argc < 4) {
    const std::filesystem::path program(argv[0]);
    std::cerr << "Usage: " << program.filename().string() << " SKPVERSION SOURCE DESTINATION" << std::endl;
    return 1;
  }

  auto version = SUModelVersion_SU2018;
  try
  {
    version = ParseVersion(argv[1]);
  }
  catch(SkpVersionParseError&)
  {
    std::cout << "Unknown SketchUp file format version: " << argv[1] << "\n";
    return 1;
  }

  const std::filesystem::path input_path(argv[2]);
  const std::filesystem::path output_path(argv[3]);

  if (!std::filesystem::is_directory(input_path))
  {
    std::cout << "Input path " << input_path.string() << "is not a directory.\n";
    return 1;
  }

  if (!std::filesystem::is_directory(output_path))
  {
    std::cout << "Output path " << output_path.string() << "is not a directory.\n";
    return 1;
  }

  std::cout << "    Version: " << argv[1] << "\n";
  std::cout << "     Source: " << input_path.string() << "\n";
  std::cout << "Destination: " << output_path.string() << "\n";

  for_files_with_extension(input_path.string(), ".skp", 
    [&output_path, &version](const auto& p)
  {
    SaveSkpAs(p, output_path, version);
  });
  return 0;
}

