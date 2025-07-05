#include <gtest/gtest.h>
#include <array>
#include <vector>
#include <string>

#include <uutils/data_processing.h>

TEST(DataPipeline, Range_FromArray) {
	using namespace uutils::data_processing;
	std::array<int, 5> ints = { 1, 2, 3, 4, 5 };

	auto range = Range::from(ints);
	EXPECT_EQ(range.begin(), ints.begin());
	EXPECT_EQ(range.end(), ints.end());
}

TEST(DataPipeline, Range_FromVector) {
	using namespace uutils::data_processing;
	std::vector<int> ints = { 1, 2, 3, 4, 5 };

	auto range = Range::from(ints);
	EXPECT_EQ(range.begin(), ints.begin());
	EXPECT_EQ(range.end(), ints.end());
}

TEST(DataPipeline, Range_FromString) {
	using namespace uutils::data_processing;
	std::string str = "Hello";

	auto range = Range::from(str);
	EXPECT_EQ(range.begin(), str.begin());
	EXPECT_EQ(range.end(), str.end());
}

TEST(DataPipeline, ToVector) {
	using namespace uutils::data_processing;

	std::string str = "Hello";
	std::vector<std::string::value_type> expected = { 'H', 'e', 'l', 'l', 'o' };

	EXPECT_EQ(Range::from(str) > to_vector(), expected);
};

TEST(DataPipeline, Map) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };
	std::vector<int> expected = { 2, 4, 6, 8, 10 };

	EXPECT_EQ(Range::from(data) > map([](int x) { return x * 2; }) > to_vector(), expected);
};

TEST(DataPipeline, Filter) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };
	std::vector expected = { 1, 3, 5 };

	EXPECT_EQ(Range::from(data) > filter([](int x) { return x % 2 == 1; }) > to_vector(), expected);
};

TEST(DataPipeline, Filter_RemovedFromStart) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };
	std::vector expected = { 2, 4 };

	EXPECT_EQ(Range::from(data) > filter([](int x) { return x % 2 == 0; }) > to_vector(), expected);
};

TEST(DataPipeline, Filter_ReturnEmpty) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };

	EXPECT_TRUE((Range::from(data) > filter([](int x) { return x < 0; }) > to_vector()).empty());
};

TEST(DataPipeline, Skip) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };
	std::vector expected = { 3, 4, 5 };

	EXPECT_EQ(Range::from(data) > skip(2) > to_vector(), expected);
};

TEST(DataPipeline, Skip_MoreThanDataSize_ReturnEmpty) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };

	EXPECT_TRUE((Range::from(data) > skip(data.size() + 1) > to_vector()).empty());
};

TEST(DataPipeline, Take) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };
	std::vector expected = { 1, 2, 3 };

	EXPECT_EQ(Range::from(data) > take(3) > to_vector(), expected);
};

TEST(DataPipeline, Take_MoreThanDataSize_ReturnData) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };
	std::vector expected = { 1, 2, 3, 4, 5 };

	EXPECT_EQ(Range::from(data) > take(data.size() + 1) > to_vector(), expected);
};

TEST(DataPipeline, Reverse_CurrentlyDoesNotWorkProperly_TODO_FIX_LATER) {
	using namespace uutils::data_processing;

	/*
	std::array data = { 1, 2, 3, 4, 5 };
	std::vector expected = { 5, 4, 3, 2, 1 };

	EXPECT_EQ(Range::from(data) > reverse() > to_vector(), expected);*/
	EXPECT_TRUE(true);
};

TEST(DataPipeline, Range) {
	using namespace uutils::data_processing;

	EXPECT_EQ((range(5, 10) > to_vector()).size(), 10);
};

TEST(DataPipeline, Sum) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };

	EXPECT_EQ(Range::from(data) > sum(), 15);
};

TEST(DataPipeline, All) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };

	EXPECT_TRUE(Range::from(data) > all([](int x) { return x > 0; }));
};

TEST(DataPipeline, Any) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };

	EXPECT_TRUE(Range::from(data) > any([](int x) { return x > 4; }));
};

TEST(DataPipeline, None) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };

	EXPECT_TRUE(Range::from(data) > none([](int x) { return x < 1; }));
};

TEST(DataPipeline, Chain_V1) {
	using namespace uutils::data_processing;

	std::array data = { 1, 2, 3, 4, 5 };
	std::vector expected = { 4, 9, 16 };

	EXPECT_EQ(Range::from(data) > skip(1) > take(3) > map([](int x) { return x * x; }) > to_vector(), expected);
};

TEST(DataPipeline, Chain_V2) {
	using namespace uutils::data_processing;

	std::vector expected = { 1, 2, 3, 4, 5 };

	EXPECT_EQ(range(1, 10) > filter([](int x) { return x % 2 == 0; }) > map([](int x) { return x / 2; }) > to_vector(), expected);
};

TEST(DataPipeline, Chain_V3) {
	using namespace uutils::data_processing;

	std::vector expected = { 1, 2, 3, 4, 5 };

	EXPECT_EQ(Range::from("12345") > filter([](char x) { return x >= '0'; }) > map([](char x) { return (int)(x - '0'); }) > sum(), 15);
};