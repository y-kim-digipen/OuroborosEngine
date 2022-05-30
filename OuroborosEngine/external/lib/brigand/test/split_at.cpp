
#include <brigand/algorithms/split_at.hpp>
#include <brigand/sequences/list.hpp>
#include <type_traits>

static_assert(
    std::is_same<brigand::split_at<brigand::empty_sequence, brigand::integral_constant<int, 0>>,
                 brigand::list<brigand::list<>, brigand::list<>>>::value,
    "invalid split on empty sequence");

using small_seq = brigand::list<char, bool, int>;

static_assert(std::is_same<brigand::split_at<small_seq, brigand::integral_constant<int, 0>>,
                           brigand::list<brigand::list<>, small_seq>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<small_seq, brigand::integral_constant<int, 3>>,
                           brigand::list<small_seq, brigand::list<>>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<small_seq, brigand::integral_constant<int, 2>>,
                           brigand::list<brigand::list<char, bool>, brigand::list<int>>>::value,
              "invalid split on sequence with no delim");

using no_fast_track_list = brigand::list<char, bool, int, void, char, char, char, bool, int, void,
                                         char, char, char, bool, int, void>;
using fast_track_list = brigand::list<char, bool, int, void, char, char, char, bool, int, void,
                                      char, char, char, bool, int, void, int>;

static_assert(std::is_same<brigand::split_at<no_fast_track_list, brigand::integral_constant<int, 0>>,
                           brigand::list<brigand::list<>, no_fast_track_list>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<no_fast_track_list, brigand::integral_constant<int, 16>>,
                           brigand::list<no_fast_track_list, brigand::list<>>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<no_fast_track_list, brigand::integral_constant<int, 14>>,
                           brigand::list<brigand::list<char, bool, int, void, char, char, char,
                                                       bool, int, void, char, char, char, bool>,
                                         brigand::list<int, void>>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<fast_track_list, brigand::integral_constant<int, 0>>,
                           brigand::list<brigand::list<>, fast_track_list>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<fast_track_list, brigand::integral_constant<int, 17>>,
                           brigand::list<fast_track_list, brigand::list<>>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<fast_track_list, brigand::integral_constant<int, 14>>,
                           brigand::list<brigand::list<char, bool, int, void, char, char, char,
                                                       bool, int, void, char, char, char, bool>,
                                         brigand::list<int, void, int>>>::value,
              "invalid split on sequence with no delim");

namespace custom
{
template <class...>
class custom_list
{
};
static_assert(std::is_same<brigand::split_at<custom_list<>, brigand::integral_constant<int, 0>>,
                           custom_list<custom_list<>, custom_list<>>>::value,
              "invalid split on empty sequence");

using small_seq = custom_list<char, bool, int>;

static_assert(std::is_same<brigand::split_at<small_seq, brigand::integral_constant<int, 0>>,
                           custom_list<custom_list<>, small_seq>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<small_seq, brigand::integral_constant<int, 3>>,
                           custom_list<small_seq, custom_list<>>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<small_seq, brigand::integral_constant<int, 2>>,
                           custom_list<custom_list<char, bool>, custom_list<int>>>::value,
              "invalid split on sequence with no delim");

using no_fast_track_list = custom_list<char, bool, int, void, char, char, char, bool, int, void,
                                       char, char, char, bool, int, void>;
using fast_track_list = custom_list<char, bool, int, void, char, char, char, bool, int, void, char,
                                    char, char, bool, int, void, int>;

static_assert(std::is_same<brigand::split_at<no_fast_track_list, brigand::integral_constant<int, 0>>,
                           custom_list<custom_list<>, no_fast_track_list>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<no_fast_track_list, brigand::integral_constant<int, 16>>,
                           custom_list<no_fast_track_list, custom_list<>>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<no_fast_track_list, brigand::integral_constant<int, 14>>,
                           custom_list<custom_list<char, bool, int, void, char, char, char, bool,
                                                   int, void, char, char, char, bool>,
                                       custom_list<int, void>>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<fast_track_list, brigand::integral_constant<int, 0>>,
                           custom_list<custom_list<>, fast_track_list>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<fast_track_list, brigand::integral_constant<int, 17>>,
                           custom_list<fast_track_list, custom_list<>>>::value,
              "invalid split on sequence with no delim");

static_assert(std::is_same<brigand::split_at<fast_track_list, brigand::integral_constant<int, 14>>,
                           custom_list<custom_list<char, bool, int, void, char, char, char, bool,
                                                   int, void, char, char, char, bool>,
                                       custom_list<int, void, int>>>::value,
              "invalid split on sequence with no delim");
}
