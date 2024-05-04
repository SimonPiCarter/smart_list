#ifndef __smart_list__
#define __smart_list__

#include <functional>
#include <vector>
#include <cassert>
#include <list>

template<typename class_t>
struct smart_list;

template<typename class_t>
struct smart_list_handle
{
    /// @brief default constructor
    smart_list_handle() = default;
    /// @brief constructor that set up handle and revision aswell as internal list pointer
    /// @param handle_p the handle refering to the object being pointed at
    /// @param revision_p the revision to ensure that the handle is still valid
    /// @param list_p the list that we refer to
    smart_list_handle(size_t handle_p, unsigned char revision_p, smart_list<class_t> *list_p) : _handle(handle_p), _revision(revision_p), _list(list_p) {}

    /// @brief get the content of the handle
    /// @return a reference to the object being pointed at
    class_t & get();
    /// @brief get the content of the handle
    /// @return a reference to the object being pointed at
    class_t const & get() const;
    /// @return the index handle
    size_t const &handle() const { return _handle; }
    /// @return the revision of the handle
    unsigned char const &revision() const { return _revision; }
    /// @return the pointer to the list handling this handle
    smart_list<class_t> *list() const { return _list; }

    /// @return true if the handle is set up and if the revision is still alive in the list
    bool is_valid() const;

private:
    size_t _handle = 0;
    unsigned char _revision = 0;
    smart_list<class_t> *_list = nullptr;
};

template<typename class_t>
struct smart_list
{
    /// @brief check if the handle is valid in the list
    /// @param idx_p the handle to be checked
    /// @return true if the handle is valid (enabled and revision matches)
    bool is_valid(smart_list_handle<class_t> const &idx_p)
    {
        assert(idx_p.list() == this);
        return idx_p.revision() == data[idx_p.handle()].revision
            && data[idx_p.handle()].enabled;
    }

    /// @brief get the content of the handle
    /// @param idx_p the handle to fetch
    /// @return the content of the handle
    class_t &get(smart_list_handle<class_t> const &idx_p) { assert(idx_p.list() == this); return data[idx_p.handle()].data; }
    /// @brief get the content of the handle
    /// @param idx_p the handle to fetch
    /// @return the content of the handle
    class_t const &get(smart_list_handle<class_t> const &idx_p) const { assert(idx_p.list() == this); return data[idx_p.handle()].data; }

    /// @brief create a new handle corresponding to the value
    /// @param val_p the value to be added
    /// @return the new handle created
    smart_list_handle<class_t> new_instance(class_t const &val_p);
    /// @brief create a new handle corresponding to the value (move)
    /// @param val_p the value to be added
    /// @return the new handle created
    smart_list_handle<class_t> new_instance(class_t &&val_p);

    /// @brief free the handle
    /// @param idx_p the handle to be freed
    void free_instance(smart_list_handle<class_t> const &idx_p)
    {
        assert(idx_p.list() == this);
        indexes.push_back(idx_p.handle());
        data[idx_p.handle()].enabled = false;
    }

    void for_each(std::function<void(class_t &)> const &func_p)
    {
        for(auto &&w : data) { if(w.enabled) { func_p(w.data); } }
    }
    void for_each_const(std::function<void(class_t const &)> const &func_p) const
    {
        for(auto &&w : data) { if(w.enabled) { func_p(w.data); } }
    }

private:

    /// @brief a wrapped handling the data, the revision and if it is alive or not
    struct wrapper
    {
        wrapper() = default;
        wrapper(class_t const &val_p) : data(val_p), enabled(true) {}
        class_t data;
        bool enabled = false;
        unsigned char revision = 0;
    };

    // internal data
    std::vector<wrapper> data;
    // list of free indexes
    std::list<size_t> indexes;
};

/////////////////////////
// smart_list_handle
/////////////////////////

template<typename class_t>
class_t & smart_list_handle<class_t>::get() { return _list->get(*this); }

template<typename class_t>
class_t const & smart_list_handle<class_t>::get() const { return _list->get(*this); }


template<typename class_t>
bool smart_list_handle<class_t>::is_valid() const
{
    return _list && _list->is_valid(*this);
}

/////////////////////////
// smart_list
/////////////////////////

template<typename class_t>
smart_list_handle<class_t> smart_list<class_t>::new_instance(class_t const &val_p)
{
    class_t new_val_l(val_p);
    new_instance(std::move(new_val_l));
}

template<typename class_t>
smart_list_handle<class_t> smart_list<class_t>::new_instance(class_t &&val_p)
{
    if(indexes.size() == 0)
    {
        smart_list_handle<class_t> handle_l(data.size(), 0, this);
        data.emplace_back(val_p);
        return handle_l;
    }
    // get free index and remove it from free list
    size_t idx_l = indexes.front();
    indexes.pop_front();
    // assert check, we whould only have disabled blob marked as free
    assert(!data[idx_l].enabled);
    // store value and re enable data
    std::swap(val_p, data[idx_l].data);
    data[idx_l].enabled = true;
    ++data[idx_l].revision;

    return smart_list_handle<class_t>(idx_l, data[idx_l].revision, this);
}


#endif