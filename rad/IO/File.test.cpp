#include <rad/IO/File.h>

#include <rad/Common/Hash.h>

#include <gtest/gtest.h>

TEST(IO, File)
{
    std::filesystem::path tempFilePath = std::filesystem::current_path() / "temp.txt";
    std::string fileName = (const char*)tempFilePath.u8string().c_str();

    constexpr size_t LineCount = 128;

    std::string refText;
    std::vector<std::string> refLines;
    { // Create a text file:
        rad::File file(fileName, std::ios::out | std::ios::trunc);
        EXPECT_TRUE(file.IsOpen());
        EXPECT_EQ(file.GetPath(), fileName);
        for (size_t i = 0; i < LineCount; ++i)
        {
            std::string line = std::format("Line#{}\n", i + 1);
            EXPECT_TRUE(file.Write(line));
            refText += line;
            line.pop_back(); // Remove the line feed.
            refLines.push_back(line);
        }
    }

    { // Read line by line:
        rad::File file;
        EXPECT_TRUE(file.Open(fileName, std::ios::in));
        EXPECT_TRUE(file.IsOpen());
        EXPECT_GT(file.GetSize(), 0);
        std::string line;
        for (size_t i = 0; i < LineCount; ++i)
        {
            ASSERT_TRUE(file.ReadLine(line));
            ASSERT_EQ(line, refLines[i]);
        }
        EXPECT_FALSE(file.ReadLine(line));
    }

    { // Read the entire file:
        std::vector<uint8_t> binary = rad::File::ReadAll(fileName);
        EXPECT_FALSE(binary.empty());
        uint64_t fileSize = rad::File::GetSize(fileName);
        EXPECT_EQ(binary.size(), fileSize);

        std::string text = rad::File::ReadAllText(fileName);
        EXPECT_FALSE(text.empty());
        EXPECT_EQ(text, refText);

        std::vector<std::string> lines = rad::File::ReadAllLines(fileName);
        ASSERT_EQ(lines.size(), LineCount);
        for (size_t i = 0; i < LineCount; ++i)
        {
            ASSERT_EQ(lines[i], refLines[i]);
        }
    }

    // Cleanup
    std::error_code ec;
    std::filesystem::remove(tempFilePath, ec);
    EXPECT_FALSE(ec);
}
