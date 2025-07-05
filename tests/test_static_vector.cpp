#include <gtest/gtest.h>

#include <uutils/static_vector.h>

TEST(StaticVector, ConstructEmpty) {
	uutils::StaticVector<int, 10> vector;

	EXPECT_EQ(vector.size(), 0);
	EXPECT_EQ(vector.capacity(), 10);
}

TEST(StaticVector, ConstructFromInitializerList) {
	uutils::StaticVector<int, 10> vector{ std::initializer_list<int> { 1, 2, 3, 4, 5 } };

	EXPECT_EQ(vector.size(), 5);
	EXPECT_EQ(vector.capacity(), 10);
	EXPECT_EQ(vector.at(1), 2);
}

TEST(StaticVector, Iterate) {
	uutils::StaticVector<int, 5> vector{ { 1, 2, 3, 4, 5 } };
	int sum = 0;
	for (int v : vector)
	{
		sum += v;
	}
	EXPECT_EQ(sum, 15);
}

TEST(StaticVector, Modify) {
	uutils::StaticVector<int, 5> vector;
	
	EXPECT_EQ(vector.size(), 0);
	EXPECT_TRUE(vector.empty());
	vector.push_back(10);
	EXPECT_EQ(vector.size(), 1);
	EXPECT_EQ(vector[0], 10);

	vector.push_back(20);
	vector.push_back(30);
	EXPECT_EQ(vector.size(), 3);
	EXPECT_EQ(vector[1], 20);
	EXPECT_EQ(vector.back(), 30);

	vector.pop_back();
	EXPECT_EQ(vector.size(), 2);
	EXPECT_EQ(vector[1], 20);

	vector.erase(vector.cbegin());
	EXPECT_EQ(vector.size(), 1);
	EXPECT_EQ(vector.front(), vector.back());
	EXPECT_EQ(vector.front(), 20);

	vector.push_back(2);
	vector.push_back(3);
	vector.push_back(4);
	vector.push_back(5);
	EXPECT_THROW(vector.push_back(6), std::length_error);
}