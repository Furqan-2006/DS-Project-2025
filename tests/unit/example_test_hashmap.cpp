// Example unit test for HashMap
// This is a sample test file to demonstrate testing structure
// To actually run tests, you need to install Google Test

#include <gtest/gtest.h>
#include "../SMP_backend/include/ADT/hash_map.hpp"
#include <string>

// Test fixture for HashMap tests
class HashMapTest : public ::testing::Test {
protected:
    HashMap<int, std::string> intMap;
    HashMap<std::string, int> stringMap;
    
    void SetUp() override {
        // Setup runs before each test
        intMap.clear();
        stringMap.clear();
    }
};

// Test basic insertion
TEST_F(HashMapTest, InsertAndRetrieve) {
    intMap.insert(1, "one");
    intMap.insert(2, "two");
    
    EXPECT_TRUE(intMap.contains(1));
    EXPECT_TRUE(intMap.contains(2));
    EXPECT_EQ(intMap.get(1), "one");
    EXPECT_EQ(intMap.get(2), "two");
}

// Test removal
TEST_F(HashMapTest, RemoveElement) {
    intMap.insert(1, "one");
    intMap.insert(2, "two");
    
    EXPECT_TRUE(intMap.erase(1));
    EXPECT_FALSE(intMap.contains(1));
    EXPECT_TRUE(intMap.contains(2));
}

// Test size tracking
TEST_F(HashMapTest, SizeTracking) {
    EXPECT_EQ(intMap.size(), 0);
    
    intMap.insert(1, "one");
    EXPECT_EQ(intMap.size(), 1);
    
    intMap.insert(2, "two");
    EXPECT_EQ(intMap.size(), 2);
    
    intMap.erase(1);
    EXPECT_EQ(intMap.size(), 1);
}

// Test clearing
TEST_F(HashMapTest, ClearMap) {
    intMap.insert(1, "one");
    intMap.insert(2, "two");
    intMap.clear();
    
    EXPECT_EQ(intMap.size(), 0);
    EXPECT_TRUE(intMap.empty());
}

// Test with string keys
TEST_F(HashMapTest, StringKeys) {
    stringMap.insert("apple", 1);
    stringMap.insert("banana", 2);
    
    EXPECT_TRUE(stringMap.contains("apple"));
    EXPECT_EQ(stringMap.get("banana"), 2);
}

// Main function for test executable
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
