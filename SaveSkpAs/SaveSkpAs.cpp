// SaveSkpAs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <filesystem>
#include <iostream>
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


int main(int argc, char* argv[])
{
  if (argc < 3) { // We expect 3 arguments: the program name, the source path and the destination path
    const std::filesystem::path program(argv[0]);
    std::cerr << "Usage: " << program.filename().string() << " SOURCE DESTINATION" << std::endl;
    return 1;
  }

  const std::filesystem::path input_path(argv[1]);
  const std::filesystem::path output_path(argv[2]);

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

  std::cout << "     Source: " << input_path.string() << "\n";
  std::cout << "Destination: " << output_path.string() << "\n";

  for_files_with_extension(input_path.string(), ".skp", 
    [&output_path](const auto& p)
  {
    SaveSkpAs(p, output_path, SUModelVersion_SU8);
  });
  return 0;
}

