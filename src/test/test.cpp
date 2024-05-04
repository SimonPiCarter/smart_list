#include "gtest/gtest.h"

#include <iostream>
#include "smart_list/smart_list.h"

TEST(simple, simple)
{
    smart_list<int> test_l;

    smart_list_handle<int> first_l = test_l.new_instance(17);
    smart_list_handle<int> second_l = test_l.new_instance(25);

    EXPECT_EQ(17, first_l.get());
    EXPECT_EQ(25, second_l.get());
    EXPECT_EQ(0u, first_l.handle());
    EXPECT_EQ(1u, second_l.handle());
    EXPECT_TRUE(first_l.is_valid());
    EXPECT_TRUE(second_l.is_valid());

    test_l.for_each([](int const &i) {
        std::cout<<i<<std::endl;
    });
    std::cout<<std::endl;

    test_l.free_instance(second_l);

    test_l.for_each([](int const &i) {
        std::cout<<i<<std::endl;
    });
    std::cout<<std::endl;


    smart_list_handle<int> third_l = test_l.new_instance(2);

    EXPECT_EQ(17, first_l.get());
    EXPECT_EQ(2, third_l.get());
    EXPECT_EQ(0u, first_l.handle());
    EXPECT_EQ(1u, third_l.handle());
    EXPECT_TRUE(first_l.is_valid());
    EXPECT_FALSE(second_l.is_valid());

    test_l.for_each([](int const &i) {
        std::cout<<i<<std::endl;
    });
}

struct TestCopy
{
    static size_t move;
    static size_t copy;
    static size_t assignation;
    static size_t creation;
    static size_t destruction;

    TestCopy() { ++creation; }
    TestCopy(int val_p) : val(val_p) { ++creation; }
    ~TestCopy() { ++destruction; }
    TestCopy(const TestCopy &other_p) : val(other_p.val)
    {
        ++copy;
    }
    TestCopy(TestCopy &&other_p) : val(other_p.val)
    {
        ++move;
    }
    TestCopy& operator=(const TestCopy &other_p)
    {
        val = other_p.val;
        ++assignation;
        return *this;
    }


    int val = 0;

};

size_t TestCopy::assignation = 0;
size_t TestCopy::move = 0;
size_t TestCopy::copy = 0;
size_t TestCopy::creation = 0;
size_t TestCopy::destruction = 0;

void display()
{
    std::cout<<" creation = "<<TestCopy::creation<<std::endl;
    std::cout<<" copy = "<<TestCopy::copy<<std::endl;
    std::cout<<" move = "<<TestCopy::move<<std::endl;
    std::cout<<" destruction = "<<TestCopy::destruction<<std::endl;
    std::cout<<std::endl;
}

TEST(simple, copy)
{
    {
        smart_list<TestCopy> test_l;

        smart_list_handle<TestCopy> first_l = test_l.new_instance({17});
        display();
        smart_list_handle<TestCopy> second_l = test_l.new_instance({2});
        display();
        smart_list_handle<TestCopy> third_l = test_l.new_instance({25});
        display();

        test_l.free_instance(first_l);
        test_l.free_instance(second_l);
        test_l.free_instance(third_l);

        display();

        first_l = test_l.new_instance({11});
        second_l = test_l.new_instance({12});
        third_l = test_l.new_instance({13});

        display();
    }

    display();
}

TEST(simple, recycle)
{
    smart_list<int> test_l;

    smart_list_handle<int> first_l = test_l.new_instance(17);
    smart_list_handle<int> second_l = test_l.new_instance(25);

    EXPECT_EQ(17, first_l.get());
    EXPECT_EQ(25, second_l.get());
    EXPECT_EQ(0u, first_l.handle());
    EXPECT_EQ(1u, second_l.handle());
    EXPECT_TRUE(first_l.is_valid());
    EXPECT_TRUE(second_l.is_valid());

	test_l.free_instance(second_l);
    EXPECT_FALSE(second_l.is_valid());

	smart_list_handle<int> third_l = test_l.recycle_instance();
    EXPECT_FALSE(second_l.is_valid());
    EXPECT_TRUE(third_l.is_valid());

    EXPECT_EQ(1u, third_l.handle());
    EXPECT_EQ(1u, third_l.revision());
    EXPECT_EQ(25, third_l.get());

	third_l.get() = 12;
    EXPECT_EQ(12, third_l.get());
    EXPECT_EQ(12, test_l.get(1));
}
