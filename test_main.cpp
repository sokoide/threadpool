#include "headers.hpp"

using namespace std;
using ll = long long;

#include <gtest/gtest.h>

// TEST(TestAdd, Add1) { EXPECT_EQ(3, add(1, 2)); }
// TEST(TestAdd, Add2) { EXPECT_EQ(-2, add(1, -3)); }
//
// class AddFixture : public ::testing::TestWithParam<std::tuple<int, int, int>>
// {
// };
// INSTANTIATE_TEST_SUITE_P(AddTest, AddFixture,
//                          ::testing::Values(std::make_tuple(1, 2, 3),
//                                            std::make_tuple(-3, 3, 0)));
//
// class SubFixture : public ::testing::TestWithParam<std::tuple<int, int, int>>
// {
// };
// INSTANTIATE_TEST_SUITE_P(SubTest, SubFixture,
//                          ::testing::Values(std::make_tuple(1, 2, -1),
//                                            std::make_tuple(-3, 3, -6)));
//
// class TrangeFixture
//     : public ::testing::TestWithParam<std::tuple<int, int, int, int, int>>
//     {};
// INSTANTIATE_TEST_SUITE_P(
//     TrangeTest, TrangeFixture,
//     ::testing::Values(
//         std::make_tuple(4, 0, 1, 0, 0), std::make_tuple(4, 0, 2, 3, 3),
//         std::make_tuple(4, 0, 3, 2, 2), std::make_tuple(4, 1, 0, 2, 3),
//         std::make_tuple(4, 1, 1, 2, 3), std::make_tuple(4, 1, 2, 0, 1),
//         std::make_tuple(4, 1, 3, 0, 1), std::make_tuple(8, 1, 0, 2, 3),
//         std::make_tuple(8, 1, 1, 2, 3), std::make_tuple(8, 1, 2, 0, 1),
//         std::make_tuple(8, 1, 3, 0, 1), std::make_tuple(8, 1, 4, 6, 7),
//         std::make_tuple(8, 1, 5, 6, 7), std::make_tuple(8, 1, 6, 4, 5),
//         std::make_tuple(8, 1, 7, 4, 5), std::make_tuple(8, 2, 7, 0, 3),
//         std::make_tuple(8, 2, 5, 0, 3), std::make_tuple(4, 0, 0, 1, 1)));
//
// TEST_P(AddFixture, hoge) {
//     int a = std::get<0>(GetParam());
//     int b = std::get<1>(GetParam());
//     int expected = std::get<2>(GetParam());
//     ASSERT_EQ(expected, add(a, b));
// }
//
// TEST_P(SubFixture, page) {
//     int a = std::get<0>(GetParam());
//     int b = std::get<1>(GetParam());
//     int expected = std::get<2>(GetParam());
//     ASSERT_EQ(expected, sub(a, b));
// }
//
// TEST_P(TrangeFixture, one) {
//     int k = std::get<0>(GetParam());
//     int kk = std::get<1>(GetParam());
//     int p = std::get<2>(GetParam());
//     int a = std::get<3>(GetParam());
//     int b = std::get<4>(GetParam());
//     pair<int, int> r = trange(k, kk, p);
//     EXPECT_EQ(r.first, a);
//     EXPECT_EQ(r.second, b);
// }
