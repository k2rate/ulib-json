#include <gtest/gtest.h>
#include <ulib/json.h>

TEST(JsonTree, AssignAndGet)
{
    {
        ulib::json obj;
        obj.assign("hello");
        ASSERT_EQ(obj.get<ulib::string>(), "hello");
    }

    {
        ulib::json obj;
        obj.assign(30);
        ASSERT_EQ(obj.get<int>(), 30);

        obj.assign(30.5f);
        float gv = obj.get<float>();
        ASSERT_TRUE(gv >= 30.4f && gv <= 30.6f);
    }
}

TEST(JsonTree, Object)
{
    {
        ulib::json obj;
        obj["port"] = 25005;
        obj["text"] = "test";

        ASSERT_EQ(obj["port"].get<int>(), 25005);
        ASSERT_EQ(obj["text"].get<ulib::string>(), "test");
    }
}

TEST(JsonTree, NestedObject)
{
    {
        ulib::json obj;
        obj["object"]["port"] = 25005;
        obj["object"]["text"] = "test";

        ASSERT_EQ(obj["object"]["port"].get<int>(), 25005);
        ASSERT_EQ(obj["object"]["text"].get<ulib::string>(), "test");
    }
}

TEST(JsonTree, Search)
{
    ulib::json value;
    value["hellword"] = "test";
    value["option"] = "check";

    auto v = value.search("hellword");
    ASSERT_TRUE(v != nullptr);
    ASSERT_TRUE(v->get<ulib::string>() == "test");

    // ASSERT_EQ(value.dump(), "\"full\\nplak\"");
}