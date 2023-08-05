#include "json.h"

namespace ulib
{
    json::json(const json &v) { copy_construct_from_other(v); }
    json::json(json &&v) { move_construct_from_other(std::move(v)); }
    json::~json() { destroy_containers(); }

    json &json::operator=(const json &right)
    {
        destroy_containers();
        copy_construct_from_other(right);
        return *this;
    }

    json &json::operator=(json &&right)
    {
        destroy_containers();
        move_construct_from_other(std::move(right));
        return *this;
    }

    json &json::push_back()
    {
        implicit_touch_array();
        return mArray.emplace_back();
    }

    // if value is exists, works like "at" otherwise creates value and set value type to undefined
    json &json::find_or_create(StringViewT name)
    {
        if (implicit_touch_object())
            return mObject.emplace_back(name).value();

        for (auto &obj : mObject)
            if (obj.name() == name)
                return obj.value();

        return mObject.emplace_back(name).value();
    }

    json &json::find_or_create(size_t idx)
    {
        if (implicit_touch_array())
            return mArray.emplace_back();

        if (idx >= mArray.size())
        {
            mArray.resize(idx + 1);
            return mArray.back();
        }

        return mArray[idx];
    }

    const json &json::find_if_exists(StringViewT name) const
    {
        implicit_const_touch_object();

        for (auto &obj : mObject)
            if (obj.name() == name)
                return obj.value();

        throw exception{"json object key not found"};
    }

    const json &json::find_if_exists(size_t idx) const
    {
        if (idx >= mArray.size())
            throw exception{"json array index out of range"};

        return mArray[idx];
    }

    // private: -----------------------

    void json::initialize_as_string()
    {
        new (&mString) StringT;
        mType = value_t::string;
    }

    void json::initialize_as_object()
    {
        new (&mObject) ObjectT;
        mType = value_t::object;
    }

    void json::initialize_as_array()
    {
        new (&mArray) ArrayT;
        mType = value_t::array;
    }

    bool json::implicit_touch_string()
    {
        if (mType == value_t::string)
            return false;

        if (mType != value_t::null)
            throw json::exception("value must be an string or null");

        return initialize_as_string(), true;
    }

    bool json::implicit_touch_object()
    {
        if (mType == value_t::object)
            return false;

        if (mType != value_t::null)
            throw json::exception("value must be an object or null");

        return initialize_as_object(), true;
    }

    bool json::implicit_touch_array()
    {
        if (mType == value_t::array)
            return false;

        if (mType != value_t::null)
            throw json::exception("value must be an array or null");

        return initialize_as_array(), true;
    }

    void json::implicit_const_touch_string() const
    {
        if (mType != value_t::string)
            throw json::exception("value must be an string or null");
    }

    void json::implicit_const_touch_object() const
    {
        if (mType != value_t::object)
            throw json::exception("value must be an object or null");
    }

    void json::implicit_const_touch_array() const
    {
        if (mType != value_t::array)
            throw json::exception("value must be an array or null");
    }

    void json::implicit_set_string(StringViewT other)
    {
        if (mType == value_t::string)
            mString.assign(other);

        if (mType != value_t::null)
            throw json::exception("value must be an string or null");

        new (&mString) StringT(other);
        mType = value_t::string;
    }

    void json::implicit_move_set_string(StringT &&other)
    {
        if (mType == value_t::string)
            mString.assign(std::move(other));

        if (mType != value_t::null)
            throw json::exception("value must be an string or null");

        new (&mString) StringT(std::move(other));
        mType = value_t::string;
    }

    void json::implicit_set_float(float other)
    {
        if (mType == value_t::floating)
        {
            mFloatVal = other;
            return;
        }

        if (mType == value_t::integer)
        {
            mFloatVal = other, mType = value_t::floating;
            return;
        }

        if (mType != value_t::null)
            throw json::exception("value must be an numeric or null");

        mFloatVal = other, mType = value_t::floating;
    }
    void json::implicit_set_integer(int64_t other)
    {
        if (mType == value_t::integer)
        {
            mIntVal = other;
            return;
        }

        if (mType == value_t::floating)
        {
            mIntVal = other, mType = value_t::integer;
            return;
        }

        if (mType != value_t::null)
            throw json::exception("value must be an numeric or null");

        mIntVal = other, mType = value_t::integer;
    }

    void json::implicit_set_boolean(bool other)
    {
        if (mType == value_t::boolean)
        {
            mBoolVal = other;
            return;
        }

        if (mType != value_t::null)
            throw json::exception("value must be an numeric or null");

        mIntVal = other, mType = value_t::boolean;
    }

    void json::construct_as_string(StringViewT other)
    {
        new (&mString) StringT(other);
        mType = value_t::string;
    }

    void json::move_construct_as_string(StringT &&other) 
    {
        new (&mString) StringT(std::move(other));
        mType = value_t::string;
    }

    void json::copy_construct_from_other(const json &other)
    {
        switch (other.mType)
        {
        case value_t::object:
            new (&mObject) ObjectT(other.mObject);
            break;
        case value_t::array:
            new (&mArray) ArrayT(other.mArray);
            break;
        case value_t::string:
            new (&mString) StringT(other.mString);
            break;
        default:
            mIntVal = other.mIntVal;
        }

        mType = other.mType;
    }

    void json::move_construct_from_other(json &&other)
    {
        switch (other.mType)
        {
        case value_t::object:
            new (&mObject) ObjectT(std::move(other.mObject));
            break;
        case value_t::array:
            new (&mArray) ArrayT(std::move(other.mArray));
            break;
        case value_t::string:
            new (&mString) StringT(std::move(other.mString));
            break;
        default:
            mIntVal = other.mIntVal;
        }

        mType = other.mType;
        other.mType = value_t::null;
    }

    void json::destroy_containers()
    {
        switch (mType)
        {
        case value_t::object:
            mObject.~ObjectT();
            break;
        case value_t::array:
            mArray.~ArrayT();
            break;
        case value_t::string:
            mString.~StringT();
            break;

        default:
            break;
        }
    }

} // namespace ulib
