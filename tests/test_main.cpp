#include <club.h>
#include <gtest/gtest.h>
#include <outputAdapter.h>
#include <tableState.h>

#include <filesystem>
#include <fstream>

TEST(UnitTest, ValidTime) {
  std::string time_str = "09:00";
  std::time_t time = parseTime(time_str);
  std::tm* tm = std::localtime(&time);
  EXPECT_EQ(tm->tm_hour, 9);
  EXPECT_EQ(tm->tm_min, 0);
}

std::string readFile(const std::string& fileName) {
  std::ifstream file(fileName);
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  return buffer.str();
}

std::string runMainWithInput(const std::string& inputFile,
                             const std::string& outputFileName) {
  std::string command = "./computerClub " + inputFile + " > " + outputFileName;

  std::system(command.c_str());

  return readFile(outputFileName);
}

TEST(IntegrationTest, CompareOutput) {
  std::string testDir = "cases/";

  for (const auto& entry : std::filesystem::directory_iterator(testDir)) {
    if (entry.path().extension() == ".txt" &&
        entry.path().filename().string().starts_with("input")) {
      std::cout << "Testing: " << entry.path() << std::endl;

      std::string inputFile = entry.path().string();
      std::string expectedOutputFile =
          testDir + "output" + entry.path().stem().string().substr(5, 1) +
          ".txt";
      std::string actualOutputFile = testDir + "actual_output" +
                                     entry.path().stem().string().substr(5, 1) +
                                     ".txt";

      std::string expectedOutput = readFile(expectedOutputFile);
      std::string actualOutput = runMainWithInput(inputFile, actualOutputFile);

      EXPECT_EQ(actualOutput, expectedOutput)
          << "Failed for input file: " << inputFile;
    }
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}